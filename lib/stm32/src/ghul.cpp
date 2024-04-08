#include "ghul.h"

int HUL_ADC_clk_enable(ADC_TypeDef *adc) {
#if defined(ADC1)
  if (adc == ADC1) {
    __HAL_RCC_ADC1_CLK_ENABLE();
    return 0;
  } 
#endif
#if defined(ADC2)
  if (adc == ADC2) {
    __HAL_RCC_ADC2_CLK_ENABLE();
    return 1;
  } 
#endif
#if defined(ADC3)
  if (adc == ADC3) {
    __HAL_RCC_ADC3_CLK_ENABLE();
    return 2;
  } 
#endif

  return -1;
}

int HUL_GPIO_clk_enable(GPIO_TypeDef *gpio) 
{
  if( ! IS_GPIO_ALL_INSTANCE(gpio) ) 
    return -1;
#if defined(GPIOA)
  if( gpio == GPIOA ) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    return 0;
  }
#endif
#if defined(GPIOB)
  if( gpio == GPIOB ) {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    return 1;
  }
#endif
#if defined(GPIOC)
  if( gpio == GPIOC ) {
    __HAL_RCC_GPIOC_CLK_ENABLE();
    return 2;
  }
#endif
#if defined(GPIOD)
  if( gpio == GPIOD ) {
    __HAL_RCC_GPIOD_CLK_ENABLE();
    return 3;
  }
#endif
#if defined(GPIOE)
  if( gpio == GPIOE ) {
    __HAL_RCC_GPIOE_CLK_ENABLE();
    return 4;
  }
#endif
#if defined(GPIOF)
  if( gpio == GPIOFF ) {
    __HAL_RCC_GPIOF_CLK_ENABLE();
    return 5;
  }
#endif
#if defined(GPIOG)
  if( gpio == GPIOG ) {
    __HAL_RCC_GPIOG_CLK_ENABLE();
    return 6;
  }
#endif
#if defined(GPIOH)
  if( gpio == GPIOH ) {
    __HAL_RCC_GPIOH_CLK_ENABLE();
    return 7;
  }
#endif
  return -1;
}

int HUL_SPI_clk_enable(SPI_TypeDef *spi, int remap)
{
  if( ! IS_SPI_ALL_INSTANCE(spi) ) 
    return -1;

#if defined(SPI1)
  if( spi == SPI1 ) {
    __HAL_RCC_SPI1_CLK_ENABLE();
    if(remap) __HAL_AFIO_REMAP_SPI1_ENABLE();
    return 0;
  }
#endif
#if defined(SPI2)
  if( spi == SPI2 ) {
    __HAL_RCC_SPI2_CLK_ENABLE();
    //if(remap) __HAL_AFIO_REMAP_SPI2_ENABLE();
    return 1;
  }
#endif
#if defined(SPI3)
  if( spi == SPI3 ) {
    __HAL_RCC_SPI3_CLK_ENABLE();
    //if(remap) __HAL_AFIO_REMAP_SPI3_ENABLE();
    return 2;
  }
#endif
#if defined(SPI4)
  if( spi == SPI4 ) {
    __HAL_RCC_SPI4_CLK_ENABLE();
    //if(remap) __HAL_AFIO_REMAP_SPI4_ENABLE();
    return 3;
  }
#endif
#if defined(SPI5)
  if( spi == SPI5 ) {
    __HAL_RCC_SPI5_CLK_ENABLE();
    //if(remap) __HAL_AFIO_REMAP_SPI5_ENABLE();
    return 4;
  }
#endif
  return -1;
}

int HUL_ADC_nvic(ADC_TypeDef *adc, int enable) {
#ifdef STM32F1 // F1  
  if( ADC1 == adc ) {
    if( enable ) {
      HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0); // M4 ADC_IRQn
      HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
    } else {
      HAL_NVIC_DisableIRQ(ADC1_2_IRQn);
    }
    return 0;
  } 
#elif defined STM32F4 
  if( ADC1 == adc ) {
    if( enable ) {
      HAL_NVIC_SetPriority(ADC1_IRQn, 0, 0); // M4 ADC_IRQn
      HAL_NVIC_EnableIRQ(ADC1_IRQn);
    } else {
      HAL_NVIC_DisableIRQ(ADC1_IRQn);
    }
    return 0;
  }  
#endif
  return -1;
}

int HUL_ADC_DMA_nvic(ADC_TypeDef *adc, int enable) {
#if STM32F1 // F1  
  if( ADC1 == adc ) {
    if( enable ) {
      __HAL_RCC_DMA1_CLK_ENABLE();

      HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
      HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);      
    } else {
      HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);   
    }
    return 0;
  } 
  return -1;
#elif defined STM32F4 // F4
  if( ADC1 == adc ) {
    if( enable ) {
      __HAL_RCC_DMA2_CLK_ENABLE();
      HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
      HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
    } else {

    }
    return 0;
  }  
#endif
  return -1;
}

int HUL_TIM_clk_enable(TIM_TypeDef *tim) {
#ifdef TIM1
    if (tim == TIM1) {
        __HAL_RCC_TIM1_CLK_ENABLE();
        return 0;
    }
#endif
#ifdef TIM2
    if (tim == TIM2) {
        __HAL_RCC_TIM2_CLK_ENABLE();
        return 1;
    }
#endif
#ifdef TIM3
    if (tim == TIM3) {
        __HAL_RCC_TIM3_CLK_ENABLE();
        return 2;
    }
#endif
#ifdef TIM4
    if (tim == TIM4) {
        __HAL_RCC_TIM4_CLK_ENABLE();
        return 3;
    }
#endif
#ifdef TIM5
    if (tim == TIM5) {
        __HAL_RCC_TIM5_CLK_ENABLE();
        return 4;
    }
#endif
#ifdef TIM6
    if (tim == TIM6) {
        __HAL_RCC_TIM6_CLK_ENABLE();
        return 5;
    }
#endif
#ifdef TIM7
    if (tim == TIM7) {
        __HAL_RCC_TIM7_CLK_ENABLE();
        return 6;
    }
#endif
#ifdef TIM8
    if (tim == TIM8) {
        __HAL_RCC_TIM8_CLK_ENABLE();
        return 7;
    }
#endif
return -1;
}
