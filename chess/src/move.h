#ifndef MOVE_H
#define MOVE_H

class Move
{
public:
	char type;
	int pieceListIndex;
	int destinationSquare;
	int originSquare;
	char pieceCaptured;
	int capturedIndex;

	Move() {}

	Move(char type, int pieceListIndex, int destinationSquare,
		int  originSquare) {
		this->type = type;
		this->pieceListIndex = pieceListIndex;
		this->destinationSquare = destinationSquare;
		this->originSquare = originSquare;
        this->pieceCaptured = '\0';
	}

	Move(const Move& m) {
		this->type = m.type;
		this->pieceListIndex = m.pieceListIndex;
		this->destinationSquare = m.destinationSquare;
		this->originSquare = m.originSquare;
		this->pieceCaptured = m.pieceCaptured;
		this->capturedIndex = m.capturedIndex;
	}
};

#endif
