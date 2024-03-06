/*
 * glog.cpp
 *
 *  Created on: Dec 15, 2021
 *      Author: seu
 */

#include <glog.h>

#include <stdarg.h>
#include <stdio.h>
#include "gserial_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

int log_level = 2;

#ifdef ARDUINO
#include <Arduino.h>

void gdebug(int lev, const char *fmt, ...){ 
	if( lev > log_level ) return;

	va_list ap; 
	va_start(ap, fmt);
	int n = vsnprintf(NULL, 0, fmt, ap);
	char *str = new char[n+1];
	vsprintf(str, fmt, ap);
	va_end(ap);

	Serial.print(str);
	delete [] str;
}

int ggetc() {
	return Serial.read();
}

int gputc(int ch) {
	return Serial.write(ch);
}


#else
void gdebug(int lev, const char* fmt, ...) {
	if( lev > log_level ) return;

	char buf[128];
	va_list ap;
	va_start(ap,fmt);
	vsnprintf(buf,127,fmt,ap);
	va_end(ap);

	gputs(buf);
	return;
}

#endif

#ifdef __cplusplus
}
#endif
