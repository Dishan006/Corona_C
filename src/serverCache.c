/*
 * ServerCache.c

 *
 *  Created on: Mar 22, 2016
 *      Author: dishan
 */
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include "../Include/serverCache.h"

typedef struct
{
	char* resourceSegment;
	fileInfo* file;
}cacheItem;

static int cacheItemCount;

static cacheItem** cache;

pthread_mutex_t lock;

fileInfo* tryGetFileFromCache(char* resourceSegment);

fileInfo* tryGetFileFromCache(char* resourceSegment)
{
	if(!cache || cacheItemCount==0)
	{
		return NULL;
	}

	int i;
	for(i=0;i<cacheItemCount;i++)
	{
		char* resource =cache[i]->resourceSegment;

		if(resource && resourceSegment)
		{
			if(strcmp(resource,resourceSegment)==0)
			{
				return cache[i]->file;
			}
		}
	}

	return NULL;
}

fileInfo* getFileFromCache(char* resourceSegment)
{
	pthread_mutex_lock(&lock);
	fileInfo* file =tryGetFileFromCache(resourceSegment);

	if(file)
	{
		printf("[Global Cache] Cache Hit!\n");
		pthread_mutex_unlock(&lock);
		return file;
	}

	printf("[Global Cache] Cache Miss!\n");
	if(isFolderPath(resourceSegment))
	{
		file= tryGetIndexFile(resourceSegment);
	}else
	{
		file= tryGetFile(resourceSegment);
	}

	if(file && file->fileData)
	{
		cacheItem* newItem = malloc(sizeof(cacheItem));
		newItem->file = file;
		newItem->resourceSegment = strdup(resourceSegment);
		cacheItemCount++;
		cache = realloc(cache,cacheItemCount*sizeof(cacheItem*));
		cache[cacheItemCount-1] = newItem;

		pthread_mutex_unlock(&lock);
		return file;
	}

	pthread_mutex_unlock(&lock);
	return NULL;
}

void initializeCache()
{
	if (pthread_mutex_init(&lock, NULL) != 0)
	{
		printf("\n mutex init failed\n");
	}
}

void cleanCache()
{
	pthread_mutex_destroy(&lock);

	int i;
	for(i=0;i<cacheItemCount;i++)
	{
		free(cache[i]->file->fileData);
		free(cache[i]->file);
		free(cache[i]->resourceSegment);
		free(cache[i]);
	}
	free(cache);
}

