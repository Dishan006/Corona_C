/*
 * requestHandler.h
 *
 *  Created on: Jan 14, 2016
 *      Author: dishan
 */
#include <openssl/ssl.h>

#ifndef REQUESTHANDLER_H_
#define REQUESTHANDLER_H_

#define CHK_NULL(x) if ((x)==NULL) exit (1)
#define CHK_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
#define CHK_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(2); }

void processRequest(char* request, int sock, SSL* ssl);

#endif /* REQUESTHANDLER_H_ */
