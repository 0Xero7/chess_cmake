
#include "bench.h"
#include "board.h"
#include <iostream>

namespace Helpers {
	void push_back_move_if_valid(Move&& item, std::vector<Move>& container) {
		if (!item.to_square.is_zero()) container.push_back(item);
	}

	void generate_and_add_move(color move_color, Board& board, Bitboard& from_square, Bitboard& to_square, piece_type piece,
		Bitboard& const our, Bitboard& const opp, std::vector<Move>& container) {

		if ((to_square & ~our).is_zero()) return;

		bool is_captures = !(to_square & opp).is_zero();
		int idx = to_square.get_last_set_bit();
		piece_type captured_piece = is_captures ? board.get_piece_at(idx) : nothing;

		auto move = Move(move_color, piece, from_square, to_square, is_captures, captured_piece);
		board.make_move(move);

		auto occupancy = (our | opp).get_board();
		if (move_color == WHITE && board.is_in_check<WHITE>(occupancy)) {
			board.unmake_move(move);
			return;
		}
		if (move_color == BLACK && board.is_in_check<BLACK>(occupancy)) {
			board.unmake_move(move);
			return;
		}

		board.unmake_move(move);
		container.push_back(move);
	}

	void generate_and_add_move_unchecked(color move_color, Board& board, Bitboard& from_square, Bitboard& to_square, piece_type piece,
		Bitboard& const our, Bitboard& const opp, std::vector<Move>& container) {

		//if ((to_square & ~our).is_zero()) return;

		bool is_captures = !(to_square & opp).is_zero();
		int idx = to_square.get_last_set_bit();
		piece_type captured_piece = is_captures ? board.get_piece_at(idx) : nothing;

		//auto move = ;
		container.emplace_back(move_color, piece, from_square, to_square, is_captures, captured_piece);
	}
}

void Board::init_zobrist() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<unsigned long long> distrib(0ull, ULLONG_MAX);

	for (int i = 0; i < 64; ++i)
		for (piece_type j = w_pawn; j <= b_king; ++j)
			zobrist_hashes[i][j] = distrib(gen);

	white_move_hash = distrib(gen);
	black_move_hash = distrib(gen);
}

uint64_t Board::get_rook_attacks_mask(int square, uint64_t& const occupancy) {
	int hash = ((occupancy & rook_magic_masks[square]) * rook_magics[square]) >> (63 - 13);
	return rook_attacks[square][hash];
}

uint64_t Board::get_bishop_attacks_mask(int square, uint64_t& const occupancy) {
	uint64_t hash = ((occupancy & bishop_magic_masks[square]) * bishop_magics[square]) >> (63 - 10);
	return bishop_attacks[square][hash];
}

uint64_t Board::get_king_attack_mask(int square) {
	Bitboard temp((1ull << square));
	auto collect = (temp.shift<0>()) | (temp.shift<1>()) | (temp.shift<2>()) | (temp.shift<3>())
		| (temp.shift<4>()) | (temp.shift<5>()) | (temp.shift<6>()) | (temp.shift<7>());
	return collect.get_board();
}

uint64_t Board::get_knight_attack_mask(int square) {
	auto temp = Bitboard((1ull << square));
	auto up = temp.shift_up_2();
	auto down = temp.shift_down_2();
	auto left = temp.shift_left_2();
	auto right = temp.shift_right_2();
	
	auto collect = (up.shift<DIR_RIGHT>() | up.shift<DIR_LEFT>()) | (down.shift<DIR_RIGHT>() | down.shift<DIR_LEFT>()) |
		(left.shift<DIR_UP>() | left.shift<DIR_DOWN>()) | (right.shift<DIR_UP>() | right.shift<DIR_DOWN>());

	return collect.get_board();
}

inline piece_type Board::get_piece_at(int square) {
	return piece_on_square[square];
	/*for (piece_type piece = w_pawn; piece <= b_king; ++piece) {
		if (this->pieces[piece].test(square))
			return piece;
	}
	return nothing;*/
}

