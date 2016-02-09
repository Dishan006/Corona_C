/*
 * apiLoader.h
 *
 *  Created on: Feb 9, 2016
 *      Author: dishan
 */

#ifndef APILOADER_H_
#define APILOADER_H_
typedef char* (*processRequest) (char*);

processRequest* loadAPIs();
#endif /* APILOADER_H_ */
