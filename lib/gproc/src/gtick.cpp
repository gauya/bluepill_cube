/*
 * gtick.cpp
 *
 *  Created on: 2021. 10. 14.
 *      Author: seu
 */
#include <stddef.h>
#include <gtick.h>
#include <giwdg.h>
#include <gproc.h>
#include "stm32f1xx.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTPROC_PER_MSEC	0

static uint32_t __usec_per_tick;
static uint32_t __usec_max;
static double __ticks_per_usec;

static __IO uint32_t __STicks = 0;
static __IO uint32_t __mTicks = 0;
static __IO uint32_t __uTicks = 0;

static eTick_vol __tick_vol = eTICK_VOL_10us;

inline uint32_t get_tick() {
	return (uint32_t)(SysTick->VAL & 0xffffff);
}

inline uint32_t get_utick() {
	return (uint32_t)(get_tick() / __ticks_per_usec);
}

uint32_t HAL_GetTick(void) {
	return __mTicks;
}


#if ( __RT_PROC__ != 0)
__attribute((__weak__)) void run_rtproc_all() {
	return;
}

__attribute((__weak__)) void run_rtproc(uint16_t i) {
	return;
}
#endif

void SysTick_Handler(void)
{
	HAL_IncTick();

	static uint16_t cnt=0, mcnt=0;
	__uTicks += __usec_per_tick;
	if(++cnt >= __tick_vol) {
		__mTicks++;
		cnt = 0;
		if(++mcnt > 1000) {
			__STicks++;
			mcnt = 0;
		}
	}

#if ( __RT_PROC__ != 0 )
	// check rtrun proc
	if( ! isinrtp() ) return;

// Adjusted to process once per msec regardless of clock
	__disable_irq();

#if (RTPROC_PER_MSEC == 1) 
	switch(__tick_vol) {
	case eTICK_VOL_1ms:
		run_rtproc_all();
		break;
	case eTICK_VOL_100us:
		run_rtproc(cnt);
		break;
	case eTICK_VOL_10us:
		if( (cnt%10) == 0 ) {
			run_rtproc(cnt /10);
		}
		break;
	case eTICK_VOL_1us:
		if( (cnt%100) == 0 ) {
			run_rtproc(cnt/100);
		}
		break;
	}
#else
	run_rtproc_all();
#endif

	__enable_irq();
#endif	  
}

#ifdef __DEBUG__
#include "glog.h"
#endif

void init_ticks(eTick_vol vol) {
	__tick_vol = vol; // eTICK_VOL_10us; //eTICK_VOL_1us; // eTICK_VOL_1ms; // default eTICK_VOL_100us
	__usec_per_tick = 1000 / __tick_vol;
	__mTicks = 0;
	__uTicks = 0;

	HAL_SYSTICK_Config(SystemCoreClock / 1000U / __tick_vol);
	__usec_max = (uint32_t)(SysTick->LOAD & 0xffffff); // + 1;
	__ticks_per_usec = __usec_max / (double)(__usec_per_tick);

#ifdef __DEBUG__
	gdebug(100,"__tick_vol=%d, __usec_max=%lu, __usec_per_tick=%lu, __ticks_per_usec=%5.2f %lu/%lu\n"
			,__tick_vol, __usec_max, __usec_per_tick, __ticks_per_usec,__mTicks,__uTicks);
#endif
}

uint32_t get_stime() {
	return __STicks;
}

uint32_t get_mtime() {
	return __mTicks;
}

uint32_t get_utime() {
	return (__uTicks + (__usec_per_tick - get_utick()));
}

#if 0
__attribute ((__weak__)) uint32_t get_stime() {
	return 0;
}

__attribute ((__weak__)) uint32_t get_mtime() {
//	 return HAL_GetTick();
	return 0;
}

__attribute ((__weak__)) uint32_t get_utime() {
	return 0;
}

__attribute ((__weak__)) uint32_t get_utick() {
	//	uint32_t ticks = SysTick->VAL;

	return 0;
}

__attribute ((__weak__)) uint32_t get_tick() {
	return 0;
}
#endif

int is_timed(timed_t *t) {
	if(t->check != 0 && elapsed_ms(t->m) >= t->check) {
		t->m = get_mtime(); 
		return 1;
	}
	return 0;
}

int is_timed_acm(timed_t *t) {
	if(t->check != 0 && elapsed_ms(t->m) >= t->check) {
		t->m += t->check;
		return 1;
	}
	return 0;
}

void set_timed(timed_t *t, uint32_t c) {
	t->check = c;
	t->m = get_mtime();
}

int is_utimed(timed_t *t) {
	if(t->check != 0 && elapsed_us(t->m) >= t->check) {
		t->m = get_utime(); 
		return 1;
	}
	return 0;
}

void set_utimed(timed_t *t, uint32_t c) {
	t->check = c;
	t->m = get_utime();
}

#define U32ALLSET (uint32_t)(-1)
//#define U32ALLSET (uint32_t)(0xffffffff)
void delay_ms( uint32_t ms ) {
#if 0
	ms += get_mtime();
	while(get_mtime() < ms);
#else
	uint32_t cm = get_mtime();
	if( (U32ALLSET - cm) < ms ) {
		while( get_mtime() >= cm );
		ms -= (U32ALLSET - cm);
	} else {
		ms += cm;
	}
	iwdg_clear();

	while(get_mtime() < ms) {
		iwdg_clear();
	}
#endif
}

void delay_us( uint32_t us ) {
#if 0
	us += get_utime();
	while(get_utime() < us);
#else
	uint32_t cu = get_utime();
	if( (U32ALLSET - cu) < us ) {
		while( get_utime() >= cu );
		us -= (U32ALLSET - cu);
	} else {
		us += cu;
	}
	while(get_utime() < us) iwdg_clear();
#endif
}
#undef U32ALLSET

uint32_t elapsed_us( uint32_t e ) {
	return dif_u32(e,get_utime());
}

uint32_t belapsed_us( uint32_t* e ) {
	uint32_t d = *e;
	*e = get_utime();
	return dif_u32(d,*e);
}

uint32_t elapsed_ms( uint32_t e ) {
	return dif_u32(e,get_mtime());
}

uint32_t belapsed_ms( uint32_t* e ) {
	uint32_t d = *e;
	*e = get_mtime();
	return dif_u32(d,*e);
}

#ifdef __cplusplus
}

gtick::gtick( uint32_t check ) {
	this->check = check;
	this->m = get_mtime();
}

int gtick::is_timed() {
	if(this->check != 0 && elapsed_ms(this->m) >= this->check) {
		this->m = get_mtime(); 
		return 1;
	}
	return 0;	
}

void gtick::set_timed(uint32_t c) {
	if( c != (uint32_t)-1 ) {
		this->check = c;
	}
	this->m = get_mtime();
}


uint32_t gtick::elapsed() {
	return dif_u32(this->m,get_mtime());
}

#endif // __cplusplus

