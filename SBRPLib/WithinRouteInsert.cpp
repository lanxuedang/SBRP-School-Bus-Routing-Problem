
#include "Stdafx.h"
#include "SBRPLib.h"

bool WithinRouteInsert::search_and_move(class SBRP *S, int j, int rules)
{
	SBRPMove M;
	SBRPMove BestM;
	PairRelocate pairrelocate;
	M.new_total_route_length=BestM.new_total_route_length=SBRPLib_INFINITY;
			
	int accept_type;
	if(S->nodes[j]->type != SBRPLib_STOP_NODE)
		return false;
	int r = S->get_stopnode_route_num(j);
	
	
	if(S->route[r].num_customers<1)
		return false;

	// Determine acceptance type
	//default setting
	accept_type=SBRPLib_FIRST_ACCEPT;

	if( rules & SBRPLib_FIRST_ACCEPT )
		accept_type=SBRPLib_FIRST_ACCEPT;
	if( rules & SBRPLib_BEST_ACCEPT )
		accept_type=SBRPLib_BEST_ACCEPT;
	if( rules & SBRPLib_LI_ACCEPT )
	{
		//not process, still use SBRPLib_FIRST_ACCEPT
	}

	// Create the search_space, rule:SPI_MOVE
	S->create_search_neighborhood(j, SBRPLib_WRI_MOVE);    
	
	if(rules & SBRPLib_TABU)
	{
		// not process , implement later
	}
	int i,k;
	int best_positon=0;
	for(int m=0;m<S->search_size;m++)
	{
		i=S->search_space[m].i;
		k=S->search_space[m].k;
		if(pairrelocate.evaluate(S,j,i,r,k,&M)==true)
		{
			// Feasible move found
			if(accept_type==SBRPLib_FIRST_ACCEPT || (accept_type==SBRPLib_LI_ACCEPT && M.saving) )
			{
				if(pairrelocate.move(S,j,i,r,k,&M)==false)
					report_error("%s: move error 1\n",__FUNCTION__);
				else
				{
					if(!(rules & SBRPLib_TABU))
						return true;
					else
					{
						// not process , implement later

						//if(V->check_tabu_status(&M, old_sol))
						//{
						//	delete [] old_sol;
						//	return true; // The move was ok
						//}
					}
				}
			}

			if(accept_type==SBRPLib_BEST_ACCEPT || accept_type==SBRPLib_LI_ACCEPT)
			{
				// store the move

				if(M.is_better(&BestM, rules))
				{
					best_positon=m;
					BestM=M;
				}
			}                
		}
	}        


	// We've considered all the possibilities now...
	if(accept_type==SBRPLib_FIRST_ACCEPT || BestM.new_total_route_length == SBRPLib_INFINITY)
	{
		// No moves found

		/*if(old_sol)
			delete [] old_sol;*/
		return false;
	}

	// else we found a move - try to make it


	if(pairrelocate.move(S,j,S->search_space[best_positon].i,r,S->search_space[best_positon].k,&BestM)==true)
	{
		if(!(rules & SBRPLib_TABU))
			return true;                    
	}

	if(rules & SBRPLib_TABU)
	{    
		// Check VRPH_TABU status of move - return true if its ok
		// or revert to old_sol if not and return

		//implete later
		//if(V->check_tabu_status(&BestM, old_sol))// was &M??
		//{
		//	delete [] old_sol;
		//	return true; // The move was ok
		//}
		//else
		//{
		//	delete [] old_sol;
		//	return false;
		//}
	}

	// Should have already returned
	report_error("%s: move error 4\n",__FUNCTION__);    

	return false;
}