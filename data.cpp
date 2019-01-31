#include "data.h"

bool Data::ready = false;
int Data::bishopDirections[4][2] = {
	{ 1, 1 },{ -1, 1 },{ -1, -1 },{ 1, -1 }
};
int Data::rookDirections[4][2] = {
	{ 0, 1 },{ -1, 0 },{ 0, -1 },{ 1, 0 }
};
u64 Data::bMagicNumbers[64] = {
	361418285364019457, 2026622040234009216, 11278791385088640, 2326115276334041168,
	4108496995879094353, 302027186368414080, 577025909907915028, 36066283527147528,
	2347516716914967041, 40536811898142976, 2454471314581233670, 32097536177153156,
	4504768932233216, 297820875187552288, 72062577341893632, 288230668352884736,
	2742740585976693800, 299489392450601984, 153403896700862992, 284258270773256,
	19140444730688512, 4904982962431337728, 2814822823632896, 1176004527183298816,
	10203469318206464, 4652579055441805570, 378382684856058368, 4828426157802194976,
	585758241435172868, 4504767867389962, 563362279196673, 72602303463680,
	580834479316992, 5086409511672068, 18599347352766464, 581562520618205266,
	4620706429003710722, 290279659676160, 1127008550256776, 37437276248146944,
	5631184373563408, 573953693261824, 844975910948866, 3648206106997557256,
	648588727987470864, 1242995705891522064, 9079784273346816, 73192295410696448,
	11830750618322944, 6926571978472628233, 1233988569946128658, 4583869890691136,
	144159477829206017, 1155525251237478400, 20268449021952000, 216740135281688674,
	4899952300539797508, 587147816210944, 144115196687157248, 6920132739920561168,
	144678172529591552, 1134140866690, 2287396569809232, 18018813744382096,
};
u64 Data::rMagicNumbers[64] = {
	3638910972838552704, 4629700554395746496, 36063981659494784, 36033263819030656,
	144124396555075600, 2522024617485533186, 72093346436219392, 36029896599814784,
	579275665283809408, 4613093668264354048, 36310551236714496, 3765290802666934272,
	563087665397768, 578149670950404104, 3837348499231605248, 10414584347295746,
	36029071966077650, 4521466695786496, 22520747192754176, 67554544837464192,
	2252349703979136, 282574623342594, 720580338459412624, 739155487872188548,
	900728997518778368, 35211215654918, 22870945666433169, 283437778866176,
	9297474619768848, 162692613849809936, 28673202196497, 316942817951889,
	1152991873896284296, 1225054002878615552, 145258817624490240, 4613392606536155144,
	11260115768313985, 576742244476782622, 72356721934205256, 19140590776093057,
	324269352780578816, 4538785341784066, 27303210181001282, 1407718506692641,
	2252920800739344, 2533310827921410, 360587042759114760, 83883942188548097,
	739787166036533760, 1157434175207243840, 35734429909504, 602738547235072,
	1162491688443453952, 2027182817435452928, 576478366100751360, 281519075754752,
	36383943846527009, 36099238780076162, 35188684370113, 144151473184591873,
	4757490641026166850, 1971037868660753, 8804700782724, 101331309997326470,
};

u64 Data::rMasks[64], Data::bMasks[64];

int Data::rBits[64], Data::bBits[64];

u64 Data::nControlBoards[64], Data::kControlBoards[64];
int Data::nValues[64];
u64* Data::rControlBoards = new u64[64 * 4096];
u64* Data::bControlBoards = new u64[64 * 512];
int* Data::rValues = new int[64 * 4096];
int* Data::bValues = new int[64 * 512];

int* Data::bishopPathLengths = new int[64 * 4];
int* Data::nMoves = new int[64 * 8];
int* Data::kMoves = new int[64 * 8];

int Data::nMovesLengths[64] = {};
int Data::kMovesLengths[64] = {};

void Data::bitBoardToGrid(int grid[8][8], u64 bitBoard) {
	for (int yc = 0; yc < 8; yc++) {
		for (int xc = 0; xc < 8; xc++) {
			grid[xc][yc] = bitBoard << 63 >> 63;
			bitBoard >>= 1;
		}
	}
}

