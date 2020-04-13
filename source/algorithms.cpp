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

bool Algorithms::doBestMove()
{
	return movePiece(bestMove);
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
	if (current_game->isCheckMate()) {
		Chess::Position king = current_game->findKing(current_game->getCurrentTurn());
		return current_game->evaluate() - current_game->pieceValue(king.iRow, king.iColumn);
	}
	vector<Move> validMoves = eachMove(player(maximizer));
	if (validMoves.size() == 0)//stalemate
		return 0;
	save();
	if (maximizer) {
		//white's turn (maximzie the value)
		//for each move
		for (const auto& move : validMoves)
		{
			//cout << '(' << char('A' + move.present.iColumn) << move.present.iRow + 1
			//	<< '-' << char('A' + move.future.iColumn) << move.future.iRow + 1 << ')';
			movePiece(move);
			int searchValue = minimaxSearch(!maximizer, ++depth, alpha, beta);
			depth--;
			if (searchValue > alpha) {
				alpha = searchValue;
				if (depth == 0)
					bestMove = move;
			}
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
			int searchValue = minimaxSearch(!maximizer, ++depth, alpha, beta);
			depth--;
			if (searchValue < beta) {
				beta = searchValue;
				if (depth == 0)
					bestMove = move;
			}
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
