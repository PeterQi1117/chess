#include "pieces.h"
#include "position.h"
#include "move.h"
#include "data.h"
//#include "bitboards.h"

class Position;

void Pawn::generateMoves(int pieceListIndex, Move* moves, int* moveCount,
	Position* p, bool q, bool redundantPromotions) {
	int x = square % 8, y = square / 8;
	int sideToMove = p->getSideToMove();
	int queeningRow = (sideToMove) ? 6 : 1;
	int enPassantRow = (sideToMove) ? 4 : 3;
	int forward = (sideToMove) ? 1 : -1;
	int beginningRow = (sideToMove) ? 1 : 6;
	char promotions[4] = { 'q', 'n', 'r', 'b' };
	int s = square + forward * 8;
	u64 sMask = (u64)1 << s;
	if (~(p->getAllPieces()) & sMask) {
		if (!q) {
			if (y != queeningRow) {
				Move m = { '\0', pieceListIndex, s, square };
				moves[(*moveCount)++] = m;
				if (y == beginningRow) {
					s = square + forward * 16;
					sMask = (u64)1 << s;
					if (~(p->getAllPieces()) & sMask) {
						moves[(*moveCount)++] = { '2', pieceListIndex, s, square };
					}
				}
			}
			else {
				for (int i = 0; i < ((redundantPromotions) ? 4 : 2); i++) {
					moves[(*moveCount)++] = { promotions[i], pieceListIndex, s, square };
				}
			}
		}
	}
	int right = 7, left = 9;
	if (x % 7 == 0) {
		if ((bool)sideToMove ^ (bool)x) {
			right = 9;
		}
		else {
			left = 7;
		}
	}
	for (int n = right; n <= left; n += 2) {
		s = square + forward * n;
		sMask = (u64)1 << s;
		if (sMask & p->getOpponentPieces()) {
			if (y != queeningRow) {
				moves[(*moveCount)++] = { 'U', pieceListIndex, s, square };
			}
			else {
				for (int i = 0; i < ((redundantPromotions) ? 4 : 2); i++) {
					moves[(*moveCount)++] = { (char)(promotions[i] & 'U'), pieceListIndex, s, square };
				}
			}
		}
	}
	// en passant
	if ((y == enPassantRow) && (abs(p->getEnPassant() - x) == 1)) {
		s = square + forward * 8 + (p->getEnPassant() - x);
		sMask = (u64)1 << s;
		if (~(p->getAllPieces()) & sMask) {
			moves[(*moveCount)++] = { 'e', pieceListIndex, s, square };
		}
	}
}


void Rook::generateMoves(int pieceListIndex, Move* moves, int* moveCount,
	Position* p, bool q, bool redundantPromotions) {
	generateRookMoves(pieceListIndex, moves, moveCount, p, square, q);
}

void Rook::generateRookMoves(int pieceListIndex, Move* moves, int* moveCount,
	Position* p, int square, bool q) {
	int x = square % 8, y = square / 8;
	int ls[4] = { 7 - y, y, 7 - x, x }, m[4] = { 8, -8, 1, -1 };
	for (int d = 0; d < 4; d++) {
		for (int i = 0; i < ls[d]; i++) {
			int s = square + (i + 1) * m[d];
			u64 sMask = (u64)1 << s;
			if (~(p->getAllyPieces()) & sMask) {
				char t = (sMask & p->getOpponentPieces()) ? 'U' : '\0';
				if (!(q && (t == '\0')))
				{
					moves[(*moveCount)++] = { t, pieceListIndex, s, square };
				}
				if (t == 'U') {
					break;
				}
			}
			else {
				break;
			}
		}
	}
}


void Bishop::generateMoves(int pieceListIndex, Move* moves, int* moveCount,
	Position* p, bool q, bool redundantPromotions) {
	generateBishopMoves(pieceListIndex, moves, moveCount, p, square, q);
}

