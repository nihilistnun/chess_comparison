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
	delete MCTree;
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
	if (validMoves.size() == 0 || current_game->fiftyMoveRule()) {
		//no more moves and not a checkmate or fifty move rule causes stalemate
		current_game->setStaleMate();
		return 0;
	}
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

bool Algorithms::monteCarloTreeSearch(bool white)
{
	save();
	srand((unsigned)time(NULL));
	//initialize tree if doesnt exists
	if (MCTree == nullptr)
		MCTree = new Node(*current_game, eachMove);
	//for x times build the tree
	for (int i = 0; i < 1000; i++)
	{
		//selection - select best child to explore including all valid moves
		//should prioritize promosing moves or unexplroed moves to balance
		Node* leaf = MCTree;
		//check if node is already terminal
		if (leaf->isTerminal())
			return false;
		if (i > 900)
			i = i;
		//if leaf has already explored all moves, pick the best result
		while (!leaf->hasPossibleChildren() && leaf->children.size() > 0) {
			leaf = leaf->bestUCTChild();
		}
		// expand if the leaf is not terminal
		if (!leaf->isTerminal()) {
			//expansion - add new child node to selected child
			Move randomMove = leaf->popRandomValidMove();
			movePiece(randomMove);
			leaf = leaf->addChild(new Node(*current_game, eachMove, leaf));
		}
		//simulation - expand the child node randomly till finished
		int result = -2;
		bool playerIsWhite = white;
		while (result == -2) {
			//end cases to stop simulation
			if (current_game->isCheckMate()) {
				if (current_game->getCurrentTurn() == 0)
					result = -1;
				else
					result = 1;
			}
			vector<Move> validMoves = eachMove(player(playerIsWhite));
			playerIsWhite = !playerIsWhite;
			if (validMoves.size() == 0 || current_game->fiftyMoveRule()) {
				//no more moves and not a checkmate or fifty move rule causes stalemate
				current_game->setStaleMate();
				result = 0;
			}
			//do rollout policy
			if (result == -2)
				movePiece(validMoves[rand() % validMoves.size()]);
		}
		//backpropagation - back propagate result up the tree
		if (!leaf->backpropagate(result, white))
			cout << "***BACKPROPAGATE ERROR***" << "\n";
		//reset to original state
		load();
	}
	saves->pop();
	//assign best move the best move
	bestMove = MCTree->bestChild()->getLastMove();
	return true;
}


