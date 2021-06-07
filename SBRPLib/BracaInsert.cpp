#include "Stdafx.h"
#include "SBRPLib.h"

void BracaInsert::search(class SBRP *S, int u,int best_m,SBRPMove* moves)
{
	SBRPMove* M = new SBRPMove();		
	int accept_type;
	if(S->nodes[u]->type != SBRPLib_STOP_NODE)
		return ;

	// Determine acceptance type
	//default setting
	accept_type=SBRPLib_BEST_ACCEPT;

	// Create the search_space, rule:SPI_MOVE
	S->create_search_neighborhood(u, SBRPLib_Braca_Insert);    

	int best_positon=0;
	for(int m=0;m<S->search_size;m++)
	{	
		if(evaluate(S,u,S->search_space[m].i,S->search_space[m].r,S->search_space[m].k,M)==true)
		{
			insert_SBRPMove(M,moves,best_m);
		}
	}
	if(moves[0].new_total_number_of_routes == -1)
	{
		moves[0].move_arguments[0] = u;
	}
	delete M;
}

bool BracaInsert::evaluate(class SBRP *S, int u, int i, int r, int k, SBRPMove *M)
{
	//u == i ,return fasle
	if(u == i)
		return false;

	int i_route= r;	//becasue i may be in mutli route, so it's routnum use given r

	//first check pair order, i can't bigger than k
	if(S->before(k,i,r))
		return false;
	if(S->nodes[u]->type == SBRPLib_DEPOT_NODE)
		return false;

	//if i and k not in route r, return false
	if(!(S->node_in_route(i,r) && S->node_in_route(k,r)) )
		return false;

	int d = S->nodes[u]->destination_id;

	//if u in other route, d already in route r, not allowed insert d to different position
	//only when u and i in the same route, shift shcool postion
	if( !S->node_in_route(u,r) && S->node_in_route(d,r) && S->get_next_node(k,r) != d)
		return false;	

	//cant' move d before it's student stops
	if(S->node_in_route(d,r))
	{
		int ii = S->get_next_node(k,r);
		while(ii != SBRPLib_ROUTE_OUTER)
		{
			if(S->nodes[ii]->type == SBRPLib_STOP_NODE && S->nodes[ii]->destination_id == d)
				return false;
			ii = S->get_next_node(ii,r);
		}
	}

	//all must have be visited
	if(!( (S->nodes[i]->routed || i==S->route[r].deopt_id) && (S->nodes[k]->routed||k==S->route[r].deopt_id)))
		return false;

	double i_length;
	int i_time;

	i_length = S->route[i_route].single_length;
	i_time = S->route[i_route].single_time;

	S->InsertNode(u,i,i_route);
	if(i == k)
		k = u;
	S->InsertNode(d,k,i_route);	
	S->update_route_info(i_route);

	//now check feasiblity and saving
	bool result = S->check_constraints(i_route);	
	if(result)
	{
		M->num_affected_routes = 1;
		M->route_nums[0] = i_route;
		M->route_custs[0] = S->route[i_route].num_customers;
		M->new_total_route_length = S->total_route_length + (S->route[i_route].single_length - i_length);// + S->route[u_route].single_length - u_length);
		//M->new_total_service_time = S->total_service_time + (S->route[i_route].single_time - i_time);// + S->route[u_route].single_time - u_time);
		M->move_arguments[0]=u;
		M->move_arguments[1]=i;
		M->move_arguments[2]=r;
		M->move_arguments[3]=k;
		M->new_total_number_of_routes = S->total_number_of_routes;
	}

	//restore old solution
	S->RemoveNode(u,i_route);
	S->RemoveNode(d,i_route);	
	S->update_route_info(i_route);	

	return result;
}


void BracaInsert::insert_SBRPMove(SBRPMove* M,SBRPMove* moves,int best_m)
{
	int rules = SBRPLib_LENGTH_SAVING_ONLY;
	for(int i = 0;i<best_m;i++)
	{
		if(M->is_better(&moves[i],rules))
		{
			for(int j = best_m-1;j>i;j--)
			{
				moves[j] = moves[j-1];
			}
			moves[i] = *M;
			break;
		}
	}
}

bool BracaInsert::move(SBRP *S, SBRPMove *M)
{
	int u = M->move_arguments[0];
	int i = M->move_arguments[1];
	int r = M->move_arguments[2];
	int k = M->move_arguments[3];

	int d = S->nodes[u]->destination_id;

	//S->Show_one_route(r);
	S->InsertNode(u,i,r);	
	if(i == k)
		k = u;
	S->InsertNode(d,k,r);	
	S->update_route_info(r);
	//S->Show_one_route(r);
	S->update(M);

	return true;
}
