#include "Stdafx.h"
#include "SBRPLib.h"

//Compute relatedness, then sort nodes by relatedness
void SBRP::sort_nodes_by_relatedness(int selected,int* stopnodes,int len)
{
	//len:当前长度；因为数组中的元素有些已经被取走；

	int k,r;
	if(releatedness==NULL)
	{
		int n= num_nodes;		
		releatedness = new double*[n];
		releatedness[0] = new double [n*n];
		for(int i = 1;i<n;i++)
		{
			releatedness[i] = releatedness[i-1]+n;
		}					
	}
	
	//compute relatedness
	for(int i = 0;i<len;i++)
	{
		if(nodes[selected]->routed && nodes[stopnodes[i]]->routed)
		{
			if(nodes[selected]->visited_record_link->first->routenum == nodes[stopnodes[i]]->visited_record_link->first->routenum)
			{
				r = 1;
			}
			else
			{
				r = 0;
			}
		}
		else
		{
			r = 0;
		}
		
		releatedness[selected][stopnodes[i]] = 1/(normalize_dmatrix[selected][stopnodes[i]] + r);
	}

	//sort by relatedness
	for(int i = len;i>0;i--)
	{
		for(int j = 1;j<i;j++)
		{
			if(releatedness[selected][stopnodes[j-1]]<releatedness[selected][stopnodes[j]])
			{
				k = stopnodes[j];
				stopnodes[j]= stopnodes[j-1];
				stopnodes[j-1] = k;
			}
		}
	}
}

void SBRP::create_stopnodes()
{
	stopnodes = new int[stop_node_num];
	int j = 0;
	for(int i = 0;i<num_nodes;i++)
	{
		if(nodes[i]->type == SBRPLib_STOP_NODE)
		{
			stopnodes[j++] = i;
		}
	}	
}

//select k nodes from stopnodes array to selectednodes
void SBRP::select_remove_nodes(int* selectednodes,int p)
{	
	if(p>stop_node_num || p>SBRPLib_MAX_REMOVE_COUNT)
	{		
		return;
	}

	int* tempstopnodes = new int[stop_node_num];
	for(int i = 0;i<stop_node_num;i++)
	{
		tempstopnodes[i] = stopnodes[i];
	}

	int uninsertnodes = 0;
	int unremovenodes = stop_node_num;

	int s = (int)SBRP_Random(0,unremovenodes);
	int selected = tempstopnodes[s];
	tempstopnodes[s] = tempstopnodes[unremovenodes-1];
	unremovenodes--;	
	selectednodes[uninsertnodes++] = selected;
	
	for(int i = 1;i<p;i++)
	{
		selected = selectednodes[(int)SBRP_Random(0,uninsertnodes-1)];
		sort_nodes_by_relatedness(selected,tempstopnodes,unremovenodes);
		selected = tempstopnodes[0];
		tempstopnodes[0] = tempstopnodes[unremovenodes-1];
		unremovenodes--;
		selectednodes[uninsertnodes++] = selected;
	}
	PairInsert PI;	

	for(int i = 0;i<uninsertnodes;i++)
	{
		printf("%d ",selectednodes[i]);					
	}
	printf("\n");

	for(int i = 0;i<uninsertnodes;i++)
	{
		int rr = get_stopnode_route_num(selectednodes[i]);
		PI.remove(this,selectednodes[i]);
	}
	delete [] tempstopnodes;
}

//current_best_s是已得到的全局最优解
void SBRP::ReInsertNodes_LDS(SBRPSolution current_best_s,int* selected,int nodescount,int maxd)
{	
	if(nodescount == 0)
	{
		if(!current_best_s.isBetter(this))
		{
			current_best_s.importsolution(this);//把目前的解记录为最优解			
		}
	}
	else
	{
		//克隆一份为的是不改变原有值
		int* selectednodes = new int[nodescount];
		for(int i = 0;i<nodescount;i++)
		{
			selectednodes[i] = selected[i];
		}

		SBRPSolution *s = new SBRPSolution(this);
		int * s_solution = new int[3*num_nodes];
		s->solution = s_solution;
		PairInsert PI;	


		//找出所有未插入点的最小插入值,然后排序
		int node = rank_selectednodes_by_min_insertcost(selectednodes,nodescount);
		
		//从未插入的点中移到未移出的数组中
		for(int j = 0;j<nodescount;j++)
		{
			if(selectednodes[j] == node)
			{
				selectednodes[j] = selectednodes[nodescount-1];
				nodescount--;
			}
		}
		
		SBRPMove* moves = new SBRPMove[maxd+1];

		s->importsolution(this);
		s->exportsolution(this);//导出的目的是整理一下路径，让其从0开始，即所有使用的路径位于前边。
		PI.search(this,node,maxd+1,moves);
		
		for(int i=0;i<=maxd;i++)
		{
			if(moves[i].new_total_number_of_routes != -1)
			{				
				s->exportsolution(this);
				PI.move(this,moves[i].move_arguments[0],moves[i].move_arguments[1],moves[i].move_arguments[2],moves[i].move_arguments[3],&moves[i]);
				
				if(!current_best_s.isBetter(this))//如果新生成的局部解优于当前最好解S
				{
					ReInsertNodes_LDS(current_best_s,selectednodes,nodescount,maxd-i);
				}
			}
			else
			{
				if(i == 0)
				{
					for(int j = 0;j<num_nodes;j++)
					{
						if(route[j].used == false)
						{
							int destination = nodes[node]->destination_id;
							//construct a route
							route[j].used = true;
							route[j].deopt_id = nodes[SBRPLib_DEPOT]->id;
							nodes[route[j].deopt_id]->routed = true;
							InsertNode(node,route[j].deopt_id,j);
							InsertNode(destination,node,j);
							update_route_info_LNS(j);
							if(!check_constraints(j))
							{
								return;//失败了，不再执行了。
							}
							total_route_length+= route[j].single_length;
							total_service_time += route[j].single_time;
							total_number_of_routes ++;

							if(!current_best_s.isBetter(this))//如果新生成的局部解优于当前最好解S
							{
								ReInsertNodes_LDS(current_best_s,selectednodes,nodescount,maxd-i);
							}		
						}
					}
				}
				else
				{
					break;
				}				
			}
		}
		delete [] moves;
		delete [] selectednodes;
		delete [] s_solution;
		delete s;
	}	
}


int SBRP::rank_selectednodes_by_min_insertcost(int* selectednodes, int uninsertnodes)
{
	//每个点找到最好的插入位置，排序，找最小插入代价最大的。
	SBRPMove* moves = new SBRPMove[uninsertnodes];
	SBRPMove* move = new SBRPMove();
	int result = SBRPLib_ROUTE_OUTER;
	PairInsert PI;
	for(int i = 0;i<uninsertnodes;i++)
	{
		PI.search(this,selectednodes[i],1,move);
		PI.insert_SBRPMove(move,moves,uninsertnodes);
	}
	
	result = moves[uninsertnodes-1].move_arguments[0];
	
	
	delete [] moves;
	delete move;
	return result;
}

