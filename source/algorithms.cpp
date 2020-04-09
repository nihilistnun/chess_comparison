#include "algorithms.h"

Algorithms::Algorithms(Game* current_game, vector<Move>(*eachMove)(Chess::Player), bool (*movePiece)(Move))
{
	saves = new stack<Game>;
	this->current_game = current_game;
	game_copy = *current_game;
	this->eachMove = eachMove;
	this->movePiece = movePiece;
}

Algorithms::~Algorithms()
{
	current_game = nullptr;
	eachMove = nullptr;
	movePiece = nullptr;
	delete saves;
}

Chess::Player Algorithms::player(const bool maximizer)
{
	return maximizer ? Chess::WHITE_PLAYER : Chess::BLACK_PLAYER;
}

void Algorithms::save()
{
	saves->push(*current_game);
}

void Algorithms::load()
{
	*current_game = saves->top();
}

int Algorithms::minimaxSearch(bool maximizer, int depth, int alpha, int beta)
{
	if (depth >= MAX_DEPTH) {
		return current_game->evaluate();
	}
	save();
	vector<Move> validMoves = eachMove(player(maximizer));
	if (maximizer) {
		//white's turn (maximzie the value)
		//for each move
		for (const auto& move : validMoves)
		{
			//cout << '(' << char('A' + move.present.iColumn) << move.present.iRow + 1
			//	<< '-' << char('A' + move.future.iColumn) << move.future.iRow + 1 << ')';
			movePiece(move);
			alpha = max(alpha, minimaxSearch(!maximizer, ++depth, alpha, beta));
			depth--;
			//reset game to previous depth after each move
			load();
			if (alpha >= beta) {
				//prune the other moves
				saves->pop();
				return alpha;
			}
		}
		saves->pop();
		return alpha;
	}
	else {
		//black's turn (minimize the value)
		//for each move
		for (const auto& move : validMoves)
		{
			//cout << '(' << char('A' + move.present.iColumn) << move.present.iRow + 1
			//	<< '-' << char('A' + move.future.iColumn) << move.future.iRow + 1 << ')';
			movePiece(move);
			beta = min(beta, minimaxSearch(!maximizer, ++depth, alpha, beta));
			depth--;
			//reset game to previous depth after each move
			load();
			if (alpha >= beta) {
				//prune the other moves
				saves->pop();
				return beta;
			}
		}
		saves->pop();
		return beta;
	}
}
