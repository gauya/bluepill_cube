#include <ggpio.h>
#include <glog.h>
#include <stm32f1xx_hal_exti.h>

#if 0


((MODE) == GPIO_MODE_INPUT)              ||\
                            ((MODE) == GPIO_MODE_OUTPUT_PP)          ||\
                            ((MODE) == GPIO_MODE_OUTPUT_OD)          ||\
                            ((MODE) == GPIO_MODE_AF_PP)              ||\
                            ((MODE) == GPIO_MODE_AF_OD)              ||\
                            ((MODE) == GPIO_MODE_IT_RISING)          ||\
                            ((MODE) == GPIO_MODE_IT_FALLING)         ||\
                            ((MODE) == GPIO_MODE_IT_RISING_FALLING)  ||\
                            ((MODE) == GPIO_MODE_EVT_RISING)         ||\
                            ((MODE) == GPIO_MODE_EVT_FALLING)        ||\
                            ((MODE) == GPIO_MODE_EVT_RISING_FALLING) ||\
                            ((MODE) == GPIO_MODE_ANALOG)
#define IS_GPIO_SPEED(SPEED) (((SPEED) == GPIO_SPEED_FREQ_LOW)  || ((SPEED) == GPIO_SPEED_FREQ_MEDIUM) || \
                              ((SPEED) == GPIO_SPEED_FREQ_HIGH) || ((SPEED) == GPIO_SPEED_FREQ_VERY_HIGH))
#define IS_GPIO_PULL(PULL) (((PULL) == GPIO_NOPULL) || ((PULL) == GPIO_PULLUP) || \
                            ((PULL) == GPIO_PULLDOWN))
#define  GPIO_NOPULL        0x00000000U   /*!< No Pull-up or Pull-down activation  */
#define  GPIO_PULLUP        0x00000001U   /*!< Pull-up activation                  */
#define  GPIO_PULLDOWN      0x00000002U   /*!< Pull-down activation                */


typedef struct
{
  __IO uint32_t ACR;      /*!< FLASH access control register,   Address offset: 0x00 */
  __IO uint32_t KEYR;     /*!< FLASH key register,              Address offset: 0x04 */
  __IO uint32_t OPTKEYR;  /*!< FLASH option key register,       Address offset: 0x08 */
  __IO uint32_t SR;       /*!< FLASH status register,           Address offset: 0x0C */
  __IO uint32_t CR;       /*!< FLASH control register,          Address offset: 0x10 */
  __IO uint32_t OPTCR;    /*!< FLASH option control register ,  Address offset: 0x14 */
  __IO uint32_t OPTCR1;   /*!< FLASH option control register 1, Address offset: 0x18 */
} FLASH_TypeDef;

/**
  * @brief General Purpose I/O
  */

typedef struct
{
  __IO uint32_t MODER;    /*!< GPIO port mode register,               Address offset: 0x00      */
  __IO uint32_t OTYPER;   /*!< GPIO port output type register,        Address offset: 0x04      */
  __IO uint32_t OSPEEDR;  /*!< GPIO port output speed register,       Address offset: 0x08      */
  __IO uint32_t PUPDR;    /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
  __IO uint32_t IDR;      /*!< GPIO port input data register,         Address offset: 0x10      */
  __IO uint32_t ODR;      /*!< GPIO port output data register,        Address offset: 0x14      */
  __IO uint32_t BSRR;     /*!< GPIO port bit set/reset register,      Address offset: 0x18      */
  __IO uint32_t LCKR;     /*!< GPIO port configuration lock register, Address offset: 0x1C      */
  __IO uint32_t AFR[2];   /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
} GPIO_TypeDef;
                            
#endif

#ifdef __cplusplus
extern "C" {
#endif

int gpio_readpin(GPIO_TypeDef *g, int pin) {
    if( pin < 0 || pin > 15) return -1;

    return (g->IDR & (1 << pin))? 1 : 0;
}

int exti_irqn(int exti) {
    switch(exti) {
    case 0: return EXTI0_IRQn;
    case 1: return EXTI1_IRQn;   
    case 2: return EXTI2_IRQn;
    case 3: return EXTI3_IRQn;    
    case 4: return EXTI4_IRQn;
    case 5:
    case 6:
    case 7:    
    case 8:
    case 9: return EXTI9_5_IRQn;   
    case 10: 
    case 11:    
    case 12:
    case 13:    
    case 14:
    case 15: return EXTI15_10_IRQn;
    default:;
    }
    return -1; 
} 

void EXTI0_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}
void EXTI1_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}
void EXTI2_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}
void EXTI3_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}
void EXTI4_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}
void EXTI9_5_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9);
}
void EXTI15_10_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
}

