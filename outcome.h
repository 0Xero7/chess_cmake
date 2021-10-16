#pragma once

#include "move.h"

class Outcome {
public:
	bool is_mate;
	int mate_in;

	float eval;
	Move move;

	Outcome() {}
	Outcome(float eval, Move& move) : eval(eval), move(move) { }
	Outcome(bool is_mate, int mate_in, float eval, Move& move) 
		: is_mate(is_mate), mate_in(mate_in), eval(eval), move(move) { }
};