u64 Data::blockerBoardToMoveBoard(u64 blockerBoard, int square, int rook) {
	int x = square % 8, y = square / 8;
	int grid[8][8];
	bitBoardToGrid(grid, blockerBoard);
	int xs[14], ys[14];
	int length = 0;
	for (int d = 0; d < 4; d++) {
		int xDirection = (rook) ? rookDirections[d][0] : bishopDirections[d][0];
		int yDirection = (rook) ? rookDirections[d][1] : bishopDirections[d][1];
		int n = 1;
		while (true) {
			int xn = x + n * xDirection;
			int yn = y + n * yDirection;
			n++;
			if ((xn >= 0) && (xn <= 7) && (yn >= 0) && (yn <= 7)) {
				xs[length] = xn;
				ys[length] = yn;
				length++;
				if (grid[xn][yn] == 1) {
					break;
				}
			}
			else {
				break;
			}
		}
	}
	return coordinatesToBitBoard(xs, ys, length);
}

u64 Data::coordinatesToBitBoard(int xs[], int ys[], int length, bool s, int sequence[]) {
	u64 board = 0;
	for (int i = 0; i < length; i++) {
		int square = ys[i] * 8 + xs[i];
		u64 b = (s) ? sequence[i] : 1;
		board |= (b << square);
	}
	return board;
}

int Data::intPow(int number, int exponent) {
	int result = 1;
	for (int i = 0; i < exponent; i++) {
		result *= number;
	}
	return result;
}


u64 Data::randomMagic() {
	u64 magic = 1;
	for (int i = 0; i < 64; i++) {
		magic <<= 1;
		magic |= 1;
	}
	for (int i = 0; i < 3; i++) {
		u64 sMagic = 0;
		for (int j = 0; j < 4; j++) {
			sMagic |= ((u64)rand() << (16 * j));
		}
		magic &= sMagic;
	}
	return magic;
}