void generate_rook_occupancies(uint64_t board, uint64_t up, uint64_t right, std::vector<uint64_t>& collect) {
	uint64_t h_outside = RANK_1 | RANK_8;
	uint64_t v_outside = FILE_A | FILE_H;

	for (int i = 0; i < 256; ++i) {
		uint64_t mask = 0ull;
		for (int j = 0; j < 8; ++j) {
			if (((i >> j) & 1) == 0) continue;

			uint64_t bit = (up << (j * 8));
			mask ^= bit;
		}

		if (mask & board) continue;
		if (mask & h_outside) continue;

		for (int j = 0; j < 256; ++j) {
			uint64_t h_mask = 0ull;
			for (int k = 0; k < 8; ++k) {
				if (((j >> k) & 1) == 0) continue;

				uint64_t bit = (right >> k);
				h_mask ^= bit;
			}

			if (h_mask & board) continue;
			if (h_mask & v_outside) continue;
			collect.push_back(mask | h_mask);
		}
	}
}

Bitboard get_attacks(uint64_t pos, uint64_t occupancy) {
	Bitboard board = Bitboard(pos);
	Bitboard attacks;

	auto mask = board.shift_up();
	while (!mask.is_zero()) {
		attacks |= mask;
		if (!(mask & occupancy).is_zero()) break;
		mask = mask.shift_up();
	}

	mask = board.shift_down();
	while (!mask.is_zero()) {
		attacks |= mask;
		if (!(mask & occupancy).is_zero()) break;
		mask = mask.shift_down();
	}

	mask = board.shift_left();
	while (!mask.is_zero()) {
		attacks |= mask;
		if (!(mask & occupancy).is_zero()) break;
		mask = mask.shift_left();
	}

	mask = board.shift_right();
	while (!mask.is_zero()) {
		attacks |= mask;
		if (!(mask & occupancy).is_zero()) break;
		mask = mask.shift_right();
	}

	return attacks;
}

void generate_occupancies(uint64_t mask, int index, uint64_t bit_mask, std::vector<uint64_t>& collect) {
	if (index == 64) {
		collect.push_back(mask);
		return;
	}

	if (mask & bit_mask) {
		auto remove = mask ^ bit_mask;
		generate_occupancies(remove, index + 1, bit_mask << 1, collect);
	}
	generate_occupancies(mask, index + 1, bit_mask << 1, collect);
}


void Board::init_rook_magics() {
	std::mt19937_64 engine;
	std::uniform_int_distribution<unsigned long long> gen(0, ULLONG_MAX);
	std::mt19937_64 engine2;
	std::uniform_int_distribution<unsigned long long> gen2(0, ULLONG_MAX);

	auto generate_rook_attacks = [](uint64_t board, uint64_t occupancy) -> uint64_t {
		auto _occupancy = Bitboard(occupancy);
		auto _mask = Bitboard(board);
		auto rook_mask = Bitboard();

		for (int dir = DIR_UP; dir <= DIR_LEFT; ++dir) {
			auto bit = Bitboard(board).shift(dir);

			while (!bit.is_zero()) {
				rook_mask = rook_mask | bit;

				if (!(bit & occupancy).is_zero()) break;
				bit = bit.shift(dir);
			}
		}

		return rook_mask.get_board();
	};

	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			int square = i * 8 + j;
			if (square == 36) {
				int x = 4;
			}
			uint64_t board = (1ull << square);

			auto mask = ((files[j] & ~(RANK_1 | RANK_8)) | (ranks[i] & ~(FILE_A | FILE_H))) & ~board;
			rook_magic_masks[square] = mask;

			std::vector<uint64_t> occupancies;
			generate_occupancies(mask, 0, 1ull, occupancies);
			std::vector<uint64_t> attacks;
			for (auto& occupancy : occupancies) {
				attacks.push_back(generate_rook_attacks(board, occupancy));
			}

			int bits = 63 - 13;
			std::unordered_map<int, uint64_t> _attacks;
			while (1) {
				auto magic = gen(engine);
				_attacks.clear();
				bool flag = true;

				for (int i = 0; i < occupancies.size(); ++i) {
					int hash = (int)((occupancies[i] * magic) >> (bits));
					auto attack = attacks[i];

					if (_attacks.count(hash)) {
						if (_attacks[hash] != attack) {
							flag = false;
							break;
						}
					}
					else {
						_attacks[hash] = attack;
					}
				}

				if (!flag) continue;

				for (auto& [hash, _attack] : _attacks) {
					rook_attacks[square][hash] = _attack;
				}

				//printf("%llu\n", magic);
				rook_magics[square] = magic;
				break;
			}
		}
	}
}

