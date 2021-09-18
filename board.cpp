
#include "board.h"

namespace Helpers {
	void push_back_move_if_valid(Move&& item, std::vector<Move>& container) {
		if (!item.to_square.is_zero()) container.push_back(item);
	}

	void generate_and_add_move(color move_color, Board& board, Bitboard& from_square, Bitboard&& to_square, piece_type piece,
								Bitboard& const our, Bitboard& const opp, std::vector<Move>& container) {

		if ((to_square & ~our).is_zero()) return;
		
		bool is_captures = !(to_square & opp).is_zero();
		int idx = to_square.get_last_set_bit();
		piece_type captured_piece = is_captures ? board.get_piece_at(idx) : nothing;

		container.push_back(Move(move_color, piece, from_square, to_square, is_captures, captured_piece));
	}

	void generate_and_add_move(color move_color, Board& board, Bitboard& from_square, Bitboard& to_square, piece_type piece,
								Bitboard& const our, Bitboard& const opp, std::vector<Move>& container) {

		if ((to_square & ~our).is_zero()) return;
		
		bool is_captures = !(to_square & opp).is_zero();
		int idx = to_square.get_last_set_bit();
		piece_type captured_piece = is_captures ? board.get_piece_at(idx) : nothing;

		container.push_back(Move(move_color, piece, from_square, to_square, is_captures, captured_piece));
	}
}

void Board::init_zobrist() {
	std::random_device rd;
	std::mt19937 gen(rd()); 
	std::uniform_int_distribution<unsigned long long> distrib(0ull, ULLONG_MAX);

	for (int i = 0; i < 64; ++i)
		for (piece_type j = w_pawn; j <= b_king; ++j)
			zobrist_hashes[i][j] = distrib(gen);
}

piece_type Board::get_piece_at(int square) {
	for (piece_type piece = w_pawn; piece <= b_king; ++piece) {
		if (this->pieces[piece].test(square))
			return piece;
	}
	return nothing;
}

std::vector<Move> Board::_get_knight_moves(color move_color, piece_type piece, Bitboard& const bb, Bitboard& const our, Bitboard& const opp)
{
	if (bb.is_zero()) return {};

	std::vector<Move> moves;

	uint64_t board = bb.get_board();
	while (board > 0) {
		int idx = 63 - __lzcnt64(board);
		uint64_t _mask = (1ull << idx);
		Bitboard mask = Bitboard(_mask);

		auto UP = mask.shift_up_2();
		Helpers::generate_and_add_move(move_color, *this, mask, UP.shift_left(), piece, our, opp, moves);
		Helpers::generate_and_add_move(move_color, *this, mask, UP.shift_right(), piece, our, opp, moves);

		auto DOWN = mask.shift_down_2();
		Helpers::generate_and_add_move(move_color, *this, mask, DOWN.shift_left(), piece, our, opp, moves);
		Helpers::generate_and_add_move(move_color, *this, mask, DOWN.shift_right(), piece, our, opp, moves);

		auto LEFT = mask.shift_left_2();
		Helpers::generate_and_add_move(move_color, *this, mask, LEFT.shift_up(), piece, our, opp, moves);
		Helpers::generate_and_add_move(move_color, *this, mask, LEFT.shift_down(), piece, our, opp, moves);

		auto RIGHT = mask.shift_right_2();
		Helpers::generate_and_add_move(move_color, *this, mask, RIGHT.shift_up(), piece, our, opp, moves);
		Helpers::generate_and_add_move(move_color, *this, mask, RIGHT.shift_down(), piece, our, opp, moves);

		board ^= _mask;
	}

	return moves;
}

std::vector<Move> Board::_get_king_moves(color move_color, piece_type piece, Bitboard& const bb, Bitboard& const our, Bitboard& const opp)
{
	if (bb.is_zero()) return {};

	std::vector<Move> moves;

	uint64_t board = bb.get_board();
	while (board > 0) {
		int idx = 63 - __lzcnt64(board);
		uint64_t _mask = (1ull << idx);
		Bitboard mask = Bitboard(_mask);

		Helpers::generate_and_add_move(move_color, *this, mask, mask.shift_left(), piece, our, opp, moves);
		Helpers::generate_and_add_move(move_color, *this, mask, mask.shift_right(), piece, our, opp, moves);
		Helpers::generate_and_add_move(move_color, *this, mask, mask.shift_up(), piece, our, opp, moves);
		Helpers::generate_and_add_move(move_color, *this, mask, mask.shift_down(), piece, our, opp, moves);

		Helpers::generate_and_add_move(move_color, *this, mask, mask.shift_left().shift_up(), piece, our, opp, moves);
		Helpers::generate_and_add_move(move_color, *this, mask, mask.shift_right().shift_up(), piece, our, opp, moves);
		Helpers::generate_and_add_move(move_color, *this, mask, mask.shift_left().shift_down(), piece, our, opp, moves);
		Helpers::generate_and_add_move(move_color, *this, mask, mask.shift_right().shift_down(), piece, our, opp, moves);

		board ^= _mask;
	}

	return moves;
}


