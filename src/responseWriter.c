/*

 * responseWriter.c
 *
 *  Created on: Mar 20, 2016
 *      Author: dishan
 */

#include "../Include/responseWriter.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include "../Include/utils.h"
#include<unistd.h>


void sendError(int sock,int code);
void sendApiOkResponse(int socket, char* result);
void sendSiteOkResponse(int socket, char* contentType, int responseLength, char* result);

void WriteResponse(int socket, bool isApiRequest, int statusCode,char* contentType, int responseLength, char* responseBody)
{
	if (statusCode == 200)
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

	sendError(socket, statusCode);
}

void sendError(int sock, int code)
{
	char* responseFormat = NULL;

	switch (code) {
	case 400:
		responseFormat = "HTTP/1.1 400 Bad Request\nServer: newServerinc\nDate: %s\n\n400 Bad Request\n\n";
		break;
	case 403:
		responseFormat = "HTTP/1.1 403 Forbidden\nServer: newServerinc\nDate: %s\n\n403 Forbidden\n\n";
		break;
	case 404:
		responseFormat = "HTTP/1.1 404 Not Found\nServer: newServerinc\nDate: %s\n\n404 Not Found\n\n";
		break;
	default:
		responseFormat = "HTTP/1.1 400 Bad Request\nServer: newServerinc\nDate: %s\n\n400 Bad Request\n\n";
		break;
	}

	char* dateTime = getDateTime();
	char* response = calloc(strlen(responseFormat) + strlen(dateTime)-1,sizeof(char));
	sprintf(response,responseFormat, dateTime);
	write(sock , response , strlen(response));
	shutdown(sock,SHUT_RDWR);
	close(sock);
	free(response);
	free(dateTime);
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
	shutdown(socket,SHUT_RDWR);
	close(socket);
	free(response);
	free(dateTime);
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
	shutdown(socket,SHUT_RDWR);
	close(socket);

	free(response);
	free(responseWithBody);
	free(dateTime);

}
