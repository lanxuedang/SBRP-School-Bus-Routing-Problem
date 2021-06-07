#ifndef _VISITED_RECORD_H
#define _VISITED_RECORD_H

//when a node is visited, new a VisitedRecord
class VisitedRecord
{
public:
	int nodeid;
	int routenum;//route index	
	int arrive_time;
	int current_students;
	VisitedRecord* next;//multi visited
	int pre_node;//
	int next_node;//
	VisitedRecord()
	{
		nodeid = SBRPLib_ROUTE_OUTER;
		routenum = SBRPLib_ROUTE_OUTER;
		arrive_time = SBRPLib_INFINITY;
		current_students = 0;
		next = NULL;
		pre_node = SBRPLib_ROUTE_OUTER;
		next_node = SBRPLib_ROUTE_OUTER;
	}
};


class VisitedRecordLink 
{
public:
	VisitedRecord *first;
	VisitedRecordLink();
	~VisitedRecordLink();
};

#endif