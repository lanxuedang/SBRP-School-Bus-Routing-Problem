#include "Stdafx.h"
#include "SBRPLib.h"

VisitedRecordLink::VisitedRecordLink()
{
	this->first = NULL;
}
VisitedRecordLink::~VisitedRecordLink()
{
	VisitedRecord *p,*q;
	p = this->first;
	while(p)
	{
		q = p;
		p=p->next;
		delete q;
	}
	this->first = NULL;
}