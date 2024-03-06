#include "gadc.h"

#ifdef __cplusplus
extern "C" {
#endif

ADC_HandleTypeDef *hadcp = 0;
DMA_HandleTypeDef *hdma_adcp; // = &hdma_adc1;

void ADC_IRQHandler(void) {
  if( hadcp )
    HAL_ADC_IRQHandler(hadcp);
}

#if 0 // F4
void DMA2_Stream0_IRQHandler(void) {
  if( hdma_adcp ) {
    HAL_DMA_IRQHandler(hdma_adcp);
  }
}
#else // F1
void DMA1_Channel1_IRQHandler(void) {
  if( hdma_adcp ) {
    HAL_DMA_IRQHandler(hdma_adcp);
  }
}
#endif

void Error_Handler(void);

void (*_stm32adc_callback)() = 0;

int adc_completed = 0;
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
  adc_completed++;

  if( _stm32adc_callback  ) {
    _stm32adc_callback();
  }
}

#ifdef __cplusplus
}
#endif

stm32adc::stm32adc() {
  _ha.Instance = 0;
  _channel_nr = 0;
  _timeout = 1; // ms
  _status = eADC_NOTSETUP; // -1
  _mode = eADC_POLLING; // 0       (_mode & 0x7)
}

//#include <stm32f4xx_hal.h>

stm32adc::stm32adc(ADC_TypeDef *adc, struct adc_channels *ac) {
  _ha.Instance = 0;
  _channel_nr = 0;
  _timeout = 1; // ms
  _status = eADC_NOTSETUP; // -1
  _mode = eADC_POLLING; // 0       (_mode & 0x7)

//  setup(adc, ac);
}

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
//__HAL_RCC_GPIOA_CLK_ENABLE
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

#include <glog.h>
void stm32adc::setup(ADC_TypeDef *adc, struct adc_channels *ac) {
  HUL_ADC_clk_enable(_ha.Instance);

  _ha.Instance = adc;
  _channel_nr = 0;
  _timeout = 1; // ms
  _status = 0;
 
  int chs = 0;
  for(; (ac+chs)->ch != -1 ; chs++);
  
  _ha.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  _ha.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  _ha.Init.NbrOfConversion = chs;                   // <> _channel_nr
  _ha.Init.DiscontinuousConvMode = DISABLE;
  _ha.Init.ScanConvMode = ENABLE;
  _ha.Init.ContinuousConvMode = ENABLE;

// multi, continues, use dma
#if defined(STM32F4) || defined(STM32F7)
  _ha.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  _ha.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  _ha.Init.Resolution = ADC_RESOLUTION_12B;
  _ha.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  if( _mode == eADC_DMA || _mode == eADC_DMAINTERRUPT ) {
    _ha.Init.DMAContinuousRequests = ENABLE;
  } else {
    _ha.Init.DMAContinuousRequests = DISABLE;
  }
#endif //STM32F4

  if (HAL_ADC_Init(&_ha) != HAL_OK) // --> call HAL_ADC_MspInit()
  {
    Error_Handler();
  }

  _chs = ac;

  for(int i=0; i < _ha.Init.NbrOfConversion; i++) {
    add_channel(ac+i);
  }

  //HAL_ADC_TempSensor_Enable();
  //HAL_ADC_TempSensorVrefintCmd(ENABLE);
  //while(HAL_ADCEx_Calibration_Start(&_ha) != HAL_OK);
  //HAL_ADC_SelfCalibration_Start();
  //HAL_ADC_Calibration_Start();
  
  /* Set ADC number of conversion */
  //hadc->Instance->SQR1 &= ~(ADC_SQR1_L);
  //hadc->Instance->SQR1 |=  ADC_SQR1(hadc->Init.NbrOfConversion);

  _status = 0;
}

int stm32adc::add_channel(uint32_t ch,uint32_t samplerate) {
  ADC_ChannelConfTypeDef sConfig = {0};
  sConfig.Channel = ch;
  sConfig.SamplingTime = samplerate; // default ADC_SAMPLETIME_84CYCLES; 
  sConfig.Rank = ++_channel_nr; // 1 ~ ,  

  if (HAL_ADC_ConfigChannel(&_ha, &sConfig) != HAL_OK) {
    Error_Handler();
  }

  return  _channel_nr; // rank is auto increase sequentially
}

