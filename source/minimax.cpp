#include "minimax.h"

int minimax::search(bool maximizer, int depth, int alpha, int beta)
{
	if (depth >= MAX_DEPTH)
		return current_game->evaluate();
	if (maximizer) {
		//white's turn (maximzie the value)
		//for each move
		{
			alpha = max(alpha, search(!maximizer, depth++, alpha, beta));
			if (alpha >= beta) {
				//prune the other moves
				return alpha;
			}
		}
		return alpha;
	}
	else {
		//black's turn (minimize the value)
		//for each move
		{
			beta = min(beta, search(!maximizer, depth++, alpha, beta));
			if (alpha >= beta) {
				//prune the other moves
				return beta;
			}
		}
		return beta;
	}
}
