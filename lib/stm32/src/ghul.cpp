#include "ghul.h"

#if 1
int HUL_ADC_clk_enable(ADC_TypeDef *adc) {
#if defined(ADC1)
  if (adc == ADC1) {
    __HAL_RCC_ADC1_CLK_ENABLE();
    return 1;
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
    return 1;
  } 
#endif

  return 0;
}

int HUL_GPIO_clk_enable(GPIO_TypeDef *gpio) {
  
  if( ! IS_GPIO_ALL_INSTANCE(gpio) ) 
    return 0;
#if defined(GPIOA)
  if( gpio == GPIOA ) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    return 1;
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
    return 1;
  }
#endif
#if defined(GPIOD)
  if( gpio == GPIOD ) {
    __HAL_RCC_GPIOD_CLK_ENABLE();
    return 1;
  }
#endif
#if defined(GPIOE)
  if( gpio == GPIOE ) {
    __HAL_RCC_GPIOE_CLK_ENABLE();
    return 1;
  }
#endif
#if defined(GPIOF)
  if( gpio == GPIOFF ) {
    __HAL_RCC_GPIOF_CLK_ENABLE();
    return 1;
  }
#endif
#if defined(GPIOG)
  if( gpio == GPIOG ) {
    __HAL_RCC_GPIOG_CLK_ENABLE();
    return 1;
  }
#endif
#if defined(GPIOH)
  if( gpio == GPIOH ) {
    __HAL_RCC_GPIOH_CLK_ENABLE();
    return 1;
  }
#endif


  return 0;
}

int HUL_ADC_nvic(ADC_TypeDef *adc, int enable) {
#if 1 // F1  
  if( ADC1 == adc ) {
    if( enable ) {
      HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0); // M4 ADC_IRQn
      HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
      return 1;
    } else {
      HAL_NVIC_DisableIRQ(ADC1_2_IRQn);
      return 0;
    }
  } else return -1;
#else 
  if( ADC1 == adc ) {
    if( enable ) {
      HAL_NVIC_SetPriority(ADC1_IRQn, 0, 0); // M4 ADC_IRQn
      HAL_NVIC_EnableIRQ(ADC1_IRQn);
      return 1;
    } else {
      HAL_NVIC_DisableIRQ(ADC1_IRQn);
      return 0;
    }
  }  else return -1;
#endif
  return 0;
}

int HUL_DMA_nvic(ADC_TypeDef *adc, int enable) {
#if 1 // F1  
  if( ADC1 == adc ) {
    if( enable ) {
      __HAL_RCC_DMA1_CLK_ENABLE();

      HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
      HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);      

      return 1;
    } else {
      HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);   
      return 0;
    }

  } else return -1;
#else // F4
  if( ADC1 == adc ) {
    if( enable ) {
      __HAL_RCC_DMA2_CLK_ENABLE();
      HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
      HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  
      return 1;
    } else {

      return 0;
    }
  }  else return -1;
#endif

}
#endif