int stm32adc::add_channel(struct adc_channels *ac) {
  ADC_ChannelConfTypeDef sConfig = {0};
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  sConfig.Channel = ac->ch;
  sConfig.Rank = ac->rank = _channel_nr; // 0~ order by
  sConfig.SamplingTime = ac->sampling; //ADC_SAMPLETIME_3CYCLES;
  
  if (HAL_ADC_ConfigChannel(&_ha, &sConfig) != HAL_OK) {
    Error_Handler();
  }

  if( ac->port != 0 ) { // not Vrefint, temperture, ..
    // ---> HUL_GPIO_CLK_Enable(ac->port)
    if( IS_GPIO_ALL_INSTANCE(ac->port)) {
      if( IS_GPIO_ALL_INSTANCE(GPIOA) && (ac->port == GPIOA) ) { __HAL_RCC_GPIOA_CLK_ENABLE(); }
      else if( IS_GPIO_ALL_INSTANCE(GPIOB) && (ac->port == GPIOB )) { __HAL_RCC_GPIOB_CLK_ENABLE(); }
      else if( IS_GPIO_ALL_INSTANCE(GPIOC) && (ac->port == GPIOC )) { __HAL_RCC_GPIOC_CLK_ENABLE(); }
      else if( IS_GPIO_ALL_INSTANCE(GPIOD) && (ac->port == GPIOD )) { __HAL_RCC_GPIOD_CLK_ENABLE(); }
      else if( IS_GPIO_ALL_INSTANCE(GPIOE) && (ac->port == GPIOE )) { __HAL_RCC_GPIOE_CLK_ENABLE(); }
//      else if( IS_GPIO_ALL_INSTANCE(GPIOH) && (ac->port == GPIOH )) { __HAL_RCC_GPIOH_CLK_ENABLE(); }
    } else {
      _status |= 4;
      return -1;
    }

    GPIO_InitStruct.Pin = (1 << (ac->pin)); //GPIO_PIN_n;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ac->port, &GPIO_InitStruct);
  }

  hadcp = &_ha;
  return ++_channel_nr;
}

void stm32adc::start() {
  if( ! isready() ) return;

  if(HAL_ADC_Start(&_ha) != HAL_OK) {
    Error_Handler();
  }
}

void stm32adc::stop() {
  if( ! isready() ) return;

  HAL_ADC_Stop(&_ha);
}

void stm32adc::conv() {
  if( isready() ) {
    HAL_ADC_PollForConversion(&_ha, _timeout);
  }
}

int stm32adc::read() {
  if( ! isready() ) { 
    return (-1);
  }
  int timeout = 1;

  if( _mode == eADC_POLLING ) {
    start();
    
    if(HAL_ADC_PollForConversion(&_ha, timeout) != HAL_OK) {
      Error_Handler();
    }
  }

  uint32_t val = HAL_ADC_GetValue(&_ha);  // get adc value

  if( _mode == eADC_POLLING ) {
    stop();
  }
  
  return (int)(val & (uint32_t)0xfffl);
}

int stm32adc::read(uint16_t *buf) {
  start();

  int i = 0;
  for(; i < _channel_nr; i++ ) {
    if(HAL_ADC_PollForConversion(&_ha, 1) != HAL_OK) {
      Error_Handler();
    }
    
    uint32_t v = HAL_ADC_GetValue(&_ha);  // get adc value
    buf[i] = (uint16_t)(v & 0x0fffU);
  }

  stop();

  return i;
}

/// @brief ////////////////////////////////////////////////////////////////////////////////////////////////////

stm32adcint::stm32adcint() 
: stm32adc() {
  _mode = eADC_INTERRUPT;
  HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0); // M4 ADC_IRQn
  HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
}

stm32adcint::stm32adcint(ADC_TypeDef *adc, struct adc_channels *ac, void (*intrf)()) 
: stm32adc(adc, ac) {
  _mode = eADC_INTERRUPT;
  HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
}

stm32adcint::~stm32adcint() {
  stop();
  
  HAL_NVIC_DisableIRQ(ADC1_2_IRQn);
  
  detach();
}