#if 0
void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin)
{
  /* EXTI line interrupt detected */
  if(__HAL_GPIO_EXTI_GET_IT(GPIO_Pin) != RESET)
  {
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
    HAL_GPIO_EXTI_Callback(GPIO_Pin);
  }
}

void HAL_EXTI_IRQHandler(EXTI_HandleTypeDef *hexti) {

}
#endif

int right_bitno( uint16_t val ) {
    for( int i=0; i < 16; i++, val >>= 1 ) {
        if( val & 1) return i;
    }
    return -1;
}

void (*_exti_callback)(uint16_t bits) = 0;
void (*_exti_callback_s[16])(uint16_t bits) = {0,};
__IO uint16_t _exti_callback_bf = 0;

void exti_handler(uint16_t GPIO_Pin) {
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_Pin) != RESET) { // (EXTI->PR & (__EXTI_LINE__)
        uint16_t fl = EXTI->PR & (GPIO_Pin);

        // use may be gwgpio
        if( _exti_callback ) {
            _exti_callback( fl ); 
        }        
       
        fl &= _exti_callback_bf;
        if( fl ) {
//            __disable_irq();
            for( int i=0; i < 15 && fl ; i++, fl >>= 1) {
                if( fl & 1 ) {
                    if( _exti_callback_s[i] ) {
                        _exti_callback_s[i](i);
                    }
//                    __HAL_GPIO_EXTI_CLEAR_IT(1 << i);         // (EXTI->PR = (__EXTI_LINE__))
                }
            }
            __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);         // (EXTI->PR = (__EXTI_LINE__)) HAL_EXTI_ClearITPendingBit(GPIO_Pin);
//            __enable_irq();
        }        
    }    
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    exti_handler(GPIO_Pin);
}
#if 0
void HAL_GPIO_EXTI0_IRQHandler(void) {
    exti_handler(GPIO_PIN_0);
}

void HAL_GPIO_EXTI1_IRQHandler(void) {
    exti_handler(GPIO_PIN_1);
}

void HAL_GPIO_EXTI2_IRQHandler(void) {
    exti_handler(GPIO_PIN_2);
}

void HAL_GPIO_EXTI3_IRQHandler(void) {
    exti_handler(GPIO_PIN_3);
}

void HAL_GPIO_EXTI4_IRQHandler(void) {
    exti_handler(GPIO_PIN_4);
}

void HAL_GPIO_EXTI9_5_IRQHandler(void) {
    exti_handler( GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 );
}

void HAL_GPIO_EXTI15_10_IRQHandler(void) {
    exti_handler( GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 );
}
#endif
#ifdef __cplusplus
}

ggpio::ggpio(GPIO_TypeDef *g, int pin, uint32_t mode, int pullup, int speed) {
    this->port = g;
    if( pin >= 0 && pin <=15 ) {
        this->pin = pin;
        this->_mask = (1 << pin );
    } else {
        this->pin = -1;
        this->_mask = 0;
    }

    _flag = 0;
    _mode = mode;
    _speed = (speed == 0 || speed == 1 || speed == 3)? speed : 2 ;
    _pullup = (pullup == 1 || pullup == 2)? pullup: 0;

    //init();
}

ggpio::ggpio() {
    this->port = 0;
    this->pin = -1;
    this->_mask = 0;
    _flag = 0;
}

ggpio::~ggpio() {
    // 
    this->port = 0;
    this->_mask = 0;

    if( _mode == eGPIO_EXTI_RISING_FALLING || _mode == eGPIO_EXTI_RISING  || _mode == eGPIO_EXTI_FALLING ) {
        detach();
    }

    _inited = 0;
}

void ggpio::init(GPIO_TypeDef *g, int pin, uint32_t mode, int pullup, int speed) {
    this->port = g;
    if( pin >= 0 && pin <=15 ) {
        this->pin = pin;
        this->_mask = (1 << pin );
    } else {
        this->pin = -1;
        this->_mask = 0;
    }

    _flag = 0;
    _mode = mode;
    _speed = (speed == 0 || speed == 1 || speed == 3)? speed : 2 ;
    _pullup = (pullup == 1 || pullup == 2)? pullup: 0;

    init();
}

