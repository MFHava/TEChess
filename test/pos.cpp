
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <catch.hpp>
#include <chess.hpp>

TEST_CASE("Notation to position", "[pos]") {
	REQUIRE(swo3::pos{"A8"} == swo3::pos{0, 0});
	REQUIRE(swo3::pos{"A7"} == swo3::pos{1, 0});
	REQUIRE(swo3::pos{"B8"} == swo3::pos{0, 1});
	REQUIRE(swo3::pos{"H8"} == swo3::pos{0, 7});
	REQUIRE(swo3::pos{"A1"} == swo3::pos{7, 0});
	REQUIRE(swo3::pos{"H1"} == swo3::pos{7, 7});
	REQUIRE(swo3::pos{"C3"} == swo3::pos{5, 2});
	REQUIRE(swo3::pos{"D2"} == swo3::pos{6, 3});
}
