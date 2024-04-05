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
#include <gprintf.h>

#ifdef __cplusplus
extern "C" {
#endif

void gdebug(int lev, const char* fmt, ...);

#define error_log(str) error_log_funcline(str, __FILE__, __FUNCTION__, __LINE__) 
#define error_log2(str) error_log2_funcline(str, __FILE__, __FUNCTION__, __LINE__) 

void error_log_funcline( const char *msg, const char *file, const char *func, int line );
void error_log2_funcline( const char *msg, const char *file, const char *func, int line );

int __io_putchar(int ch);
int __io_getchar(void);

#ifdef ARDUINO
int ggetc();
int gputc(int ch);
#endif // ARDUINO

#ifdef __cplusplus
}
#endif // __cplusplus

int& log_level(int log=-1);

#endif /* GLOG_H_ */