void Board::init_bishop_magics() {
	for (int j = 0; j < 64; ++j) {
		auto sq = Bitboard((1ull << j));
		Bitboard mask;

		auto generate_attacks = [](Bitboard square, uint64_t _occupancy) -> Bitboard {
			auto mask = Bitboard();
			auto occupancy = Bitboard(_occupancy);

			if (_occupancy == 594475150812987392 && square.get_last_set_bit() == 22) {
				int y = 2;
			}

			for (int dir = DIR_UP_RIGHT; dir <= DIR_UP_LEFT; ++dir) {
				Bitboard bit_mask = square;
				while (1) {
					bit_mask = bit_mask.shift(dir);
					if (bit_mask.is_zero()) break;

					mask = mask | bit_mask;
					if (!(bit_mask & occupancy).is_zero()) break;
				}
			}

			return mask;
		};

		for (int dir = DIR_UP_RIGHT; dir <= DIR_UP_LEFT; ++dir) {
			auto bit_mask = sq;
			while (1) {
				bit_mask = bit_mask.shift(dir);
				if (bit_mask.is_zero()) break;

				mask = mask | bit_mask;
			}
		}

		mask = mask & ~(FILE_A | FILE_H | RANK_1 | RANK_8) & ~(sq);
		bishop_magic_masks[j] = mask.get_board();

		std::vector<uint64_t> occupancies;
		generate_occupancies(mask.get_board(), 0, 1ull, occupancies);
		std::vector<uint64_t> attacks;

		for (auto& occ : occupancies) {
			attacks.push_back(generate_attacks(sq, occ).get_board());
		}

		std::mt19937 device;
		std::mt19937 device2;
		std::uniform_int_distribution<unsigned long long> rng(0ull, LLONG_MAX);
		std::uniform_int_distribution<unsigned long long> rng2(0ull, LLONG_MAX);

		while (1) {
			uint64_t magic = rng(device);
			uint64_t rnd = rng2(device2);

			std::unordered_map<int, uint64_t> attack_holder;
			bool flag = true;
			int hash = 0;
			int max_hash = 0;

			for (int i = 0; i < occupancies.size(); ++i) {
				hash = (((occupancies[i]) * (rnd * magic))) >> (63 - 10);
				max_hash = std::max(max_hash, hash);

				if (attack_holder.count(hash)) {
					if (attack_holder[hash] != attacks[i]) {
						flag = false;
						break;
					}
				}
				else {
					attack_holder[hash] = attacks[i];
				}
			}

			if (flag) {
				for (auto [_hash, _attack] : attack_holder)
					bishop_attacks[j][_hash] = _attack;
				bishop_magics[j] = rnd * magic;
				break;
			}
		}
	}
}


void Board::_get_knight_moves(color move_color, piece_type piece, Bitboard& const bb, Bitboard& const our, Bitboard& const opp, std::vector<Move>& moves)
{
	if (bb.is_zero()) return;

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
}

