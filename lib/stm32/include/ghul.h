#ifndef __GHUL_H
#define __GHUL_H

//#include <stm32f1xx_hal_def.h>
//#include <stm32f103xb.h>
#include <stm32f1xx_hal.h>
#include <stm32f1xx_hal_adc.h>
#include <stm32f1xx_hal_dma.h>
#include <stm32f1xx_hal_gpio.h>
#include <stm32f1xx_hal_tim.h>

#ifdef __cplusplus 
extern "C" {
#endif

// return -1 on error
int HUL_ADC_clk_enable(ADC_TypeDef *adc);
int HUL_GPIO_clk_enable(GPIO_TypeDef *gpio);
int HUL_ADC_nvic(ADC_TypeDef *adc, int enable);
int HUL_ADC_DMA_nvic(ADC_TypeDef *adc, int enable);
int HUL_TIM_clk_enable(TIM_TypeDef *tim);
int HUL_SPI_clk_enable(SPI_TypeDef *spi, int remap=0);
int HUL_SPI_nvic(SPI_TypeDef *spi, int enable);
int HUL_SPI_idx(SPI_TypeDef *spi);

#ifdef __cplusplus 
}
#endif

#endif // __GHUL_H
