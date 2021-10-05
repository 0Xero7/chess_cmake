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

	Bitboard shift_up_right()	{ return ((board << 8) & ~FILE_H) >> 1; }
	Bitboard shift_up_left()	{ return ((board << 8) & ~FILE_A) << 1; }
	Bitboard shift_down_right()	{ return ((board >> 8) & ~FILE_H) >> 1; }
	Bitboard shift_down_left()	{ return ((board >> 8) & ~FILE_A) << 1; }

	Bitboard shift(int dir) {
		switch (dir) {
		case DIR_UP: return shift_up();
		case DIR_DOWN: return shift_down();
		case DIR_LEFT: return shift_left();
		case DIR_RIGHT: return shift_right();
			 
		case DIR_UP_LEFT: return shift_up_left();
		case DIR_UP_RIGHT: return shift_up_right();
		case DIR_DOWN_LEFT: return shift_down_left();
		case DIR_DOWN_RIGHT: return shift_down_right();
		}
	}

	template <int> Bitboard shift();
	template <> Bitboard shift<DIR_UP>()			{ return this->shift_up(); }
	template <> Bitboard shift<DIR_DOWN>()			{ return this->shift_down(); }
	template <> Bitboard shift<DIR_LEFT>()			{ return this->shift_left(); }
	template <> Bitboard shift<DIR_RIGHT>()			{ return this->shift_right(); }
	template <> Bitboard shift<DIR_UP_LEFT>()		{ return this->shift_up_left(); }
	template <> Bitboard shift<DIR_DOWN_LEFT>()		{ return this->shift_down_left(); }
	template <> Bitboard shift<DIR_UP_RIGHT>()		{ return this->shift_up_right(); }
	template <> Bitboard shift<DIR_DOWN_RIGHT>()	{ return this->shift_down_right(); }
	

	bool operator == (const Bitboard& board)	{ return this->board == board.board; }
	
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