void ggpio::init() {
    if( !this->port || this->pin < 0 || this->pin > 15) {
        this->pin = -1;
        _flag = 0;
        return;
    }
    GPIO_InitTypeDef gi;

    if( IS_GPIO_ALL_INSTANCE(this->port)) {
        if( IS_GPIO_ALL_INSTANCE(GPIOA) && (this->port == GPIOA) ) { __HAL_RCC_GPIOA_CLK_ENABLE(); }
        else if( IS_GPIO_ALL_INSTANCE(GPIOB) && (this->port == GPIOB )) { __HAL_RCC_GPIOB_CLK_ENABLE(); }
        else if( IS_GPIO_ALL_INSTANCE(GPIOC) && (this->port == GPIOC )) { __HAL_RCC_GPIOC_CLK_ENABLE(); }
        else if( IS_GPIO_ALL_INSTANCE(GPIOD) && (this->port == GPIOD )) { __HAL_RCC_GPIOD_CLK_ENABLE(); }
        else if( IS_GPIO_ALL_INSTANCE(GPIOE) && (this->port == GPIOE )) { __HAL_RCC_GPIOE_CLK_ENABLE(); }
//        else if( IS_GPIO_ALL_INSTANCE(GPIOH) && (this->port == GPIOH )) { __HAL_RCC_GPIOH_CLK_ENABLE(); }
    } else {
        return ;
    }

    uint32_t mode = 0;
    switch(_mode) {
        case eGPIO_OUTPP:
            mode = GPIO_MODE_OUTPUT_PP;
            break;
        case eGPIO_OUTOD:
            mode = GPIO_MODE_OUTPUT_OD;
            break;
        case eGPIO_INPUT:
            mode = GPIO_MODE_INPUT;
            break;
        case eGPIO_ADC:
            mode = GPIO_MODE_ANALOG;
            break;
        case eGPIO_EXTI_RISING:
            mode = GPIO_MODE_IT_RISING; //MODE_INPUT | EXTI_IT | TRIGGER_RISING; // 
            break;
        case eGPIO_EXTI_FALLING:
            mode = GPIO_MODE_IT_FALLING; //MODE_INPUT | EXTI_IT | TRIGGER_FALLING; // GPIO_MODE_IT_FALLING
            break;
        case eGPIO_EXTI_RISING_FALLING:
            mode = GPIO_MODE_IT_RISING_FALLING; // MODE_INPUT | EXTI_IT | TRIGGER_RISING | TRIGGER_FALLING; // 
            break;
        default:;
    }

    if( mode | (GPIO_MODE_IT_RISING & GPIO_MODE_IT_FALLING) ) { // if IT
        uint16_t p = this->_mask;
        for(int i=0; i < 16; i++, p >>=1 ) {
            if( !(p & 1) ) continue; // not pin
            int irqn = exti_irqn(i);
            if( irqn < 0 ) continue; // error
#if 0
  EXTI_InitTypeDef EXTI_InitStruct;
  EXTI_InitStruct.Line = EXTI_LINE5;
  EXTI_InitStruct.Mode = EXTI_MODE_INTERRUPT;
  EXTI_InitStruct.Trigger = EXTI_TRIGGER_RISING;
  HAL_EXTI_Init(&EXTI_InitStruct);
#endif
            HAL_NVIC_SetPriority( (IRQn_Type)irqn, 0, 0 );
            HAL_NVIC_EnableIRQ( (IRQn_Type)irqn );
            
            //_pullup = GPIO_PULLDOWN;
        }
    }

    gi.Pin   = (uint32_t)this->_mask;
    gi.Mode  = (uint32_t)mode;
    gi.Speed = (uint32_t)_speed;
    gi.Pull  = (uint32_t)_pullup;

    HAL_GPIO_Init( this->port, &gi );

    _inited = 1;
}

void ggpio::deinit() {
    if( isvalidpin() ) {
        detach();
    }
    this->port = 0;
    this->pin = -1;
    this->_mode = 0;
    this->_mask = 0;
    this->_flag = 0;
    this->_pullup = 0;
    this->_speed = 0;
}

void ggpio::attach( void (*extif)(uint16_t), int exti, int updn) { // exti=1(rise),2(fall),3(both) , pullup=0(nopullup),1(pullup),2(pulldn) 
    if( !isvalidpin() ) return;

    if( updn != -1 || exti != -1) {
        if(updn < 0 || updn > 2) updn = _pullup;
        if(exti < 1 || exti > 3) exti = 3;

        _pullup = updn;
        _speed = 0;
 
        GPIO_InitTypeDef gi;
        gi.Pin   = (uint32_t)this->_mask;
        gi.Mode  = (exti == 1)? GPIO_MODE_IT_RISING : (exti == 2)? GPIO_MODE_IT_FALLING : GPIO_MODE_IT_RISING_FALLING;
        gi.Speed = (uint32_t)_speed;
        gi.Pull  = (uint32_t)_pullup;
        HAL_GPIO_Init( this->port, &gi );

        int irqn = exti_irqn(this->pin);
        HAL_NVIC_SetPriority( (IRQn_Type)irqn, 0, 0 );
        HAL_NVIC_EnableIRQ( (IRQn_Type)irqn );    
    }

    __disable_irq();

    _exti_callback_s[this->pin] = extif;
    _exti_callback_bf |= _mask;

    __enable_irq();
}

