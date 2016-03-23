/*
 * apiLoader.h
 *
 *  Created on: Feb 9, 2016
 *      Author: dishan
 */

#ifndef APILOADER_H_
#define APILOADER_H_
typedef char* (*processRequest) (char*);

typedef struct
{
	char* path;
	char* apiName;
} apiInfo;

typedef struct
{
	apiInfo **apiInfo;
	int count;
} apiInfoCollection;


void loadAPIs();
void freeAPIs();
apiInfo* tryGetAPI(char* request);
#endif /* APILOADER_H_ */
