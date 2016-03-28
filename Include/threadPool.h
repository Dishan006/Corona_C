/*
 * threadPool.h
 *
 *  Created on: Mar 27, 2016
 *      Author: dishan
 */

#include<pthread.h>
#ifndef THREADPOOL_H_
#define THREADPOOL_H_

typedef enum {RUNNING, WAITING} threadState;

typedef struct
{
	pthread_t* thread;
	threadState state;
}threadInfo;

void InitializePool();
threadInfo* TryGetThread();

#endif /* THREADPOOL_H_ */
