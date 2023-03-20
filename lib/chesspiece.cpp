
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "chess.hpp"

namespace swo3 {
	auto chesspiece::is_valid_move(const chessboard & board, move move) const noexcept -> move_valid_result {
		if(move.from == move.to) return false; //nop is never a valid move

		//can never land on field with piece of same color
		if(const auto & piece{board[move.to]})
			if(piece->color() == vptr->color)
				return false;

		auto result{vptr->is_valid_move(board, move, moved_)};
		if(!result) return false;

		//check that essential figure of same color does not get exposed by this move sequence
		for(auto tmp{board}; const auto & m : result.value_or(move)) {
			tmp[m.to] = std::exchange(tmp[m.from], {});
			if(tmp.test_in_check(vptr->color)) return false;
		}

		return result;
	}

	auto chesspiece::valid_moves(chessboard board, pos pos) const -> generator<move_valid_result> {
		for(int i{0}; i < 8; ++i)
			for(int j{0}; j < 8; ++j) {
				const move m{pos, {i, j}};
				if(auto tmp{is_valid_move(board, m)})
					if(tmp.empty()) co_yield m;
					else co_yield tmp;
			}
	}
}
