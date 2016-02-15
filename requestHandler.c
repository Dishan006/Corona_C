#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include <dirent.h>
#include <dlfcn.h>
#include "utils.h"
#include "requestHandler.h"
#include "apiLoader.h"

struct httpHeader
{
	char* name;
	char* value;
};


struct httpRequestMessage
{
	char* method;
	char* httpVersion;
	char* resource;
	struct httpHeader headers[10];
	char* body;
};


fileInfo*  tryGetIndexFile(char* url);
struct httpRequestMessage* readMessage(char* MessageString);
bool isAPIRequest(char* MessageString);
void processAPIRequest(char* messageString);

void processRequestMessage(char* request, int sock)
{
	struct httpRequestMessage* requestMessage = readMessage(request);

	//printf("[Handler] readMessage end.\n");
	if(requestMessage)
	{
		fileInfo* file;
		char* resourceSegement = requestMessage->resource;

		if(isAPIRequest(resourceSegement))
		{
			processAPIRequest(resourceSegement);
		}else
		{

			if(isFolderPath(resourceSegement))
			{
				file= tryGetIndexFile(resourceSegement);
			}else
			{
				file= tryGetFile(resourceSegement);
			}

			if(file->fileData)
			{
				char* contentType = "text/html";

				char *dot = strrchr(resourceSegement, '.');
				//printf("[Handler] Extension: %s\n",dot);
				if (dot && !strcmp(dot, ".jpg"))
				{
					contentType = "image/jpeg";
				}else if (dot && !strcmp(dot, ".css"))
				{
					contentType = "text/css";
				}

				long contentLength = file->length;
				//dumpToFile(file.fileData,contentLength,"new1.jpg");

				//printf("[Handler] Content Length: %ld\n",contentLength);

				char* responseFormat = "HTTP/1.1 200 OK\nServer: newServerinc\nContent-Type: %s\nContent-Length: %d\nConnection: Keep-Alive\nDate: %s\n\n";
				char* dateTime = getDateTime();


				char* response = calloc(strlen(responseFormat) + strlen(contentType)+ strlen(dateTime)+10,sizeof(char));
				sprintf(response,responseFormat, contentType,contentLength,dateTime);

				//printf("[Handler] Response Without body: %zd\n",strlen(response));

				long finalLength =strlen(response)+contentLength+1;
				char* responseWithBody = calloc(finalLength,sizeof(int));
				strcat(responseWithBody,response);

				memcpy(responseWithBody+strlen(response),file->fileData,contentLength);

				// dumpToFile(responseWithBody,"new2.jpg");
				//printf("[Handler] Response: %s\n",responseWithBody);
				write(sock , responseWithBody , finalLength);

				// printBytes(responseWithBody,finalLength);

				//printf("[Handler] Bytes Written: %ld\n",written);
				shutdown(sock,SHUT_WR);

				free(response);
				free(file->fileData);
				free(file);
				free(responseWithBody);

			}else
			{
				char* responseFormat = "HTTP/1.1 404 Not Found\nServer: newServerinc\nDate: %s\n\n404 Not Found\n\n";
				char* dateTime = getDateTime();

				char* response = calloc(strlen(responseFormat) + strlen(dateTime)-1,sizeof(char));
				sprintf(response,responseFormat, dateTime);
				write(sock , response , strlen(response));
				shutdown(sock,SHUT_WR);
				free(response);
			}
		}
		free(requestMessage);
	}

}

struct httpRequestMessage* readMessage(char* MessageString)
{
	struct httpRequestMessage* message = malloc(sizeof(struct httpRequestMessage));
	if(strncmp(MessageString,"GET",3)==0)
	{
		message->method = "GET";
	}else if(strncmp(MessageString,"POST",4)==0)
	{
		message->method = "POST";
	}
	// printf("[Handler] readMessage Method found.\n");

	char* resourceSegement = strtok(MessageString," ");
	resourceSegement = strtok(NULL," ");
	message->resource = resourceSegement;
	message->httpVersion = strtok(NULL," ");

	return message;
}


fileInfo* tryGetIndexFile(char* url)
{
	DIR *d;
	fileInfo* result;
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

bool isAPIRequest(char* MessageString)
{
	return strncmp(MessageString,"/api/",5)==0;
}

void processAPIRequest(char* messageString)
{
	messageString = messageString+5;
	printf("[API] %s\n",messageString);
	char* apiName = strtok(messageString, "/");

	printf("[API NAME] %s\n",apiName);
	apiInfo* api = tryGetAPI(apiName);
	if(api)
	{
		printf("[API Found]\n");
		messageString = messageString+strlen(apiName);
		void *handle;
		char* (*processRequest) (char*);
		char *error;

		handle = dlopen(api->path, RTLD_LAZY);
		if (!handle) {
			fputs (dlerror(), stderr);
			exit(1);
		}

		processRequest = dlsym(handle, "ProcessRequest");
		if ((error = dlerror()) != NULL)  {
			fputs(error, stderr);
			exit(1);
		}

		//(*hello)();
		char* result = processRequest(messageString);
		printf("[API Result] %s\n",result);
		dlclose(handle);

	}
}
