/*
 * base64.h
 *
 *  Created on: Jan 9, 2016
 *      Author: dishan
 */


#include <stdint.h>
#include <stdlib.h>

#ifndef BASE64_H_
#define BASE64_H_

char *base64_encode(unsigned char *data,
                    long input_length);

#endif /* BASE64_H_ */
