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
#include "requestHandler.h"

#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

struct connectionInfo
{
	int* requestSocket;
	SSL* ssl;
};

int stopIssued = 0;
pthread_mutex_t stopMutex;
/* define HOME to be dir for key and cert files... */
#define HOME "/home/dishan/workspace/httpServerInC/Certs/"
/* Make these what you want for cert & key files */
#define CERTF  HOME "server.pem"
#define KEYF  HOME  "server.pem"




void *connection_handler(void* info);

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
	server.sin_port = htons(443);

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

	while( client_sock>0 && abortState!=1)
	{
		abortState = getStopIssued();
		puts("Waiting for incoming connections...");
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);

		puts("Connection accepted");



		/*char buf [4096];
		err = SSL_read (ssl, buf, sizeof(buf) - 1);                   CHK_SSL(err);
		buf[err] = '\0';
		printf ("Got %d chars:'%s'\n", err, buf);

		err = SSL_write (ssl, "I hear you.", strlen("I hear you."));  CHK_SSL(err); */

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

		struct connectionInfo* info  = malloc(sizeof(struct connectionInfo));
		//info->ssl = ssl;
		info->requestSocket = new_sock;

		if(pthread_create( &sniffer_thread , NULL ,  connection_handler , info) < 0)
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

	puts("Server Stopped");
	return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void* value)
{
	SSL_CTX* ctx;
	int err;
	X509*    client_cert;
	char*    str;
	const SSL_METHOD *meth;
	struct connectionInfo* info =(struct connectionInfo*)value;
	puts("[Handler] Started\n");
	//Get the socket descriptor
	int sock = *info->requestSocket;



	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();
	// meth = TLS_server_method();
	meth =SSLv23_server_method();

	ctx = SSL_CTX_new (meth);
	if (!ctx) {
		ERR_print_errors_fp(stderr);
		exit(2);
	}

	SSL_CTX_set_options(ctx, SSL_OP_SINGLE_DH_USE);

	if (SSL_CTX_use_certificate_file(ctx, CERTF, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		exit(3);
	}
	if (SSL_CTX_use_PrivateKey_file(ctx, KEYF, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		exit(4);
	}

	if (!SSL_CTX_check_private_key(ctx)) {
		fprintf(stderr,"Private key does not match the certificate public key\n");
		exit(5);
	}

	/* ----------------------------------------------- */
	/* TCP connection is ready. Do server side SSL. */
	SSL*     ssl;
	ssl = SSL_new (ctx);                           CHK_NULL(ssl);
	SSL_set_fd (ssl, sock);
	err = SSL_accept (ssl);                        CHK_SSL(err);

	/* Get the cipher - opt */

	printf ("SSL connection using %s\n", SSL_get_cipher (ssl));

	/* Get client's certificate (note: beware of dynamic allocation) - opt */

	client_cert = SSL_get_peer_certificate (ssl);
	if (client_cert != NULL) {
		printf ("Client certificate:\n");

		str = X509_NAME_oneline (X509_get_subject_name (client_cert), 0, 0);
		CHK_NULL(str);
		printf ("\t subject: %s\n", str);
		OPENSSL_free (str);

		str = X509_NAME_oneline (X509_get_issuer_name  (client_cert), 0, 0);
		CHK_NULL(str);
		printf ("\t issuer: %s\n", str);
		OPENSSL_free (str);

		/* We could do all sorts of certificate verification stuff here before
			       deallocating the certificate. */

		X509_free (client_cert);
	} else
		printf ("Client does not have certificate.\n");

	char* client_message = NULL;
	err=1;

	while(err>0)
	{
		printf ("While..\n");
		char buf[100];
		err = SSL_read(ssl, buf, sizeof(buf));
		int32_t ssl_error = SSL_get_error(ssl, err);
		switch (ssl_error) {
		case SSL_ERROR_NONE:
			printf("SSL_ERROR_NONE\n");
			break;
		case SSL_ERROR_WANT_READ:
			printf("SSL_ERROR_WANT_READ\n");
			break;
		case SSL_ERROR_WANT_WRITE:
			printf("SSL_ERROR_WANT_WRITE\n");
			break;
		case SSL_ERROR_ZERO_RETURN:
			printf("SSL_ERROR_ZERO_RETURN\n");
			break;
		default:
			break;
		}

		if(err>0 && (ssl_error == SSL_ERROR_NONE))
		{
			buf[err] = 0;

			int newLength = err+1;

			if(client_message)
			{
				newLength+=strlen(client_message);
			}

			char* newArray = calloc(newLength,sizeof(char));
			if(newArray)
			{
				if(client_message)
				{
					strcpy (newArray,client_message);
					strcat (newArray,buf);
				}else
				{
					strcpy (newArray,buf);
				}

				free(client_message);
				client_message = newArray;
			}

		}else
		{
			break;
		}
	}
	// read_size = recv(sock , client_message , 2000 , 0);

	//printf("[Handler] read %d\n", err);
	//client_message[err] = '\0';
	//if(read_size>0 && err==0)

	printf("[Handler] Request: %s\n",client_message);
	processRequest(client_message, sock, ssl);

	puts("[Handler] Client disconnected");
	fflush(stdout);

	//Free the socket pointer
	//free(info->requestSocket);

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


