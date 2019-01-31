#ifndef PIECES_H
#define PIECES_H

typedef unsigned long long int u64;

class Position;
class Move;
class BitBoards;

class Piece
{
public:
    virtual ~Piece() {}
	char type = '\0';
	int square = -1;
	u64* b = 0;
	u64 control = 0;
    int value;
	//bool captured = false;
	virtual void generateMoves(int pieceListIndex, Move* moves, int* moveCount,
		Position* p, bool q = false, bool redundantPromotions = false) {};
	virtual int getValue(u64 allPieces = 0) { return 0; };
};

class Pawn : public Piece
{
public:
	Pawn(int square, u64* b) {
		this->type = 'p';
		this->square = square;
		this->b = b;
        this->value = 100;
	}
	void generateMoves(int pieceListIndex, Move* moves, int* moveCount,
		Position* p, bool q = false, bool redundantPromotions = false);
	int getValue(u64 allPieces = 0);
};

class Rook : public Piece
{
public:
	Rook(int square, u64* b) {
		this->type = 'r';
		this->square = square;
		this->b = b;
        this->value = 500;
	}
	void generateMoves(int pieceListIndex, Move* moves, int* moveCount,
		Position* p, bool q = false, bool redundantPromotions = false);
	static void generateRookMoves(int pieceListIndex, Move* moves, int* moveCount,
		Position* p, int square, bool q = false);
	int getValue(u64 allPieces = 0);
};

class Bishop : public Piece
{
public:
	Bishop(int square, u64* b) {
		this->type = 'b';
		this->square = square;
		this->b = b;
        this->value = 300;
	}
	void generateMoves(int pieceListIndex, Move* moves, int* moveCount,
		Position* p, bool q = false, bool redundantPromotions = false);
	static void generateBishopMoves(int pieceListIndex, Move* moves, int* moveCount,
		Position* p, int square, bool q = false);
	int getValue(u64 allPieces = 0);
};

class Queen : public Piece
{
public:
	Queen(int square, u64* b) {
		this->type = 'q';
		this->square = square;
		this->b = b;
        this->value = 900;
	}
	void generateMoves(int pieceListIndex, Move* moves, int* moveCount,
		Position* p, bool q = false, bool redundantPromotions = false);
	int getValue(u64 allPieces = 0);
};

class Knight : public Piece
{
public:
	Knight(int square, u64* b) {
		this->type = 'n';
		this->square = square;
		this->b = b;
        this->value = 290;
	}
	void generateMoves(int pieceListIndex, Move* moves, int* moveCount,
		Position* p, bool q = false, bool redundantPromotions = false);
	int getValue(u64 allPieces = 0);
};

class King : public Piece
{
public:
	King(int square, u64* b) {
		this->type = 'k';
		this->square = square;
		this->b = b;
        this->value = 1000000000;
	}
	void generateMoves(int pieceListIndex, Move* moves, int* moveCount,
		Position* p, bool q = false, bool redundantPromotions = false);
	int getValue(u64 allPieces = 0);
	int getKingSafety(u64 allPieces);
};

#endif