void Bishop::generateBishopMoves(int pieceListIndex, Move* moves, int* moveCount,
	Position* p, int square, bool q) {
	int	m[4] = { 9, 7, -9, -7 };
	for (int d = 0; d < 4; d++) {
		int ls = Data::bishopPathLengths[square * 4 + d];
		for (int i = 0; i < ls; i++) {
			int s = square + (i + 1) * m[d];
			u64 sMask = (u64)1 << s;
			if (~(p->getAllyPieces()) & sMask) {
				char t = (sMask & p->getOpponentPieces()) ? 'U' : '\0';
				if (!(q && (t == '\0')))
				{
					moves[(*moveCount)++] = { t, pieceListIndex, s, square };
				}
				if (t == 'U') {
					break;
				}
			}
			else {
				break;
			}
		}
	}
}

void Queen::generateMoves(int pieceListIndex, Move* moves, int* moveCount,
	Position* p, bool q, bool redundantPromotions) {
	Rook::generateRookMoves(pieceListIndex, moves, moveCount, p, square, q);
	Bishop::generateBishopMoves(pieceListIndex, moves, moveCount, p, square, q);
}

void Knight::generateMoves(int pieceListIndex, Move* moves, int* moveCount,
	Position* p, bool q, bool redundantPromotions) {
	for (int i = 0; i < Data::nMovesLengths[square]; i++) {

		int s = Data::nMoves[square * 8 + i];
		u64 sMask = (u64)1 << s;
		if (~(p->getAllyPieces()) & sMask) {
			char t = (sMask & p->getOpponentPieces()) ? 'U' : '\0';
			if (!(q && (t == '\0')))
			{
				moves[(*moveCount)++] = { t, pieceListIndex, s, square };
			}
		}
	}

}

void King::generateMoves(int pieceListIndex, Move* moves, int* moveCount,
	Position* p, bool q, bool redundantPromotions) {
	for (int i = 0; i < Data::kMovesLengths[square]; i++) {
		int s = Data::kMoves[square * 8 + i];
		u64 sMask = (u64)1 << s;
		if (~p->getAllyPieces() & sMask) {
			char t = (sMask & p->getOpponentPieces()) ? 'U' : '\0';
			if (!(q && (t == '\0')))
			{
				moves[(*moveCount)++] = { t, pieceListIndex, s, square };
			}
		}
	}
	if (q)
	{
		return;
	}
	char t;
	int destSquare;
	u64 castleRight;
	u64 path;
	int sideToMove = p->getSideToMove();
	BitBoards& boards = (sideToMove) ? p->getWBitBoards() : p->getBBitBoards();
	for (int i = 0; i < 2; i++) {
		if (i)
		{
			castleRight = (sideToMove) ? p->getwksCastling() : p->getbksCastling();
			path = (sideToMove) ? ((u64)1 << 1) | ((u64)1 << 2) | ((u64)1 << 3) :
				((u64)1 << 57) | ((u64)1 << 58) | ((u64)1 << 59);
			t = 'c';
			destSquare = (sideToMove) ? 1 : 57;
		}
		else {
			castleRight = (sideToMove) ? p->getwqsCastling() : p->getbqsCastling();
			path = (sideToMove) ? ((u64)1 << 3) | ((u64)1 << 4) | ((u64)1 << 5) | ((u64)1 << 6) :
				((u64)1 << 59) | ((u64)1 << 60) | ((u64)1 << 61) | ((u64)1 << 62);//condense
			t = 'C';
			destSquare = (sideToMove) ? 5 : 61;
		}
		if (castleRight && !((path & ~boards.king) & boards.all)) {
			p->generateControlBoards(!sideToMove);
			u64 opponentControl = (sideToMove) ? p->getBBitBoards().allControl : p->getWBitBoards().allControl;
			if (!(path & opponentControl)) {
				moves[(*moveCount)++] = { t, pieceListIndex, destSquare, square };
			}
		}
	}
}

int Pawn::getValue(u64 allPieces) {
	return value;
}

int Rook::getValue(u64 allPieces) {
	return (value + 6 * Data::getRookMobility(square, allPieces));
}

int Bishop::getValue(u64 allPieces) {
	return (value + 4 * Data::getBishopMobility(square, allPieces));
}

int Queen::getValue(u64 allPieces) {
	return (value + 2 * Data::getQueenMobility(square, allPieces));
}

int Knight::getValue(u64 allPieces) {
	return (value + 4 * Data::getKnightMobility(square));
}

int King::getValue(u64 allPieces) {
	return value;
}

int King::getKingSafety(u64 allPieces) {
	return Data::getQueenMobility(square, allPieces);
}
