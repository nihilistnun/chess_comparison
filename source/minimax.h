#pragma once
#include "chess.h"
#include <algorithm>

class minimax
{
	const int MAX_DEPTH = 2;
	Game* current_game = NULL;
	Game* future_game = NULL;
	Chess::Position bestMoveOrigin;
	Chess::Position bestMoveDestin;

	int search(bool maximizer, int depth = 0, int alpha = std::numeric_limits<int>::min(), int beta = std::numeric_limits<int>::max());

};

