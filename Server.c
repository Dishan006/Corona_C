#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include "Server.h"
#include <dirent.h>
#include "apiLoader.h"
#include "requestHandler.h"


int stopIssued = 0;
pthread_mutex_t stopMutex;

//the thread function
void *connection_handler(void *);


void* StartServer(void* vp)
{
	int socket_desc , client_sock , c , *new_sock;
	struct sockaddr_in server , client;


	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(80);

	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
	}

	puts("bind done");

	//Listen
	listen(socket_desc , 3);

	//Accept and incoming connection

	c = sizeof(struct sockaddr_in);
	client_sock=1;

	int abortState = getStopIssued();
	apiInfoCollection* apis= loadAPIs();

	while( client_sock>0 && abortState!=1)
	{
		abortState = getStopIssued();
		puts("Waiting for incoming connections...");
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);

		puts("Connection accepted");

		pthread_t sniffer_thread;
		/* pthread_attr_t threadAttribute;
        int res = pthread_attr_init(&threadAttribute);
          if (res != 0) {
              perror("Attribute init failed");
              exit(EXIT_FAILURE);
          }
          res = pthread_attr_setdetachstate(&threadAttribute, PTHREAD_CREATE_DETACHED); // The worker threads are detached.
          if (res != 0) {
              perror("Setting detached state failed");
              exit(EXIT_FAILURE);
          }*/

		new_sock = malloc(sizeof(new_sock));
		*new_sock = client_sock;

		if(pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
		{
			perror("could not create thread");
		}
		puts("Handler assigned");
	}

	free(new_sock);

	if (client_sock < 0)
	{
		perror("accept failed");
	}

	if(abortState ==1)
	{
		puts("Abort signal received.");
	}

	freeAPIs(apis);
	puts("Server Stopped");
	return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
	puts("[Handler] Started");
	//Get the socket descriptor
	int sock = *(int*)socket_desc;
	int read_size;

	char client_message[2000];

	read_size = recv(sock , client_message , 2000 , 0);

	if(read_size>0)
	{
		printf("[Handler] Request: %s",client_message);
		processRequestMessage(client_message, sock);
	}

	puts("[Handler] Client disconnected");
	fflush(stdout);

	//Free the socket pointer
	free(socket_desc);

	return 0;
}

int getStopIssued(void) {
	int ret = 0;
	pthread_mutex_lock(&stopMutex);
	ret = stopIssued;
	pthread_mutex_unlock(&stopMutex);
	return ret;
}

void setStopIssued(int val) {
	pthread_mutex_lock(&stopMutex);
	stopIssued = val;
	pthread_mutex_unlock(&stopMutex);
}

