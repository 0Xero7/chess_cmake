#pragma once

#define nothing		-1

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


#define FILE_A 0x8080808080808080ull
#define FILE_B (FILE_A >> 1)
#define FILE_C (FILE_B >> 1)
#define FILE_D (FILE_C >> 1)
#define FILE_E (FILE_D >> 1)
#define FILE_F (FILE_E >> 1)
#define FILE_G (FILE_F >> 1)
#define FILE_H (FILE_G >> 1)

#define RANK_1	0x00000000000000ffull
#define RANK_2	(RANK_1 << 8)
#define RANK_3	(RANK_2 << 8)
#define RANK_4	(RANK_3 << 8)
#define RANK_5	(RANK_4 << 8)
#define RANK_6	(RANK_5 << 8)
#define RANK_7	(RANK_6 << 8)
#define RANK_8	(RANK_7 << 8)
