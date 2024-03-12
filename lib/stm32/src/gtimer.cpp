#include "gtimer.h"
#include "stm32f1xx_hal_rcc.h"
#include "glog.h"

#ifdef __cplusplus

TIM_HandleTypeDef *htim1;
TIM_HandleTypeDef *htim2;
TIM_HandleTypeDef *htim3;
TIM_HandleTypeDef *htim4;

extern int timer_cnt; // test
void (*TIM_callback)(TIM_HandleTypeDef *) = 0;

void TIM1_IRQHandler(void) {
  HAL_TIM_IRQHandler(htim1);
	if( TIM_callback ) {
        TIM_callback(htim1);
    }
}

void TIM2_IRQHandler(void) {
  HAL_TIM_IRQHandler(htim2);
	if( TIM_callback ) {
        TIM_callback(htim2);
    }
  TIM2->SR &= ~TIM_SR_UIF;  
}

void TIM3_IRQHandler(void) {
  HAL_TIM_IRQHandler(htim3);
	if( TIM_callback ) {
        TIM_callback(htim3);
    }
    timer_cnt++; // test
    TIM3->SR &= ~TIM_SR_UIF;
}

void TIM4_IRQHandler(void) {
  HAL_TIM_IRQHandler(htim4);
	if( TIM_callback ) {
        TIM_callback(htim4);
    }
}


#if 0
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if( TIM_callback ) {
        TIM_callback(htim);
    }
}
#endif

gtimer::gtimer() {
    _mode = 0;
    _ht.Instance = 0;;
}

gtimer::gtimer(TIM_TypeDef *TIMx, uint32_t PSC, uint32_t ARR, uint16_t mode, void (*f)(TIM_HandleTypeDef *)) {
    set(TIMx, PSC, ARR, mode, f);
}

gtimer::~gtimer() {

}

void HUL_TIM_clk_enable( TIM_TypeDef* tim ) {
#ifdef TIM1
    if(tim == TIM1) {
        __HAL_RCC_TIM1_CLK_ENABLE(); return;
    }
#endif
#ifdef TIM2
    if(tim == TIM2) {
        __HAL_RCC_TIM2_CLK_ENABLE(); return;
    }
#endif
#ifdef TIM3
    if(tim == TIM3) {
        __HAL_RCC_TIM3_CLK_ENABLE(); return;
    }
#endif
#ifdef TIM4
    if(tim == TIM4) {
        __HAL_RCC_TIM4_CLK_ENABLE(); return;
    }
#endif
}

void gtimer::set(TIM_TypeDef *TIMx, uint32_t PSC, uint32_t ARR, uint16_t mode, void (*f)(TIM_HandleTypeDef *)) {
    TIM_Base_InitTypeDef bi;

    bi.ClockDivision = 0;
    bi.Period = ARR;
    bi.Prescaler = PSC;
    bi.CounterMode = TIM_COUNTERMODE_UP; 
    bi.RepetitionCounter = 0;
    bi.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; //TIM_AUTORELOAD_PRELOAD_ENABLE;

    _ht.Instance = TIMx;
    _ht.Init = bi;

    _mode = 0;

    if( mode == 1 ) {
        _int = 1;

        if(f) attach(f);
       
        IRQn_Type irqn = (TIMx == TIM2)? TIM2_IRQn : (TIMx == TIM3)? TIM3_IRQn : (TIMx == TIM4)? TIM4_IRQn : (IRQn_Type)0;
    
        if( irqn ) {
            HAL_NVIC_SetPriority(irqn, 0, 0);
            HAL_NVIC_EnableIRQ(irqn);
        }
    }

    HUL_TIM_clk_enable(_ht.Instance);
#if 1
    if (HAL_TIM_Base_Init(&_ht) != HAL_OK)
    {
        ERROR_LOG("");
    }
#else
    _ht.State = HAL_TIM_STATE_BUSY;

    /* Set the Time Base configuration */
    TIM_Base_SetConfig(_ht.Instance, &_ht.Init);

    /* Initialize the DMA burst operation state */
    _ht.DMABurstState = HAL_DMA_BURST_STATE_READY;

    /* Initialize the TIM channels state */
    TIM_CHANNEL_STATE_SET_ALL(&_ht, HAL_TIM_CHANNEL_STATE_READY);
    TIM_CHANNEL_N_STATE_SET_ALL(&_ht, HAL_TIM_CHANNEL_STATE_READY);

    /* Initialize the TIM state*/
    _ht.State = HAL_TIM_STATE_READY;
#endif
}

