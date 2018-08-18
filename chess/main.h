#include "data.h"
#include "position.h"
#include <time.h>

class Chess {
public:
	static i32 minimax(int depth, Position* p, move* m = 0);
	static i32 minimax2(int depth, Position* p, move* m = 0);

	static void test();
	static void test2();
	static void process();
private:


};