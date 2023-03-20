
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <chesspieces.hpp>
#include "util.hpp"

TEST_CASE("Moving bishop", "[bishop] [move]") {
	swo3::chessboard b;
	const swo3::pos from{"C3"};
	b[from] = swo3::bishop<swo3::color::white>{};

	test::check_valid_endpositions(b, from, "A1", "B2", "D4", "E5", "F6", "G7", "H8", "A5", "B4", "D2", "E1");

	b["E5"] = swo3::pawn<swo3::color::black>{}; //block F6, G7, and H8
	test::check_valid_endpositions(b, from, "A1", "B2", "D4", "E5", "A5", "B4", "D2", "E1");

	b["B4"] = swo3::pawn<swo3::color::white>{}; //block B4, and A5
	test::check_valid_endpositions(b, from, "A1", "B2", "D4", "E5", "D2", "E1");
}

TEST_CASE("Moving rook", "[rook] [move]") {
	swo3::chessboard b;
	const swo3::pos from{"D4"};
	b[from] = swo3::rook<swo3::color::white>{};

	test::check_valid_endpositions(b, from, "C4", "B4", "A4", "E4", "F4", "G4", "H4", "D5", "D6", "D7", "D8", "D3", "D2", "D1");

	b["E4"] = swo3::pawn<swo3::color::black>{}; //blocking F4, G4, and H4
	test::check_valid_endpositions(b, from, "C4", "B4", "A4", "E4", "D5", "D6", "D7", "D8", "D3", "D2", "D1");

	b["C4"] = swo3::pawn<swo3::color::white>{}; //blocking C4, B4, and A4
	test::check_valid_endpositions(b, from, "E4", "D5", "D6", "D7", "D8", "D3", "D2", "D1");
}

TEST_CASE("Moving knight", "[knight] [move]") {
	swo3::chessboard b;
	const swo3::pos from{"D4"};
	b[from] = swo3::knight<swo3::color::white>{};

	test::check_valid_endpositions(b, from, "E6", "F5", "F3", "E2", "C2", "B3", "B5", "C6");
	b["C4"] = b["C5"] = b["D5"] = swo3::pawn<swo3::color::black>{}; //jump over enemy pieces
	test::check_valid_endpositions(b, from, "E6", "F5", "F3", "E2", "C2", "B3", "B5", "C6");
	b["E4"] = b["E3"] = b["D3"] = swo3::pawn<swo3::color::white>{}; //jump over own pieces
	test::check_valid_endpositions(b, from, "E6", "F5", "F3", "E2", "C2", "B3", "B5", "C6");
	b["E6"] = swo3::pawn<swo3::color::black>{}; //piece to take
	test::check_valid_endpositions(b, from, "E6", "F5", "F3", "E2", "C2", "B3", "B5", "C6");
	b["F5"] = swo3::pawn<swo3::color::white>{}; //own piece blocking
	test::check_valid_endpositions(b, from, "E6", "F3", "E2", "C2", "B3", "B5", "C6");
}

TEST_CASE("Moving pawn", "[pawn] [move]") {
	swo3::chessboard b;
	const swo3::pos from{"D2"};
	b[from] = swo3::pawn<swo3::color::white>{};

	test::check_valid_endpositions(b, from, "D3", "D4");
	b[from]->mark_as_moved(); //prevent initial double move
	test::check_valid_endpositions(b, from, "D3");
	b["E3"] = b["C3"] = swo3::pawn<swo3::color::black>{}; //taking enemy piece
	test::check_valid_endpositions(b, from, "C3", "D3", "E3");
	b["E3"] = swo3::pawn<swo3::color::white>{}; //can't take own piece
	test::check_valid_endpositions(b, from, "C3", "D3");
}

TEST_CASE("En passant pawn", "[pawn] [move]") {
	swo3::chessboard b;
	const swo3::pos from{"B5"};
	b[from] = swo3::pawn<swo3::color::white>{};
	b[from]->mark_as_moved();
	b["C7"] = swo3::pawn<swo3::color::black>{};
	b.move({"C7", "C5"});
	REQUIRE(b[from]->is_valid_move(b, {from, "B6"}));
	REQUIRE(b[from]->is_valid_move(b, {from, "C6"}));
}
//TODO: how to test promotion? (current logic is fully internal to chesspiece)


TEST_CASE("Moving king", "[king] [move]") {
	swo3::chessboard b;
	const swo3::pos from{"D4"};
	b[from] = swo3::king<swo3::color::white>{};

	test::check_valid_endpositions(b, from, "D5", "E5", "E4", "E3", "D3", "C3", "C4", "C5");
	b["E6"] = swo3::rook<swo3::color::black>{}; //block E5, E4, and E3
	test::check_valid_endpositions(b, from, "D5", "D3", "C3", "C4", "C5");
	b["B2"] = swo3::knight<swo3::color::black>{}; //block C4, and D3
	test::check_valid_endpositions(b, from, "D5", "C3", "C5");
}

TEST_CASE("Casteling king", "[king] [move]") {
	swo3::chessboard b;
	const swo3::pos from{"E1"};
	b[from] = swo3::king<swo3::color::white>{};

	b["H1"] = b["A1"] = swo3::rook<swo3::color::white>{};
	REQUIRE(b[from]->is_valid_move(b, {from, "G1"}));
	REQUIRE(b[from]->is_valid_move(b, {from, "C1"}));

	b["E8"] = swo3::queen<swo3::color::black>{}; //prevent all casteling
	REQUIRE(!b[from]->is_valid_move(b, {from, "G1"}));
	REQUIRE(!b[from]->is_valid_move(b, {from, "C1"}));
	b.move({"E8", "D8"}); //allow short casteling
	REQUIRE(b[from]->is_valid_move(b, {from, "G1"}));
	REQUIRE(!b[from]->is_valid_move(b, {from, "C1"}));
	b["H1"]->mark_as_moved(); //prevent casteling as rook already moved
	REQUIRE(!b[from]->is_valid_move(b, {from, "G1"}));
	REQUIRE(!b[from]->is_valid_move(b, {from, "C1"}));
	b.move({"D8", "F8"}); //allow long casteling
	REQUIRE(!b[from]->is_valid_move(b, {from, "G1"}));
	REQUIRE(b[from]->is_valid_move(b, {from, "C1"}));
	b[from]->mark_as_moved(); //prevent casteling as king already moved
	REQUIRE(!b[from]->is_valid_move(b, {from, "G1"}));
	REQUIRE(!b[from]->is_valid_move(b, {from, "C1"}));
}
