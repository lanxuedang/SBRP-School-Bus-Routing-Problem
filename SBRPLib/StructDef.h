#ifndef _SBRP_STRUCT_DEF_H
#define _SBRP_STRUCT_DEF_H

struct SearchSpace 
{
	int i;
	int k;
	int r;
};

class VRPNeighborElement
{
public:
	double val;
	int position;
};

int VRPNeighborCompare (const void *a, const void *b);
int VRPNeighborCompare2 (const void *a, const void *b);

#endif