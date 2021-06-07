
#ifndef _PAIR_RELOCATE_H
#define _PAIR_RELOCATE_H

//move a pair whithin one route
class PairRelocate
{
public:
	bool evaluate(class SBRP *S, int u, int i, int r, int k, SBRPMove *M);
	bool move(SBRP *S, int u, int i, int r, int k, SBRPMove *M);
};

#endif