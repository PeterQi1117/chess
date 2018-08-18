#include "position.h"

Position::Position(bool startingPostition)
{
	enPassantLane = new int[11797];
	wksCastling = new u64[11797];
	wqsCastling = new u64[11797];
	bksCastling = new u64[11797];
	bqsCastling = new u64[11797];

	enPassantLane[0] = -10;

	wPieces = new piece[16];
	bPieces = new piece[16];

	wBitBoards = new bitBoards;
	bBitBoards = new bitBoards;

	for (int c = 0; c < 2; c++) {
		bitBoards& boards = (c) ? *wBitBoards : *bBitBoards;
		boards.pawns = 0; boards.rooks = 0; boards.knights = 0;
		boards.bishops = 0; boards.queens = 0; boards.king = 0;
		boards.all = 0;
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
	//delete[] moves;
	delete[] wPieces; delete[] bPieces;
	delete wBitBoards; delete bBitBoards;
}

void Position::generateMoves(move* moves, int* moveCount) {
	*moveCount = 0;
	opponentPieces = (sideToMove) ? bBitBoards->all : wBitBoards->all;
	allyPieces = (sideToMove) ? wBitBoards->all : bBitBoards->all;
	allPieces = (wBitBoards->all | bBitBoards->all);
	piece* pieces = (sideToMove) ? wPieces : bPieces;

	if (pieces[11].type == '\0') {
		return;
	}

	for (int i = 0; i < 16; i++) {
		int square = (pieces[i].square);
		switch (pieces[i].type)
		{
		case 'p':
			generatePawnMoves(square, i, moves, moveCount);
			break;
		case 'r':
			generateRookMoves(square, i, moves, moveCount);
			break;
		case 'n':
			generateKnightMoves(square, i, moves, moveCount);
			break;
		case 'b':
			generateBishopMoves(square, i, moves, moveCount);
			break;
		case 'q':
			generateQueenMoves(square, i, moves, moveCount);
			break;
		case 'k':
			generateKingMoves(square, i, moves, moveCount);
			break;
		default:
			break;
		}
	}
}

void Position::generatePawnMoves(int square, int pieceListIndex, move* moves, int* moveCount) {
	int x = square % 8, y = square / 8;
	u64 mask = 1;
	int queeningRow = (sideToMove) ? 6 : 1;
	int enPassantRow = (sideToMove) ? 4 : 3;
	int forward = (sideToMove) ? 1 : -1;
	int beginningRow = (sideToMove) ? 1 : 6;
	char promotions[4] = { 'q', 'b', 'r', 'n' };
	int s = square + forward * 8;
	u64 sMask = mask << s;
	if (~allPieces & sMask) {
		if (y != queeningRow) {
			move m = { '\0', pieceListIndex, s, square };
			moves[(*moveCount)++] = m;
			if (y == beginningRow) {
				s = square + forward * 16;
				sMask = mask << s;
				if (~allPieces & sMask) {
					moves[(*moveCount)++] = { '2', pieceListIndex, s, square };
				}
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				moves[(*moveCount)++] = { promotions[i], pieceListIndex, s, square };
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
		sMask = mask << s;
		if (sMask & opponentPieces) {
			if (y != queeningRow) {
				moves[(*moveCount)++] = { 'U', pieceListIndex, s, square };
			}
			else {
				for (int i = 0; i < 4; i++) {
					moves[(*moveCount)++] = { promotions[i] & 'U', pieceListIndex, s, square };
				}
			}
		}
	}
	// en passant
	if ((y == enPassantRow) && (abs(enPassantLane[turn] - x) == 1)) {
		s = square + forward * 8 + (enPassantLane[turn] - x);
		sMask = mask << s;
		if (~allPieces & sMask) {
			moves[(*moveCount)++] = { 'e', pieceListIndex, s, square };
		}
	}
}

void Position::generateRookMoves(int square, int pieceListIndex, move* moves, int* moveCount) {
	int x = square % 8, y = square / 8;
	u64 mask = 1;

	int ls[4] = { 7 - y, y, 7 - x, x }, m[4] = { 8, -8, 1, -1 };
	for (int d = 0; d < 4; d++) {
		for (int i = 0; i < ls[d]; i++) {
			int s = square + (i + 1) * m[d];
			u64 sMask = mask << s;
			if (~allyPieces & sMask) {
				char t = (sMask & opponentPieces) ? 'U' : '\0';
				moves[(*moveCount)++] = { t, pieceListIndex, s, square };
				if (sMask & opponentPieces) {
					break;
				}
			}
			else {
				break;
			}
		}
	}
}
void Position::generateKnightMoves(int square, int pieceListIndex, move* moves, int* moveCount) {
	u64 mask = 1;
	for (int i = 0; i < Data::nMovesLengths[square]; i++) {
		int s = Data::nMoves[square * 8 + i];
		u64 sMask = mask << s;
		if (~allyPieces & sMask) {
			char t = (sMask & opponentPieces) ? 'U' : '\0';
			moves[(*moveCount)++] = { t, pieceListIndex, s, square };
		}
	}
}
void Position::generateBishopMoves(int square, int pieceListIndex, move* moves, int* moveCount) {
	u64 mask = 1;
	int	m[4] = { 9, 7, -9, -7 };
	for (int d = 0; d < 4; d++) {
		int ls = Data::bishopPathLengths[square * 4 + d];
		for (int i = 0; i < ls; i++) {
			int s = square + (i + 1) * m[d];
			u64 sMask = mask << s;
			if (~allyPieces & sMask) {
				char t = (sMask & opponentPieces) ? 'U' : '\0';
				moves[(*moveCount)++] = { t, pieceListIndex, s, square };
				if (sMask & opponentPieces) {
					break;
				}
			}
			else {
				break;
			}
		}
	}
}
void Position::generateKingMoves(int square, int pieceListIndex, move* moves, int* moveCount) {
	u64 mask = 1;
	for (int i = 0; i < Data::kMovesLengths[square]; i++) {
		int s = Data::kMoves[square * 8 + i];
		u64 sMask = mask << s;
		if (~allyPieces & sMask) {
			char t = (sMask & opponentPieces) ? 'U' : '\0';
			moves[(*moveCount)++] = { t, pieceListIndex, s, square };
		}
	}
	char t;
	int destSquare;
	u64 castleRight;
	u64 path;
	bitBoards& boards = (sideToMove) ? *wBitBoards : *bBitBoards;
	for (int i = 0; i < 2; i++) {
		if (i)
		{
			castleRight = (sideToMove) ? wksCastling[turn] : bksCastling[turn];
			path = (sideToMove) ? (mask << 1) | (mask << 2) | (mask << 3) : 
				(mask << 57) | (mask << 58) | (mask << 59);
			t = 'c';
			destSquare = (sideToMove) ? 1 : 57;
		}
		else {
			castleRight = (sideToMove) ? wqsCastling[turn] : bqsCastling[turn];
			path = (sideToMove) ? (mask << 3) | (mask << 4) | (mask << 5) | (mask << 6) :
				(mask << 59) | (mask << 60) | (mask << 61) | (mask << 62);//condense
			t = 'C';
			destSquare = (sideToMove) ? 5 : 61;
		}
		if (castleRight && !((path & ~boards.king) & boards.all)) {
			generateControlBoards(!sideToMove);
			u64 opponentControl = (sideToMove) ? bBitBoards->allControl : wBitBoards->allControl;
			if (!(path & opponentControl)) {
				moves[(*moveCount)++] = { t, pieceListIndex, destSquare, square };
			}
		}
	}
}
void Position::generateQueenMoves(int square, int pieceListIndex, move* moves, int* moveCount) {
	generateRookMoves(square, pieceListIndex, moves, moveCount);
	generateBishopMoves(square, pieceListIndex, moves, moveCount);
}

void Position::makeMove(move* m) {
	char moveType =(*m).type; 
	int destSquare = (*m).destinationSquare;
	bitBoards& boards = (sideToMove) ? *wBitBoards : *bBitBoards;
	piece* pieces = (sideToMove) ? wPieces : bPieces;
	piece* opponentPieces = (sideToMove) ? bPieces : wPieces;

	piece* p = &pieces[(*m).pieceListIndex]; 
	
	u64* b = 0;
	u64* nb = 0;
	u64 mask = 1;

	bool promoting = 1;
	char promotedTo;

	switch ((*p).type)
	{
	case 'p':
		b = &boards.pawns;
		break;
	case 'r':
		b = &boards.rooks;
		break;
	case 'n':
		b = &boards.knights;
		break;
	case 'b':
		b = &boards.bishops;
		break;
	case 'q':
		b = &boards.queens;
		break;
	case 'k':
		b = &boards.king;
		break;
	default:
		break;
	}

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
		for (int i = 0; i < 16; i++) {
			if (opponentPieces[i].square == destSquare) {
				(*m).pieceCaptured = opponentPieces[i].type;
				(*m).capturedIndex = i;
				removePiece(!sideToMove, i);
				break;
			}
		}
	}
	else if (moveType == 'C' || moveType == 'c') {
		// move rook
		int rookSquare;
		int rookDest;
		int rookIndex;
		switch (destSquare)
		{
		case (1):
			rookSquare = 0;
			rookDest = 2;
			rookIndex = 8;
			break;
		case (5):
			rookSquare = 7;
			rookDest = 4;
			rookIndex = 15;
			break;
		case (57):
			rookSquare = 56;
			rookDest = 58;
			rookIndex = 8;
			break;
		case (61):
			rookSquare = 63;
			rookDest = 60;
			rookIndex = 15;
			break;
		default:
			break;
		}
		boards.rooks &= ~(mask << rookSquare);
		boards.all &= ~(mask << rookSquare);
		boards.rooks |= (mask << rookDest);
		boards.all |= (mask << rookDest);

		pieces[rookIndex].square = rookDest;
	}
	else if (moveType == '2') {
		enPassantLane[turn + 1] = (*p).square % 8;
	}


	if (promoting) {
		(*p).type = promotedTo;
	}

	if ((((u64)1 << (*p).square) | ((u64)1 << destSquare)) & wksCastling[turn]) {
		wksCastling[turn + 1] = 0;
		//cout << "\n white king castle disabled\n";
	}
	else {
		wksCastling[turn + 1] = wksCastling[turn];
	}
	if ((((u64)1 << (*p).square) | ((u64)1 << destSquare)) & wqsCastling[turn]) {
		wqsCastling[turn + 1] = 0;
		//cout << "\n white queen castle disabled\n";
	}
	else {
		wqsCastling[turn + 1] = wqsCastling[turn];
	}
	if ((((u64)1 << (*p).square) | ((u64)1 << destSquare)) & bksCastling[turn]) {
		bksCastling[turn + 1] = 0;
		//cout << "\n black king castle disabled\n";
	}
	else {
		bksCastling[turn + 1] = bksCastling[turn];
	}
	if ((((u64)1 << (*p).square) | ((u64)1 << destSquare)) & bqsCastling[turn]) {
		bqsCastling[turn + 1] = 0;
		//cout << "\n black queen castle disabled\n";
	}
	else {
		bqsCastling[turn + 1] = bqsCastling[turn];
	}

	(*p).square = destSquare;
	sideToMove = !sideToMove;
	turn++;
}

void Position::unMakeMove(move* m) {
	sideToMove = !sideToMove;

	char moveType = (*m).type;
	int originSquare = (*m).originSquare;
	int destSquare = (*m).destinationSquare;

	bitBoards& boards = (sideToMove) ? *wBitBoards : *bBitBoards;
	bitBoards& opponentBoards = (sideToMove) ? *bBitBoards : *wBitBoards;

	piece* pieces = (sideToMove) ? wPieces : bPieces;
	piece* opponentPieces = (sideToMove) ? bPieces : wPieces;

	piece* p;
	p = &pieces[(*m).pieceListIndex];

	u64* b = 0;
	u64* nb = 0;
	u64 mask = 1;

	bool promoting = 1;

	switch ((*p).type)
	{
	case 'p':
		b = &boards.pawns;
		break;
	case 'r':
		b = &boards.rooks;
		break;
	case 'n':
		b = &boards.knights;
		break;
	case 'b':
		b = &boards.bishops;
		break;
	case 'q':
		b = &boards.queens;
		break;
	case 'k':
		b = &boards.king;
		break;
	default:
		break;
	}

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
		promoting = 1;
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
		int rookSquare;
		int rookDest;
		int rookIndex;
		switch (destSquare) //condense
		{
		case (1):
			rookSquare = 0;
			rookDest = 2;
			rookIndex = 8;
			break;
		case (5):
			rookSquare = 7;
			rookDest = 4;
			rookIndex = 15;
			break;
		case (57):
			rookSquare = 56;
			rookDest = 58;
			rookIndex = 8;
			break;
		case (61):
			rookSquare = 63;
			rookDest = 60;
			rookIndex = 15;
			break;
		default:
			break;
		}
		boards.rooks &= ~(mask << rookDest);
		boards.all &= ~(mask << rookDest);
		boards.rooks |= (mask << rookSquare);
		boards.all |= (mask << rookSquare);

		pieces[rookIndex].square = rookSquare;
	}
	else if (moveType == 'e') {
		int s = destSquare + 8 - ((originSquare / 32) * 16);
		addPiece(!sideToMove, 'p', s, enPassantLane[turn - 1]);
	}

	if (promoting) {
		(*p).type = 'p';
	}

	(*p).square = originSquare;
	turn--;
}

void Position::generateControlBoards(int color) {
	piece* pieces = (color) ? wPieces : bPieces;
	bitBoards& boards = (color) ? *wBitBoards : *bBitBoards;
	u64 allPieces = wBitBoards->all | bBitBoards->all;

	boards.allControl = 0;

	int forward = (color) ? 1 : -1;
	u64 mask = 1;

	for (int i = 0; i < 16; i++) {
		int square = (pieces[i].square);
		switch (pieces[i].type)
		{
		case 'p':
			for (int n = 7; n <= 9; n += 2) {
				pieces[i].control |= mask << (square + forward * n);
			}
			break;
		case 'r':
			pieces[i].control = Data::getRookControl(square, allPieces);
			break;
		case 'n':
			pieces[i].control = Data::getKnightControl(square);
			break;
		case 'b':
			pieces[i].control = Data::getBishopControl(square, allPieces);
			break;
		case 'q':
			pieces[i].control = Data::getQueenControl(square, allPieces);
			break;
		case 'k':
			pieces[i].control = Data::getKingControl(square);
			break;
		default:
			break;
		}
		boards.allControl |= pieces[i].control;
	}
}

void Position::addPiece(int color, char type, int square, int index) {
	piece* pieces = (color) ? wPieces : bPieces;

	bitBoards& boards = (color) ? *wBitBoards : *bBitBoards;
	piece p = { type, square };
	pieces[index] = p;
	u64 place = 1; place <<= square;
	switch (type)
	{
	case 'p':
		boards.pawns |= place;
		boards.all |= place;
		break;
	case 'r':
		boards.rooks |= place;
		boards.all |= place;
		break;
	case 'n':
		boards.knights |= place;
		boards.all |= place;
		break;
	case 'b':
		boards.bishops |= place;
		boards.all |= place;
		break;
	case 'q':
		boards.queens |= place;
		boards.all |= place;
		break;
	case 'k':
		boards.king |= place;
		boards.all |= place;
		break;
	default:
		break;
	}
}

void Position::removePiece(int color, int index) {
	piece* pieces = (color) ? wPieces : bPieces;
	bitBoards& boards = (color) ? *wBitBoards : *bBitBoards;
	char type = pieces[index].type;
	u64 place = 1;
	place <<= (pieces[index].square);

	//cout << "removesPiece " << color << " " << "index: " << index << "\n";

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

	pieces[index].type = '\0';
	pieces[index].square = -1;
}

i32 Position::evaluate() {
	int result = 0;
	piece* pieces;
	bitBoards* boards;
	int side;
	for (int i = 0; i < 2; i++)
	{
		if (i) {
			pieces = wPieces;
			boards = wBitBoards;
			side = 1;
		}
		else {
			pieces = bPieces;
			boards = bBitBoards;
			side = -1;
		}

		for (int i = 0; i < 16; i++)
		{
			//countpieces
			char t = pieces[i].type;
			switch (t)
			{
			case 'p':
				result += side * 100;
				break;
			case 'r':
				result += side * 500;
				result += side * 4 * Data::getRookValue(pieces[i].square, allPieces);
				break;
			case 'n':
				result += side * 290;
				result += side * 8 * Data::getKnightValue(pieces[i].square);
				break;
			case 'b':
				result += side * 300;
				result += side * 6 * Data::getBishopValue(pieces[i].square, allPieces);
				break;
			case 'q':
				result += side * 900;
				result += side * Data::getQueenValue(pieces[i].square, allPieces);
				break;
			case 'k':
				result += side * 1000000000;
				break;
			default:
				break;
			}

		}
	}
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
		break;
	}
}