void Board::_get_king_moves(color move_color, piece_type piece, Bitboard& const bb, Bitboard& const our, Bitboard& const opp, std::vector<Move>& moves)
{
	if (bb.is_zero()) return;

	uint64_t board = bb.get_board();
	int idx = 63 - __lzcnt64(board);
	auto occupancy_mask = (our | opp).get_board();

	while (board) {
		int idx = 63 - __lzcnt64(board);
		uint64_t _mask = (1ull << idx);
		Bitboard mask = Bitboard(_mask);

		Helpers::generate_and_add_move(move_color, *this, mask, mask.shift<DIR_LEFT>(), piece, our, opp, moves);
		Helpers::generate_and_add_move(move_color, *this, mask, mask.shift<DIR_RIGHT>(), piece, our, opp, moves);
		Helpers::generate_and_add_move(move_color, *this, mask, mask.shift<DIR_UP>(), piece, our, opp, moves);
		Helpers::generate_and_add_move(move_color, *this, mask, mask.shift<DIR_DOWN>(), piece, our, opp, moves);

		Helpers::generate_and_add_move(move_color, *this, mask, mask.shift<DIR_UP_LEFT>(), piece, our, opp, moves);
		Helpers::generate_and_add_move(move_color, *this, mask, mask.shift<DIR_UP_RIGHT>(), piece, our, opp, moves);
		Helpers::generate_and_add_move(move_color, *this, mask, mask.shift<DIR_DOWN_RIGHT>(), piece, our, opp, moves);
		Helpers::generate_and_add_move(move_color, *this, mask, mask.shift<DIR_DOWN_LEFT>(), piece, our, opp, moves);

		board ^= _mask;
	}
}

void Board::_get_rook_moves(color move_color, piece_type piece, Bitboard& const bb, Bitboard& const our, Bitboard& const opp, std::vector<Move>& moves)
{
	if (bb.is_zero()) return;

	auto occupancy = (our | opp).get_board();
	uint64_t board = bb.get_board();

	while (board > 0) {
		int idx = 63 - __lzcnt64(board);
		uint64_t _mask = (1ull << idx);
		auto mask = Bitboard(_mask);

		auto attacks = get_rook_attacks_mask(idx, occupancy) & ~our.get_board();
		
		while (attacks) {
			int index = 63 - __lzcnt64(attacks);
			uint64_t move_mask = (1ull << index);
			attacks ^= move_mask;

			Helpers::generate_and_add_move(move_color, *this, mask, Bitboard(move_mask), piece, our, opp, moves);
		}

		board ^= _mask;
	}
}

void Board::_get_fast_rook_moves(color move_color, color other_color, piece_type piece, Bitboard& const bb, Bitboard& const our, Bitboard& const opp, std::vector<Move>& moves) {
	auto occupancy_mask = (our | opp).get_board();

	auto wking = (pieces[move_color + delta_king]).get_board();
	auto b_hor = (pieces[other_color + delta_rook] | pieces[other_color + delta_queen]).get_board();
	auto b_diag = (pieces[other_color + delta_bishop] | pieces[other_color + delta_queen]).get_board();

	uint64_t board = bb.get_board();

	while (board) {
		int idx = 63 - __lzcnt64(board);
		uint64_t _mask = (1ull << idx);
		auto mask = Bitboard(_mask);

		int rank = (idx >> 3);
		int file = idx - (rank << 3);

		auto rank_mask = ranks[rank];
		auto file_mask = files[file];

		auto attacks = get_rook_attacks_mask(idx, occupancy_mask);
		auto rank_attacks = (attacks & rank_mask);
		auto file_attacks = (attacks & file_mask);

		bool is_rank_blocker = ((rank_attacks & wking) && (rank_attacks & b_hor));
		bool is_file_blocker = ((file_attacks & wking) && (file_attacks & b_hor));

		// Diagonal attack check
		auto bishop_attacks = get_bishop_attacks_mask(idx, occupancy_mask);
		auto main_diag_attacks = (bishop_attacks & main_diagonals[main_diagonal_index[idx]]);
		auto anti_diag_attacks = (bishop_attacks & anti_diagonals[anti_diagonal_index[idx]]);

		bool is_main_diag_blocker = ((main_diag_attacks & wking) && (main_diag_attacks & b_diag));
		bool is_anti_diag_blocker = ((anti_diag_attacks & wking) && (anti_diag_attacks & b_diag));

		uint64_t correct_attacks = ((attacks * !(is_rank_blocker || is_file_blocker)) +
			(is_rank_blocker * rank_attacks) | (is_file_blocker * file_attacks)) * !(is_main_diag_blocker || is_anti_diag_blocker);

		/*uint64_t correct_attacks;
		if (!(is_rank_blocker || is_file_blocker)) correct_attacks = attacks;
		else correct_attacks = (rank_attacks * is_rank_blocker) | (file_attacks * is_file_blocker);*/

		/*uint64_t correct_attacks;
		if (is_rank_blocker) correct_attacks = rank_attacks;
		else if (is_file_blocker) correct_attacks = file_attacks;
		else correct_attacks = attacks;*/

		correct_attacks = correct_attacks & ~our.get_board();

		auto _attacks = correct_attacks & opp.get_board();
		auto _quiets = correct_attacks & ~_attacks;
		while (_attacks) {
			int index = 63 - __lzcnt64(_attacks);
			uint64_t move_mask = (1ull << index);
			_attacks ^= move_mask;

			moves.emplace_back(move_color, piece, mask, move_mask, true, piece_on_square[index]);
		}

		while (_quiets) {
			int index = 63 - __lzcnt64(_quiets);
			uint64_t move_mask = (1ull << index);
			_quiets ^= move_mask;

			moves.emplace_back(move_color, piece, mask, move_mask, false, nothing);
		}

		/*while (correct_attacks) {
			int index = 63 - __lzcnt64(correct_attacks);
			uint64_t move_mask = (1ull << index);
			correct_attacks ^= move_mask;

			Helpers::generate_and_add_move_unchecked(move_color, *this, mask, Bitboard(move_mask), piece, our, opp, moves);
		}*/

		board ^= _mask;
	}
}

