#include "Stdafx.h"
#include "SBRPLib.h"

//implement park alogrithm
void SBRP::minimize_route()
{
	store_solution(old_solution);
	for(int i = 0;i<num_nodes;)
	{
		if(route[i].used)
		{			
			if(!merge_route(i))
			{
				load_solution(old_solution);
				i++;
			}
			else
			{
				store_solution(old_solution);
				i = 0;
				continue;
			}
		}
		else
		{
			i++;
		}
	}
}

bool SBRP::merge_route(int r)
{
	if(!route[r].used)
		return false;

	int *perm;
	perm=new int[this->num_nodes];
	int p = -1;
	int j = route[r].start;
	while(j != SBRPLib_ROUTE_OUTER)
	{
		if(nodes[j]->type == SBRPLib_STOP_NODE)
		{
			perm[++p] = j;
		}
		j = get_next_node(j,r);
	}
	int student_Stop_num = p + 1;
	int rules = SBRPLib_BEST_ACCEPT + SBRPLib_NUM_ROUTES_MINIMIZE;
	SinglePairInsert SPI;
	int i = 0;
	while(i < student_Stop_num)
	{
		if(!SPI.search_and_move(this,perm[i],rules))
			return false;
		i++;
	}
	
	return true;
}

void SBRP::store_solution(int * s)
{
	int k = 0;
	int j;
	for(int i = 0;i<num_nodes;i++)
	{
		if(route[i].used)
		{
			s[k++] = route[i].deopt_id;
			j = route[i].start;
			while(j != SBRPLib_ROUTE_OUTER)
			{
				s[k++] = j;
				j = get_next_node(j,i);
			}
		}
	}
	s[k] = SBRPLib_ROUTE_OUTER;
	
}
void SBRP::load_solution(int * s)
{
	int i,j,y;
	i = 0;
	j = -1;
	y = -1;
	clear_route();
	//int count = num_nodes *(2*num_nodes+1);
	while(s[i] != SBRPLib_ROUTE_OUTER)	
	{
		if(s[i] == 0)
		{
			route[++j].used = true;
			route[j].deopt_id = SBRPLib_DEPOT;
			nodes[SBRPLib_DEPOT]->routed = true;
			if(j>0)
			{
				update_route_info_LNS(j-1);				
			}
		}
		else
		{	
			InsertNode(s[i],y,j);
		}
		y=s[i];
		i++;
	}	
	if(j>= 0 && j<num_nodes)
	{
		update_route_info_LNS(j);
	}
	if(j>0)
	{
		total_number_of_routes = j+1;
	}
	else
	{
		this->total_number_of_routes = 0;
	}
	
	this->total_route_length = 0.0;
	this->total_service_time = 0;
	for(i =0;i<total_number_of_routes;i++)
	{
		if(route[i].used)
		{
			this->total_route_length += route[i].single_length;
			this->total_service_time += route[i].single_time;
		}
	}
	
}

void SBRP::clear_route()
{
	for(int i = 0;i<num_nodes;i++)
	{
		nodes[i]->routed = false;
		VisitedRecord * p = nodes[i]->visited_record_link->first;
		VisitedRecord * q;
		while(p)
		{
			q = p;
			p = p->next;
			delete q;
		}
		nodes[i]->visited_record_link->first = NULL;
		clear_route(i);
	}
}
void SBRP::clear_route(int i)
{
	route[i].used = false;
	route[i].deopt_id = SBRPLib_ROUTE_OUTER;// start depot
	route[i].start = SBRPLib_ROUTE_OUTER;
	SchoolNode * p = route[i].school_node_link->first;
	SchoolNode * q;
	while(p)
	{
		q = p;
		p = p->next;
		delete q;
	}
	route[i].school_node_link->first = NULL;
	route[i].num_customers = 0;//# of student stop
	route[i].school_count = 0;//# of school stop

	route[i].obj_val = SBRPLib_INFINITY;
	route[i].single_length = SBRPLib_INFINITY;
	route[i].single_time = SBRPLib_INFINITY;
	route[i].throughput = -SBRPLib_INFINITY;//record total student ,include get down at one school
}

void SBRP::WRI_SBR()
{
	int heuristics = SBRPLib_WRI_MOVE + SBRPLib_SBR_MOVE;
	int rules = SBRPLib_USE_NEIGHBOR_LIST+SBRPLib_BEST_ACCEPT+SBRPLib_NUM_ROUTES_MINIMIZE;
	int *perm;
	perm=new int[this->num_nodes];
	int p = -1;
	int j;
	for(int i = 0;i<num_nodes;i++)
	{
		if(route[i].used)
		{
			j = route[i].start;
			while(j!= SBRPLib_ROUTE_OUTER)
			{
				if(nodes[j]->type == SBRPLib_STOP_NODE)
				{
					perm[++p] = j;
				}
				j = get_next_node(j,i);
			}
		}
	}
	int student_Stop_num = p + 1;

	WithinRouteInsert WRI;
	SwapBetweenRoutes SBR;
	if(heuristics & SBRPLib_WRI_MOVE)
	{
		for(int i=0;i<student_Stop_num;i++)    
		{
			WRI.search_and_move(this,perm[i],rules);			
		}
	}				

	if(heuristics & SBRPLib_SBR_MOVE )
	{
		for(int i=1; i<num_nodes; i++)    
		{
			for(int j=0;j<num_nodes;j++)
			{
				if(i == j)
					break;
				SBR.search_and_move(this,i, j,rules);				
			}
		}
	}
}