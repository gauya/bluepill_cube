/*
 * gpwr.cpp
 *
 *  Created on: 2022. 3. 14.
 *      Author: seu
 */

#if 0
#include "stm32h7xx_hal.h"
#else
#include "stm32f1xx_hal.h"
#endif
//#include "stm32h7xx_hal_pwr.h"
//#include "stm32h7xx_hal_rcc.h"
#include "gpwr.h"
#include "gstr.h"
#include "glog.h"

#ifdef __cplusplus
extern "C" {
#endif

extern IWDG_HandleTypeDef hiwdg1;
extern RTC_HandleTypeDef hrtc;

/*
void HAL_GPIO_EXTI_Callback(n) {
	HAL_ResumeTick();
	HAL_PWR_DisableSleepOnExit();
}
*/

void pwr_sleep_mode(int intr) {
	if( intr )
		HAL_PWR_EnableSleepOnExit();

	/* Suspend SysTick */
	HAL_SuspendTick();

//	__HAL_RCC_PWR_CLK_ENABLE();
	/* Sleep Mode PWR_MAINREGULATOR_ON,PWR_SLEEPENTRY_WFI*/
	HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFE);

	/* Resume SysTick When System Wake-up */
	HAL_ResumeTick();
}
extern void SystemClock_Config();
extern void PeriphCommonClock_Config();

extern UART_HandleTypeDef huart1;
extern void SystemClock_Config(void);
extern void uart_receive_it();
void pwr_stop_mode(void) {
	/*
	HAL_PWR_EnableWakeUpPin(WakeSpi_Pin);
	HAL_PWREx_EnableUltraLowPower(); // Enable Ultra low power mode
	HAL_PWREx_EnableFastWakeUp(); // Enable the fast wake up from Ultra low power mode

	 */

	/* Suspend SysTick */
	HAL_SuspendTick();

//	__HAL_RCC_PWR_CLK_ENABLE();

	/* STOP Mode : PWR_MAINREGULATOR_ON,PWR_LOWPOWERREGULATOR_ON*/
//	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

	SystemClock_Config();
	/* Resume SysTick When System Wake-up */
	HAL_ResumeTick();

#if 0
	init_ticks(eTICK_VOL_100us);
	uart_receive_it();
#endif

//	PeriphCommonClock_Config();


	/* PLL Clock Recovery */
//	SystemClock_Config();
//	__HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc);
	/* Wait until the shadow registers are synchronized */
//	HAL_RTC_WaitForSynchro(&hrtc);
	/* Enable again the write-protection to prevent registers corruption */
//	__HAL_RTC_WRITEPROTECTION_ENABLE(&hrtc);
}

void pwr_standby_mode(void) {
//	__HAL_RCC_PWR_CLK_ENABLE();
#if 0
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 40960, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
#endif
	/* Enter STANDBY mode */
	HAL_PWR_EnterSTANDBYMode();
}

/*
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);

    HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 40960, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

    HAL_PWR_EnterSTANDBYMode();

 */
int check_pwr_flag() {
	if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB)) {
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU | PWR_FLAG_SB);
		return 1; // standby reset
	}
	return 0;
}

void pwr_mode(const char*s) {
	const char *cmd[] = { "sleep","stop","standby",0 };

	switch(instrs(s,cmd)) {
	case 0:
		gdebug(2,0,"pwr enter sleep mode\n");
		pwr_sleep_mode(0);
		gdebug(2,0,"woke up sleep mode\n");
		break;
	case 1:
		gdebug(2,0,"pwr enter stop mode\n");
		pwr_stop_mode();
		gdebug(2,0,"woke up stop mode\n");
		break;
	case 2:
		gdebug(2,0,"pwr enter standby mode\n");
		pwr_standby_mode();
//		gdebug(2,0,"pwr mode=%d\n",no);
		break;
	default:
		return;
	}
}

