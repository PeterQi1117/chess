#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <algorithm> 
#include <vector>

using std::cout;
using std::cin;
using std::min;
using std::max;

typedef uint64_t u64;
typedef int32_t i32;


class Data {
public:
	static void init(bool newMagics);

	static u64 getBishopControl(int square, u64 allPieces);
	static u64 getRookControl(int square, u64 allPieces);
	static u64 getKnightControl(int square);
	static u64 getKingControl(int square);
	static u64 getQueenControl(int square, u64 allPieces);

	static int getBishopValue(int square, u64 allPieces);
	static int getRookValue(int square, u64 allPieces);
	static int getKnightValue(int square);
	static int getQueenValue(int square, u64 allPieces);

	static void test(bool newMagics = false);
	static void print(u64 board);

	static int *bishopPathLengths;
	static int *nMoves;
	static int *kMoves;
	static int nMovesLengths[64];
	static int kMovesLengths[64];
private:
	static bool ready;

	static int bishopDirections[4][2], rookDirections[4][2];
	static u64 rMasks[64], bMasks[64];

	static int rBits[64], bBits[64];
	static u64 rMagicNumbers[64], bMagicNumbers[64];

	static u64 *rControlBoards, *bControlBoards;
	static int *rValues, *bValues;
	static u64 nControlBoards[64], kControlBoards[64];
	static int nValues[64];

	static void initSlidingPieces(bool newMagics = false);
	static void initKnightsAndKings();

	static int count(u64 board);

	static void bitBoardToGrid(int grid[8][8], u64 bitBoard);
	static u64 blockerBoardToMoveBoard(u64 blockerBloard,
		int square, int piece);
	static u64 coordinatesToBitBoard(int xs[], int ys[],
		int length, bool s = false, int sequence[] = 0);
	static int intPow(int number, int exponent);
	static u64 randomMagic();
};

#endif