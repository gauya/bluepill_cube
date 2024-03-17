/*
 * gpwr.h
 *
 *  Created on: 2022. 3. 14.
 *      Author: seu
 */

#ifndef _GPWR_H_
#define _GPWR_H_

//#define RTC_KEY_DR1	(0x32f2)
#define RTC_KEY_DR1	(0x0523)

#ifdef __cplusplus
extern "C" {
#endif

#if 0
void pwr_sleep_mode(int);
void pwr_stop_mode();
void pwr_standby_mode();
int check_pwr_flag();
void pwr_mode(const char*s);

void init_rtc();
void rtc(const char *msg);
void get_rtcdate(RTC_DateTypeDef *d,RTC_TimeTypeDef *t);
#endif

#ifdef __cplusplus
}

namespace std {

class gpwr final {
public:
	gpwr();
	virtual ~gpwr();
};

} /* namespace std */

#endif // __cplusplus
#endif /* _GPWR_H_ */