void stm32adcint::setup(ADC_TypeDef *adc, struct adc_channels *ac, void (*intrf)()) {
  stm32adc::setup(adc, ac);
  attach(intrf);
}

void stm32adcint::attach( void (*inf)()) {
  _stm32adc_callback = inf;
  hadcp = &_ha;
}

void stm32adcint::start() {
  HAL_ADC_Start_IT(get_handle());
}

void stm32adcint::stop() {
  if( ! isready() ) return;

  HAL_ADC_Stop_IT(get_handle());
}

void stm32adcint::detach() {
  _stm32adc_callback = (void(*)())0;
}

int stm32adcint::read() {
  if( ! isready() ) return -1;

  int val = stm32adc::read();
  return val;
}

stm32adcdma::stm32adcdma() 
: stm32adcint() {
  _mode = eADC_DMAINTERRUPT;
}

stm32adcdma::stm32adcdma(ADC_TypeDef *adc, struct adc_channels *ac, void (*intrf)(), uint16_t *dmabuf) 
: stm32adcint(adc, ac, intrf) {
  _mode = eADC_DMAINTERRUPT;
  _dmabuf = dmabuf;
}

 DMA_HandleTypeDef hdma_adc1;
void stm32adcdma::setup(ADC_TypeDef *adc, struct adc_channels *ac, void (*intrf)(), uint16_t *dmabuf) {
  if( !dmabuf ) {
    _dmabuf = dmabuf = new uint16_t[channel_nr()*2];
  }
  hdma_adcp = 0;
#if 1  // F1
    __HAL_RCC_DMA1_CLK_ENABLE();
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    hdma_adc1.Instance = DMA1_Channel1;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
      Error_Handler();
    }

#else  // F4
  //__HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

  hdma_adc1.Instance = DMA2_Stream0;
  hdma_adc1.Init.Channel = DMA_CHANNEL_0;
  hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
  hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  hdma_adc1.Init.Mode = DMA_CIRCULAR;
  hdma_adc1.Init.Priority = DMA_PRIORITY_MEDIUM; //DMA_PRIORITY_LOW;
  hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
  {
    Error_Handler();
  }

  hdma_adcp = &hdma_adc1;

#endif  
  stm32adcint::setup(adc, ac, intrf);
  __HAL_LINKDMA((&_ha),DMA_Handle,hdma_adc1);
}

stm32adcdma::~stm32adcdma() {
  stop();
  if( _dmabuf ) delete[]_dmabuf;
}

void stm32adcdma::start() {
  if( ! isready() ) return;

  stm32adcint::start(); 
  HAL_ADC_Start_DMA(get_handle(), (uint32_t*)_dmabuf, channel_nr());
}

void stm32adcdma::stop() {
  if( ! isready() ) return;

  stm32adcint::stop(); 
  HAL_ADC_Stop_IT(get_handle());
  HAL_ADC_Stop_DMA(get_handle());
}


uint16_t *stm32adcdma::read() {
  return _dmabuf;
}

int stm32adcdma::read( uint16_t *bp) {
  if( adc_completed ) {
    for(int i=0; i < channel_nr(); i++ ) {
      bp[i] = _dmabuf[i];
    }
    adc_completed = 0;
    return channel_nr();
  }
  return 0;
}

#if 0
#define ADC_CHANNEL_0           0x00000000U
#define ADC_CHANNEL_1           ((uint32_t)ADC_CR1_AWDCH_0)
#define ADC_CHANNEL_2           ((uint32_t)ADC_CR1_AWDCH_1)
#define ADC_CHANNEL_3           ((uint32_t)(ADC_CR1_AWDCH_1 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_4           ((uint32_t)ADC_CR1_AWDCH_2)
#define ADC_CHANNEL_5           ((uint32_t)(ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_6           ((uint32_t)(ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_1))
#define ADC_CHANNEL_7           ((uint32_t)(ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_1 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_8           ((uint32_t)ADC_CR1_AWDCH_3)
#define ADC_CHANNEL_9           ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_10          ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_1))
#define ADC_CHANNEL_11          ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_1 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_12          ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_2))
#define ADC_CHANNEL_13          ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_14          ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_1))
#define ADC_CHANNEL_15          ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_1 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_16          ((uint32_t)ADC_CR1_AWDCH_4)
#define ADC_CHANNEL_17          ((uint32_t)(ADC_CR1_AWDCH_4 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_18          ((uint32_t)(ADC_CR1_AWDCH_4 | ADC_CR1_AWDCH_1))

