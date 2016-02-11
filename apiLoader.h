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
	processRequest *method;
	char* apiName;
} apiInfo;

typedef struct
{
	apiInfo **apiInfo;
	int count;
} apiInfoCollection;


apiInfoCollection* loadAPIs();
void freeAPIs(apiInfoCollection* collection);
#endif /* APILOADER_H_ */
