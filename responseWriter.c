/*

 * responseWriter.c
 *
 *  Created on: Mar 20, 2016
 *      Author: dishan
 */

#include "responseWriter.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include "utils.h"
#include<unistd.h>

void sendNotFound(int sock);
void sendBadRequest(int sock);
void sendError(int sock,char* responseFormat);
void sendApiOkResponse(int socket, char* result);
void sendSiteOkResponse(int socket, char* contentType, int responseLength, char* result);

void WriteResponse(int socket, bool isApiRequest, int statusCode,char* contentType, int responseLength, char* responseBody)
{

	if(statusCode == 404)
	{
		sendNotFound(socket);
		return;
	}else if (statusCode == 200)
	{
		if(isApiRequest)
		{
			sendApiOkResponse(socket,responseBody);
		}else
		{
			sendSiteOkResponse(socket,contentType,responseLength,responseBody);
		}
		return;
	}

	sendBadRequest(socket);
}


void sendNotFound(int sock)
{
	char* responseFormat = "HTTP/1.1 404 Not Found\nServer: newServerinc\nDate: %s\n\n404 Not Found\n\n";
	sendError(sock,responseFormat);
}

void sendBadRequest(int sock)
{
	char* responseFormat = "HTTP/1.1 400 Bad Request\nServer: newServerinc\nDate: %s\n\n400 Bad Request\n\n";
	sendError(sock,responseFormat);
}

void sendError(int sock, char* responseFormat)
{
	char* dateTime = getDateTime();
	char* response = calloc(strlen(responseFormat) + strlen(dateTime)-1,sizeof(char));
	sprintf(response,responseFormat, dateTime);
	write(sock , response , strlen(response));
	shutdown(sock,SHUT_WR);
	free(response);
}

void sendApiOkResponse(int socket, char* result)
{
	char* contentType = "application/json";

	long contentLength = strlen(result);

	char* responseFormat = "HTTP/1.1 200 OK\nServer: newServerinc\nContent-Type: %s\nContent-Length: %d\nConnection: Keep-Alive\nDate: %s\n\n%s";
	char* dateTime = getDateTime();
	char* response = calloc(strlen(responseFormat) + strlen(contentType)+ strlen(dateTime)+contentLength+10,sizeof(char));
	sprintf(response,responseFormat, contentType,contentLength,dateTime,result);

	printf("[Handler] Response With body: %s\n",response);

	write(socket , response , strlen(response));
	shutdown(socket,SHUT_WR);
	free(response);
}

void sendSiteOkResponse(int socket, char* contentType, int responseLength, char* result)
{


	long contentLength = responseLength;
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

	memcpy(responseWithBody+strlen(response),result,contentLength);

	// dumpToFile(responseWithBody,"new2.jpg");
	//printf("[Handler] Response: %s\n",responseWithBody);
	write(socket , responseWithBody , finalLength);

	// printBytes(responseWithBody,finalLength);

	//printf("[Handler] Bytes Written: %ld\n",written);
	shutdown(socket,SHUT_WR);

	free(response);
	free(responseWithBody);

}