void Data::initSlidingPieces(bool newMagics) {
	int rMaskCoords[64][12][2], bMaskCoords[64][9][2];
	int permutationSequence[12] = {};
	int rPermutations[64] = {}, bPermutations[64] = {};
	u64 *rBlockers = new u64[4096], *bBlockers = new u64[512];
	u64 *rOrderedControlBoards = new u64[4096], *bOrderedControlBoards = new u64[512];

	for (int s = 0; s < 64; s++) {
		int x = s % 8, y = s / 8;
		int xs[12], ys[12];
		int length = 0;
		for (int i = 1; i <= 6; i++) {
			if (i != x) {
				xs[length] = rMaskCoords[s][length][0] = i;
				ys[length] = rMaskCoords[s][length][1] = y;
				length++;
			}
			if (i != y) {
				xs[length] = rMaskCoords[s][length][0] = x;
				ys[length] = rMaskCoords[s][length][1] = i;
				length++;
			}
		}
		rMasks[s] = 0 + coordinatesToBitBoard(xs, ys, length);//
		rBits[s] = length;
		length = 0;
		for (int d = 0; d < 4; d++) {
			int xDirection = bishopDirections[d][0];
			int yDirection = bishopDirections[d][1];
			for (int i = 1; i <= 6; i++) {
				int xn = x + xDirection * i;
				int yn = y + yDirection * i;
				if (0 < xn && xn < 7 && 0 < yn && yn < 7) {
					xs[length] = bMaskCoords[s][length][0] = xn;
					ys[length] = bMaskCoords[s][length][1] = yn;
					length++;
				}
			}
		}
		bMasks[s] = 0 + coordinatesToBitBoard(xs, ys, length);
		bBits[s] = length;
	}

	for (int rook = 0; rook <= 1; rook++) {
		for (int s = 0; s < 64; s++) {

			int length = (rook) ? rBits[s] : bBits[s];
			int* permutations = (rook) ? rPermutations : bPermutations;
			permutations[s] = intPow(2, length);
			u64* blockers = (rook) ? rBlockers : bBlockers;

			for (int p = 0; p < permutations[s]; p++) {
				int pCopy = p;
				for (int j = 0; j < length; j++) {
					permutationSequence[j] = pCopy % 2;
					pCopy >>= 1;
				}
				int xs[12], ys[12];
				for (int l = 0; l < length; l++) {
					xs[l] = (rook) ? rMaskCoords[s][l][0] : bMaskCoords[s][l][0];
					ys[l] = (rook) ? rMaskCoords[s][l][1] : bMaskCoords[s][l][1];
				}
				blockers[p] = coordinatesToBitBoard(xs, ys, length, true, permutationSequence);
			}

			u64* orderedControlBoards = (rook) ? rOrderedControlBoards : bOrderedControlBoards;
			for (int i = 0; i < permutations[s]; i++) {
				orderedControlBoards[i] = blockerBoardToMoveBoard(blockers[i], s, ((rook) ? 1 : 0));
			}

			int n = (rook) ? rPermutations[s] : bPermutations[s];
			int b = (rook) ? rBits[s] : bBits[s];
			for (int i = 0; i < n; i++) {
				u64 moveBoard = (rook) ? rOrderedControlBoards[i] : bOrderedControlBoards[i];
				u64 magic = (rook) ? rMagicNumbers[s] : bMagicNumbers[s];
				int index = (blockers[i] * magic) >> (64 - b);
				if (rook) {
					rControlBoards[s * 4096 + index] = moveBoard;
					//
					rValues[s * 4096 + index] = count(moveBoard);
				}
				else {
					bControlBoards[s * 512 + index] = moveBoard;
					//
					bValues[s * 512 + index] =  count(moveBoard);
				}
			}

			if (newMagics) {
				u64 magic;
				bool found = false;
				int n = (rook) ? rPermutations[s] : bPermutations[s];
				int b = (rook) ? rBits[s] : bBits[s];
				int i = 0, max = 1000000;
				while ((!found) && (i++ < max)) {
					u64 m[4096] = {};
					magic = randomMagic();
					for (int j = 0; j <= n; j++) {
						if (j == n) {
							found = true;
							break;
						}
						u64 blocker = (rook) ? rBlockers[j] : bBlockers[j];
						u64 moveBoard = (rook) ? rOrderedControlBoards[j] : bOrderedControlBoards[j];
						int index = (blocker * magic) >> (64 - b);
						if (m[index] == 0) {
							m[index] = moveBoard;
						}
						else if (m[index] != moveBoard) {
							break;
						}
					}
				}
				if (found) {
					u64 *index = (rook) ? &rMagicNumbers[s] : &bMagicNumbers[s];
					*index = magic;
					if (s == 0) {
						char piece = (rook) ? 'r' : 'b';
						cout << "uint64 Data::" << piece << "MagicNumbers[64] = {\n";
					}
					cout << magic << ", ";
					if ((s + 1) % 4 == 0) {
						cout << "\n";
					}
					if (s == 63) {
						cout << "};\n";
					}
				}
				else {
					std::cerr << "None found.\n";
				}
			}
		}
	}
	for (int s = 0; s < 64; s++) {
		int x = s % 8, y = s / 8;
		int ds[4] = { min(7 - x, 7 - y), min(x, 7 - y), min(x, y), min(7 - x, y) };
		for (int d = 0; d < 4; d++) {
			bishopPathLengths[s * 4 + d] = ds[d];
		}
	}
	delete[] rBlockers; delete[] bBlockers;
	delete[] rOrderedControlBoards, delete[] bOrderedControlBoards;
}

