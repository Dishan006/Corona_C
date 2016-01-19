#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<pthread.h>
#include <unistd.h>
#include "Server.h"


int main()
{
    pthread_t server_thread;

    if(pthread_create( &server_thread , NULL ,  StartServer , NULL) < 0)
     {
         perror("could not create the main server thread");
     }

    char* command = NULL;
    while (1)
     {
         printf("\nServer:");
         int n;
         scanf ("%d",&n);

         if(n==0)
         {
             printf("Exit requested!!! \n");
             setStopIssued(1);
             sleep(5);
             pthread_cancel(server_thread);
             break;
         }else
         {
        	 printf("Unknown Command! \n");
         }

         free(command);
     }

    pthread_join(server_thread, NULL);


	return 0;
}
