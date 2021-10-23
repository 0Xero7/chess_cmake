#pragma once
#include "commons.h"
#include "bitboard.h"
#include <cstdint>

class Move {
public:
	color move_color;
	piece_type piece;
	//Bitboard from_square;
	int from_index;

	piece_type captured_piece;
	//Bitboard to_square;
	int to_index;
	bool is_capture;

	Move() {}
	Move(color move_color, piece_type piece,// Bitboard from_square, Bitboard to_square,
		int from_index, int to_index, bool is_capture, piece_type captured_piece) :
		move_color(move_color), piece(piece), //from_square(from_square), to_square(to_square),
		from_index(from_index), to_index(to_index), captured_piece(captured_piece), is_capture(is_capture) { }


};