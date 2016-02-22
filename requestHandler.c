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

typedef struct
{
	char* name;
	char* value;
}httpHeader;


typedef struct
{
	char* method;
	char* httpVersion;
	char* resource;
	httpHeader** headers;
	int headerCount;
	char* body;
}httpRequestMessage;


fileInfo*  tryGetIndexFile(char* url);
httpRequestMessage* readMessage(char* MessageString);
bool isAPIRequest(char* MessageString);
void processAPIRequest(httpRequestMessage* message, int sock);
httpHeader* getHeaderFromString(char* headerLine);

void processRequestMessage(char* request, int sock)
{
	httpRequestMessage* requestMessage = readMessage(request);

	//printf("[Handler] readMessage end.\n");
	if(requestMessage)
	{
		fileInfo* file;
		char* resourceSegement = requestMessage->resource;

		if(isAPIRequest(resourceSegement))
		{
			processAPIRequest(requestMessage, sock);
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
		free(requestMessage->headers);
		free(requestMessage->body);
		free(requestMessage);
	}

}

httpRequestMessage* readMessage(char* MessageString)
{
	httpRequestMessage* message = malloc(sizeof(httpRequestMessage));
	message->headers = NULL;
	message->headerCount =0;
	if(strncmp(MessageString,"GET",3)==0)
	{
		message->method = "GET";
		MessageString+=3;
	}else if(strncmp(MessageString,"POST",4)==0)
	{
		message->method = "POST";
		MessageString+=4;
	}else if(strncmp(MessageString,"PATCH",5)==0)
	{
		message->method = "PATCH";
		MessageString+=5;
	}else if(strncmp(MessageString,"DELETE",6)==0)
	{
		message->method = "DELETE";
		MessageString+=6;
	}else
	{
		return NULL;
	}

	char *save;

	char* firstLine = strtok_r(MessageString,"\n", &save);

	char* nextLine = NULL;
	char* body = NULL;
	int headerCount =0;
	int headersFinished =0;
	do
	{
		nextLine = strtok_r(NULL,"\n", &save);
		if(nextLine)
		{
			if(headersFinished == 1)
			{
				int newLegth = strlen(nextLine);
				int oldLength =0;

				if(body)
				{
					oldLength = strlen(body);
					newLegth+=oldLength;
				}

				body = realloc(body,(newLegth+1)*sizeof(char));
				strcpy (body+ oldLength,nextLine);

				body[newLegth] = '\0';
			}else
			{
				if(headersFinished == 0)
				{
					if(strlen(nextLine)==1)
					{
						headersFinished =1;
						continue;
					}

					headerCount++;
					httpHeader* header = getHeaderFromString(strdup(nextLine));
					message->headers = realloc(message->headers,headerCount*sizeof(httpHeader*));
					message->headers[headerCount-1] = header;
				}
			}
		}
	}while(nextLine);

	message->resource = strtok(firstLine," ");
	message->httpVersion = strtok(NULL," ");
	message->headerCount = headerCount;
	message->body = body;

	printf("[readMessage] header Count :%d\n",headerCount);
	printf("[readMessage] body :%s\n",body);

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

void processAPIRequest(httpRequestMessage* message, int sock)
{
	char* messageString = message->resource;
	messageString = messageString+5;
	char* apiName = strtok(messageString, "/");
	char* resource = strtok(NULL, " ");
	apiInfo* api = tryGetAPI(apiName);
	if(api)
	{
		//messageString = messageString+strlen(apiName);
		void *handle;
		const char* (*processRequest) (char*,char*);
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
		const char* result = processRequest(resource, message->method);
		printf("[API Result] %s\n",result);
		dlclose(handle);
		char* contentType = "application/json";


		long contentLength = strlen(result);

		char* responseFormat = "HTTP/1.1 200 OK\nServer: newServerinc\nContent-Type: %s\nContent-Length: %d\nConnection: Keep-Alive\nDate: %s\n\n%s";
		char* dateTime = getDateTime();


		char* response = calloc(strlen(responseFormat) + strlen(contentType)+ strlen(dateTime)+contentLength+10,sizeof(char));
		sprintf(response,responseFormat, contentType,contentLength,dateTime,result);


		//memcpy(responseWithBody+strlen(response),result,contentLength);
		printf("[Handler] Response With body: %s\n",response);

		write(sock , response , strlen(response));
		shutdown(sock,SHUT_WR);
		free(response);
	}
}

httpHeader* getHeaderFromString(char* headerLine)
{
	httpHeader* header = malloc(sizeof(httpHeader));
	header->name = strtok(headerLine,":");
	header->value = strtok(NULL,":");
	return header;
}
