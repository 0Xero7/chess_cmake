#pragma once

#include <limits>
#include <algorithm>
#include "board.h"
#include <unordered_map>

std::unordered_map<unsigned long long, std::pair<float, Move>> table;

std::pair<float, Move> minimax(Board& board, bool white, int depth, float alpha, float beta) {
	if (depth == 0) 
		return { board.evaluate(), {} };


	if (white) {
		auto hash = board.get_hash(board.white_move_hash);
		//if (table.count(hash)) return table[hash];

		std::vector<Move> moves;
		board.get_moves<WHITE>(moves);

		if (moves.empty()) return { board.evaluate(), {} };

		std::sort(moves.begin(), moves.end(), [](auto& a, auto& b) {
			return a.is_capture > b.is_capture;
		});
		
		float best = NEG_INF;
		Move best_move;

		for (auto& move : moves) {
			board.make_move(move);
			auto [score, _] = minimax(board, false, depth - 1, alpha, beta);
			board.unmake_move(move);

			alpha = std::max(alpha, score);
			//if (score >= beta) break;

			if (score > best) {
				best = score;
				best_move = move;
			}
		}

		return table[hash] = { best, best_move };
	}
	else {
		auto hash = board.get_hash(board.black_move_hash);
		//if (table.count(hash)) return table[hash];

		std::vector<Move> moves;
		board.get_moves<BLACK>(moves);

		if (moves.empty()) return { board.evaluate(), {} };

		std::sort(moves.begin(), moves.end(), [](auto& a, auto& b) {
			return a.is_capture > b.is_capture;
		});

		float best = INF;
		Move best_move;

		for (auto& move : moves) {
			board.make_move(move);
			auto [score, _] = minimax(board, true, depth - 1, alpha, beta);
			board.unmake_move(move);

			beta = std::min(beta, score);
			//if (score <= alpha) break;

			if (score < best) {
				best = score;
				best_move = move;
			}
		}

		return table[hash] = { best, best_move };
	}
}