/*
 * threadPool.c
 *
 *  Created on: Mar 27, 2016
 *      Author: dishan
 */

#include "../Include/threadPool.h"

static const int maxThreadCount =50;

static int currentPoolSize;

static threadInfo** threadPool;

threadInfo* TryGetFromPool();

void InitializePool()
{

}

threadInfo* TryGetThread()
{
	threadInfo* result = NULL;
	result = TryGetFromPool();
	if(result)
	{
		return result;
	}

	return NULL;
}


threadInfo* TryGetFromPool()
{
	int i;

	for(i=0;i<currentPoolSize;i++)
	{
		if(threadPool[i]->state == WAITING)
		{
			return threadPool[i];
		}
	}

	return NULL;
}
