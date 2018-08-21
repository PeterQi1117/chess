#ifndef BITBOARDS_H
#define BITBOARDS_H

typedef unsigned long long int u64;

class BitBoards
{
public:
	u64 all;
	u64 pawns, rooks, knights, bishops, queens, king;
	u64 allControl;
};

#endif 