template <> std::vector<Move> Board::get_moves<WHITE>() {
	auto king = get_king_moves<WHITE>();
	auto knight = get_knight_moves<WHITE>();
	auto pawn = get_pawn_moves<WHITE>();
	auto ret = king;
	for (auto& move : knight) ret.push_back(move);
	for (auto& move : pawn) ret.push_back(move);
	
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(ret.begin(), ret.end(), g);
	return ret;
}

template <> std::vector<Move> Board::get_moves<BLACK>() {
	auto king = get_king_moves<BLACK>();
	auto knight = get_knight_moves<BLACK>();
	auto pawn = get_pawn_moves<BLACK>();
	auto ret = king;
	for (auto& move : knight) ret.push_back(move);
	for (auto& move : pawn) ret.push_back(move);

	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(ret.begin(), ret.end(), g);
	return ret;
}


std::vector<Move> Board::_get_w_pawn_moves(color move_color, piece_type piece, Bitboard& const bb, Bitboard& const our, Bitboard& const opp) 
{
	if (bb.is_zero()) return {};

	std::vector<Move> moves;

	uint64_t board = bb.get_board();
	while (board > 0) {
		int idx = 63 - __lzcnt64(board);
		uint64_t _mask = (1ull << idx);
		Bitboard mask = Bitboard(_mask);

		auto up = mask.shift_up() & ~(our | opp);
		Helpers::generate_and_add_move(WHITE, *this, mask, up, w_pawn, our, opp, moves);

		if (!(mask & RANK_2).is_zero()) {
			auto up2 = up.shift_up() & ~(our | opp);
			Helpers::generate_and_add_move(WHITE, *this, mask, up2, w_pawn, our, opp, moves);
		}

		auto cap_left = mask.shift_up().shift_left() & opp;
		Helpers::generate_and_add_move(WHITE, *this, mask, cap_left, w_pawn, our, opp, moves);
		auto cap_right = mask.shift_up().shift_right() & opp;
		Helpers::generate_and_add_move(WHITE, *this, mask, cap_right, w_pawn, our, opp, moves);

		board ^= _mask;
	}

	return moves;
}

std::vector<Move> Board::_get_b_pawn_moves(color move_color, piece_type piece, Bitboard& const bb, Bitboard& const our, Bitboard& const opp) 
{
	if (bb.is_zero()) return {};

	std::vector<Move> moves;

	uint64_t board = bb.get_board();
	while (board > 0) {
		int idx = 63 - __lzcnt64(board);
		uint64_t _mask = (1ull << idx);
		Bitboard mask = Bitboard(_mask);

		auto down = mask.shift_down() & ~(our | opp);
		Helpers::generate_and_add_move(BLACK, *this, mask, down, b_pawn, our, opp, moves);

		if (!(mask & RANK_7).is_zero()) {
			auto down2 = down.shift_down() & ~(our | opp);
			Helpers::generate_and_add_move(BLACK, *this, mask, down2, b_pawn, our, opp, moves);
		}

		auto cap_left = mask.shift_down().shift_left() & opp;
		Helpers::generate_and_add_move(BLACK, *this, mask, cap_left, b_pawn, our, opp, moves);
		auto cap_right = mask.shift_down().shift_right() & opp;
		Helpers::generate_and_add_move(BLACK, *this, mask, cap_right, b_pawn, our, opp, moves);

		board ^= _mask;
	}

	return moves;
}


void Board::make_move(Move& move) {
	pieces[move.piece] ^= (move.from_square | move.to_square);
	if (move.move_color == WHITE) {
		white_pieces ^= (move.from_square | move.to_square);
		if (move.is_capture)
			black_pieces = black_pieces & ~move.to_square;
	}
	else {
		black_pieces ^= (move.from_square | move.to_square);
		if (move.is_capture)
			white_pieces = white_pieces & ~move.to_square;
	}

	if (move.is_capture) {
		pieces[move.captured_piece] ^= move.to_square;
	}
}

void Board::unmake_move(Move& move) {
	pieces[move.piece] ^= (move.from_square | move.to_square);
	if (move.is_capture) {
		if (move.move_color == 0) black_pieces = black_pieces | move.to_square;
		else white_pieces = white_pieces | move.to_square;
		pieces[move.captured_piece] ^= move.to_square;
	}
	if (move.move_color == WHITE) {
		white_pieces ^= (move.from_square | move.to_square);
	}
	else {
		black_pieces ^= (move.from_square | move.to_square);
	}
}

float Board::evaluate() {
	return (this->pieces[w_knight].set_bits() - this->pieces[b_knight].set_bits()) * 3
		+ (this->pieces[w_pawn].set_bits() - this->pieces[b_pawn].set_bits()) * 1
		+ (this->pieces[w_king].set_bits() - this->pieces[b_king].set_bits()) * 1000;
}


unsigned long long Board::get_hash() {
	auto hash = 0ull;
	for (int i = 0; i < 64; ++i) {
		piece_type piece = get_piece_at(i);
		if (piece == nothing) continue;
		hash = hash ^ zobrist_hashes[i][piece];
	}
	return hash;
}