void ggpio::detach() {
    if( !isvalidpin() ) return;
    
    __disable_irq();
    _exti_callback_s[this->pin] = 0;
    _exti_callback_bf &= ~_mask;
    __enable_irq();
//    _exti_callback = 0;
}

int ggpio::read() {
    if( ! isinit() ) return -1;
  
    if(_mode == eGPIO_INPUT || _mode == eGPIO_EXTI_RISING || _mode == eGPIO_EXTI_FALLING || _mode == eGPIO_EXTI_RISING_FALLING ) {
        return (this->port->IDR & this->_mask)? 1 : 0;
    }
    if(_mode == eGPIO_OUTPP || _mode == eGPIO_OUTOD ) {
        return (this->port->ODR & this->_mask)? 1 : 0;
    }

}

int ggpio::write( int val ) {
    if( ! isinit() || (this->_mode != eGPIO_OUTPP && this->_mode != eGPIO_OUTOD) ) return -1;
#if 0
    uint16_t v = this->port->ODR & ~(_mask);
    if( val ) v |= _mask;
    this->port->ODR = v;
#else
    if( val ) {
        this->port->BSRR = _mask; 
    } else {
        this->port->BRR = _mask;
    }
#endif
    return (val)? 1 : 0;
}

void ggpio::high() {
    this->write(1);
}

void ggpio::low() {
    this->write(0);
}

void ggpio::toggle() {
    if( ! isinit() || (this->_mode != eGPIO_OUTPP && this->_mode != eGPIO_OUTOD) ) {
        return;
    }
    if( this->port->ODR & _mask ) {
        this->port->BRR = _mask; 
    } else {
        this->port->BSRR = _mask; 
    }
}

// ---------------------------------------------------------------------------------------------------------------------

gwgpio::gwgpio(GPIO_TypeDef *g,uint16_t mask,uint32_t mode, int pullup, int speed) {
    this->port = g;
    this->mask = mask;

    _flag = 0;
    _mode = mode;
    _speed = (speed == 0 || speed == 1 || speed == 3)? speed : 2 ;
    _pullup = (pullup == 1 || pullup == 2)? pullup: 0;

    // init();
}

gwgpio::gwgpio() {
    this->port = 0;
    this->mask = 0;
    _flag = 0;
}

gwgpio::~gwgpio() {
    // 
    this->port = 0;
    this->mask = 0;

    if( _mode == eGPIO_EXTI_RISING_FALLING || _mode == eGPIO_EXTI_RISING  || _mode == eGPIO_EXTI_FALLING ) {
        detach();
    }

    _inited = 0;
}

void gwgpio::init(GPIO_TypeDef *g,uint16_t mask,uint32_t mode, int pullup, int speed) {
    this->port = g;
    this->mask = mask;

    _flag = 0;
    _mode = mode;
    _speed = (speed == 0 || speed == 1 || speed == 3)? speed : 2 ;
    _pullup = (pullup == 1 || pullup == 2)? pullup: 0;

    init();
}

