
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <vector>
#include <iterator>
#include <algorithm>
#include <chess.hpp>
#include <catch.hpp>

namespace test {
	template<std::convertible_to<swo3::pos>... Positions>
	void check_valid_endpositions(const swo3::chessboard & b, swo3::pos from, Positions &&... positions) {
		std::array<swo3::pos, sizeof...(positions)> expected{std::forward<Positions>(positions)...};
		std::ranges::sort(expected);

		std::vector<swo3::pos> actual;
		std::ranges::transform(b[from]->valid_moves(b, from), std::back_inserter(actual), [](auto moves) {
			REQUIRE(moves.size() == 1); //TODO: will not hold for pawn and king
			return moves.begin()->to;
		});
		std::ranges::sort(actual);

		REQUIRE(std::ranges::equal(actual, expected));
	}
}