void Board::get_vfast_rook_moves(std::vector<Move>& moves) {
	auto occupancy_mask = get_occupancy_mask();

	auto wking = (pieces[w_king]).get_board();
	auto b_hor = (pieces[b_rook] | pieces[b_queen]).get_board();
	auto b_diag = (pieces[b_bishop] | pieces[b_queen]).get_board();

	uint64_t board = pieces[w_rook].get_board();

	auto our = white_pieces;

	while (board) {
		int idx = 63 - __lzcnt64(board);
		uint64_t _mask = (1ull << idx);
		auto mask = Bitboard(_mask);

		int rank = (idx >> 3);
		int file = idx - (rank << 3);

		auto rank_mask = ranks[rank];
		auto file_mask = files[file];

		auto rook_attacks = get_rook_attacks_mask(idx, occupancy_mask);
		auto rank_attacks = (rook_attacks & rank_mask);
		auto file_attacks = (rook_attacks & file_mask);

		bool is_rank_blocker = ((rank_attacks & wking) && (rank_attacks & b_hor));
		bool is_file_blocker = ((file_attacks & wking) && (file_attacks & b_hor));


		// Diagonal attack check
		auto bishop_attacks = get_bishop_attacks_mask(idx, occupancy_mask);
		auto main_diag_attacks = (bishop_attacks & main_diagonals[main_diagonal_index[idx]]);
		auto anti_diag_attacks = (bishop_attacks & anti_diagonals[anti_diagonal_index[idx]]);

		bool is_main_diag_blocker = ((main_diag_attacks & wking) && (main_diag_attacks & b_diag));
		bool is_anti_diag_blocker = ((anti_diag_attacks & wking) && (anti_diag_attacks & b_diag));
		
		uint64_t correct_attacks = ((rook_attacks * !(is_rank_blocker || is_file_blocker)) +
			(is_rank_blocker * rank_attacks) | (is_file_blocker * file_attacks)) * !(is_main_diag_blocker || is_anti_diag_blocker);

		/*uint64_t correct_attacks;
		if (!(is_rank_blocker || is_file_blocker)) correct_attacks = attacks;
		else correct_attacks = (rank_attacks * is_rank_blocker) | (file_attacks * is_file_blocker);*/

		/*uint64_t correct_attacks;
		if (is_rank_blocker) correct_attacks = rank_attacks;
		else if (is_file_blocker) correct_attacks = file_attacks;
		else correct_attacks = attacks;*/

		correct_attacks = correct_attacks & ~our.get_board();

		while (correct_attacks) {
			int index = 63 - __lzcnt64(correct_attacks);
			uint64_t move_mask = (1ull << index);
			correct_attacks ^= move_mask;

			Helpers::generate_and_add_move_unchecked(WHITE, *this, mask, Bitboard(move_mask), w_rook, our, black_pieces, moves);
		}

		board ^= _mask;
	}
}

