#pragma once

#include <array>
#include <vector>
#include "commons.h"
#include "bitboard.h"
#include "move.h"
#include <unordered_map>
#include <ctype.h>
#include <random>

class Board {
private:
	std::array<std::array<unsigned long long, 12>, 64> zobrist_hashes;

	std::array<Bitboard, 12> pieces;
	std::array<piece_type, 64> piece_on_square;
	Bitboard white_pieces;
	Bitboard black_pieces;

	std::vector<Move> _get_w_pawn_moves(color, piece_type, Bitboard&, Bitboard&, Bitboard&);
	std::vector<Move> _get_b_pawn_moves(color, piece_type, Bitboard&, Bitboard&, Bitboard&);

	void init_zobrist();
public:
	Board() { 
		init_zobrist();
	}

	Board(std::string fen) {
		init_zobrist();

		int idx = 63;
		const std::unordered_map<char, piece_type> notation_to_piece = 
		{
			{ 'P', w_pawn },{ 'R', w_rook },{ 'N', w_knight },{ 'B', w_bishop },{ 'Q', w_queen },{ 'K', w_king },
			{ 'p', b_pawn },{ 'r', b_rook },{ 'n', b_knight },{ 'b', b_bishop },{ 'q', b_queen },{ 'k', b_king },
		};

		for (char c : fen) {
			if (c == '/') continue;
			if (isdigit(c)) idx -= (c - '0');
			else {
				pieces[notation_to_piece.at(c)] |= (1ull << idx);
				--idx;
			}
		}

		for (piece_type w = w_pawn; w <= w_king; ++w) white_pieces = white_pieces | pieces[w];
		for (piece_type b = b_pawn; b <= b_king; ++b) black_pieces = black_pieces | pieces[b];
	}

	piece_type get_piece_at(int);

	unsigned long long get_hash();

	template <int> std::vector<Move> get_moves();

	template <int> std::vector<Move> get_pawn_moves();
	template <> std::vector<Move> get_pawn_moves<WHITE>() {
		return _get_w_pawn_moves(WHITE, w_pawn, pieces[w_pawn], white_pieces, black_pieces);
	}
	template <> std::vector<Move> get_pawn_moves<BLACK>() {
		return _get_b_pawn_moves(BLACK, b_pawn, pieces[b_pawn], black_pieces, white_pieces);
	}



	template <int> std::vector<Move> get_knight_moves();
	template <> std::vector<Move> get_knight_moves<WHITE>() {
		return _get_knight_moves(WHITE, w_knight, pieces[w_knight], white_pieces, black_pieces);
	}
	template <> std::vector<Move> get_knight_moves<BLACK>() {
		return _get_knight_moves(BLACK, b_knight, pieces[b_knight], black_pieces, white_pieces);
	}
	std::vector<Move> _get_knight_moves(color move_color, piece_type piece, Bitboard& const bb, Bitboard& const our, Bitboard& const opp);



	template <int> std::vector<Move> get_king_moves();
	template <> std::vector<Move> get_king_moves<WHITE>() {
		return _get_king_moves(WHITE, w_king, pieces[w_king], white_pieces, black_pieces);
	}
	template <> std::vector<Move> get_king_moves<BLACK>() {
		return _get_king_moves(BLACK, b_king, pieces[b_king], black_pieces, white_pieces);
	}
	std::vector<Move> _get_king_moves(color move_color, piece_type piece, Bitboard& const bb, Bitboard& const our, Bitboard& const opp);

	void make_move(Move&);
	void unmake_move(Move&);

	float evaluate();
	
};