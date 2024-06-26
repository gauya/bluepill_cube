
/*
 * app_etc.h
 *
 *  Created on: Mar 25, 2022
 *      Author: seu
 */

#ifndef LIB_INC_APP_ETC_H_
#define LIB_INC_APP_ETC_H_

#include "gparse.h"
#include "glog.h"
#include "gtty.h"
#include "gproc.h"
#include "gtick.h"
#include "gutils.h"
#include "gprintf.h"
#include "gserial.h"

int add_proc(const char*pn, void (*f)(const char*), int timer=-1, int act=1);

void setup();
void loop();

void command_list(const char*str);
//void scadule_traffic(const char*);
void system_reset(const char *s);
void dis_tinycalc(const char *s);
void ps_ctrl(const char *s);

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* LIB_INC_APP_ETC_H_ */
