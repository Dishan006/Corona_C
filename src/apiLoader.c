/*
 * apiLoader.c

 *
 *  Created on: Feb 9, 2016
 *      Author: dishan
 */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <dlfcn.h>
#include "../Include/utils.h"
#include "../Include/apiLoader.h"

apiInfo* getApiInfo(char* configLine);

static apiInfoCollection* apis;

void loadAPIs()
{
	fileInfo *info = readFile("/home/dishan/workspace/httpServerInC/config/apiConfig");
	apiInfo **list= NULL;
	list = malloc(sizeof(apiInfo*)*5);
	int count=0;
	int processedLength =0;
	char* saveTok;

	char* configLine = strtok_r(info->fileData, "\n",&saveTok);
	while (configLine) {
		printf("Loading API: %s\n", configLine);
		processedLength+= strlen(configLine);
		apiInfo *item = getApiInfo(configLine);
		if(item)
		{
			list[count] = item;
			count++;
		}

		if(processedLength+count<info->length)
		{
			configLine = strtok_r(NULL, "\n",&saveTok);
		}else
			break;
	}

	apiInfoCollection* collection = calloc(sizeof(apiInfoCollection),1);
	collection->apiInfo = list;
	collection->count = count;

	apis= collection;
}

apiInfo* getApiInfo(char* configLine)
{
	char* saveSeg;
	char* apiName = strtok_r(configLine, ":",&saveSeg);
	char* path = strtok_r(NULL, ":",&saveSeg);
	if(apiName && path)
	{
		apiInfo* info = calloc(1,sizeof(apiInfo));
		info->apiName = apiName;
		info->path = path;
		printf("API Name: %s loaded!\n", apiName);
		return info;
	}

	return NULL;
}

void freeAPIs()
{
	if(apis)
	{

		int i;
		for(i=0;i<apis->count;i++)
		{
			free(apis->apiInfo[i]);
		}
		free(apis->apiInfo);
		free(apis);
	}
}

apiInfo* tryGetAPI(char* request)
{
	if(apis)
	{
		int i;
		for(i=0;i<apis->count;i++)
		{
			char* currentAPI = apis->apiInfo[i]->apiName;
			if(strncmp(request,currentAPI,strlen(currentAPI))==0)
			{
				return apis->apiInfo[i];
			}
		}
	}

	return NULL;
}