void Board::_get_bishop_moves(color move_color, piece_type piece, Bitboard& const bb, Bitboard& const our, Bitboard& const opp, std::vector<Move>& moves)
{
	if (bb.is_zero()) return;

	auto occupancy = (our | opp).get_board();
	uint64_t board = bb.get_board();

	while (board) {
		int idx = 63 - __lzcnt64(board);
		uint64_t _mask = (1ull << idx);
		auto mask = Bitboard(_mask);

		auto attacks = get_bishop_attacks_mask(idx, occupancy) & ~our.get_board();

		while (attacks) {
			int index = 63 - __lzcnt64(attacks);
			uint64_t move_mask = (1ull << index);
			attacks ^= move_mask;

			Helpers::generate_and_add_move(move_color, *this, mask, Bitboard(move_mask), piece, our, opp, moves);
		}

		board ^= _mask;
	}
}

void Board::_get_queen_moves(color move_color, piece_type piece, Bitboard& const bb, Bitboard& const our, Bitboard& const opp, std::vector<Move>& moves)
{
	if (bb.is_zero()) return;

	auto occupancy = (our | opp).get_board();
	uint64_t board = bb.get_board(); 

	while (board > 0) {
		int idx = 63 - __lzcnt64(board);
		uint64_t _mask = (1ull << idx);
		auto mask = Bitboard(_mask);

		auto attacks = (get_rook_attacks_mask(idx, occupancy) | get_bishop_attacks_mask(idx, occupancy));
		attacks = attacks & ~our.get_board();

		while (attacks) {
			int index = 63 - __lzcnt64(attacks);
			uint64_t move_mask = (1ull << index);
			attacks ^= move_mask;

			Helpers::generate_and_add_move(move_color, *this, mask, Bitboard(move_mask), piece, our, opp, moves);
		}

		board ^= _mask;
	}
}


template <> void Board::get_moves<WHITE>(std::vector<Move>& moves) {
	get_king_moves<WHITE>(moves);
	get_knight_moves<WHITE>(moves);
	get_pawn_moves<WHITE>(moves);
	get_rook_moves<WHITE>(moves);
	get_bishop_moves<WHITE>(moves);
	get_queen_moves<WHITE>(moves);

	//std::random_device rd;
	//std::mt19937 g(rd());
	/*sort(moves.begin(), moves.end(), [](Move& a, Move& b) {
		return a.is_capture > b.is_capture;
		});*/
	//std::shuffle(moves.begin(), moves.end(), g);
}

template <> void Board::get_moves<BLACK>(std::vector<Move>& moves) {
	get_king_moves<BLACK>(moves);
	get_knight_moves<BLACK>(moves);
	get_pawn_moves<BLACK>(moves);
	get_rook_moves<BLACK>(moves);
	get_bishop_moves<BLACK>(moves);
	get_queen_moves<BLACK>(moves);

	/*std::random_device rd;
	std::mt19937 g(rd());*/
	/*sort(moves.begin(), moves.end(), [](Move& a, Move& b) {
		return a.is_capture > b.is_capture;
		});*/
	//std::shuffle(moves.begin(), moves.end(), g);
}


void Board::_get_w_pawn_moves(color move_color, piece_type piece, Bitboard& const bb, Bitboard& const our, Bitboard& const opp, std::vector<Move>& moves)
{
	if (bb.is_zero()) return;

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
}

void Board::_get_b_pawn_moves(color move_color, piece_type piece, Bitboard& const bb, Bitboard& const our, Bitboard& const opp, std::vector<Move>& moves)
{
	if (bb.is_zero());

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
}