#if 0
void MX_RTC_Init(void) {

   RTC_TimeTypeDef sTime = {0};
   RTC_DateTypeDef sDate = {0};

   /** Initialize RTC Only  */   hrtc.Instance = RTC;
   hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
   hrtc.Init.AsynchPrediv =127;
   hrtc.Init.SynchPrediv = 255;
   hrtc.Init.OutPut = RTC_OUTPUT_WAKEUP;
   hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
   hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
   if (HAL_RTC_Init(&hrtc) != HAL_OK)   {
      Error_Handler();
   }

   /** Initialize RTC and set the Time and Date */
   sTime.Hours = 0x13;
   sTime.Minutes = 0x20;
   sTime.Seconds = 0x0;
   sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
   sTime.StoreOperation = RTC_STOREOPERATION_RESET;
   if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
   {
      Error_Handler();
   }
   sDate.WeekDay = RTC_WEEKDAY_THURSDAY;
   sDate.Month = RTC_MONTH_JANUARY;
   sDate.Date = 0x30;
   sDate.Year = 0x20;

   if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
   {
      Error_Handler();
   }
   HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
}
void Enter_StandbyMode(void)
{
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);

    HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 40960, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

    HAL_PWR_EnterSTANDBYMode();
}


#include <stdio.h>
#include "glog.h"

void check_rtc(const char *msg) {
	/* USER CODE BEGIN Check_RTC_BKUP */

	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
//	RTC_AlarmTypeDef sAlarm = {0};
	/* Set Date and Time (if not already done before)*/
	/* Read the Back Up Register 0 Data */

	int y,M,d,w,h,m,s;
	if(msg && *msg) {
//		gdebug(0,0,"  %s",msg);
		sscanf(msg,"%02x.%02x.%02x %x %02x.%02x.%02x",&y,&M,&d,&w,&h,&m,&s);
		gdebug(0,0,"set date 20%02x.%02x.%02x %x %02x:%02x:%02x",y,M,d,w,h,m,s);
// 22.03.16 4 09:14:55

	} else {
		return;
	}

	if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != RTC_KEY_DR1) {
//		RTC_DR1 = a7d0336e
//2069.1f.33 (7) 15:39:2e

	/* Configure RTC Calendar */
	/* USER CODE END Check_RTC_BKUP */

	/** Initialize RTC and set the Time and Date
	*/
		sTime.Hours = h;
		sTime.Minutes = m;
		sTime.Seconds = s;
		sTime.SubSeconds = 0x0;
		sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		sTime.StoreOperation = RTC_STOREOPERATION_RESET;
		if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
		{
//			Error_Handler();
		}
		sDate.WeekDay = w;
		sDate.Month = M;
		sDate.Date = d;
		sDate.Year = y;

		if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
		{
//			Error_Handler();
		}
		/* USER CODE BEGIN RTC_Init 2 */

		/* Writes a data in a RTC Backup data Register0 */
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_KEY_DR1);
	}
	else
	{
		/* Check if the Power On Reset flag is set */
		if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST) != RESET)
		{
			/* Turn on LED2: Power on reset occurred */
			//BSP_LED_On(LED2);
		}

		/* Check if Pin Reset flag is set */
		if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
		{
			/* Turn on LED3: External reset occurred */
			//BSP_LED_On(LED3);
		}
	}
		/* Clear source Reset Flag */
	__HAL_RCC_CLEAR_RESET_FLAGS();

	/* USER CODE END RTC_Init 2 */

}
#endif

