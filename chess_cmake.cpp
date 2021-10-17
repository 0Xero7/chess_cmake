// chess_cmake.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "engine.h"
#include "debug_utils.h"
#include <unordered_set>
#include <random>
#include <bitset>

using namespace std;



int main()
{
	//bishop_masks();
	//getchar();

	Board board("7k/7P/6P1/8/6RR/8/3K4/8");// 7k / 8 / R7 / R7 / 8 / 5K2 / 8 / 8");// "6k1 / 4Q3 / 8 / 6K1 / 8 / 8 / 8 / 8");// "rnbqkbnr / pppppppp / 8 / 8 / 8 / 8 / PPPPPPPP / RNBQKBNR");// "5k2 / 8 / 8 / 8 / 2n5 / 8 / 2K5 / n7"); // ("4k3 / 8 / 8 / 8 / nnn5 / nKn5 / 8 / 8");
	
	auto x = board.get_occupancy_mask();

	auto xx = board.get_rook_attacks_mask(4, x);
	auto yy = board.get_bishop_attacks_mask(4, x);
	DEBUG::show_uint64(xx | yy);


	std::vector<Move> moves;
	board.get_queen_moves<WHITE>(moves);

	DEBUG::show_board(board);
	getchar();

	bool white = 1;
	for (int i = 0; i < 100; ++i) {
		table.clear();
		table.reserve(1024ll * 1024ll * 100ll);

		auto [score, move] = minimax(board, white, 0, 4, 2 * NEG_INF - 1, 2 * INF + 1);
		if (move.to_square.is_zero()) break;
		system("cls");
		board.make_move(move);
		DEBUG::show_board(board);

		if (score > INF) {
			printf("M%d\n", (int)(score - INF));
		}
		else {
			printf("%f\n", score);
		}
		white = !white;

		getchar();
	}
}
