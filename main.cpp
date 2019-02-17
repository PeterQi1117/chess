#include "main.h"


int main() {
	Data::init(false);

	//Chess::test();
	Position::test();
	system("pause");
	return 0;
}

int Chess::minimax(int depth, Position* p, Move* m) {
	if (depth == 0) {
		return (*p).evaluate();
	}
	Move moves[218];
	int indexOfStongestMove = 0;
	int moveCount = 0;
	(*p).generateMoves(moves, &moveCount);
	if (moveCount == 0) {
		return (*p).evaluate();
	}
	int evaluation = (*p).getSideToMove() ? INT32_MIN : INT32_MAX;
	for (int i = 0; i < moveCount; i++) {
		(*p).makeMove(&(moves[i]));
		int moveValue = minimax(depth - 1, p);
		(*p).unMakeMove(&(moves[i]));
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
	}
	if (m != 0) {
		*m = moves[indexOfStongestMove];
	}
	return evaluation;
}


int Chess::alphaBetaSearch(int a, int b, int depth, Position* p, Move* m) {
	if (depth == 0) {
		return qSearch(a, b, 6, p);
	}
	Move* moves = new Move[218];
	int indexOfStongestMove = 0;
	int moveCount = 0;
	(*p).generateMoves(moves, &moveCount);
	if (moveCount == 0) {
		delete[] moves;
		return (*p).evaluate();
	}
	for (int i = 0; i < moveCount; i++) {
		(*p).makeMove(&(moves[i]));
		int value = alphaBetaSearch(a, b, depth - 1, p);
		(*p).unMakeMove(&(moves[i]));
		if ((*p).getSideToMove()) {
			if (value >= b) {
				delete[] moves;
				return b;
			}
			if (value > a) {
				a = value;
				indexOfStongestMove = i;
			}
		}
		else {
			if (value <= a) {
				delete[] moves;
				return a;
			}
			if (value < b) {
				b = value;
				indexOfStongestMove = i;
			}
		}
	}
	if (m != 0) {
		*m = moves[indexOfStongestMove];
	}
	delete[] moves;
	return ((*p).getSideToMove()) ? a : b;
}

int Chess::qSearch(int a, int b, int depth, Position* p, Move* m) {
	if (depth == 0) {
		return (*p).evaluate();
	}
	Move* moves = new Move[218];
	int indexOfStongestMove = 0;
	int moveCount = 0;
	(*p).generateMoves(moves, &moveCount, true);

	if (moveCount == 0) {
		delete[] moves;
		return (*p).evaluate();
	}

    for (int i = 0; i < moveCount; i++) {
        (*p).assessMove(&moves[i]);
    }

    Move** pMoves = new Move*[moveCount];
    for (int i = 0; i < moveCount; i++) {
        pMoves[i] = &moves[i];
    }
    //std::sort(moves, moves + moveCount, compareMoves);
    //sortMoves(0, moveCount, pMoves);

	for (int i = 0; i < moveCount; i++) {
		(*p).makeMove(pMoves[i]);
		int value = qSearch(a, b, depth - 1, p);
		(*p).unMakeMove(pMoves[i]);
		if ((*p).getSideToMove()) {
			if (value >= b) {
				delete[] moves;
				return b;
			}
			if (value > a) {
				a = value;
				indexOfStongestMove = i;
			}
		}
		else {
			if (value <= a) {
				delete[] moves;
				return a;
			}
			if (value < b) {
				b = value;
				indexOfStongestMove = i;
			}
		}
	}
	if (m != 0) {
		*m = moves[indexOfStongestMove];
	}
	delete[] moves;
	return ((*p).getSideToMove()) ? a : b;
}

void Chess::sortMoves(int l, int r, Move** pMoves) {
    if (l >= r) {
        return;
    }
    int pivot = (*pMoves[r]).value;

}

bool Chess::compareMoves(Move m1, Move m2) {
    return m1.value > m2.value;
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
	//Chess::process();
	Chess::test2();

	int a = INT32_MAX;
	cout << a << "======" << -a << "======\n";

	t2 = clock();
	diff = ((float)t2 - (float)t1);
	cout << "Test: " << diff / CLOCKS_PER_SEC << " seconds.\n" << "\n";
}

void Chess::test2() {
	Position p;
	int t = 0;

	Move* moveHistory = new Move[500];
	for (int i = 0; i < 20; i++) {
		Move moves[218];
		int moveCount = 0;
		//p.print();
		p.generateMoves(moves, &moveCount);
		int a = INT32_MIN + 1;
		int b = INT32_MAX;
		if (moveCount > 0) {
			Move engineMove;
			int evaluation = Chess::alphaBetaSearch(a, b, 4, &p, &engineMove);
			//int evaluation = Chess::minimax(4, &p, &engineMove);
			cout << "\nevaluation: " << evaluation << "\n";
			p.makeMove(&engineMove);
			p.print();
			moveHistory[t] = engineMove;
			t++;
		}
		else {
			break;
		}

	}
	delete[] moveHistory;
}

void Chess::process() {
	Position p;
	int t = 0;
	Move* moveHistory = new Move[500];
	int a = INT32_MIN + 1;
	int b = INT32_MAX;
	while (true) {
		Move moves[218];
		int moveCount = 0;

		p.print();

		if (t % 2 == 0)
		{
			int moveChoice = -3;
			p.generateMoves(moves, &moveCount);
			p.printMoves(moves, moveCount);
			while (moveChoice < -2 || moveChoice >= moveCount)
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
                return;
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
			Move engineMove;
			int evaluation = Chess::alphaBetaSearch(a, b, 6, &p, &engineMove);
            std::cout << evaluation << '\n';
			p.makeMove(&engineMove);
			moveHistory[t] = engineMove;
			t++;
		}
	}
	delete[] moveHistory;
}
