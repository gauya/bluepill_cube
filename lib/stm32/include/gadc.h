#ifndef __GADC_H__
#define __GADC_H__

#ifdef __cplusplus
extern "C" {
#endif

//#include <stm32yyxx_hal_conf.h>
//#include <stm32yyxx_ll_adc.h>
//#include <stm32yyxx_ll_gpio.h>
//#include <stm32yyxx_ll_rcc.h>
#include <stm32f1xx_hal.h>
#include <stm32f1xx_hal_adc.h>
#include <stm32f1xx_hal_gpio.h>
#include <stm32f1xx_hal_rcc.h>
#include <stm32f1xx_hal_dma.h>


extern void (*_stm32adc_callback)();

#ifdef __cplusplus
}
#endif

struct adc_channels {
  int ch;
  int sampling; // default 

  GPIO_TypeDef *port;
  int pin;

  int rank;
};

enum { // adc mode
    eADC_POLLING = 0,
    eADC_INTERRUPT,
    eADC_DMA,
    eADC_DMAINTERRUPT
};

enum { // adc error
    eADC_NOTSETUP = -1,
    eADC_NOK = -1,
    eADC_OK = 0,

};

class stm32adc {
private:
protected:
  ADC_HandleTypeDef *_ha; // ->Instance (ADC_TypeDef *adc);
  ADC_ChannelConfTypeDef _ac;
  struct adc_channels *_chs;  
  uint16_t _mode;
  int _status; 
  uint16_t _timeout;
  
  int add_channel(adc_channels *ac); // return channels;
  int add_channel(uint32_t ch, uint32_t samplerate=ADC_SAMPLETIME_41CYCLES_5); // Vref, temp, ADC_SAMPLETIME_2CYCLE_5
  void conv();
public:
  int _channel_num;

public:
  stm32adc();
  stm32adc(ADC_TypeDef *adc, struct adc_channels *ac);
  virtual ~stm32adc(){};
  
  void setup(ADC_TypeDef *adc, struct adc_channels *ac);
  void setup();

  void start();
  void stop();

  bool isready() { return (_status != eADC_NOTSETUP);}
  int channel_num() { return _channel_num; };
  uint16_t& mode() { return _mode; }
  ADC_HandleTypeDef *get_handle() { return _ha; }

  int read();
  int read(uint16_t *buf); // buf length = chs
};

class stm32adcint : public stm32adc {
protected:
public:
public:
    stm32adcint();
    stm32adcint(ADC_TypeDef *adc, struct adc_channels *ac, void (*intrf)());
    ~stm32adcint();
    void setup(ADC_TypeDef *adc, struct adc_channels *ac, void (*intrf)());
    void setup();

    void attach( void (*intrf)() );
    void detach();

    void start();
    void stop();

    int read();
};

class stm32adcdma : public stm32adc {
protected:
    uint16_t *_dmabuf;
    uint16_t *_outbuf;
public:
public:
    stm32adcdma();
    stm32adcdma(ADC_TypeDef *adc, struct adc_channels *ac, void (*intrf)(), uint16_t *dmabuf);
    ~stm32adcdma();
    
    void setup(ADC_TypeDef *adc, struct adc_channels *ac, void (*intrf)(), uint16_t *dmabuf);
    void setup();

    void attach( void (*intrf)() );
    void detach();

    void start();
    void stop();

    int read(uint16_t *bp);
    uint16_t *read();

};

#endif // __GADC_H__
