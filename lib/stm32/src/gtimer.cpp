#include "gtimer.h"
#include "stm32f1xx_hal_rcc.h"
#include "glog.h"
#include "ghul.h"

#ifdef __cplusplus
extern "C"
{
#endif

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

extern int timer_cnt; // test
void (*TIM_callback)(TIM_HandleTypeDef *) = 0;
#if 0
    HAL_TIM_IC_CaptureCallback(htim);
    HAL_TIM_OC_DelayElapsedCallback(htim);
    HAL_TIM_PWM_PulseFinishedCallback(htim);
    HAL_TIM_PeriodElapsedCallback(htim);
    HAL_TIMEx_BreakCallback(htim);
    HAL_TIM_TriggerCallback(htim);
    HAL_TIMEx_CommutCallback(htim);
#endif

void TIM1_IRQHandler(void) {
    HAL_TIM_IRQHandler(&htim1);
    if (TIM_callback) {
        TIM_callback(&htim1);
    }
}

void TIM2_IRQHandler(void) {
    HAL_TIM_IRQHandler(&htim2);
    if (TIM_callback) {
        TIM_callback(&htim2);
    }
    TIM2->SR &= ~TIM_SR_UIF;
}

void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim3);
    if (TIM_callback) {
        TIM_callback(&htim3);
    }
    timer_cnt++; // test
    TIM3->SR &= ~TIM_SR_UIF;
}

void TIM4_IRQHandler(void) {
    HAL_TIM_IRQHandler(&htim4);
    if (TIM_callback)
    {
        TIM_callback(&htim4);
    }
}

#if 1
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    timer_cnt++;
    if (TIM_callback)
    {
        TIM_callback(htim);
    }
}
#endif

#ifdef __cplusplus
}
#endif

gtimer::gtimer() {
    _mode = 0;
    _ht = 0;
}

gtimer::gtimer(TIM_TypeDef *tim, uint32_t period, uint32_t psc, void (*f)(TIM_HandleTypeDef *)) {
    _ht = 0;
    _mode = 0;
    set(tim, period, psc, f);
}

gtimer::~gtimer() {
    stop();
    detach();
}


void gtimer::set(TIM_TypeDef *tim, uint32_t ARR, uint32_t PSC, void (*f)(TIM_HandleTypeDef *)) {
    _mode = 0;

    _ht = (tim == TIM1)? &htim1 : (tim == TIM2)? &htim2 : (tim == TIM3)? &htim3 : (tim == TIM4)? &htim4 : (TIM_HandleTypeDef *)0;
    if (_ht == 0)
        return;

    _ht->Instance = tim;
    _ht->Init.Period = ARR;
    _ht->Init.Prescaler = PSC;
    _ht->Init.CounterMode = TIM_COUNTERMODE_UP;
    //_ht->Init.RepetitionCounter = 0;
    _ht->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    _ht->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; //TIM_AUTORELOAD_PRELOAD_DISABLE; // TIM_AUTORELOAD_PRELOAD_ENABLE;

    if (HAL_TIM_Base_Init(_ht) != HAL_OK) {
        error_log("timer baseinit");
    }

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(_ht, &sClockSourceConfig) != HAL_OK) {
        error_log("timer config clock source");
    }

    HUL_TIM_clk_enable(_ht->Instance);
#if 1
    if (f) {
        _int = 1;

        attach(f);

        IRQn_Type irqn = (tim == TIM2) ? TIM2_IRQn : (tim == TIM3) ? TIM3_IRQn
                                                 : (tim == TIM4)   ? TIM4_IRQn
                                                                   : (IRQn_Type)0;

        if (irqn) {
            HAL_NVIC_SetPriority(irqn, 0, 0);
            HAL_NVIC_EnableIRQ(irqn);
        }
    }
#endif
    _ht->State = HAL_TIM_STATE_READY;
    _inited = 1;
}

void gtimer::start() {
    if (_int) {
        HAL_TIM_Base_Start_IT(_ht);
    }
    else {
        HAL_TIM_Base_Start(_ht);
    }
}

void gtimer::stop() {
    if (_int) {
        HAL_TIM_Base_Stop_IT(_ht);
    }
    else {
        HAL_TIM_Base_Stop(_ht);
    }
}

