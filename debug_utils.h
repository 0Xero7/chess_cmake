#pragma once

#include <string>
#include "bitboard.h"
#include <iostream>

namespace DEBUG {
	void show_bitboard(Bitboard& const board) {
		std::cout << "+---+---+---+---+---+---+---+---+\n";
		int idx = 64;
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				if (board.test(--idx)) std::cout << "| X ";
				else std::cout << "|   ";
			}
			std::cout << "|\n+---+---+---+---+---+---+---+---+\n";
		}
	}

	void show_uint64(const uint64_t& board) {
		std::cout << "+---+---+---+---+---+---+---+---+\n";
		int idx = 64;
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				if ((board >> (--idx) & 1) == 1) std::cout << "| X ";
				else std::cout << "|   ";
			}
			std::cout << "|\n+---+---+---+---+---+---+---+---+\n";
		}
	}

	void show_board(Board& const board) {
		std::array<std::string, 12> pieces = { "P", "R", "N", "B", "Q", "K", "p", "r", "n", "b", "q", "k" };
		std::cout << "\n+---+---+---+---+---+---+---+---+\n";
		int idx = 64;
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				piece_type piece = board.get_piece_at(--idx);

				std::string _piece = piece != -1 ? pieces[piece] : " ";
				std::cout << "| " << _piece << " ";
			}
			std::cout << "|\n+---+---+---+---+---+---+---+---+\n";
		}
	}
}