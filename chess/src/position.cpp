#include "position.h"
#include "pieces.h"
#include "move.h"
#include "data.h"

Position::Position(bool startingPostition)
{
	enPassantLane = new int[500];
	wksCastling = new u64[500];
	wqsCastling = new u64[500];
	bksCastling = new u64[500];
	bqsCastling = new u64[500];

	enPassantLane[0] = -10;

	wPieces = new Piece*[16];
	bPieces = new Piece*[16];

	wBitBoards = new BitBoards;
	bBitBoards = new BitBoards;

	for (int c = 0; c < 2; c++) {
		BitBoards& boards = (c) ? *wBitBoards : *bBitBoards;
		boards.pawns = 0; boards.rooks = 0; boards.knights = 0;
		boards.bishops = 0; boards.queens = 0; boards.king = 0;
		boards.all = 0;
	}

	for (int i = 0; i < 16; i++) {
		wPieces[i] = new Piece;
		bPieces[i] = new Piece;
	}

	if (startingPostition) {
		for (int i = 0; i < 8; i++) {
			addPiece(1, 'p', (i + 8), i);
			addPiece(0, 'p', (i + 48), i);
		}
		//char edgeRow[8] = { 'r', '\0', '\0', 'k', '\0', '\0', '\0', 'r' };
		char edgeRow[8] = { 'r', 'n', 'b', 'k', 'q', 'b', 'n', 'r' };
		for (int white = 0; white < 2; white++) {
			for (int s = 0; s < 8; s++) {
				int square = (white) ? s : 56 + s;
				addPiece(white, edgeRow[s], square, s + 8);
			}
		}
		wksCastling[0] = (1 << 3) | (u64)1;
		wqsCastling[0] = (1 << 7) | ((u64)1 << 3);
		bksCastling[0] = ((u64)1 << 56) | ((u64)1 << 59);
		bqsCastling[0] = ((u64)1 << 59) | ((u64)1 << 63);
	}
	turn = 0;
}

Position::~Position()
{
	delete enPassantLane;
	delete wksCastling;
	delete wqsCastling;
	delete bksCastling;
	delete bqsCastling;
	delete[] wPieces; delete[] bPieces;
	delete wBitBoards; delete bBitBoards;
}

void Position::generateMoves(Move* moves, int* moveCount, bool q, bool redundantPromotions) {
	*moveCount = 0;
	opponentPieces = (sideToMove) ? bBitBoards->all : wBitBoards->all;
	allyPieces = (sideToMove) ? wBitBoards->all : bBitBoards->all;
	allPieces = (wBitBoards->all | bBitBoards->all);
	Piece** pieces = (sideToMove) ? wPieces : bPieces;

	if ((*pieces[11]).type == '\0') {
		return;
	}

	for (int i = 0; i < 16; i++) {
		(*pieces[i]).generateMoves(i, moves, moveCount, this, q, redundantPromotions);
	}
}