void gtimer::pwm(int channel, uint32_t pulse, uint32_t mode, gpio_t *gpio_ch) {
    if (channel < 1 || channel > 4) {
        error_log("pwm channel over range");
        return;
    }
    uint32_t ch = (channel == 1) ? TIM_CHANNEL_1 : (channel == 2) ? TIM_CHANNEL_2
                                               : (channel == 3)   ? TIM_CHANNEL_3
                                                                  : TIM_CHANNEL_4;

//  __HAL_AFIO_REMAP_TIM2_PARTIAL_1();

    if (HAL_TIM_OC_Init(_ht) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(_ht) != HAL_OK) {
        Error_Handler();
    }

    TIM_MasterConfigTypeDef sMasterConfig = {0};
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(_ht, &sMasterConfig) != HAL_OK) {
        error_log("timer MasterConfigSync");
    }

    TIM_OC_InitTypeDef sConfigOC;

#if 0  
    sConfigOC.OCMode = TIM_OCMODE_PWM1; //mode;
    sConfigOC.Pulse = pulse;
    HAL_TIM_OC_ConfigChannel(_ht, &sConfigOC, ch);
#else
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = pulse;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(_ht, &sConfigOC, ch) != HAL_OK) {
        error_log("timer pwm");
    }
#endif
    if(channel==1) { _ch1 = 1;} else
    if(channel==2) { _ch2 = 1;} else
    if(channel==3) { _ch3 = 1;} else
    if(channel==4) { _ch4 = 1;} 
    
    if (gpio_ch) {
        HUL_GPIO_clk_enable(gpio_ch->port);

        GPIO_InitTypeDef gi;
        gi.Pin = (1 << gpio_ch->pin);
        gi.Mode = GPIO_MODE_AF_PP;
        gi.Speed = GPIO_SPEED_FREQ_HIGH;
        gi.Pull = GPIO_PULLUP;
        //    gi.Alternate = GPIO_AFx_TIMx;
        HAL_GPIO_Init(gpio_ch->port, &gi);
    }
}

void gtimer::pwm_start(int channel) {
    if (channel < 1 || channel > 4) {
        error_log("pwm channel over range");
        return;
    }
    uint32_t ch = (channel == 1) ? TIM_CHANNEL_1 : (channel == 2) ? TIM_CHANNEL_2
                                               : (channel == 3)   ? TIM_CHANNEL_3
                                                                  : TIM_CHANNEL_4;
    HAL_TIM_PWM_Start(_ht, ch);
}

void gtimer::pwm_stop(int channel) {
    if (channel < 1 || channel > 4) {
        error_log("pwm channel over range");
        return;
    }
    uint32_t ch = (channel == 1) ? TIM_CHANNEL_1 : (channel == 2) ? TIM_CHANNEL_2
                                               : (channel == 3)   ? TIM_CHANNEL_3
                                                                  : TIM_CHANNEL_4;
    HAL_TIM_PWM_Stop(_ht, ch);
}

uint32_t gtimer::psc(int v) {
    if (v < 0) {
        return _ht->Instance->PSC;
    }
    else {
        _ht->Instance->PSC = v;
        return _ht->Instance->PSC;
    }
}

uint32_t gtimer::arr(int v) {
    if (v < 0) {
        return _ht->Instance->ARR;
    }
    else {
        _ht->Instance->ARR = v;
        return _ht->Instance->ARR;
    }
}

uint32_t gtimer::cnt(int v) {
    if (v < 0) {
        return _ht->Instance->CNT;
    }
    else {
        _ht->Instance->CNT = v;
        return _ht->Instance->CNT;
    }
}

int gtimer::ccr(int ch, int v) {
    if( ch < 1 || ch > 4 ) return -1;
    if( v == -1 ) {
        return (ch==1)? _ht->Instance->CCR1 : (ch==2)? _ht->Instance->CCR2 :(ch==3)? _ht->Instance->CCR3 : _ht->Instance->CCR4;
    }
    switch(ch) {
    case 1:
        _ht->Instance->CCR1 = v; break;
    case 2:
        _ht->Instance->CCR2 = v; break;
    case 3:
        _ht->Instance->CCR3 = v; break;
    case 4:
        _ht->Instance->CCR4 = v; break;
    }
    return v;
}

void gtimer::attach(void (*f)(TIM_HandleTypeDef *h)) {
    TIM_callback = f;
}

void gtimer::detach() {
    TIM_callback = 0;
}
