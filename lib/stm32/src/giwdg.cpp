#ifdef __cplusplus
extern "C" {
#endif

#include <giwdg.h>
#include <stm32f1xx_hal.h>
#include <stm32f1xx_ll_iwdg.h>
#include "main.h"
#include "glog.h"

IWDG_HandleTypeDef hiwdg;

void iwdg_init(int rl) {
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    ERROR_LOG(""); //Error_Handler();
  }

#if 0 //LL_DRIVER
  HAL_IWDG_Init(&hiwdg);
  LL_IWDG_EnableWriteAccess(IWDG);

  // 32kHz / 32 = 1kHz
  LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_32);

  // counter (999+1) is 1sec
  LL_IWDG_SetReloadCounter(IWDG, rl);

  while (LL_IWDG_IsReady(IWDG) != 1);
  LL_IWDG_ReloadCounter(IWDG);
#endif  
}

void iwdg_clear() {
	HAL_IWDG_Refresh(&hiwdg);
}

void iwdg_set(uint16_t ms) {
  LL_IWDG_SetReloadCounter(IWDG, ms); // WRITE_REG(IWDGx->RLR, IWDG_RLR_RL & Counter);
}

#ifdef __cplusplus
}
#endif
