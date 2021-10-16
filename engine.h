#pragma once

#include <limits>
#include <algorithm>
#include "board.h"
#include <unordered_map>

std::unordered_map<unsigned long long, std::pair<float, Move>> table;

std::pair<float, Move> minimax(Board& board, bool white, int cur_depth, int depth, float alpha, float beta) {
	if (depth == 0) 
		return { board.evaluate(), {} };


	if (white) {
		auto hash = board.get_hash(board.white_move_hash);
		//if (table.count(hash)) return table[hash];

		std::vector<Move> moves;
		board.get_moves<WHITE>(moves);

		if (moves.empty()) {
			auto mask = board.get_occupancy_mask();
			bool in_check = board.is_in_check<WHITE>(mask);

			if (in_check) return  { NEG_INF - depth - 1, {} };
			else return { board.evaluate(), {} };
		}

		std::sort(moves.begin(), moves.end(), [](auto& a, auto& b) {
			return a.is_capture > b.is_capture;
		});
		
		float best = 2 * NEG_INF - 1;
		Move best_move;

		for (auto& move : moves) {
			board.make_move(move);
			auto [score, _] = minimax(board, false, cur_depth + 1, depth - 1, alpha, beta);
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

		if (moves.empty()) {
			auto mask = board.get_occupancy_mask();
			bool in_check = board.is_in_check<BLACK>(mask);

			if (in_check) return  { INF + depth + 1, {} };
			else return { board.evaluate(), {} };
		}

		std::sort(moves.begin(), moves.end(), [](auto& a, auto& b) {
			return a.is_capture > b.is_capture;
		});

		float best = 2 * INF + 1;
		Move best_move;

		for (auto& move : moves) {
			board.make_move(move);
			auto [score, _] = minimax(board, true, cur_depth + 1, depth - 1, alpha, beta);
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