// chess_cmake.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "engine.h"
#include "debug_utils.h"
#include <unordered_set>
#include <random>
#include <bitset>
#include "bench.h"
#include <chrono>
#include <assert.h>

using namespace std;



int main()
{
	//bishop_masks();
	//getchar();

	//Board rboard("1bk5/2b5/7b/4B3/5b1B/B2K4/3B4/8"); // "1r6/2r2k2/7r/4R3/3K1r2/8/R2R2R1/8"); // 1r5k / pp5p / 2p3p1 / 5b2 / 2q5 / P3Q3 / 1PP4P / 2K1R3");// 7k / 8 / R7 / R7 / 8 / 5K2 / 8 / 8");// "6k1 / 4Q3 / 8 / 6K1 / 8 / 8 / 8 / 8");// "rnbqkbnr / pppppppp / 8 / 8 / 8 / 8 / PPPPPPPP / RNBQKBNR");// "5k2 / 8 / 8 / 8 / 2n5 / 8 / 2K5 / n7"); // ("4k3 / 8 / 8 / 8 / nnn5 / nKn5 / 8 / 8");
	Board rboard("7k/7P/8/3pK3/8/8/8/8");
	//Board rboard("1r6/2r2k2/1R5r/3KR3/5r1R/2R5/R5R1/5R2");
	//Board rboard("k1b5/8/8/4R3/8/2K5/8/8");
	//Board rboard("k7/8/8/8/r1R4K/8/8/8");
	//auto occr = rboard.get_occupancy_mask();

	//Board bboard("1b6/2b2k2/1B5b/3KB3/5b1B/2B5/B5B1/5B2");

	//Board board("1bk5/2b5/7b/4B3/5b1B/B2K4/3B4/8");
	//auto occb = bboard.get_occupancy_mask();

	std::vector<Move> collect;
	auto ms1 = 0ll;
	for (int i = 0; i <= 10000000; ++i) {
		collect.clear();
		collect.reserve(128);

		auto bench = Benchmark();

		rboard.get_king_moves<BLACK>(collect);

		ms1 += bench.end_benchmark(false);
	}

	std::cout << ms1 / 1000 << "us\n";

	std::vector<Move> collect2;
	auto ms2 = 0ll;
	for (int i = 0; i <= 10000000; ++i) {
		collect2.clear();
		collect2.reserve(128);
		auto bench = Benchmark();
		/*bboard.get_bishop_moves<WHITE>(collect);*/

		rboard.get_fast_king_moves<BLACK>(collect2);

		ms2 += bench.end_benchmark(false);
	}

	std::cout << ms2 / 1000 << "us\n";
	std::cout << (ms1 * 100) / ms2 << "% faster\n";
	std::cout << "\nMoves: " << collect.size() << " " << collect2.size() << "\n";
	assert(collect.size() == collect2.size());
	getchar();

	/*auto x = board.get_occupancy_mask();

	auto xx = board.get_rook_attacks_mask(4, x);
	auto yy = board.get_bishop_attacks_mask(4, x);
	DEBUG::show_uint64(xx | yy);*/


	//std::vector<Move> moves;
	////board.get_queen_moves<WHITE>(moves);

	//DEBUG::show_board(board);
	//getchar();

	//bool white = 1;
	//for (int i = 0; i < 100; ++i) {
	//	table.clear();
	//	table.reserve(1024ll * 1024ll * 100ll);


	//	// BENCHMARK
	//	auto start = chrono::high_resolution_clock::now();

	//	auto [score, move] = minimax(board, white, 0, 6, 2 * NEG_INF - 1, 2 * INF + 1);

	//	// before optimization : 1400ms
	//	// optimize get_occupancy_mask : 1250ms

	//	auto end = chrono::high_resolution_clock::now();

	//	auto duration = chrono::duration_cast<chrono::microseconds>(end - start).count();

	//	//if (move.to_square.is_zero()) break;
	//	system("cls");
	//	board.make_move(move);
	//	DEBUG::show_board(board);

	//	if (score > INF) {
	//		printf("M%d\n", (int)(score - INF));
	//	}
	//	else {
	//		printf("%f\n", score);
	//	}
	//	white = !white;

	//	std::cout << "Took " << duration << "us\n";
	//	getchar();
	//}
}
