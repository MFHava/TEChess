
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "chess.hpp"

namespace swo3 {
	template<color Color>
	struct rook final {
		static
		constexpr
		glyph glyph{Color == color::white ? 'R' : 'r'};

		static
		constexpr
		color color{Color};

		static
		auto is_valid_move(const chessboard & board, move move) noexcept -> bool {
			const auto & to{move.to};
			const auto & from{move.from};
			const auto drank{from.rank - to.rank}, dfile{from.file - to.file};

			if(drank == 0 && dfile != 0) {
				const auto lastFile{dfile < 0 ? to.file : from.file};
				for(auto file{(dfile < 0 ? from.file : to.file) + 1}; file < lastFile; ++file)
					if(board[{from.rank, file}])
						return false;
				return true;
			} else if(dfile == 0 && drank != 0) {
				const auto lastRank{drank < 0 ? to.rank : from.rank};
				for(auto rank{(drank < 0 ? from.rank : to.rank) + 1}; rank < lastRank; ++rank)
					if(board[{rank, from.file}])
						return false;
				return true;
			}
			return false;
		}
	};

	template<color Color>
	struct king final {
		using essential = void;

		static
		constexpr
		glyph glyph{Color == color::white ? 'K' : 'k'};

		static
		constexpr
		color color{Color};

		static
		auto is_valid_move(const chessboard & board, move move, bool moved) noexcept -> move_valid_result { //TODO: logic without hard-coded positions?
			const auto & to{move.to};
			const auto & from{move.from};

			if(!moved && !board[to]) { //logic for castling
				static constexpr pos g{Color == color::white ? "G1" : "G8"};
				static constexpr pos h{Color == color::white ? "H1" : "H8"};
				if(const auto & piece{board[h]}; to == g && piece && piece->glyph() == rook<Color>::glyph && !piece->moved()) {
					static constexpr pos f{Color == color::white ? "F1" : "F8"};
					if(board[f]) return false; //can't move through figure for castling
					return {swo3::move{from, from} /*implicitly validates that king is not in check!*/, swo3::move{from, f}, swo3::move{f, g}, swo3::move{h, f}};
				}
				static constexpr pos c{Color == color::white ? "C1" : "C8"};
				static constexpr pos a{Color == color::white ? "A1" : "A8"};
				if(const auto & piece{board[a]}; to == c && piece && piece->glyph() == rook<Color>::glyph && !piece->moved()) {
					static constexpr pos d{Color == color::white ? "D1" : "D8"};
					static constexpr pos b{Color == color::white ? "B1" : "B8"};
					if(board[d] || board[c] || board[b]) return false; //can't move through figures for castling
					return {swo3::move{from, from} /*implicitly validates that king is not in check!*/, swo3::move{from, d}, swo3::move{d, c}, swo3::move{c, b}, swo3::move{a, d}};
				}
			}

			const auto drank{std::abs(from.rank - to.rank)};
			const auto dfile{std::abs(from.file - to.file)};
			return drank < 2 && dfile < 2 && drank + dfile <= 2;
		}
	};

	template<color Color>
	struct bishop final {
		static
		constexpr
		glyph glyph{Color == color::white ? 'B' : 'b'};

		static
		constexpr
		color color{Color};

		static
		auto is_valid_move(const chessboard & board, move move) noexcept -> bool {
			const auto & to{move.to};
			const auto & from{move.from};
			const auto drank{to.rank - from.rank}, dfile{to.file - from.file};

			if(std::abs(drank) != std::abs(dfile)) return false; //not diagonal

			const auto count{std::abs(drank)};
			for(auto i{1}; i < count; ++i)
				if(board[{from.rank + (i * drank / count), from.file + (i * dfile / count)}])
					return false;
			return true;
		}
	};

	template<color Color>
	struct queen final {
		static
		constexpr
		glyph glyph{Color == color::white ? 'Q' : 'q'};

		static
		constexpr
		color color{Color};

		static
		auto is_valid_move(const chessboard & board, move move) noexcept -> bool { return bishop<Color>::is_valid_move(board, move) || rook<Color>::is_valid_move(board, move); }
	};

	template<color Color>
	struct knight final {
		static
		constexpr
		glyph glyph{Color == color::white ? 'K' : 'k'};

		static
		constexpr
		color color{Color};

		static
		auto is_valid_move(const chessboard &, move move) noexcept -> bool {
			const auto & to{move.to};
			const auto & from{move.from};
			const auto drank{std::abs(from.rank - to.rank)};
			const auto dfile{std::abs(from.file - to.file)};
			return drank < 3 && dfile < 3 && drank + dfile == 3;
		}
	};

	template<color Color>
	struct pawn final {
		static
		constexpr
		glyph glyph{Color == color::white ? 'P' : 'p'};

		static
		constexpr
		color color{Color};

		static
		auto is_valid_move(const chessboard & board, move move, bool moved) noexcept -> move_valid_result {
			const auto & to{move.to};
			const auto & from{move.from};
			static constexpr int step{Color == color::white ? -1 : +1};

			if(!moved && (from.file == to.file && from.rank + 2 * step == to.rank) && !board[{from.rank + step, from.file}] && !board[to]) //move two steps if still unmoved and no field in path is occupied
				return true;

			if(from.file == to.file && from.rank + step == to.rank && !board[to]) //move one step if field not occupied
				return true;

			if((from.file + step == to.file && from.rank + step == to.rank) || (from.file - step == to.file && from.rank + step == to.rank)) {//take opposing piece
				if(const auto & last_move{board.last_move()}) //en passant support
					if(board[last_move->to]->glyph() == pawn<~Color>::glyph && std::abs(last_move->from.rank - last_move->to.rank) == 2) { //enemy pawn moved two spaces
						if(from.rank == last_move->to.rank && to.file == last_move->to.file) //TODO: verify stability of this check...
							return {swo3::move{last_move->to, to}, swo3::move{from, to}};
					}
				return static_cast<bool>(board[to]);
			}

			return false;
		}

		static
		auto promotion(pos pos) noexcept -> std::optional<chesspiece> {
			if constexpr(Color == color::white) {
				if(pos.rank != 0) return std::nullopt;
				return queen<Color>{}; //TODO: actual selection of replacement piece
			} else {
				if(pos.rank != 7) return std::nullopt;
				return queen<Color>{}; //TODO: actual selection of replacement piece
			}
		}
	};
}
