
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ostream>
#include "chess.hpp"

namespace swo3 {
	auto chessboard::move(swo3::move move) -> state {
		auto & self{*this};
		if(!self[move.from]) throw std::invalid_argument{"no figure at " + to_string(move.from)};
		const auto result{self[move.from]->is_valid_move(self, move)};
		if(!result) throw std::invalid_argument{"move from " + to_string(move.from) + " to " + to_string(move.to) + " is invalid"};

		//actually do the move by means of intermediate moves
		for(const auto & m : result.value_or(move)) {
			self[m.to] = std::exchange(self[m.from], {});
			self[m.to]->mark_as_moved();
		}
		self[move.to]->promote(move.to);

		//record actual input move
		last_move_ = move;

		if(test_checkmate(~self[move.to]->color())) return state::checkmate;
		if(test_stalemate_due_to_no_valid_moves(~self[move.to]->color())) return state::stalemate;

		//TODO: check for stalemate due to not enough material for checkmate
		//TODO: check for stalemate due to 3 repetitions
		//TODO: check for stalemate due to 50 moves without captures or pawn moves

		return state::ongoing;
	}

	auto chessboard::test_checkmate(color color) const noexcept -> bool {
		if(!test_in_check(color)) return false;

		//is there a move that removes check? if not => checkmate!
		for(auto i{0}; i < 8; ++i)
			for(auto j{0}; j < 8; ++j)
				if(const auto & field{fields[i][j]})
					if(field->color() == color)
						for(const auto & result : field->valid_moves(*this, {i, j})) {
							auto copy{*this};
							for(const auto & m : result) copy[m.to] = std::exchange(copy[m.from], {});
							if(!copy.test_in_check(color)) //found move that prevents checkmate
								return false;
						}

		return true;
	}

	auto chessboard::test_stalemate_due_to_no_valid_moves(color color) const noexcept -> bool {
		//is there any move color can make?
		for(auto i{0}; i < 8; ++i)
			for(auto j{0}; j < 8; ++j)
				if(const auto & field{fields[i][j]})
					if(field->color() == color)
						for([[maybe_unused]] const auto & move : field->valid_moves(*this, {i, j})) //NOTE: triggers bogus warning C4702 (as ++it will never be reached once the loop is entered)
							return false; //valid move exists => don't care about specifics, just the existence
		return true;
	}

	auto chessboard::test_in_check(color color) const noexcept -> bool {
		auto enemy_can_move_here{[&](pos essential) {
			for(auto i{0}; i < 8; ++i)
				for(auto j{0}; j < 8; ++j)
					if(const auto & field{fields[i][j]})
						if(field->color() != color)
							if(field->is_valid_move(*this, {{i, j}, essential}))
								return true;
			return false;
		}};

		for(auto i{0}; i < 8; ++i)
			for(auto j{0}; j < 8; ++j)
				if(const auto & field{fields[i][j]})
					if(field->color() == color && field->essential()) //assume multiple essentials are possible and all must be checked
						if(enemy_can_move_here({i, j}))
							return true;
		return false;
	}

	auto operator<<(std::ostream & os, const chessboard & self) -> std::ostream & {
		os << "   |";
		for(auto i{0}; i < 8; ++i) os << ' ' << static_cast<char>('a' + i) << ' ';
		os << "|  \n --+";
		for(auto i{0}; i < 8; ++i) os << "---";
		os << "+--\n";
		for(auto i{0}; i < 8; ++i) {
			os << " " << (8 - i) << " |";
			for(auto j{0}; j < 8; ++j) {
				const auto & field{self.fields[i][j]};
				os << ' ' << (field ? field->glyph() : (i + j) % 2 ? '#' : ' ') << ' ';
			}
			os << "| " << (8 - i) << "\n";
		}

		os << " --+";
		for(auto i{0}; i < 8; ++i) os << "---";
		os << "+--\n   |";
		for(auto i{0}; i < 8; ++i) os << ' ' << static_cast<char>('a' + i) << ' ';
		os << "|  ";
		return os;
	}
}
