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
#include "utils.h"
#include "apiLoader.h"

apiInfo* getApiInfo(char* configLine);

apiInfoCollection* loadAPIs()
{
	fileInfo *info = readFile("/home/dishan/workspace/httpServerInC/config/apiConfig");
	apiInfo **list= NULL;
	list = malloc(sizeof(apiInfo*)*5);
	int count=0;

	char* configLine = strtok(info->fileData, "\n");
	while (configLine) {
		printf("Loading API: %s\n", configLine);
		apiInfo *item = getApiInfo(configLine);
		if(item)
		{
			list[count] = item;
			count++;
		}

		configLine = strtok(NULL, "\n");
	}

	apiInfoCollection* collection = calloc(sizeof(apiInfoCollection),1);
	collection->apiInfo = list;
	collection->count = count;

	return collection;
}

apiInfo* getApiInfo(char* configLine)
{
	char* apiName = strtok(configLine, ":");
	char* path = strtok(NULL, ":");
	if(apiName && path)
	{
		apiInfo* info = calloc(1,sizeof(apiInfo));
		info->apiName = apiName;


		void *handle;

		char *error;

		handle = dlopen(path, RTLD_LAZY);
		if (!handle) {
			fputs (dlerror(), stderr);
			exit(1);
		}

		processRequest *request = dlsym(handle, "ProcessRequest");
		if ((error = dlerror()) != NULL)  {
			fputs(error, stderr);
			exit(1);
		}

		info->method = request;
		printf("API Name: %s ProcessRequest loaded!\n", apiName);

		return info;
	}

	return NULL;
}

void freeAPIs(apiInfoCollection* collection)
{
	if(collection)
	{
		int i;
		for(i=0;i<collection->count;i++)
		{
			free(collection->apiInfo[i]);
		}
		free(collection->apiInfo);
		free(collection);
	}
}