void gtimer::start() {
    if( _int ) {
        HAL_TIM_Base_Start_IT(&_ht);
    } else {
        HAL_TIM_Base_Start(&_ht);
    }
}

void gtimer::stop() {
    if( _int ) {
        HAL_TIM_Base_Stop_IT(&_ht);
    } else {
        HAL_TIM_Base_Stop(&_ht);
    }
}

void gtimer::pwm(int channel, uint32_t pulse, uint32_t mode, gpio_t *gpio_ch) {
  if( channel < 0 || channel > 3) {
    ERROR_LOG("pwm channel over range");
    return;
  }

    // GPIO, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_AFx_TIMx
    if(gpio_ch) {
        GPIO_InitTypeDef gi;
        gi.Pin = (1 << gpio_ch->pin);
        gi.Mode = GPIO_MODE_AF_PP;
        gi.Pull = GPIO_PULLUP;
    //    gi.Alternate = GPIO_AFx_TIMx;
        HAL_GPIO_Init(gpio_ch->port, &gi);
    }

  uint32_t ch = (channel == 0)? TIM_CHANNEL_1 : (channel == 1)? TIM_CHANNEL_2: (channel == 2)? TIM_CHANNEL_3: TIM_CHANNEL_4;

  TIM_OC_InitTypeDef sConfigOC;
  sConfigOC.OCMode = mode;
  sConfigOC.Pulse = pulse;
  HAL_TIM_OC_ConfigChannel(&_ht, &sConfigOC, ch);
  HAL_TIM_PWM_Start(&_ht, ch);
}

void gtimer::pwm_stop(int channel) {
  if( channel < 0 || channel > 3) {
    ERROR_LOG("pwm channel over range");
    return;
  }
  uint32_t ch = (channel == 0)? TIM_CHANNEL_1 : (channel == 1)? TIM_CHANNEL_2: (channel == 2)? TIM_CHANNEL_3: TIM_CHANNEL_4;
  HAL_TIM_PWM_Stop(&_ht, ch);
}

uint32_t gtimer::psc(int v) {
    if(v < 0) {
        return _ht.Instance->PSC;
    } else {
        _ht.Instance->PSC = v;
        return _ht.Instance->PSC;
    }
}

uint32_t gtimer::arr(int v) {
    if(v < 0) {
        return _ht.Instance->ARR;
    } else {
        _ht.Instance->ARR = v;
        return _ht.Instance->ARR;
    }
}

uint32_t gtimer::cnt(int v) {
    if(v < 0) {
        return _ht.Instance->CNT;
    } else {
        _ht.Instance->CNT = v;
        return _ht.Instance->CNT;
    }
}

uint32_t gtimer::ccr1(int v) {
    if(v < 0) {
        return _ht.Instance->CCR1;
    } else {
        _ht.Instance->CCR1 = v;
        return _ht.Instance->CCR1;
    }
}

uint32_t gtimer::ccr2(int v) {
    if(v < 0) {
        return _ht.Instance->CCR2;
    } else {
        _ht.Instance->CCR2 = v;
        return _ht.Instance->CCR2;
    }
}

uint32_t gtimer::ccr3(int v) {
    if(v < 0) {
        return _ht.Instance->CCR3;
    } else {
        _ht.Instance->CCR3 = v;
        return _ht.Instance->CCR3;
    }
}

uint32_t gtimer::ccr4(int v) {
    if(v < 0) {
        return _ht.Instance->CCR4;
    } else {
        _ht.Instance->CCR4 = v;
        return _ht.Instance->CCR4;
    }
}


void gtimer::attach( void (*f)(TIM_HandleTypeDef *h)) {
    TIM_callback = f;
}

void gtimer::detach() {
    TIM_callback = 0;
}

#endif // __cplusplus