void Position::makeMove(Move* m) {
	char moveType =(*m).type;
	int destSquare = (*m).destinationSquare;
	BitBoards& boards = (sideToMove) ? *wBitBoards : *bBitBoards;
	Piece** pieces = (sideToMove) ? wPieces : bPieces;
	Piece** opponentPieces = (sideToMove) ? bPieces : wPieces;
	Piece* p = pieces[(*m).pieceListIndex];
	u64* b = p->b;
	u64* nb = 0;
	u64 mask = 1;

	bool promoting = 1;
	char promotedTo;

	switch (moveType) {
	case 'r':
	case 'P':
		nb = &boards.rooks;
		promotedTo = 'r';
		break;
	case 'n':
	case 'D':
		nb = &boards.knights;
		promotedTo = 'n';
		break;
	case 'b':
	case '@':
		nb = &boards.bishops;
		promotedTo = 'b';
		break;
	case 'q':
	case 'Q':
		nb = &boards.queens;
		promotedTo = 'q';
		break;
	default:
		nb = b;
		promoting = 0;
		break;
	}

	if (moveType == 'e') {
		removePiece(!sideToMove, enPassantLane[turn]);
	}

	enPassantLane[turn + 1] = -10;

	*b &= ~(mask << (*p).square);
	*nb |= (mask << destSquare);
	boards.all &= ~(mask << (*p).square);
	boards.all |= (mask << destSquare);

	if (moveType == 'U' ||
		moveType == 'P' || moveType == 'D' || moveType == '@' || moveType == 'Q') {
        if (!((*m).pieceCaptured)) {
            for (int i = 0; i < 16; i++) {
                if ((*opponentPieces[i]).square == destSquare) {
                    (*m).pieceCaptured = (*opponentPieces[i]).type;
                    (*m).capturedIndex = i;
                    removePiece(!sideToMove, i);
                    break;
                }
            }
        }
        else{
            removePiece(!sideToMove, (*m).capturedIndex);
        }
	}
	else if (moveType == 'C' || moveType == 'c') {
		// move rook
		int rookSquare, rookDest, rookIndex;
        getCastleData(destSquare, &rookSquare, &rookDest, &rookIndex);
		boards.rooks &= ~(mask << rookSquare);
		boards.all &= ~(mask << rookSquare);
		boards.rooks |= (mask << rookDest);
		boards.all |= (mask << rookDest);

		(*pieces[rookIndex]).square = rookDest;
	}
	else if (moveType == '2') {
		enPassantLane[turn + 1] = (*p).square % 8;
	}



	if ((((u64)1 << (*p).square) | ((u64)1 << destSquare)) & wksCastling[turn]) {
		wksCastling[turn + 1] = 0;
	}
	else {
		wksCastling[turn + 1] = wksCastling[turn];
	}
	if ((((u64)1 << (*p).square) | ((u64)1 << destSquare)) & wqsCastling[turn]) {
		wqsCastling[turn + 1] = 0;
	}
	else {
		wqsCastling[turn + 1] = wqsCastling[turn];
	}
	if ((((u64)1 << (*p).square) | ((u64)1 << destSquare)) & bksCastling[turn]) {
		bksCastling[turn + 1] = 0;
	}
	else {
		bksCastling[turn + 1] = bksCastling[turn];
	}
	if ((((u64)1 << (*p).square) | ((u64)1 << destSquare)) & bqsCastling[turn]) {
		bqsCastling[turn + 1] = 0;
	}
	else {
		bqsCastling[turn + 1] = bqsCastling[turn];
	}

	if (promoting) {
		removePiece(sideToMove, (*m).pieceListIndex);
		addPiece(sideToMove, promotedTo, destSquare, (*m).pieceListIndex);
	}

	(*p).square = destSquare;
	sideToMove = !sideToMove;
	turn++;
}

