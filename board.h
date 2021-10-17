#pragma once

#include <array>
#include <vector>
#include "commons.h"
#include "bitboard.h"
#include "eval_constants.h"
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

	void _get_w_pawn_moves(color, piece_type, Bitboard&, Bitboard&, Bitboard&, std::vector<Move>&);
	void _get_b_pawn_moves(color, piece_type, Bitboard&, Bitboard&, Bitboard&, std::vector<Move>&);

	std::array<std::array<uint64_t, 16384>, 64> rook_attacks;
	std::array<uint64_t, 64> rook_magics;
	std::array<uint64_t, 64> rook_magic_masks;
	void init_rook_magics(); 

	std::array<std::array<uint64_t, 2048>, 64> bishop_attacks;
	std::array<uint64_t, 64> bishop_magics;
	std::array<uint64_t, 64> bishop_magic_masks;
	void init_bishop_magics();

	uint64_t Board::get_king_attack_mask(int);

	void init_zobrist();
public:
	Board() { 
		init_zobrist();
		init_rook_magics();
		printf("rook magics done\n");
		init_bishop_magics();
		printf("bishop magics done\n");
	}

	Board(std::string fen) {
		init_zobrist();
		init_rook_magics();
		printf("rook magics done\n");
		init_bishop_magics();
		printf("bishop magics done\n");

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
	uint64_t Board::get_knight_attack_mask(int);

	uint64_t white_move_hash, black_move_hash;

	piece_type get_piece_at(int);
	uint64_t get_occupancy_mask();

	unsigned long long get_hash(uint64_t);

	template <int> void get_moves(std::vector<Move>&);
	uint64_t Board::get_rook_attacks_mask(int, uint64_t& const);
	uint64_t Board::get_bishop_attacks_mask(int, uint64_t& const);

	template <int> bool is_in_check(uint64_t& const);
	template <> bool is_in_check<WHITE>(uint64_t& const occupancy) {
		auto king = pieces[w_king].get_board();
		if (king == 0) return false;

		auto idx = 63 - __lzcnt64(king);
		auto rook_queen = (pieces[b_rook] | pieces[b_queen]).get_board();
		auto bishop_queen = (pieces[b_bishop] | pieces[b_queen]).get_board();
		auto knight = (pieces[b_knight]).get_board();

		auto rook_attacks = get_rook_attacks_mask(idx, occupancy);
		auto bishop_attacks = get_bishop_attacks_mask(idx, occupancy);
		auto king_attacks = get_king_attack_mask(idx);
		auto knight_attacks = get_knight_attack_mask(idx);

		return ((rook_queen & rook_attacks) | (bishop_queen & bishop_attacks)) 
			| ((king_attacks & pieces[b_king].get_board())) | (knight & knight_attacks);
	}

	template <> bool is_in_check<BLACK>(uint64_t& const occupancy) {
		auto king = pieces[b_king].get_board();
		if (king == 0) return false;

		auto idx = 63 - __lzcnt64(king);
		auto rook_queen = (pieces[w_rook] | pieces[w_queen]).get_board();
		auto bishop_queen = (pieces[w_bishop] | pieces[w_queen]).get_board();
		auto knight = (pieces[w_knight]).get_board();

		auto rook_attacks = get_rook_attacks_mask(idx, occupancy);
		auto bishop_attacks = get_bishop_attacks_mask(idx, occupancy);
		auto king_attacks = get_king_attack_mask(idx);
		auto knight_attacks = get_knight_attack_mask(idx);

		return ((rook_queen & rook_attacks) | (bishop_queen & bishop_attacks))
			| ((king_attacks & pieces[w_king].get_board())) | (knight & knight_attacks);
	}

	template <int> void get_pawn_moves(std::vector<Move>&);
	template <> void get_pawn_moves<WHITE>(std::vector<Move>& moves) {
		_get_w_pawn_moves(WHITE, w_pawn, pieces[w_pawn], white_pieces, black_pieces, moves);
	}
	template <> void get_pawn_moves<BLACK>(std::vector<Move>& moves) {
		_get_b_pawn_moves(BLACK, b_pawn, pieces[b_pawn], black_pieces, white_pieces, moves);
	}



	template <int> void get_knight_moves(std::vector<Move>&);
	template <> void get_knight_moves<WHITE>(std::vector<Move>& moves) {
		_get_knight_moves(WHITE, w_knight, pieces[w_knight], white_pieces, black_pieces, moves);
	}
	template <> void get_knight_moves<BLACK>(std::vector<Move>& moves) {
		_get_knight_moves(BLACK, b_knight, pieces[b_knight], black_pieces, white_pieces, moves);
	}
	void _get_knight_moves(color move_color, piece_type piece, Bitboard& const bb, Bitboard& const our, Bitboard& const opp, std::vector<Move>& moves);



	template <int> void get_rook_moves(std::vector<Move>&);
	template <> void get_rook_moves<WHITE>(std::vector<Move>& moves) {
		_get_rook_moves(WHITE, w_rook, pieces[w_rook], white_pieces, black_pieces, moves);
	}
	template <> void get_rook_moves<BLACK>(std::vector<Move>& moves) {
		_get_rook_moves(BLACK, b_rook, pieces[b_rook], black_pieces, white_pieces, moves);
	}
	void _get_rook_moves(color move_color, piece_type piece, Bitboard& const bb, Bitboard& const our, Bitboard& const opp, std::vector<Move>& moves);


	template <int> void get_bishop_moves(std::vector<Move>&);
	template <> void get_bishop_moves<WHITE>(std::vector<Move>& moves) {
		_get_bishop_moves(WHITE, w_bishop, pieces[w_bishop], white_pieces, black_pieces, moves);
	}
	template <> void get_bishop_moves<BLACK>(std::vector<Move>& moves) {
		_get_bishop_moves(BLACK, b_bishop, pieces[b_bishop], black_pieces, white_pieces, moves);
	}
	void _get_bishop_moves(color move_color, piece_type piece, Bitboard& const bb, Bitboard& const our, Bitboard& const opp, std::vector<Move>& moves);
	

	template <int> void get_queen_moves(std::vector<Move>&);
	template <> void get_queen_moves<WHITE>(std::vector<Move>& moves) {
		_get_queen_moves(WHITE, w_queen, pieces[w_queen], white_pieces, black_pieces, moves);
	}
	template <> void get_queen_moves<BLACK>(std::vector<Move>& moves) {
		_get_queen_moves(BLACK, b_queen, pieces[b_queen], black_pieces, white_pieces, moves);
	}
	void _get_queen_moves(color move_color, piece_type piece, Bitboard& const bb, Bitboard& const our, Bitboard& const opp, std::vector<Move>& moves);



	template <int> void get_king_moves(std::vector<Move>&);
	template <> void get_king_moves<WHITE>(std::vector<Move>& moves) {
		_get_king_moves(WHITE, w_king, pieces[w_king], white_pieces, black_pieces, moves);
	}
	template <> void get_king_moves<BLACK>(std::vector<Move>& moves) {
		_get_king_moves(BLACK, b_king, pieces[b_king], black_pieces, white_pieces, moves);
	}
	void _get_king_moves(color move_color, piece_type piece, Bitboard& const bb, Bitboard& const our, Bitboard& const opp, std::vector<Move>& moves);

	void make_move(Move&);
	void unmake_move(Move&);

	float evaluate();
	
};