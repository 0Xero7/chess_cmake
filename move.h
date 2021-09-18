#pragma once
#include "commons.h"
#include "bitboard.h"
#include <cstdint>

class Move {
public:
	color move_color;
	piece_type piece;
	Bitboard from_square;

	piece_type captured_piece;
	Bitboard to_square;
	bool is_capture;

	Move() {}
	Move(color move_color, piece_type piece, Bitboard from_square, Bitboard to_square,
	  bool is_capture, piece_type captured_piece) :
		move_color(move_color), piece(piece), from_square(from_square), to_square(to_square),
		captured_piece(captured_piece), is_capture(is_capture) { }


};