/*
 * ServerCache.h

 *
 *  Created on: Mar 22, 2016
 *      Author: dishan
 */

#include<stdbool.h>
#include<stdio.h>
#include "../Include/utils.h"

#ifndef SERVERCACHE_H_
#define SERVERCACHE_H_

void initializeCache();
void cleanCache();
fileInfo* getFileFromCache(char* resourceSegment);


#endif /* SERVERCACHE_H_ */