void Board::make_move(Move& move) {
	auto mask = (move.from_square | move.to_square);
	pieces[move.piece] ^= mask;

	if (move.move_color == WHITE) {
		white_pieces ^= mask;
	}
	else {
		black_pieces ^= mask;
	}

	if (move.is_capture) {
		if (move.move_color == WHITE) black_pieces = black_pieces ^ move.to_square;
		else white_pieces = white_pieces ^ move.to_square;
		pieces[move.captured_piece] = pieces[move.captured_piece] ^ move.to_square;
	}

	piece_on_square[move.from_square.get_last_set_bit()] = nothing;
	piece_on_square[move.to_square.get_last_set_bit()]   = move.piece;
}

void Board::unmake_move(Move& move) {
	auto mask = (move.from_square | move.to_square);

	pieces[move.piece] ^= mask;

	if (move.is_capture) {
		if (move.move_color == 0) black_pieces = black_pieces | move.to_square;
		else white_pieces = white_pieces | move.to_square;
		pieces[move.captured_piece] ^= move.to_square;
	}
	if (move.move_color == WHITE) {
		white_pieces ^= mask;
	}
	else {
		black_pieces ^= mask;
	}

	piece_on_square[move.from_square.get_last_set_bit()] = move.piece;
	piece_on_square[move.to_square.get_last_set_bit()] = move.captured_piece;
}

float Board::evaluate() {
	/*std::vector<Move> collect_w, collect_b;
	get_moves<WHITE>(collect_w);
	get_moves<BLACK>(collect_b);*/
	bool endgame = (__popcnt64(get_occupancy_mask()) < 12);

	int PSQ = 0;
	for (int i = 0; i < 64; ++i) {
		switch (get_piece_at(i)) {
		case w_pawn: PSQ += PSQ_PAWN[i];
			break;
		case w_rook: PSQ += PSQ_ROOK[i];
			break;
		case w_knight: PSQ += PSQ_KNIGHT[i];
			break;
		case w_bishop: PSQ += PSQ_BISHOP[i];
			break;
		case w_queen: PSQ += PSQ_QUEEN[i];
			break;
		case w_king: PSQ += (endgame ? PSQ_END_KING[i] : PSQ_MID_KING[i]);
			break;

		case b_pawn: PSQ += PSQ_PAWN[63 - i];
			break;
		case b_rook: PSQ += PSQ_ROOK[63 - i];
			break;
		case b_knight: PSQ += PSQ_KNIGHT[63 - i];
			break;
		case b_bishop: PSQ += PSQ_BISHOP[63 - i];
			break;
		case b_queen: PSQ += PSQ_QUEEN[63 - i];
			break;
		case b_king: PSQ += (endgame ? PSQ_END_KING[63 - i] : PSQ_MID_KING[63 - i]);
			break;
		}
	}
	

	return (this->pieces[w_knight].set_bits() - this->pieces[b_knight].set_bits()) * KNIGHT_SCORE
		+ (this->pieces[w_bishop].set_bits() - this->pieces[b_bishop].set_bits()) * BISHOP_SCORE
		+ (this->pieces[w_queen].set_bits() - this->pieces[b_queen].set_bits()) * QUEEN_SCORE
		+ (this->pieces[w_rook].set_bits() - this->pieces[b_rook].set_bits()) * ROOK_SCORE
		+ (this->pieces[w_pawn].set_bits() - this->pieces[b_pawn].set_bits()) * PAWN_SCORE
		+ (this->pieces[w_king].set_bits() - this->pieces[b_king].set_bits()) * KING_SCORE

		+ PSQ;

		//+ ((int)collect_w.size() - (int)collect_b.size()) * 30;
}


unsigned long long Board::get_hash(uint64_t move_hash) {
	auto hash = 0ull;
	for (int i = 0; i < 64; ++i) {
		piece_type piece = get_piece_at(i);
		if (piece == nothing) continue;
		hash = hash ^ zobrist_hashes[i][piece];
	}
	return hash ^ move_hash;
}

uint64_t Board::get_occupancy_mask() {
	return (white_pieces | black_pieces).get_board();
}