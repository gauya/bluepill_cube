#ifndef __GTIMER_H__
#define __GTIMER_H__

enum {
    eTimerBase = 0,
    eTimerInt,
};


#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "stm32f103xb.h"
#include "main.h"
#include "ggpio.h"

#ifdef __cplusplus
}

class gtimer {
protected:
    union {
        uint16_t _mode;
        struct {
            uint16_t _inited:1; // setup 
            uint16_t _int:1; // interrupt
            uint16_t _dma:1; // use dma
            uint16_t _ch1:2; // channel PWM:1, INPUT:2
            uint16_t _ch2:2; // 
            uint16_t _ch3:2;
            uint16_t _ch4:2;
            uint16_t _ovf:1;    // ovf, cmp
            uint16_t _opm:1;
            uint16_t _down:1;   // 0: up, 
        };
    };
    TIM_HandleTypeDef *_ht;
public:
    gtimer();
    gtimer(TIM_TypeDef *TIMx, uint32_t ARR, uint32_t PSC=0, void (*f)(TIM_HandleTypeDef *) = 0); // mode = 1 interrupt
    ~gtimer();

    void set(TIM_TypeDef *TIMx, uint32_t ARR, uint32_t PSC=0, void (*f)(TIM_HandleTypeDef *) = 0);
    void start();
    void stop();

    void pwm(int channel, uint32_t pulse, uint32_t mode=TIM_OCMODE_PWM1,gpio_t *g=0); // mode=TIM_OCMODE_INACTIVE,TIM_OCMODE_FROZEN, TIM_OCMODE_TIMING,PWM1,PWM2, ... pulse : CCR, channel=0~3
    void pwm_start(int channel);
    void pwm_stop(int channel);

    uint32_t psc(int v= -1);
    uint32_t arr(int v= -1);
    uint32_t cnt(int v= -1);
    int ccr(int ch, int v=-1);

    void attach( void (*fi)(TIM_HandleTypeDef*) );
    void detach();
};

#endif // __cplusplus

#endif // __GTIMER_H__