void Position::unMakeMove(Move* m) {
	sideToMove = !sideToMove;

	char moveType = (*m).type;
	int originSquare = (*m).originSquare;
	int destSquare = (*m).destinationSquare;

	BitBoards& boards = (sideToMove) ? *wBitBoards : *bBitBoards;
	//BitBoards& opponentBoards = (sideToMove) ? *bBitBoards : *wBitBoards;

	Piece** pieces = (sideToMove) ? wPieces : bPieces;
	//Piece** opponentPieces = (sideToMove) ? bPieces : wPieces;

	Piece* p;
	p = pieces[(*m).pieceListIndex];

	u64* b = p->b;
	u64* nb = 0;
	u64 mask = 1;

	bool promoting = 1;

	switch (moveType) {
	case 'r':
	case 'P':
	case 'n':
	case 'D':
	case 'b':
	case '@':
	case 'q':
	case 'Q':
		nb = &boards.pawns;
		//promoting = 1;
		break;
	default:
		nb = b;
		promoting = 0;
		break;
	}

	*b &= ~(mask << destSquare);
	*nb |= (mask << originSquare);
	boards.all &= ~(mask << destSquare);
	boards.all |= (mask << originSquare);

	if (moveType == 'U' ||
		moveType == 'P' || moveType == 'D' || moveType == '@' || moveType == 'Q') {
		addPiece(!sideToMove, (*m).pieceCaptured, destSquare, (*m).capturedIndex);
	}
	else if (moveType == 'C' || moveType == 'c') {
	    int rookSquare, rookDest, rookIndex;
        getCastleData(destSquare, &rookSquare, &rookDest, &rookIndex);
		boards.rooks &= ~(mask << rookDest);
		boards.all &= ~(mask << rookDest);
		boards.rooks |= (mask << rookSquare);
		boards.all |= (mask << rookSquare);

		(*pieces[rookIndex]).square = rookSquare;
	}
	else if (moveType == 'e') {
		int s = destSquare + 8 - ((originSquare / 32) * 16);
		addPiece(!sideToMove, 'p', s, enPassantLane[turn - 1]);
	}

	if (promoting) {
		removePiece(sideToMove, (*m).pieceListIndex);
		addPiece(sideToMove, 'p', originSquare, (*m).pieceListIndex);
	}

	(*p).square = originSquare;
	turn--;
}

void Position::getCastleData(int destSquare, int* rookSquare, int* rookDest, int* rookIndex) {
    switch (destSquare)
    {
    case (1):
        *rookSquare = 0;
        *rookDest = 2;
        *rookIndex = 8;
        break;
    case (5):
        *rookSquare = 7;
        *rookDest = 4;
        *rookIndex = 15;
        break;
    case (57):
        *rookSquare = 56;
        *rookDest = 58;
        *rookIndex = 8;
        break;
    case (61):
        *rookSquare = 63;
        *rookDest = 60;
        *rookIndex = 15;
        break;
    default:
        break;
    }
}

void Position::generateControlBoards(int color) {
	Piece** pieces = (color) ? wPieces : bPieces;
	BitBoards& boards = (color) ? *wBitBoards : *bBitBoards;
	u64 allPieces = wBitBoards->all | bBitBoards->all;

	boards.allControl = (u64)0;

	int forward = (color) ? 1 : -1;

	for (int i = 0; i < 16; i++) {
		int square = ((*pieces[i]).square);
		switch ((*pieces[i]).type)
		{
		case 'p':
            {
                int right = 7, left = 9;
                if ((square % 8) % 7 == 0) {
                    if ((bool)color ^ (bool)(square % 8)) {
                        right = 9;
                    }
                    else {
                        left = 7;
                    }
                }
                for (int n = right; n <= left; n += 2) {
                    (*pieces[i]).control |= (u64)1 << (square + forward * n);
                }
            }
			break;
		case 'r':
			(*pieces[i]).control = Data::getRookControl(square, allPieces);
			break;
		case 'n':
			(*pieces[i]).control = Data::getKnightControl(square);
			break;
		case 'b':
			(*pieces[i]).control = Data::getBishopControl(square, allPieces);
			break;
		case 'q':
			(*pieces[i]).control = Data::getQueenControl(square, allPieces);
			break;
		case 'k':
			(*pieces[i]).control = Data::getKingControl(square);
			break;
		default:
			break;
		}
		boards.allControl |= (*pieces[i]).control;
	}
}

