#include "pieces.h"
#include "position.h"
#include "data.h"
#include "move.h"
#include <time.h>

class Chess {
public:
	static int minimax(int depth, Position* p, Move* m = 0);

	static int alphaBetaSearch(int a, int b, int depth, Position* p, Move* m = 0);
	static int  qSearch(int a, int b, int depth, Position* p, Move* m = 0);

	static void test();
	static void test2();
	static void process();
private:


};