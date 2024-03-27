/*
 * gtick.h
 *
 *  Created on: 2021. 10. 14.
 *      Author: seu
 */

#ifndef GTICK_H_
#define GTICK_H_

#include <stdint.h>
#include "gutils.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum  { // 1:1ms, 10:100us, 100:10us, 1000:1us
	eTICK_VOL_1ms = 1,
	eTICK_VOL_100us = 10,
	eTICK_VOL_10us = 100,
	eTICK_VOL_1us = 1000
} eTick_vol;

void init_ticks(eTick_vol);
uint32_t get_utick();

// must be recreate get_mtime() and get_utime()
// __weak attribute is applied
uint32_t get_stime(void);
uint32_t get_mtime(void);
uint32_t get_utime(void);
uint32_t get_utick(void);
uint32_t get_tick(void);

void delay_ms(uint32_t msec);
void delay_us(uint32_t msec);

uint32_t elapsed_us(uint32_t m );
uint32_t belapsed_us(uint32_t *m );
uint32_t elapsed_ms(uint32_t m );
uint32_t belapsed_ms(uint32_t *m );

typedef struct {
	uint32_t	check;
	uint32_t	m;
} timed_t;

int is_timed(timed_t *t);
int is_timed_acm(timed_t *t); // accumulated by the elapsed time
void set_timed(timed_t *t, uint32_t c);

int is_utimed(timed_t *t);
void set_utimed(timed_t *t, uint32_t c);

#ifdef __cplusplus
 }

class gtick : timed_t {
public:
	gtick( uint32_t val=0 );
	virtual ~gtick() {};

	int is_timed();
	void set_timed( uint32_t val );
	uint32_t elapsed(); // milisecond
};

#endif // __cplusplus

#endif /* GTICK_H_ */