void Position::addPiece(int color, char type, int square, int index) {
	Piece** pieces = (color) ? wPieces : bPieces;
	BitBoards& boards = (color) ? *wBitBoards : *bBitBoards;
	u64 place = (u64)1 << square;

	delete pieces[index];

	switch (type)
	{
	case 'p':
		boards.pawns |= place;
		boards.all |= place;
		pieces[index] = new Pawn(square, (color) ? &((*wBitBoards).pawns) : &((*bBitBoards).pawns));
		break;
	case 'r':
		boards.rooks |= place;
		boards.all |= place;
		pieces[index] = new Rook(square, (color) ? &((*wBitBoards).rooks) : &((*bBitBoards).rooks));
		break;
	case 'n':
		boards.knights |= place;
		boards.all |= place;
		pieces[index] = new Knight(square, (color) ? &((*wBitBoards).knights) : &((*bBitBoards).knights));
		break;
	case 'b':
		boards.bishops |= place;
		boards.all |= place;
		pieces[index] = new Bishop(square, (color) ? &((*wBitBoards).bishops) : &((*bBitBoards).bishops));
		break;
	case 'q':
		boards.queens |= place;
		boards.all |= place;
		pieces[index] = new Queen(square, (color) ? &((*wBitBoards).queens) : &((*bBitBoards).queens));
		break;
	case 'k':
		boards.king |= place;
		boards.all |= place;
		pieces[index] = new King(square, (color) ? &((*wBitBoards).king) : &((*bBitBoards).king));
		break;
	default:
        pieces[index] = new Piece();
		break;
	}

}

void Position::removePiece(int color, int index) {
	Piece** pieces = (color) ? wPieces : bPieces;
	BitBoards& boards = (color) ? *wBitBoards : *bBitBoards;
	char type = (*pieces[index]).type;
	u64 place = (u64)1 << ((*pieces[index]).square);

	switch (type)
	{
	case 'p':
		boards.pawns &= (~place);
		boards.all &= (~place);
		break;
	case 'r':
		boards.rooks &= (~place);
		boards.all &= (~place);
		break;
	case 'n':
		boards.knights &= (~place);
		boards.all &= (~place);
		break;
	case 'b':
		boards.bishops &= (~place);
		boards.all &= (~place);
		break;
	case 'q':
		boards.queens &= (~place);
		boards.all &= (~place);
		break;
	case 'k':
		boards.king &= (~place);
		boards.all &= (~place);
		break;
	default:
		break;
	}

	delete pieces[index];
	pieces[index] = new Piece;
}

int Position::evaluate() {
	int result = 0;
	allPieces = (wBitBoards->all | bBitBoards->all);

	if (wPieces[11]->type == '\0') {
		return INT32_MIN;
	}
	else if (bPieces[11]->type == '\0') {
		return INT32_MAX;
	}

	for (int i = 0; i < 16; i++)
	{
		result += (wPieces[i]->type == '\0') ? 0 : wPieces[i]->getValue(allPieces);
		result -= (bPieces[i]->type == '\0') ? 0 : bPieces[i]->getValue(allPieces);
	}

	result -= (2 * Data::getQueenMobility(wPieces[11]->square, allPieces));
	result += (2 * Data::getQueenMobility(bPieces[11]->square, allPieces));
	result += (sideToMove) ? 25 : -25;

	//if (wksCastling)
	//{

	//}

	return result;
}

char Position::columnNumberToLetter(int n) {
	switch (n)
	{
	case 0:
		return 'h';
	case 1:
		return 'g';
	case 2:
		return 'f';
	case 3:
		return 'e';
	case 4:
		return 'd';
	case 5:
		return 'c';
	case 6:
		return 'b';
	case 7:
		return 'a';
	default:
		return '\0';
	}
}

void Position::printMoves(Move* moves, int moveCount) {
	int ss = -1;

	for (int i = 0; i < moveCount; i++) {
		Piece** pieces = (sideToMove) ? wPieces : bPieces;
		Move m = moves[i];
		int square = moves[i].originSquare;
		if (square != ss) {
			cout << "\n" << (*pieces[moves[i].pieceListIndex]).type << " ";
		}
		int x = square % 8, y = square / 8;
		cout << "" << i << ": ";
		cout << columnNumberToLetter(x);
		cout << y + 1 << "";
		x = m.destinationSquare % 8, y = m.destinationSquare / 8;
		cout << columnNumberToLetter(x);
		cout << y + 1 << " " << m.type << " ";

		ss = square;
	}
	cout << "\n\n";
}

