
#ifndef _Braca_INSERT_H
#define _Braca_INSERT_H

//insert a pair(unvisited) to one  route
class BracaInsert
{
public:
	//return best_m insert positons(SBRPMove[]) 
	void search(class SBRP *S, int u,int best_m, SBRPMove* moves);
	bool evaluate(class SBRP *S, int u, int i, int r, int k, SBRPMove *M);
	bool move(SBRP *S, SBRPMove *M);
	void insert_SBRPMove(SBRPMove* M,SBRPMove* moves,int best_m);
};

#endif