#define ADC_CHANNEL_VREFINT     ((uint32_t)ADC_CHANNEL_17)
#define ADC_CHANNEL_VBAT        ((uint32_t)ADC_CHANNEL_18)
/**
  * @}
  */

/** @defgroup ADC_sampling_times  ADC Sampling Times
  * @{
  */
#define ADC_SAMPLETIME_3CYCLES    0x00000000U
#define ADC_SAMPLETIME_15CYCLES   ((uint32_t)ADC_SMPR1_SMP10_0)
#define ADC_SAMPLETIME_28CYCLES   ((uint32_t)ADC_SMPR1_SMP10_1)
#define ADC_SAMPLETIME_56CYCLES   ((uint32_t)(ADC_SMPR1_SMP10_1 | ADC_SMPR1_SMP10_0))
#define ADC_SAMPLETIME_84CYCLES   ((uint32_t)ADC_SMPR1_SMP10_2)
#define ADC_SAMPLETIME_112CYCLES  ((uint32_t)(ADC_SMPR1_SMP10_2 | ADC_SMPR1_SMP10_0))
#define ADC_SAMPLETIME_144CYCLES  ((uint32_t)(ADC_SMPR1_SMP10_2 | ADC_SMPR1_SMP10_1))
#define ADC_SAMPLETIME_480CYCLES  ((uint32_t)ADC_SMPR1_SMP10)

  __HAL_RCC_DMA2_CLK_ENABLE();
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

  HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(ADC_IRQn);
  HAL_ADC_ConvCpltCallBack(&hadc1);

#if 0
extern DMA_HandleTypeDef hdma_adc1;
extern ADC_HandleTypeDef hadc1;

  __HAL_RCC_DMA2_CLK_ENABLE();
void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(AdcHandle.DMA_Handle);
}

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

  HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(ADC_IRQn);
  
  HAL_ADC_Start_IT(&hadc1); // _ha.Init.ContinuousConvMode = ENABLE; disable,. ... once or every time
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC1Result, ADC1_NUM); // perform only once when _ha.Init.DMAContinuousRequests = ENABLE ;  when disable perform every time
  HAL_ADC_ConvCpltCallBack(&hadc1);


HAL_ADC_Start(&hadc1); // start the adc 

HAL_ADC_PollForConversion(&hadc1, 100); // poll for conversion 
adc_val = HAL_ADC_GetValue(&hadc1); // get the adc value 

HAL_ADC_Stop(&hadc1); // stop adc 

#endif

void ADC_IRQHandler(void)
{
  /* USER CODE BEGIN ADC_IRQn 0 */
 
  /* USER CODE END ADC_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc1);
  /* USER CODE BEGIN ADC_IRQn 1 */
 
  /* USER CODE END ADC_IRQn 1 */
}

void DMA2_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */
 
  /* USER CODE END DMA2_Stream0_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */
 
  /* USER CODE END DMA2_Stream0_IRQn 1 */
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
  if(hadc->Instance == hadc1.Instance)
    BSP_LED_On(LED1);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  adcVal[adcIndex] = HAL_ADC_GetValue(&hadc1);

  if(adcIndex == 1)
  {
    vol1 = 3000 * adcVal[0] / 4096;
    vol2 = 3000 * adcVal[1] / 4096;

    /* LM35DZ 온도 = 전압(mV) / 10 */
    temp1 = vol1 / 10.0;

    /* 내장온도센서 = (Vsens - V25) / 2.5 + 25도 */
    temp2 = ((float)vol2 - 760.0) / 2.5 + 25.0;
  }

  adcIndex++;
  if(adcIndex > 1) adcIndex = 0;
}

DMA_HandleTypeDef hdma_adc1;
    hdma_adc1.Instance = DMA2_Stream0;
    hdma_adc1.Init.Channel = DMA_CHANNEL_0;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(_ha,DMA_Handle,hdma_adc1);


int main() {
  stm32adc adc(ADC1,0);
  while(1) {
    int val = adc.read();
  }
}
#endif
