#ifndef POSITION_H
#define POSITION_H

#include "data.h"

class bitBoards
{
public:
	u64 all;
	u64 pawns, rooks, knights, bishops, queens, king;
	u64 allControl;
};

class move 
{
public:
	char type;
	int pieceListIndex;
	int destinationSquare;
	int originSquare;
	char pieceCaptured;
	int capturedIndex;

	move() {}

	move(char type, int pieceListIndex, int destinationSquare,
		int  originSquare) {
		this->type = type;
		this->pieceListIndex = pieceListIndex;
		this->destinationSquare = destinationSquare;
		this->originSquare = originSquare;
	}

	move(const move& m) {
		this->type = m.type;
		this->pieceListIndex = m.pieceListIndex;
		this->destinationSquare = m.destinationSquare;
		this->originSquare = m.originSquare;
		this->pieceCaptured = m.pieceCaptured;
		this->capturedIndex = m.capturedIndex;
	}
};

class piece
{
public:
	char type;
	int square;
	u64 control;
};

class Position
{
public:
	Position(bool startingPostition = true);
	~Position();

	void generateMoves(move* moves, int* moveCount);

	void makeMove(move* move);
	void unMakeMove(move* moves);

	int getSideToMove() {
		return sideToMove;
	}

	i32 evaluate();

	void print();
	void printMoves(move* moves, int moveCount);
	static void test();
	static void process();
	static u64 perft(int depth, Position* p);
private:
	void addPiece(int color, char type, int square, int index);
	void removePiece(int color, int index);

	void generateControlBoards(int color);

	void generatePawnMoves(int square, int pieceListIndex, move* moves, int* moveCount);
	void generateRookMoves(int square, int pieceListIndex, move* moves, int* moveCount);
	void generateKnightMoves(int square, int pieceListIndex, move* moves, int* moveCount);
	void generateBishopMoves(int square, int pieceListIndex, move* moves, int* moveCount);
	void generateKingMoves(int square, int pieceListIndex, move* moves, int* moveCount);
	void generateQueenMoves(int square, int pieceListIndex, move* moves, int* moveCount);

	char columnNumberToLetter(int n);

	bitBoards *wBitBoards, *bBitBoards;
	piece *wPieces, *bPieces; 

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
