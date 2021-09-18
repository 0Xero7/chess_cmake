// chess_cmake.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "engine.h"
#include "debug_utils.h"

using namespace std;

int main()
{
	Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");// "5k2/8/8/8/2n5/8/2K5/n7"); // ("4k3/8/8/8/nnn5/nKn5/8/8");
	DEBUG::show_board(board);
	getchar();

	bool white = 1;
	for (int i = 0; i < 100; ++i) {
		table.clear();
		table.reserve(1024ll * 1024ll * 100ll);

		auto [score, move] = minimax(board, white, 8, -100000000, 100000000);
		if (move.to_square.is_zero()) break;
		system("cls");
		board.make_move(move);
		DEBUG::show_board(board);
		printf("%f\n", score);
		white = !white;

		getchar();
	}
}
