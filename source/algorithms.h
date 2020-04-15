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

	//for mcts
	struct Node {
		//node vars
		const int MAX_NODE_DEPTH = 3;
		int depth;
		Node* parent = nullptr;
		vector<Node*> children;
		//game data
		Game data;
		vector<Move> validMoves;
		//stats
		int visitCount;
		int gamesWon;
		int gamesLost;//to calculate games drawn
		//functions
		Node(Game& game, vector<Move>(*eachMove)(Chess::Player), Node* parent = nullptr) {
			data = game;
			validMoves = eachMove(static_cast<Chess::Player>(data.getCurrentTurn()));
			if (parent == nullptr)
				depth = 0;
			else {
				this->parent = parent;
				depth = parent->depth + 1;
			}
		}
		~Node() {
			parent = nullptr;
			delete parent;
			for (auto& child : children) {
				delete child;
			}
		}
		void addChild(Node* node) {
			children.push_back(node);
		}
		bool isLeaf() {
			return children.size() == 0;
		}
		bool isTerminal() {
			//max depth
			if (depth >= MAX_NODE_DEPTH)
				return true;
			//check mate
			if (data.isCheckMate())
				return true;
			//stalemate
			if (data.fiftyMoveRule())
				return true;
			if (validMoves.size() == 0)
				return true;
			//not terminal
			return false;
		}

		//-1 for black win 0 for stalemate 1 for white win
		int result() {
			if (data.isCheckMate()) {
				//checkmate on white's turn aka black win
				if (data.getCurrentTurn() == 0)
					return -1;
				else//vice versa
					return 1;
			}
			if (data.fiftyMoveRule() || validMoves.size() == 0)
				return 0;
		}

		void setRoot() {
			if (parent != nullptr) {
				bool unlinked = false;
				for (auto& child : parent->children) {
					if (child == this) {//find the child pointer to this node
						child = nullptr;
						unlinked = true;
					}
				}
				if (unlinked) {
					delete parent;//delete everything in parent except this instance
					parent = nullptr;
					depth = 0;
				}
			}
		}
	};

	const int MAX_DEPTH = 5;
	Move bestMove;
	//game pointer
	Game* current_game = nullptr;
	//each move function pointer
	vector<Move>(*eachMove)(Chess::Player) { nullptr };
	//move piece function pointer
	bool (*movePiece)(Move) { nullptr };

	Algorithms(Game* current_game, vector<Move>(*eachMove)(Chess::Player), bool (*movePiece)(Move));
	~Algorithms();

	bool doBestMove();

	Chess::Player player(const bool maximizer);

	void save();

	void load();

	int minimaxSearch(bool maximizer, int depth = 0, int alpha = std::numeric_limits<int>::min(), int beta = std::numeric_limits<int>::max());

	void monteCarloTreeSearch();

private:
	//original game copy to backup current state
	Game game_copy = NULL;
	//game copies to save and revert to at each depth
	stack<Game>* saves;
};