#pragma once
#include "chess.h"
#include <algorithm>
#include <stack> 

class Algorithms
{
public:
	struct Move {
		Chess::Position present;
		Chess::Position future;
		Chess::EnPassant S_enPassant;
		Chess::Castling S_castling;
		Chess::Promotion S_promotion;
	};

	const int MAX_DEPTH = 4;
	Chess::Position bestMoveOrigin = { 0,0 };
	Chess::Position bestMoveDestin = { 0,0 };
	//game pointer
	Game* current_game = nullptr;
	//each move function pointer
	vector<Move> (*eachMove)(Chess::Player) { nullptr };
	//move piece function pointer
	bool (*movePiece)(Move) { nullptr };

	Algorithms(Game* current_game, vector<Move>(*eachMove)(Chess::Player), bool (*movePiece)(Move));
	~Algorithms();

	Chess::Player player(const bool maximizer);

	void save();

	void load();

	int minimaxSearch(bool maximizer, int depth = 0, int alpha = std::numeric_limits<int>::min(), int beta = std::numeric_limits<int>::max());

private:
	//original game copy to backup current state
	Game game_copy = NULL;
	//game copies to save and revert to at each depth
	stack<Game>* saves;
};