void Position::print() {

	u64 mask = (u64)1 << 63;
	cout << "\n    _________________________________";
	cout << "\n   |                                 |\n";
	for (int y = 7; y >= 0; y--) {
		cout << " " << (y + 1) << " | ";
		for (int x = 7; x >= 0; x--) {
			int shift = (63 - (y * 8 + x));
			char color = '\0', piece;
			for (int c = 0; c < 2; c++) {
				BitBoards& boards = (c) ? *wBitBoards : *bBitBoards;
				if ((boards.pawns << shift) & mask) {
					color = ((c) ? '+' : '-');
					piece = 'p';
				}
				else if ((boards.rooks << shift) & mask) {
					color = ((c) ? '+' : '-');
					piece = 'R';
				}
				else if ((boards.knights << shift) & mask) {
					color = ((c) ? '+' : '-');
					piece = 'N';
				}
				else if ((boards.bishops << shift) & mask) {
					color = ((c) ? '+' : '-');
					piece = 'B';
				}
				else if ((boards.queens << shift) & mask) {
					color = ((c) ? '+' : '-');
					piece = 'Q';
				}
				else if ((boards.king << shift) & mask) {
					color = ((c) ? '+' : '-');
					piece = 'K';
				}
			}
			if (color) {
				cout << " " << color << piece << " ";
			}
			else {
				cout << " .. ";
			}
		}
		if (y == 0) {
			cout << "|\n   |_________________________________|\n";
			cout << "       A   B   C   D   E   F   G   H\n";
			cout << "\n";
		}
		else {
			cout << "|\n   |                                 |\n";
		}
	}
	//allPieces = (wBitBoards->all | bBitBoards->all);
	//cout << "\n;;;;;;;;;;;;" << Data::getQueenMobility(wPieces[11]->square, allPieces);
}

void Position::test() {
	Position p;
	u64 perftResult = perft(6, &p);
	cout << "Result: " << perftResult << "\n";

	//Position p;
	//p.print();
	//cout << "========" << p.evaluate() << "========\n";
	//Data::print(p.allPieces);

	//cout << (char)('U'&'r') << "\n"; //P
	//cout << (char)('U'&'n') << "\n"; //D
	//cout << (char)('U'&'b') << "\n"; //@
	//cout << (char)('U'&'q') << "\n"; //Q
}

void Position::process() {
	Position p;
	int t = 0;
	Move *moveHistory = new Move[1000];
	while (true) {
		Move moves[218];
		int moveCount = 0;
		int moveChoice = -100;
		p.print();
		p.generateMoves(moves, &moveCount);
        //Data::print(p.allPieces);
		p.printMoves(moves, moveCount);

		while (moveChoice < -2 || moveChoice >= moveCount)
		{
			cin >> moveChoice;
		}

		if (moveChoice == -1 && t != 0)
		{
			p.unMakeMove(&moveHistory[t - 1]);
			t--;
			//Move moves[218];
			//p.generateMoves(moves, &moveCount, true);
			//p.printMoves(moves, moveCount);
		}
        else if (moveChoice == -2) {
            return;
        }
		else {
			p.makeMove(&(moves[moveChoice]));
			moveHistory[t] = moves[moveChoice];
			t++;
		}
	}
	delete[] moveHistory;
}

u64 Position::perft(int depth, Position* p) {
	u64 nodes = 0;

	if (depth == 0) {
		return 1;
	}

	Move moves[218];
	int moveCount = 0;

	(*p).generateMoves(moves, &moveCount);

	if (moveCount == 0) {
		return 1;
	}

	for (int i = 0; i < moveCount; i++) {
		(*p).makeMove(&(moves[i]));
		nodes += perft(depth - 1, p);
		(*p).unMakeMove(&(moves[i]));
	}

	return nodes;
}
