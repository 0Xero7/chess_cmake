#pragma once

#include <cstdint>
#include <intrin.h>
#include "commons.h"

class Bitboard {
private:
	uint64_t board;
public:
	Bitboard() : board(0ull) { }
	Bitboard(uint64_t board) : board(board) { }

	bool test(int index) { return ((board >> index) & 1) == 1; }
	uint64_t get_board() { return board; }
	int get_last_set_bit() { return 63 - __lzcnt64(board); }

	bool is_zero() { return board == 0; }
	int set_bits() { return __popcnt64(board); }

	Bitboard shift_up()			{ return board << 8; }
	Bitboard shift_down()		{ return board >> 8; }
	Bitboard shift_left()		{ return (board & ~FILE_A) << 1; }
	Bitboard shift_right()		{ return (board & ~FILE_H) >> 1; }
								
	Bitboard shift_up_2()		{ return board << 16; }
	Bitboard shift_down_2()		{ return board >> 16; }
	Bitboard shift_left_2()		{ return (board & ~(FILE_A | FILE_B)) << 2; }
	Bitboard shift_right_2()	{ return (board & ~(FILE_G | FILE_H)) >> 2; }

	
	Bitboard operator & (const Bitboard& board) { return Bitboard(board.board & this->board); }
	Bitboard operator | (const Bitboard& board) { return Bitboard(board.board | this->board); }
	Bitboard operator ^ (const Bitboard& board) { return Bitboard(board.board ^ this->board); }
	Bitboard operator >> (const int shift)		{ return Bitboard(this->board >> shift); }
	Bitboard operator << (const int shift)		{ return Bitboard(this->board << shift); }

	Bitboard operator |= (const Bitboard& board){ this->board |= board.board; return *this; }

	Bitboard operator ~ ()						{ return ~this->board; }

	Bitboard& operator ^= (const Bitboard& rhs) {
		this->board ^= rhs.board;
		return *this;
	}
};