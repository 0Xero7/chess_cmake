#pragma once

constexpr float INF = 1000000;
constexpr float NEG_INF = -INF;

#define nothing		-1

#define delta_pawn		0
#define delta_rook		1
#define delta_knight	2
#define delta_bishop	3
#define delta_queen		4
#define delta_king		5

#define w_pawn		0
#define w_rook		1
#define w_knight	2
#define w_bishop	3
#define w_queen		4
#define w_king		5

#define b_pawn		6
#define b_rook		7
#define b_knight	8
#define b_bishop	9
#define b_queen		10
#define b_king		11

using piece_type = int;
using color = int;

#define WHITE		0
#define BLACK		1

#define DIR_UP			0
#define DIR_RIGHT		1
#define DIR_DOWN		2
#define DIR_LEFT		3

#define DIR_UP_RIGHT	4
#define	DIR_DOWN_RIGHT	5
#define DIR_DOWN_LEFT	6
#define DIR_UP_LEFT		7


constexpr auto FILE_A = 0x8080808080808080ull;
constexpr auto FILE_B = (FILE_A >> 1);
constexpr auto FILE_C = (FILE_B >> 1);
constexpr auto FILE_D = (FILE_C >> 1);
constexpr auto FILE_E = (FILE_D >> 1);
constexpr auto FILE_F = (FILE_E >> 1);
constexpr auto FILE_G = (FILE_F >> 1);
constexpr auto FILE_H = (FILE_G >> 1);
constexpr uint64_t files[8] = { FILE_H, FILE_G, FILE_F, FILE_E, FILE_D, FILE_C, FILE_B, FILE_A };

constexpr auto RANK_1 = 0x00000000000000ffull;
constexpr auto RANK_2 = (RANK_1 << 8);
constexpr auto RANK_3 = (RANK_2 << 8);
constexpr auto RANK_4 = (RANK_3 << 8);
constexpr auto RANK_5 = (RANK_4 << 8);
constexpr auto RANK_6 = (RANK_5 << 8);
constexpr auto RANK_7 = (RANK_6 << 8);
constexpr auto RANK_8 = (RANK_7 << 8);
constexpr uint64_t ranks[8] = { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8 };

constexpr auto MASK_LEFT_2 = ~(FILE_A | FILE_B);
constexpr auto MASK_RIGHT_2 = ~(FILE_G | FILE_H);

constexpr auto H1 = (1ull);
constexpr auto G1 =	(H1 << 1);
constexpr auto F1 =	(G1 << 1);
constexpr auto E1 =	(F1 << 1);
constexpr auto D1 =	(E1 << 1);
constexpr auto C1 =	(D1 << 1);
constexpr auto B1 =	(C1 << 1);
constexpr auto A1 =	(B1 << 1);

constexpr auto H2 = (A1 << 1);
constexpr auto G2 = (H2 << 1);
constexpr auto F2 = (G2 << 1);
constexpr auto E2 = (F2 << 1);
constexpr auto D2 = (E2 << 1);
constexpr auto C2 = (D2 << 1);
constexpr auto B2 = (C2 << 1);
constexpr auto A2 = (B2 << 1);

constexpr auto H3 = (A2 << 1);
constexpr auto G3 = (H3 << 1);
constexpr auto F3 = (G3 << 1);
constexpr auto E3 = (F3 << 1);
constexpr auto D3 = (E3 << 1);
constexpr auto C3 = (D3 << 1);
constexpr auto B3 = (C3 << 1);
constexpr auto A3 = (B3 << 1);

constexpr auto H4 = (A3 << 1);
constexpr auto G4 = (H4 << 1);
constexpr auto F4 = (G4 << 1);
constexpr auto E4 = (F4 << 1);
constexpr auto D4 = (E4 << 1);
constexpr auto C4 = (D4 << 1);
constexpr auto B4 = (C4 << 1);
constexpr auto A4 = (B4 << 1);

constexpr auto H5 = (A4 << 1);
constexpr auto G5 = (H5 << 1);
constexpr auto F5 = (G5 << 1);
constexpr auto E5 = (F5 << 1);
constexpr auto D5 = (E5 << 1);
constexpr auto C5 = (D5 << 1);
constexpr auto B5 = (C5 << 1);
constexpr auto A5 = (B5 << 1);

constexpr auto H6 = (A5 << 1);
constexpr auto G6 = (H6 << 1);
constexpr auto F6 = (G6 << 1);
constexpr auto E6 = (F6 << 1);
constexpr auto D6 = (E6 << 1);
constexpr auto C6 = (D6 << 1);
constexpr auto B6 = (C6 << 1);
constexpr auto A6 = (B6 << 1);

constexpr auto H7 = (A6 << 1);
constexpr auto G7 = (H7 << 1);
constexpr auto F7 = (G7 << 1);
constexpr auto E7 = (F7 << 1);
constexpr auto D7 = (E7 << 1);
constexpr auto C7 = (D7 << 1);
constexpr auto B7 = (C7 << 1);
constexpr auto A7 = (B7 << 1);

constexpr auto H8 = (A7 << 1);
constexpr auto G8 = (H8 << 1);
constexpr auto F8 = (G8 << 1);
constexpr auto E8 = (F8 << 1);
constexpr auto D8 = (E8 << 1);
constexpr auto C8 = (D8 << 1);
constexpr auto B8 = (C8 << 1);
constexpr auto A8 = (B8 << 1);

constexpr uint64_t main_diagonals[15] = 
{ 
	72624976668147840, 283691315109952, 1108169199648, 4328785936,
	16909320, 66052, 258, 1,
	145249953336295424, 290499906672525312, 580999813328273408, 1161999622361579520,
	2323998145211531264, 4647714815446351872, 9223372036854775808
};

constexpr uint64_t main_diagonal_index[64] = {
	7, 6, 5, 4, 3, 2, 1, 0,
	6, 5, 4, 3, 2, 1, 0, 8,
	5, 4, 3, 2, 1, 0, 8, 9,
	4, 3, 2, 1, 0, 8, 9, 10,
	3, 2, 1, 0, 8, 9, 10, 11,
	2, 1, 0, 8, 9, 10, 11, 12,
	1, 0, 8, 9, 10, 11, 12, 13,
	0, 8, 9, 10, 11, 12, 13, 14
};

constexpr uint64_t anti_diagonals[15] =
{
	9241421688590303745, 4620710844295151872, 2310355422147575808, 1155177711073755136, 
	577588855528488960, 288794425616760832, 144396663052566528, 72057594037927936,
	36099303471055874, 141012904183812, 550831656968, 2151686160, 8405024, 32832, 128
};

constexpr uint64_t anti_diagonal_index[64] = {
	0, 8, 9, 10, 11, 12, 13, 14,
	1, 0, 8, 9, 10, 11, 12, 13,
	2, 1, 0, 8, 9, 10, 11, 12,
	3, 2, 1, 0, 8, 9, 10, 11,
	4, 3, 2, 1, 0, 8, 9, 10,
	5, 4, 3, 2, 1, 0, 8, 9,
	6, 5, 4, 3, 2, 1, 0, 8,
	7, 6, 5, 4, 3, 2, 1, 0
};