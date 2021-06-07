#include "Stdafx.h"
#include "SBRPLib.h"

//move pair within a route
bool PairRelocate::evaluate(class SBRP *S, int u, int i, int r, int k, SBRPMove *M)
{
	//u -sudent depot; insert after node i; destination of u insert afer k
	//r- routenum; i and k must in route r;
	
	//all must have be visited
	int d = S->nodes[u]->destination_id;
	if(!(S->nodes[u]->routed && (S->nodes[i]->routed || i==S->route[r].deopt_id) && (S->nodes[k]->routed||k==S->route[r].deopt_id) && S->nodes[d]->routed))
		return false;
	if(S->get_next_node(u,r) == d && d == i && d==k)
	{
		return false;
	}
	//if u, i and k not in route r, return false
	if(!(S->node_in_route(u,r) && S->node_in_route(i,r) && S->node_in_route(k,r)) )
		return false;

	//first check pair order, i can't bigger than k
	if(S->before(k,i,r))
		return false;

	if(S->nodes[u]->type == SBRPLib_DEPOT_NODE)
		return false;


	if(S->nodes[i]->type == SBRPLib_STOP_NODE && S->nodes[u]->type == SBRPLib_STOP_NODE)
	{
		int d1 = S->nodes[u]->destination_id;
		int d2 = S->nodes[i]->destination_id;

		if((S->tmatrix[d1][d2] <= abs(S->nodes[d1]->end_tw - S->nodes[d2]->start_tw) || S->tmatrix[d1][d2] <= abs(S->nodes[d2]->end_tw - S->nodes[d1]->start_tw))&& (S->nodes[i]->demand + S->nodes[u]->demand <= S->max_veh_capacity) && (S->tmatrix[i][u] <= S->max_riding_time))
		{
					
		}
		else
		{
			return false;
		}
	}


	if((S->get_next_node(i,r) == u || u==i) && (S->get_next_node(k,r) == d || k==d))
		return false;

	//u will be relocate. so k will be change.
	if(u == k)
	{
		return false;
	}

	//can't move d before it's student stops
	
	int ii = S->get_next_node(k,r);
	while(ii != SBRPLib_ROUTE_OUTER)
	{
		if(S->nodes[ii]->type == SBRPLib_STOP_NODE && S->nodes[ii]->destination_id == d)
			return false;
		ii = S->get_next_node(ii,r);
	}

	VisitedRecord *vr_u ,*vr_d;

	vr_u = S->get_visitedrecord_inroute(u,r);
	vr_d = S->get_visitedrecord_inroute(d,r);

	double r_length;
	int r_time;
	r_length = S->route[r].single_length;	
	r_time = S->route[r].single_time;	

	int old_u_pre = S->get_pre_node(u,r);
	int old_u_next = S->get_pre_node(u,r);
	int old_d_pre = S->get_pre_node(d,r);
	int old_d_next = S->get_next_node(d,r);
	
	S->MoveNodeInRoute(vr_u,i,r);
	if(i == k)
		k = u;
	S->MoveNodeInRoute(vr_d,k,r);
	
	S->update_route_info(r);

	M->num_affected_routes = 1;
	M->route_nums[0] = r;
	M->route_custs[0] = S->route[r].num_customers;
	M->new_total_route_length = S->total_route_length + (S->route[r].single_length - r_length);
	//M->new_total_service_time = S->total_service_time + (S->route[r].single_time - r_time);
	M->new_total_number_of_routes = S->total_number_of_routes;		
	
	//now check feasiblity and saving
	bool result = S->check_constraints(r);
	M->saving = S->check_saving(M,SBRPLib_WRI_MOVE);
	result = result & M->saving;
	//restore old solution
	S->MoveNodeInRoute(vr_u,old_u_pre,r);
	S->MoveNodeInRoute(vr_d,old_d_pre,r);	
	S->update_route_info(r);	
	//result = true;
	if(M->saving) S->WRICount++;
	return result;
}

bool PairRelocate::move(SBRP *S, int u, int i, int r, int k, SBRPMove *M)
{
	VisitedRecord *vr_u,*vr_d;
	
	int d = S->nodes[u]->destination_id;
	vr_u = S->get_visitedrecord_inroute(u,r);
	vr_d = S->get_visitedrecord_inroute(d,r);
	S->MoveNodeInRoute(vr_u,i,r);
	if(i == k)
		k = u;
	S->MoveNodeInRoute(vr_d,k,r);

	S->update_route_info(r);
	
	/*S->Show_one_route(r);	*/
	S->update(M);
	S->WRIOK++;
	return true;
}





