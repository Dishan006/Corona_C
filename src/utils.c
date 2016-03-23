/*
 * utils.c
 *
 *  Created on: Feb 9, 2016
 *      Author: dishan
 */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<unistd.h>
#include<pthread.h>
#include <dirent.h>

#include "../Include/utils.h"


char* getDateTime()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char* dateTimeFormat = "%d-%d-%d %d:%d:%d";
	char* dateTime = malloc(100);
	sprintf(dateTime,dateTimeFormat ,tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	return dateTime;
}

fileInfo* tryGetFile(char* path)
{
	fileInfo* file;
	char* baseUrl = "/var/www";
	char* fileFullPath = calloc(strlen(baseUrl)+strlen(path)+1,sizeof(char));
	strcat(fileFullPath,baseUrl);
	strcat (fileFullPath,path);
	file= readFile(fileFullPath);
	free(fileFullPath);
	return file;
}


fileInfo* tryGetIndexFile(char* url)
{
	DIR *d =NULL;
	fileInfo* result = NULL;
	char* baseUrl = "/var/www";
	char* indexFileName = "index.html";
	char* folderPath = calloc(strlen(url)+strlen(baseUrl)+1,sizeof(char));
	strcat(folderPath,baseUrl);
	strcat (folderPath,url);
	//printf("[Handler] Resource Folder Path:%s\n",folderPath);

	struct dirent *dir;
	d = opendir(folderPath);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			//printf("%s\n", dir->d_name);
			if(strcmp(indexFileName,dir->d_name)==0)
			{
				char* fileFullPath = calloc(strlen(folderPath)+strlen(indexFileName)+1,sizeof(char));
				strcat(fileFullPath,folderPath);
				strcat (fileFullPath,indexFileName);
				// printf("[Handler] File Full Path: %s\n", fileFullPath);
				result = readFile(fileFullPath);

				free(fileFullPath);
				// printf("File: %s\n", result);
				break;
			}

		}

		closedir(d);
	}

	free(folderPath);
	return result;
}


fileInfo* readFile(char* path)
{
	char* buffer = NULL;
	long length =0;
	// long bytesActuallyRead =0;
	FILE * f = fopen (path, "rb");
	int seekResult =0;

	if (f)
	{
		seekResult= fseek (f, 0, SEEK_END);
		if(seekResult==0)
		{
			length = ftell (f);
			//printf("[readFile] File Length: %ld\n", length);

			if(length>0)
			{
				seekResult=  fseek (f, 0, SEEK_SET);
				if(seekResult==0)
				{
					buffer = (char*) malloc (sizeof(char)*length);
					//printf("[readFile] malloced \n");
					if (buffer)
					{
						fread (buffer, 1, length, f);
						//printf("[readFile] Read end \n");
					}
				}
			}
		}
		fclose (f);
	}

	fileInfo *file = malloc(sizeof(fileInfo));
	file->fileData = buffer;
	file->length = length;

	//printf("Bytes read: %ld\n", bytesActuallyRead);
	return file;
}

void dumpToFile (char* array, long length, const char* name)
{
	FILE *file = fopen(name, "wb");

	int results = fwrite(array, sizeof(array[0]), length, file);
	if (results == EOF) {
		// Failed to write do error code here.
	}
	fclose(file);
}

void printBytes(char* bytesArray, int count)
{
	int  i =0;
	while (i < count)
	{
		printf("%02X",(int)bytesArray[i]);
		i++;
	}
}

bool isFolderPath(char* path)
{
	DIR *d;
	char* baseUrl = "/var/www";
	char* folderPath = calloc(strlen(path)+strlen(baseUrl)+1,sizeof(char));
	strcat(folderPath,baseUrl);
	strcat (folderPath,path);

	d = opendir(folderPath);
	if (d)
	{
		//printf("[Handler] Request for folder\n");

		free(folderPath);
		closedir(d);
		return true;
	}

	free(folderPath);
	return false;
}