#if 0
void rtc(const char*str) {
	RTC_DateTypeDef rd;
	RTC_TimeTypeDef rt;
	RTC_AlarmTypeDef ra;
	char buf[48];
	int y,M,d,w=0,h,m,s,num;

	const char*cms[] = { "-c","-d","-t","-a",0 };

	str = get_token(str, buf);
	if( buf[0] == '\0' ) {
		get_rtcdate(&rd,&rt);

		gdebug(2,0,"20%02x.%02x.%02x (%x) %02x:%02x:%02x\n"
				,rd.Year,rd.Month,rd.Date,rd.WeekDay,rt.Hours,rt.Minutes,rt.Seconds);
		return;
	}

	hrtc.Instance = RTC;
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	hrtc.Init.AsynchPrediv = 127;
	hrtc.Init.SynchPrediv = 255;
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
	HAL_RTC_Init(&hrtc);

	switch( instrs(buf,cms) ) {
	case 0: // -c clear DR1
		HAL_PWR_EnableBkUpAccess();
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x0);
		return;
	case 1: // -d chg date
		num = sscanf(str,"%02x%02x%02x %x",&y,&M,&d,&w);
		if(num != 4) {
			gdebug(0,0,">> rtc -d yymmdd w\n");
			break;
		}
		rd.WeekDay = w;
		rd.Month = M;
		rd.Date = d;
		rd.Year = y;

		HAL_RTC_SetDate(&hrtc, &rd, RTC_FORMAT_BCD);
		break;;
	case 2: // -t chg time
		num = sscanf(str,"%02x.%02x.%02x",&h,&m,&s);
		if(num != 3) {
			gdebug(0,0,">> rtc -t hh.mm.ss\n");
			break;
		}
		rt.Hours = h;
		rt.Minutes = m;
		rt.Seconds = s;
		rt.SubSeconds = 0x0;
		rt.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		rt.StoreOperation = RTC_STOREOPERATION_RESET;
		HAL_RTC_SetTime(&hrtc, &rt, RTC_FORMAT_BCD);
		break;
	case 3: // -a alarm
		num = sscanf(str,"%02x.%02x.%02x",&h,&m,&s);
		if(num != 3) {
			gdebug(0,0,">> rtc -a hh.mm.ss\n");
			break;
		}
		ra.AlarmTime.Hours = h;
		ra.AlarmTime.Minutes = m;
		ra.AlarmTime.Seconds = s;
		ra.AlarmTime.SubSeconds = 0x0;
		ra.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		ra.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
		ra.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
		ra.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
		ra.AlarmDateWeekDay = 0x1;
		ra.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS |RTC_ALARMMASK_MINUTES;
//		ra.AlarmMask = RTC_ALARMMASK_NONE;
		ra.Alarm = RTC_ALARM_A;

		HAL_RTC_SetAlarm_IT(&hrtc, &ra, RTC_FORMAT_BCD);

		break;
	default:;
	}
	HAL_PWR_EnableBkUpAccess();
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_KEY_DR1);
}


void init_rtc() {
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	RTC_AlarmTypeDef sAlarm = {0};

	hrtc.Instance = RTC;
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	hrtc.Init.AsynchPrediv = 127;
	hrtc.Init.SynchPrediv = 255;
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
	HAL_RTC_Init(&hrtc);

	HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_A, RTC_FORMAT_BCD);
	HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD);

	uint32_t rtc = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
	if ( rtc == RTC_KEY_DR1) {// already set
//		gdebug(10,0,"rtc (%x) already set\n",rtc);
		return;
	} else {
		gdebug(2,0,"rtc (%x) init\n",rtc);
	}

	sTime.Hours = 0x13;
	sTime.Minutes = 0x12;
	sTime.Seconds = 0x12;
	sTime.TimeFormat = RTC_HOURFORMAT_24;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);

	/* 19-12-12 */
	sDate.Year = 0x22;
	sDate.Month = 0x03;
	sDate.Date = 0x15;
	sDate.WeekDay = RTC_WEEKDAY_WEDNESDAY;

	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);

	/* Alarm every minute @ XX:XX:20 */
	  /** Enable the Alarm A
	  */
	sAlarm.AlarmTime.Hours = 0x0;
	sAlarm.AlarmTime.Minutes = 0x0;
	sAlarm.AlarmTime.Seconds = 0x20;
	sAlarm.AlarmTime.SubSeconds = 0x0;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = 0x1;
	sAlarm.Alarm = RTC_ALARM_A;
//	sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
	sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS |RTC_ALARMMASK_MINUTES;
	HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD);

	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_KEY_DR1);

	__HAL_RCC_CLEAR_RESET_FLAGS();
}

// time_t
void get_rtcdate(RTC_DateTypeDef *d,RTC_TimeTypeDef *t) {
	HAL_RTC_GetTime(&hrtc, t, RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc, d, RTC_FORMAT_BCD);

	return;
}
#endif

#ifdef __cplusplus
}

namespace std {

gpwr::gpwr() {
	// TODO Auto-generated constructor stub

}

gpwr::~gpwr() {
	// TODO Auto-generated destructor stub
}

} /* namespace std */
#endif // __cplusplus
