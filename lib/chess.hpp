
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <span>
#include <iosfwd>
#include <compare>
#include <concepts>
#include <optional>
#include <stdexcept>
#include "generator.hpp"

namespace swo3 {
	namespace internal {
		[[noreturn]]
		inline
		void unreachable() {} //TODO: [C++23] replace with std::unreachable
	}


	enum class state { ongoing, checkmate, stalemate, };


	using glyph = char;


	enum class color { white, black, };

	constexpr
	auto operator~(color self) noexcept -> color {
		switch(self) {
			case color::white: return color::black;
			case color::black: return color::white;
			default: internal::unreachable();
		}
	}


	struct pos final {
		int rank, file;

		constexpr
		pos() noexcept =default;
		constexpr
		pos(int rank, int file) noexcept : rank{rank}, file{file} {} //TODO: validation for rank & file

		constexpr
		pos(const char (&str)[3]) {
			if(str[0] >= 'A' && str[0] <= 'H') file = str[0] - 'A';
			else if(str[0] >= 'a' && str[0] <= 'h') file = str[0] - 'a';
			else throw std::invalid_argument{"invalid position"};
			if(str[1] < '1' || str[1] > '8') throw std::invalid_argument{"invalid position"};
			//TODO: assert(str[2] == '\0');
			rank = '8' - str[1];
		}

		friend
		auto operator==(const pos &, const pos &) noexcept -> bool =default;
		friend
		auto operator<=>(const pos &, const pos &) noexcept -> std::strong_ordering =default;

		friend
		auto to_string(pos self) noexcept -> std::string { //relying on SSO for exception specification
			std::string result;
			result += static_cast<char>('A' + self.file);
			result += static_cast<char>('8' - self.rank);
			return result;
		}
	};


	struct move final { pos from, to; };


	class move_valid_result final {
		static
		constexpr
		int max_counts{5};

		int count; //-1 ... false, 0 ... true, without replacement moves, >0 ... true, with replacement moves
		move moves_[max_counts];
	public:
		move_valid_result(bool flag) noexcept : count{flag ? 0 : -1} {}

		template<typename... T>
		requires(sizeof...(T) >= 1 && (std::same_as<std::decay_t<T>, move> &&...))
		move_valid_result(T &&... moves) {
			static_assert(sizeof...(T) <= max_counts);
			count = sizeof...(T);
			int i{0};
			const int hack[]{((moves_[i++] = moves), 0)...};
			(void)hack;
		}

		auto value_or(const move & move) const noexcept -> std::span<const swo3::move> {
			if(count <= 0) return {&move, 1};
			return {moves_, static_cast<std::size_t>(count)};
		}
		void value_or(const move &&) =delete; //prevent binding to temporary

		auto size() const noexcept -> std::size_t {
			if(count == -1) return 0;
			return static_cast<std::size_t>(count);
		}
		auto empty() const noexcept -> bool { return size() == 0; }

		auto begin() const noexcept -> const move * { return moves_; }
		auto end() const noexcept -> const move * { return begin() + size(); }

		explicit
		operator bool() const noexcept { return count >= 0; }
	};


	class chessboard;
	class chesspiece;


	namespace internal {
		template<typename T>
		concept valid_move_with_moved_info = requires(const chessboard & board) {
			{ T::is_valid_move(board, move{}, true) } noexcept -> std::convertible_to<move_valid_result>;
		};

		template<typename T>
		concept valid_move_without_moved_info = requires(const chessboard & board) {
			{ T::is_valid_move(board, move{}) } noexcept -> std::convertible_to<move_valid_result>;
		};

		template<typename T>
		concept promotable = requires {
			{ T::promotion(pos{}) } noexcept -> std::same_as<std::optional<chesspiece>>;
		};
	}


	class chesspiece final { //runtime "type-erased" wrapper
		bool moved_{false}; //only mutating state information needed for any chesspiece
		const struct vtable final { //per-type shared static information (not really a vtable as it turns out that chesspieces are actually stateless...)
			const color & color;
			const glyph & glyph;
			const bool essential;
			move_valid_result(*is_valid_move)(const chessboard &, move, bool) noexcept;
			std::optional<chesspiece>(*promotion)(pos) noexcept;
		} * vptr;
	public:
		template<typename T>
		requires(internal::valid_move_with_moved_info<T> || internal::valid_move_without_moved_info<T>) && requires {
			{ T::color } noexcept -> std::same_as<const color &>;
			{ T::glyph } noexcept -> std::same_as<const glyph &>;
		}
		chesspiece(T &&) noexcept {
			using U = std::decay_t<T>;
			static constexpr vtable vtable {
				U::color,
				U::glyph,
				requires { typename U::essential; },
				+[](const chessboard & board, move move, [[maybe_unused]] bool moved) noexcept -> move_valid_result {
					auto result{[&] {
						if constexpr(internal::valid_move_with_moved_info<U>) return U::is_valid_move(board, move, moved);
						else return U::is_valid_move(board, move);
					}()};
					return result;
				},
				+[]([[maybe_unused]] pos pos) noexcept -> std::optional<chesspiece> {
					if constexpr(internal::promotable<U>) return U::promotion(pos);
					else return std::nullopt;
				}
			};
			vptr = &vtable;
		}

		auto moved() const noexcept -> bool { return moved_; }
		void mark_as_moved() noexcept { moved_ = true; }

		auto color() const noexcept -> color { return vptr->color; }
		auto glyph() const noexcept -> glyph { return vptr->glyph; }
		auto essential() const noexcept -> bool { return vptr->essential; }

		//central validation:
		// * nop moves are never valid
		// * from and to having pieces of the same color is never valid
		// * exposing an essential figure is never valid
		auto is_valid_move(const chessboard & board, move move) const noexcept -> move_valid_result;
		auto valid_moves(chessboard board, pos pos) const -> generator<move_valid_result>;

		void promote(pos pos) noexcept { if(auto tmp{vptr->promotion(pos)}) vptr = tmp->vptr; } //switch "dynamic" type of piece
	};


	class chessboard final {
		std::optional<chesspiece> fields[8][8];
		std::optional<move> last_move_;

		auto test_checkmate(color color) const noexcept -> bool;
		auto test_stalemate_due_to_no_valid_moves(color color) const noexcept -> bool;
	public:
		auto operator[](pos pos) const noexcept -> const std::optional<chesspiece> & { return fields[pos.rank][pos.file]; }
		auto operator[](pos pos)       noexcept ->       std::optional<chesspiece> & { return fields[pos.rank][pos.file]; }

		auto last_move() const noexcept -> const std::optional<move> & { return last_move_; }

		auto move(move move) -> state;

		auto test_in_check(color color) const noexcept -> bool;

		friend
		auto operator<<(std::ostream & os, const chessboard & self) -> std::ostream &;
	};
}