void Position::printMoves(move* moves, int moveCount) {
	int ss = -1;

	for (int i = 0; i < moveCount; i++) {
		piece* pieces = (sideToMove) ? wPieces : bPieces;
		move m = moves[i];
		int square = moves[i].originSquare;
		if (square != ss) {
			cout << "\n" << pieces[moves[i].pieceListIndex].type << " ";
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

	u64 mask = 1; mask <<= 63;
	cout << "\n    _________________________________";
	cout << "\n   |                                 |\n";
	for (int y = 7; y >= 0; y--) {
		cout << " " << (y + 1) << " | ";
		for (int x = 7; x >= 0; x--) {
			int shift = (63 - (y * 8 + x));
			char color = '\0', piece;
			for (int c = 0; c < 2; c++) {
				bitBoards& boards = (c) ? *wBitBoards : *bBitBoards;
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
}

void Position::test() {
	Position p;
	u64 perftResult = perft(6, &p);
	cout << "Result: " << perftResult << "\n";

	//Position::process();

	//cout << (char)('U'&'r') << "\n"; //P
	//cout << (char)('U'&'n') << "\n"; //D
	//cout << (char)('U'&'b') << "\n"; //@
	//cout << (char)('U'&'q') << "\n"; //Q
}

void Position::process() {
	Position p;
	int t = 0;
	move moveHistory[1000];
	while (true) {
		move moves[218];
		int moveCount = 0;
		int moveChoice = -2;
		p.print();
		p.generateMoves(moves, &moveCount);
		p.printMoves(moves, moveCount);

		while (moveChoice < -1 || moveChoice >= moveCount)
		{
			cin >> moveChoice;
		}

		if (moveChoice == -1 && t != 0)
		{
			p.unMakeMove(&moveHistory[t - 1]);
			t--;
		}
		else {
			p.makeMove(&(moves[moveChoice]));
			moveHistory[t] = moves[moveChoice];
			t++;
		}
	}
}

u64 Position::perft(int depth, Position* p) {
	u64 nodes = 0;

	if (depth == 0) {
		return 1;
	}

	move moves[218];
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