#include "main.h"


int main() {
	Chess::test();

	system("pause");
	return 0;
}

i32 Chess::minimax(int depth, Position* p, move* m) {

	if (depth == 0) {
		return (*p).evaluate();
	}

	move moves[218];
	int indexOfStongestMove = 0;
	int moveCount = 0;

	(*p).generateMoves(moves, &moveCount);

	if (moveCount == 0) {
		return (*p).evaluate();
	}

	i32 evaluation = (*p).getSideToMove() ? INT32_MIN : INT32_MAX;

	for (int i = 0; i < moveCount; i++) {
		(*p).makeMove(&(moves[i]));
		int moveValue = minimax(depth - 1, p);
		(*p).unMakeMove(&(moves[i]));
		//
		if ((*p).getSideToMove()) {
			if (moveValue > evaluation) {
				indexOfStongestMove = i;
				evaluation = moveValue;
			}
		}
		else {
			if (moveValue < evaluation) {
				indexOfStongestMove = i;
				evaluation = moveValue;
			}
		}
		//
	}

	if (m != 0) {
		*m = moves[indexOfStongestMove];
	}

	return evaluation;
}


i32 Chess::minimax2(int depth, Position* p, move* m) {

	if (depth == 0) {
		return (*p).evaluate();
	}

	move moves[218];
	int indexOfStongestMove = 0;
	int moveCount = 0;

	(*p).generateMoves(moves, &moveCount);

	if (moveCount == 0) {
		return (*p).evaluate();
	}

	i32 evaluation = (*p).getSideToMove() ? INT32_MIN : INT32_MAX;

	for (int i = 0; i < moveCount; i++) {
		(*p).makeMove(&(moves[i]));
		int moveValue = minimax(depth - 1, p);
		(*p).unMakeMove(&(moves[i]));
		//
		if ((*p).getSideToMove()) {
			if (moveValue > evaluation) {
				indexOfStongestMove = i;
				evaluation = moveValue;
			}
		}
		else {
			if (moveValue < evaluation) {
				indexOfStongestMove = i;
				evaluation = moveValue;
			}
		}
		//
	}

	if (m != 0) {
		*m = moves[indexOfStongestMove];
	}

	return evaluation;
}

void Chess::test() {
	clock_t t1, t2;	float diff;
	t1 = clock();

	Data::init(false);

	t2 = clock();
	diff = ((float)t2 - (float)t1);
	cout << "Initialization: " << diff / CLOCKS_PER_SEC << " seconds.\n" << "\n";

	t1 = clock();

	//Position::test();
	//Position::process();
	//Position p;
	//p.print();

	Chess::process();
	//Chess::test2();

	t2 = clock();
	diff = ((float)t2 - (float)t1);
	cout << "Test: " << diff / CLOCKS_PER_SEC << " seconds.\n" << "\n";
}

void Chess::test2() {
	Position p;
	int t = 0;
	move moveHistory[1000];
	for (int i = 0; i < 20; i++) {
		move moves[218];
		int moveCount = 0;
		p.print();
		p.generateMoves(moves, &moveCount);
		//p.printMoves(moves, moveCount);
		if (moveCount > 0) {
			move engineMove;
			int evaluation = Chess::minimax(4, &p, &engineMove);
			cout << "\nevaluation: " << evaluation << "\n";
			p.makeMove(&engineMove);
			moveHistory[t] = engineMove;
			t++;
		}
		else {
			break;
		}

	}
}

void Chess::process() {
	Position p;
	int t = 0;
	move moveHistory[1000];
	while (true) {
		move moves[218];
		int moveCount = 0;

		p.print();

		if (t % 2 == 0)
		{
			int moveChoice = -2;
			p.generateMoves(moves, &moveCount);
			p.printMoves(moves, moveCount);		
			while (moveChoice < -1 || moveChoice >= moveCount)
			{
				cin >> moveChoice;
			}
			if (moveChoice == -1)
			{
				p.unMakeMove(&moveHistory[t - 1]);
				t--;
				p.unMakeMove(&moveHistory[t - 1]);
				t--;
			}
			else if (moveChoice == -2)
			{
				move engineMove;
				int evaluation = Chess::minimax(4, &p, &engineMove);
				p.makeMove(&engineMove);
				moveHistory[t] = engineMove;
				t++;
			}
			else {
				p.makeMove(&(moves[moveChoice]));
				moveHistory[t] = moves[moveChoice];
				t++;
			}
		}
		else
		{
			//p.generateMoves(moves, &moveCount);
			//p.printMoves(moves, moveCount);
			move engineMove;
			int evaluation = Chess::minimax(4, &p, &engineMove);
			//cout << "======" << engineMove.destinationSquare << "======";

			p.makeMove(&engineMove);
			moveHistory[t] = engineMove;
			t++;
		}
	}
}