void gwgpio::init() {
    GPIO_InitTypeDef gi;

    if( IS_GPIO_ALL_INSTANCE(this->port)) {
        if( IS_GPIO_ALL_INSTANCE(GPIOA) && (this->port == GPIOA) ) { __HAL_RCC_GPIOA_CLK_ENABLE(); }
        else if( IS_GPIO_ALL_INSTANCE(GPIOB) && (this->port == GPIOB )) { __HAL_RCC_GPIOB_CLK_ENABLE(); }
        else if( IS_GPIO_ALL_INSTANCE(GPIOC) && (this->port == GPIOC )) { __HAL_RCC_GPIOC_CLK_ENABLE(); }
        else if( IS_GPIO_ALL_INSTANCE(GPIOD) && (this->port == GPIOD )) { __HAL_RCC_GPIOD_CLK_ENABLE(); }
        else if( IS_GPIO_ALL_INSTANCE(GPIOE) && (this->port == GPIOE )) { __HAL_RCC_GPIOE_CLK_ENABLE(); }
//        else if( IS_GPIO_ALL_INSTANCE(GPIOH) && (this->port == GPIOH )) { __HAL_RCC_GPIOH_CLK_ENABLE(); }
    } else {
        return ;
    }

    uint32_t mode = 0;
    switch(_mode) {
        case eGPIO_OUTPP:
            mode =  GPIO_MODE_OUTPUT_PP;
            break;
        case eGPIO_OUTOD:
            mode = GPIO_MODE_OUTPUT_OD;
            break;
        case eGPIO_AFPP:
            mode = GPIO_MODE_AF_PP;
            break;
        case eGPIO_INPUT:
            mode = GPIO_MODE_INPUT;
            break;
        case eGPIO_ADC:
            mode = GPIO_MODE_ANALOG;
            break;
        case eGPIO_EXTI_RISING:
            mode = GPIO_MODE_IT_RISING; //MODE_INPUT | EXTI_IT | TRIGGER_RISING; // 
            break;
        case eGPIO_EXTI_FALLING:
            mode = GPIO_MODE_IT_FALLING; //MODE_INPUT | EXTI_IT | TRIGGER_FALLING; // GPIO_MODE_IT_FALLING
            break;
        case eGPIO_EXTI_RISING_FALLING:
            mode = GPIO_MODE_IT_RISING_FALLING; // MODE_INPUT | EXTI_IT | TRIGGER_RISING | TRIGGER_FALLING; // 
            break;
        default:;
    }

    if( mode | (GPIO_MODE_IT_RISING & GPIO_MODE_IT_FALLING) ) { // if IT
        uint16_t p = this->mask;
        for(int i=0; i < 16; i++, p >>=1 ) {
            if( !(p & 1) ) continue; // not pin
            int irqn = exti_irqn(i);
            if( irqn < 0 ) continue; // error
            
            HAL_NVIC_SetPriority( (IRQn_Type)irqn, 0, 0 );
            HAL_NVIC_EnableIRQ( (IRQn_Type)irqn );

        }
    }

    gi.Pin = this->mask;
    gi.Mode = mode;
    gi.Speed = _speed;
    gi.Pull = _pullup; //

    HAL_GPIO_Init( this->port, &gi );

    _inited = 1;
}

void gwgpio::attach( void (*extif)(uint16_t)) {
    _exti_callback = extif;
}

void gwgpio::detach() {
    _exti_callback = 0;
}

int gwgpio::read() {
    if( ! isinit() || _mode != eGPIO_INPUT ) return -1;
    
    return (this->port->IDR & this->mask);
}


int gwgpio::write(uint16_t val) {
    if( ! isinit() || (this->_mode != eGPIO_OUTPP && this->_mode != eGPIO_OUTOD) ) return -1;
    
    this->port->BSRR = ( val & this->mask );
    
    return (val & this->mask);
}

// specific one pin

int gwgpio::read(int pin) {
    if( ! isinit() || !isvalidpin(pin) ) return -1;
    if(this->_mode == eGPIO_INPUT) {
        return (this->port->IDR & (1 << pin))? 1 : 0;
    } else 
    if(this->_mode == eGPIO_ADC) {
        // adc
    }
    return -1;
}

int gwgpio::write(int pin, int val) {
    if( ! isinit() || !isvalidpin(pin) || (this->_mode != eGPIO_OUTPP && this->_mode != eGPIO_OUTOD) ) return -1;

    if( val ) {
        this->port->BSRR = (1 << pin);
    } else {
        this->port->BRR = (1 << pin);
    }

    return (val)? 1 : 0;
}

void gwgpio::high( int pin ) {
    this->write(pin, 1);
}

void gwgpio::low( int pin ) {
    this->write(pin, 0);
}

void gwgpio::toggle(int pin) {
    if( ! isinit() || !isvalidpin(pin) || (this->_mode != eGPIO_OUTPP && this->_mode != eGPIO_OUTOD) ) return;

    if( this->port->ODR & (1 << pin) ) {
        this->port->BRR = (1 << pin ); 
    } else {
        this->port->BSRR = (1 << pin); 
    }
}

void gwgpio::high() {
    if( ! isinit() || (this->_mode != eGPIO_OUTPP && this->_mode != eGPIO_OUTOD) ) return;
    this->port->BSRR = this->mask;
}

void gwgpio::low() {
    if( ! isinit() || (this->_mode != eGPIO_OUTPP && this->_mode != eGPIO_OUTOD) ) return;
    this->port->BRR = this->mask;
}

void gwgpio::toggle() {
    if( ! isinit() || (this->_mode != eGPIO_OUTPP && this->_mode != eGPIO_OUTOD) ) return;

    uint16_t cval = this->port->ODR & this->mask;

    this->port->BRR = cval;
    this->port->BSRR = ( ~cval & this->mask );
}

#endif // __cplusplus