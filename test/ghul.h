#ifndef __GHUL_H
#define __GHUL_H

// #incluce <stm32f103xb.h>

#ifdef __cplusplus 
extern "C" {
#endif

#include <stm32f1xx_hal_adc.h>

int HUL_ADC_clk_enable(ADC_TypeDef *adc);
int HUL_GPIO_clk_enable(GPIO_TypeDef *gpio);
int HUL_ADC_nvic(ADC_TypeDef *adc, int enable);
int HUL_DMA_nvic(ADC_TypeDef *adc, int enable);

#ifdef __cplusplus 
}
#endif

#endif // __GHUL_H
