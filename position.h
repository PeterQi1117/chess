#ifndef POSITION_H
#define POSITION_H

#include "bitboards.h"

class Piece;
class Move;

class Position
{
public:
	Position(bool startingPostition = true);
	~Position();

	void generateMoves(Move* moves, int* moveCount, bool q = false,
        bool redundantPromotions = false);

	void makeMove(Move* move);
	void unMakeMove(Move* moves);

	int getSideToMove() {
		return sideToMove;
	}
	u64 getAllyPieces() {
		return allyPieces;
	}
	u64 getOpponentPieces() {
		return opponentPieces;
	}
	u64 getAllPieces() {
		return allPieces;
	}
	int getEnPassant() {
		return enPassantLane[turn];
	}
	BitBoards& getWBitBoards() {
		return *wBitBoards;
	}
	BitBoards& getBBitBoards() {
		return *bBitBoards;
	}
	u64 getwksCastling() {
		return wksCastling[turn];
	}
	u64 getwqsCastling() {
		return wqsCastling[turn];
	}
	u64 getbksCastling() {
		return bksCastling[turn];
	}
	u64 getbqsCastling() {
		return bqsCastling[turn];
	}

	int evaluate();

	void generateControlBoards(int color);

    void assessMove(Move* m);

	void print();
	void printMoves(Move* moves, int moveCount);
	static void test();
	static void process();
	static u64 perft(int depth, Position* p);
private:
    void getCastleData(int destSquare, int* rookSquare, int* rookDest, int* rookIndex);
	void addPiece(int color, char type, int square, int index);
	void removePiece(int color, int index);
	char columnNumberToLetter(int n);
	BitBoards *wBitBoards, *bBitBoards;
	Piece **wPieces, **bPieces;
	u64 opponentPieces, allyPieces;
	u64 allPieces;
	bool sideToMove = 1;
	//int fifty = 0;
	int* enPassantLane;
	u64* wksCastling;
	u64* wqsCastling;
	u64* bksCastling;
	u64* bqsCastling;

	int turn;
};

#endif
