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
//#include "stm32f4xx_hal_tim.h"
//#include "stm32_hal_tim.h"
#include "stm32f103xb.h"

#ifdef __cplusplus
}

class gtimer {
protected:
    union {
        uint16_t _mode;
        struct {
            uint16_t _inited:1;
            uint16_t _int:1;
            uint16_t _dma:1;
            uint16_t _ch1:1;
            uint16_t _ch2:1;
            uint16_t _ch3:1;
            uint16_t _ch4:1;
            uint16_t _ovf:1;    // ovf, cmp
            uint16_t _opm:1;
            uint16_t _down:1;   // 0: up, 
        };
    };
    TIM_HandleTypeDef _ht;
public:
    gtimer();
    gtimer(TIM_TypeDef *TIMx, uint32_t PSC, uint32_t ARR, uint16_t mode=0, void (*f)(TIM_HandleTypeDef *) = 0); // mode = 1 interrupt
    ~gtimer();

    void set(TIM_TypeDef *TIMx, uint32_t PSC, uint32_t ARR, uint16_t mode=0, void (*f)(TIM_HandleTypeDef *) = 0);
    void start();
    void stop();

    uint32_t psc(int v= -1);
    uint32_t arr(int v= -1);
    uint32_t cnt(int v= -1);

    uint32_t ccr1(int v= -1);
    uint32_t ccr2(int v= -1);
    uint32_t ccr3(int v= -1);
    uint32_t ccr4(int v= -1);

    void attach( void (*fi)(TIM_HandleTypeDef*) );
    void detach();
};

#endif // __cplusplus

#endif // __GTIMER_H__