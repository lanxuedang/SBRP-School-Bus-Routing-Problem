#include "Stdafx.h"
#include "SBRPLib.h"

//find two student stops in different rotues ( i and j) to swap.
bool SwapBetweenRoutes::search_and_move(SBRP *S, int i, int j, int rules)
{
	if(i == j || S->route[i].used== false || S->route[j].used == false)
	{
		return false;
	}
	PairSwap pairswap;
	SBRPMove M,BestM;
	int best_m ,best_n;
	M.new_total_route_length=BestM.new_total_route_length=SBRPLib_INFINITY;
	BestM.saving = false;
	int start_i,start_j,m,n;
	start_i = m = S->route[i].start;
	start_j = n = S->route[j].start;

	int accept_type;
	accept_type=SBRPLib_FIRST_ACCEPT;

	if( rules & SBRPLib_FIRST_ACCEPT )
		accept_type=SBRPLib_FIRST_ACCEPT;
	if( rules & SBRPLib_BEST_ACCEPT )
		accept_type=SBRPLib_BEST_ACCEPT;
	if( rules & SBRPLib_LI_ACCEPT )
		accept_type = SBRPLib_LI_ACCEPT;

	while(m!=SBRPLib_ROUTE_OUTER)
	{
		while(n!=SBRPLib_ROUTE_OUTER)
		{
			if(S->nodes[m]->type == SBRPLib_STOP_NODE && S->nodes[n]->type == SBRPLib_STOP_NODE)
			{
				if(pairswap.evaluate(S,m,n,&M,&BestM,rules))
				{
					if(accept_type == SBRPLib_FIRST_ACCEPT || (accept_type == SBRPLib_LI_ACCEPT && M.saving))
					{
						if(pairswap.move(S,m,n,&M))
						{
							return true;
							//process later 

							//if(!(rules & VRPH_TABU))
							//	return true;
							//else
							//{
							//	// Check VRPH_TABU status of move - return true if its ok
							//	// or revert to old_sol if not and continue to search.
							//	if(V->check_tabu_status(&M, old_sol))
							//	{
							//		delete [] old_sol;
							//		return true; // The move was ok
							//	}
							//	// else we reverted back - continue the search for a move
							//}
						}
						else
						{
							return false;
							//error message
						}
					}
					if(accept_type == SBRPLib_LI_ACCEPT || accept_type == SBRPLib_BEST_ACCEPT)
					{
						if(M.is_better(&BestM,rules))
						{
							BestM = M;
							best_m = m;
							best_n = n;							
						}
					}
				}
			}
			n = S->get_next_node(n,j);
		}
		m = S->get_next_node(m,i);
	}

	if(BestM.new_total_route_length==SBRPLib_INFINITY || accept_type == SBRPLib_FIRST_ACCEPT)
	{
		if(rules & SBRPLib_TABU)
		{
			//process later

		}
		return false;
	}

	if((accept_type == SBRPLib_LI_ACCEPT || accept_type == SBRPLib_BEST_ACCEPT) && BestM.new_total_route_length!=SBRPLib_INFINITY)
	{
		if(pairswap.move(S,best_m,best_n,&BestM))
		{
			return true;
			//process later 

			//if(!(rules & VRPH_TABU))
			//	return true;
			//else
			//{
			//	// Check VRPH_TABU status of move - return true if its ok
			//	// or revert to old_sol if not and continue to search.
			//	if(V->check_tabu_status(&M, old_sol))
			//	{
			//		delete [] old_sol;
			//		return true; // The move was ok
			//	}
			//	// else we reverted back - continue the search for a move
			//}
		}
		else
		{
			return false;
			//error message
		}
	}

	return false;
}