
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <iostream>
#include <chesspieces.hpp>

auto initial_board() -> swo3::chessboard {
	swo3::chessboard b;

	b["A8"] = swo3::rook<swo3::color::black>{};
	b["B8"] = swo3::knight<swo3::color::black>{};
	b["C8"] = swo3::bishop<swo3::color::black>{};
	b["D8"] = swo3::queen<swo3::color::black>{};
	b["E8"] = swo3::king<swo3::color::black>{};
	b["F8"] = swo3::bishop<swo3::color::black>{};
	b["G8"] = swo3::knight<swo3::color::black>{};
	b["H8"] = swo3::rook<swo3::color::black>{};
	for(auto i{0}; i < 8; ++i) {
		b[{1, i}] = swo3::pawn<swo3::color::black>{};
		b[{6, i}] = swo3::pawn<swo3::color::white>{};
	}
	b["A1"] = swo3::rook<swo3::color::white>{};
	b["B1"] = swo3::knight<swo3::color::white>{};
	b["C1"] = swo3::bishop<swo3::color::white>{};
	b["D1"] = swo3::queen<swo3::color::white>{};
	b["E1"] = swo3::king<swo3::color::white>{};
	b["F1"] = swo3::bishop<swo3::color::white>{};
	b["G1"] = swo3::knight<swo3::color::white>{};
	b["H1"] = swo3::rook<swo3::color::white>{};

	return b;
}

int main() {
	auto b{initial_board()};

	//TODO: proof of concept parser...
	std::cout << b << "\nenter move: ";
	for(std::string input; std::getline(std::cin, input);) try {
		if(input.size() != 4) throw std::invalid_argument{"unknown move notation"};
		char from[3], to[3];
		from[2] = to[2] = 0;
		from[0] = input[0];
		from[1] = input[1];
		to[0] = input[2];
		to[1] = input[3];

		switch(b.move(swo3::move{from, to})) {
			case swo3::state::checkmate:
				std::cout << "CHECKMATE!\n";
				goto end;
			case swo3::state::stalemate:
				std::cout << "STALEMATE!\n";
				goto end;
			default: break;
		}
		std::cout << b << "\nenter move: ";
	} catch(const std::exception & exc) {
		std::cerr << "ERR: " << exc.what() << "\n";
		std::cout << b << "\nenter move: ";
	}
end:
	(void)0; //TODO: [C++23] C++23 fixes this language oddity
}