void Data::initKnightsAndKings() {
	int knightSquares[8][2] = {
		{ 1,2 },{ 2,1 },{ 2,-1 },{ 1,-2 },{ -1,-2 },{ -2,-1 },{ -2,1 },{ -1,2 }
	};
	int kingSquares[8][2] = {
		{ 0,1 },{ 1,1 },{ 1,0 },{ 1,-1 },{ 0,-1 },{ -1,-1 },{ -1,0 },{ -1,1 }
	};
	for (int knight = 0; knight <= 1; knight++) {
		for (int s = 0; s < 64; s++) {
			int x = s % 8, y = s / 8;
			int xs[8], ys[8];
			int length = 0;
			u64 board = 0;
			for (int i = 0; i < 8; i++) {
				int xn = x + ((knight) ? knightSquares[i][0] : kingSquares[i][0]);
				int yn = y + ((knight) ? knightSquares[i][1] : kingSquares[i][1]);
				if ((xn >= 0) && (xn <= 7) && (yn >= 0) && (yn <= 7)) {
					xs[length] = xn;
					ys[length] = yn;
					length++;
				}
			}
			if (knight) {
				u64 b = coordinatesToBitBoard(xs, ys, length);
				nControlBoards[s] = b;
				nValues[s] = count(b);
			}
			else {
				kControlBoards[s] = coordinatesToBitBoard(xs, ys, length);
			}

		}
	}
	for (int s = 0; s < 64; s++) {
		u64 nControl = nControlBoards[s];
		u64 kControl = kControlBoards[s];
		u64 mask = 1;
		for (int st = 0; st < 64; st++) {
			if ((nControl >> st) & mask) {
				nMoves[s * 8 + nMovesLengths[s]] = st;
				nMovesLengths[s]++;
			}
			if ((kControl >> st) & mask) {
				kMoves[s * 8 + kMovesLengths[s]] = st;
				kMovesLengths[s]++;
			}
		}
	}
}

int Data::count(u64 board) {
	int result = 0;
	for (int i = 0; i < 64; i++)
	{
		if (board & (u64)1)
		{
			result++;
		}
		board >>= 1;
	}
	return result;
}

u64 Data::getBishopControl(int square, u64 allPieces) {
	u64 blocker = bMasks[square] & allPieces;
	int index = (bMagicNumbers[square] * blocker) >> (64 - bBits[square]);
	return bControlBoards[square * 512 + index];
}
u64 Data::getRookControl(int square, u64 allPieces) {
	u64 blocker = rMasks[square] & allPieces;
	int index = (rMagicNumbers[square] * blocker) >> (64 - rBits[square]);
	return rControlBoards[square * 4096 + index];
}
u64 Data::getKnightControl(int square) {
	return nControlBoards[square];
}
u64 Data::getKingControl(int square) {
	return kControlBoards[square];
}
u64 Data::getQueenControl(int square, u64 allPieces) {
	return (getBishopControl(square, allPieces) | getRookControl(square, allPieces));
}



int Data::getBishopMobility(int square, u64 allPieces) {
	u64 blocker = bMasks[square] & allPieces;
	int index = (bMagicNumbers[square] * blocker) >> (64 - bBits[square]);
	return bValues[square * 512 + index];
}
int Data::getRookMobility(int square, u64 allPieces) {
	u64 blocker = rMasks[square] & allPieces;
	int index = (rMagicNumbers[square] * blocker) >> (64 - rBits[square]);
	return rValues[square * 4096 + index];
}
int Data::getKnightMobility(int square) {
	return nValues[square];
}
int Data::getQueenMobility(int square, u64 allPieces) {
	return (getBishopMobility(square, allPieces) + getRookMobility(square, allPieces));
}



void Data::init(bool newMagics) {
	if (!ready) {
		initSlidingPieces(newMagics);
		initKnightsAndKings();
		ready = true;
	}
}

void Data::print(u64 board) {
	for (int y = 8; y > 0; y--) {
		u64 row = board << ((8 - y) * 8);
		for (int x = 8; x > 0; x--) {
			u64 square = (row << (8 - x) >> 7 >> 56);
			cout << square << " ";
		}
		cout << "\n";
	}
}

void Data::test(bool newMagics) {
	Data::init(newMagics);
	for (int i = 0; i < 10; i++) {
		cout << "\n---------------\n\n";
		u64 m = randomMagic();
		print(m);
		cout << "\n";
		print(getQueenControl(44, m));
	}
}
