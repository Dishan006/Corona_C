/*
 * responseWriter.h
 *
 *  Created on: Mar 20, 2016
 *      Author: dishan
 */
#include<stdbool.h>

#ifndef RESPONSEWRITER_H_
#define RESPONSEWRITER_H_

void WriteResponse(int socket, bool isApiRequest, int statusCode, char* contentType, int responseLength, char* responseBody);

#endif /* RESPONSEWRITER_H_ */
