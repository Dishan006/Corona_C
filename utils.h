/*
 * utils.h
 *
 *  Created on: Feb 9, 2016
 *      Author: dishan
 */
#include<stdbool.h>

#ifndef UTILS_H_
#define UTILS_H_
struct fileInfo
{
  char* fileData;
  long length;
};

char* getDateTime();
struct fileInfo* tryGetFile(char* path);
struct fileInfo*  readFile(char* path);
void dumpToFile(char* array, long length, const char* name);
void printBytes(char* bytesArray, int count);
bool isFolderPath(char* path);

#endif /* UTILS_H_ */
