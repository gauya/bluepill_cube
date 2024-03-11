/*
 * glog.h
 *
 *  Created on: Dec 15, 2021
 *      Author: seu
 */

#ifndef GLOG_H_
#define GLOG_H_

#include <stdarg.h>
#include <stdio.h>
#include <gserial.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int log_level;
void gdebug(int lev, const char* fmt, ...);

#define ERROR_OCR(str) error_log(str, __FILE__, __LINE__) 
#define ERROR_LOG(str) error_log2(str, __FILE__, __LINE__) 

void error_log( const char *msg, const char *file, int line );
void error_log2( const char *msg, const char *file, int line );

int __io_putchar(int ch);
int __io_getchar(void);

#ifdef ARDUINO
int ggetc();
int gputc(int ch);
#endif // ARDUINO

#ifdef __cplusplus
}
#endif // __cplusplus
#endif /* GLOG_H_ */
