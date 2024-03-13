#include "gadc.h"
#include <glog.h>

#ifdef __cplusplus
extern "C" {
#endif

DMA_HandleTypeDef hdma_adc1,hdma_adc2;
ADC_HandleTypeDef hadc1,hadc2;

extern uint16_t *adc_buffer;
void (*_stm32adc_callback)() = 0;

int adc_completed = 0;
int dma_finish1 = 0;
int dma_finish2 = 0;

int adc_mode=1;
int adc_count=0;

gadc __adc1;

int HUL_ADC_clk_enable(ADC_TypeDef *adc);
int HUL_GPIO_clk_enable(GPIO_TypeDef *gpio);
int HUL_ADC_nvic(ADC_TypeDef *adc, int enable);

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
  if( _stm32adc_callback ) {
    _stm32adc_callback();
  }
  if(__adc1.status() == -1) {
    //
  }
  adc_completed++;
  if( adc_mode == 1) {

  } else {

  }
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
  //dma_finish1++;
}

void HAL_DMA_TransferCpltCallback(DMA_HandleTypeDef *hdma) {
  //dma_finish2++;
}

#if 1 // F1
void ADC1_2_IRQHandler(void) {
  HAL_ADC_IRQHandler(&hadc1); 
}

void DMA1_Channel1_IRQHandler(void) {
  HAL_DMA_IRQHandler(&hdma_adc1);
  dma_finish2++;
}
#else
void ADC_IRQHandler(void) {
  HAL_ADC_IRQHandler(&hadc1);
}
void DMA2_Stream0_IRQHandler(void) {
    HAL_DMA_IRQHandler(&hdma_adc1);
}
#endif

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
}
#endif
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

/**
* @brief ADC MSP Initialization
* This function configures the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PA0-WKUP     ------> ADC1_IN0
    PA1     ------> ADC1_IN1
    PA2     ------> ADC1_IN2
    PA3     ------> ADC1_IN3
    PA4     ------> ADC1_IN4
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* ADC1 DMA Init */
    /* ADC1 Init */
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

    __HAL_LINKDMA(hadc,DMA_Handle,hdma_adc1);

    /* ADC1 interrupt Init */
    HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }

}


/**
* @brief ADC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PA0-WKUP     ------> ADC1_IN0
    PA1     ------> ADC1_IN1
    PA2     ------> ADC1_IN2
    PA3     ------> ADC1_IN3
    PA4     ------> ADC1_IN4
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4);

    /* ADC1 DMA DeInit */
    HAL_DMA_DeInit(hadc->DMA_Handle);

    /* ADC1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(ADC1_2_IRQn);
  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }

}

// multichannel, use dma, scan mode, not continuous, one adc

// ---------------------------------------------------------------------------------
gadc::gadc() { //LSE_STARTUP_TIMEOUT
  _status = eADC_NOTSETUP;
  _channel_num = 0;
  _timeout = 1; // ms
  _mode = 1; // continuous
}

//#include <stm32f4xx_hal.h>

gadc::gadc(ADC_TypeDef *adc, struct adc_channels *ac) {
  _status = eADC_NOTSETUP;
  _chs = ac;
  _ha = (adc == ADC1)? &hadc1 : (adc == ADC2)? &hadc2: 0;
  if( !_ha ) return;

  _ha->Instance = adc;
  _channel_num = 0;
  _timeout = 1; // ms

  setup();
}

gadc::~gadc() {
  stop();
  // nvic off
  HAL_DMA_DeInit(_ha->DMA_Handle);
  HAL_NVIC_DisableIRQ(ADC1_2_IRQn);
  __HAL_RCC_ADC1_CLK_DISABLE();

  if( _dmabuf ) {
    delete[] _dmabuf;
    delete[] _outbuf;
  }
//  detach();
}

void gadc::setup() {
  if( _ha && _chs ) this->setup( _ha->Instance, _chs);
}

void gadc::setup(ADC_TypeDef *adc, struct adc_channels *ac) {
  _status = eADC_NOTSETUP;
  _chs = ac;
  _ha = (adc == ADC1)? &hadc1 : (adc == ADC2)? &hadc2: 0;
  if( !_ha ) return;

  _ha->Instance = adc;
  _channel_num = 0;
  _timeout = 1; // ms
 
  int channel_cnt = 0;
  for(; (ac+channel_cnt)->ch != -1 ; channel_cnt++);
  
  _ha->Init.ScanConvMode = ENABLE;
  _ha->Init.ExternalTrigConv = ADC_SOFTWARE_START;
  _ha->Init.ContinuousConvMode = DISABLE; //ENABLE;
  _ha->Init.DiscontinuousConvMode = DISABLE;
  _ha->Init.DataAlign = ADC_DATAALIGN_RIGHT;
  _ha->Init.NbrOfConversion = channel_cnt;                   // <> _channel_num

  HUL_ADC_clk_enable(_ha->Instance);

// multi, continues, use dma
#if defined(STM32F4) || defined(STM32F7)
  _ha->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  _ha->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  _ha->Init.Resolution = ADC_RESOLUTION_12B;
  _ha->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  if( _mode == eADC_DMA || _mode == eADC_DMAINTERRUPT ) {
    _ha->Init.DMAContinuousRequests = ENABLE;
  } else {
    _ha->Init.DMAContinuousRequests = DISABLE;
  }
#endif //STM32F4

  if (HAL_ADC_Init(_ha) != HAL_OK) {// --> call HAL_ADC_MspInit()
      ERROR_LOG("adc init fail"); //Error_Handler();
  }


  for(int i=0; i < _ha->Init.NbrOfConversion; i++) {
    add_channel(_chs+i);
  }

  if( !_dmabuf ) {
    _dmabuf = new uint16_t[channel_num()*2];
    _outbuf = new uint16_t[channel_num()*2];
  }
#if 1  // F1
    __HAL_RCC_DMA1_CLK_ENABLE();

    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

// --- ~MspInit()
    _hd->Instance = DMA1_Channel1;
    _hd->Init.Direction = DMA_PERIPH_TO_MEMORY;
    _hd->Init.PeriphInc = DMA_PINC_DISABLE;
    _hd->Init.MemInc = DMA_MINC_ENABLE;
    _hd->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    _hd->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    _hd->Init.Mode = DMA_CIRCULAR;
    _hd->Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK) {
      ERROR_LOG("dma init fail"); //Error_Handler();
    }

#else  // F4
  __HAL_RCC_DMA2_CLK_ENABLE();
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

  _hd->Instance = DMA2_Stream0;
  _hd->Init.Channel = DMA_CHANNEL_0;
  _hd->Init.Direction = DMA_PERIPH_TO_MEMORY;
  _hd->Init.PeriphInc = DMA_PINC_DISABLE;
  _hd->Init.MemInc = DMA_MINC_ENABLE;
  _hd->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  _hd->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  _hd->Init.Mode = DMA_CIRCULAR;
  _hd->Init.Priority = DMA_PRIORITY_MEDIUM; //DMA_PRIORITY_LOW;
  _hd->Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
  {
    ERROR_LOG(""); //Error_Handler();
  }

  hdma_adcp = &hdma_adc1;

#endif  

  __HAL_LINKDMA((_ha),DMA_Handle,hdma_adc1);

  HUL_ADC_nvic(_ha->Instance, 1);


  if(HAL_ADC_Start_DMA(_ha, (uint32_t*)_dmabuf, channel_num()) != HAL_OK) {
    ERROR_LOG("adc start dma nok"); //Error_Handler();
  }

  _status = 0;
}

int gadc::add_channel(uint32_t ch,uint32_t samplerate) {
  ADC_ChannelConfTypeDef sConfig = {0};
  sConfig.Channel = ch;
  sConfig.SamplingTime = samplerate; // default ADC_SAMPLETIME_84CYCLES; 
  sConfig.Rank = ++_channel_num; // 1 ~ ,  

  if (HAL_ADC_ConfigChannel(_ha, &sConfig) != HAL_OK) {
    ERROR_LOG("add adc channel"); //Error_Handler();
  }

  return  _channel_num; // rank is auto increase sequentially
}

int gadc::add_channel(struct adc_channels *ac) {
  ADC_ChannelConfTypeDef sConfig = {0};
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  sConfig.Channel = ac->ch;
  sConfig.Rank = ac->rank = _channel_num; // 0~ order by
  sConfig.SamplingTime = ac->sampling; //ADC_SAMPLETIME_3CYCLES;
  
  if (HAL_ADC_ConfigChannel(_ha, &sConfig) != HAL_OK) {
    ERROR_LOG("add adc channel"); //Error_Handler();
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
    GPIO_InitStruct.Pull = GPIO_NOPULL; //GPIO_PULLDOWN; //GPIO_NOPULL;
    HAL_GPIO_Init(ac->port, &GPIO_InitStruct);
  }

  return ++_channel_num;
}

int gadc::start() {
  if(HAL_ADC_Start(_ha) != HAL_OK) {
    return 0;
  }
  return -1;
}

int gadc::stop() {
  if(HAL_ADC_Stop(_ha) != HAL_OK) {
    return 0;
  }
  return -1;
}

int gadc::read() {
  if( ! isready() ) { 
    return (-1);
  }
#if 0
  _converted = 0;
  HAL_ADC_Start(_ha);

  while( _converted == 0 ) {
    delay_us(_timeout);
  };
#endif
}

int gadc::read(uint16_t *buf) {

  int i = 0;
  for(; i < _channel_num; i++ ) {
    if(HAL_ADC_PollForConversion(_ha, 1) != HAL_OK) {
    ERROR_LOG(""); //Error_Handler();
    }
    
    uint32_t v = HAL_ADC_GetValue(_ha);  // get adc value
    buf[i] = (uint16_t)(v & 0x0fffU);
  }

  stop();

  return i;
}
// -----------------------------------------------------------------------------------------------

#if 0
void HAL_DMA_TransferCpltCallback(DMA_HandleTypeDef *hdma) {
  // 버퍼 처리 (예: 데이터 분석, 전송 등)
  dma_finish1++;
}

void HAL_DMA_StreamTransferCpltCallback(DMA_HandleTypeDef *hdma) {
  // DMA 재시작 (연속 읽어오기)

  HAL_DMA_Start_IT(hdma, (uint32_t)&ADC1->DR, (uint32_t)adc_buffer, 7);
}
#endif

#if 0
int adc_completed = 0;
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
  adc_completed++;

  if( _stm32adc_callback  ) {
    _stm32adc_callback();
  }


  adc_data[0] = HAL_ADC_GetValue(hadc, ADC_CHANNEL_1);
  adc_data[1] = HAL_ADC_GetValue(hadc, ADC_CHANNEL_2);
  ...


  HAL_ADC_Start_IT(hadc);
}
#endif

#ifdef __cplusplus
}
#endif

stm32adc::stm32adc() { //LSE_STARTUP_TIMEOUT
  _channel_num = 0;
  _timeout = 100; // ms
  _status = eADC_NOTSETUP; // -1
  _mode = eADC_POLLING; // 0       (_mode & 0x7)
}

//#include <stm32f4xx_hal.h>

stm32adc::stm32adc(ADC_TypeDef *adc, struct adc_channels *ac) {
  if(adc == ADC1) {
    _ha = &hadc1;
  } else {
  //if(adc == ADC2) {
    return;
    //_ha = &hadc2;
  }
  _channel_num = 0;
  _timeout = 100; // ms
  _status = eADC_NOTSETUP; // -1
  _mode = eADC_POLLING; // 0       (_mode & 0x7)

//  setup(adc, ac);
}

void stm32adc::setup(ADC_TypeDef *adc, struct adc_channels *ac) {
  _status = -1;
  if(adc == ADC1) {
    _ha = &hadc1;
  } else {
  //if(adc == ADC2) {
    return;
    //_ha = &hadc2;
  }
  HUL_ADC_clk_enable(_ha->Instance);

  _ha->Instance = adc;
  _channel_num = 0;
  _timeout = 1; // ms
  _status = 0;
 
  int chs = 0;
  for(; (ac+chs)->ch != -1 ; chs++);
  
  _ha->Init.ExternalTrigConv = ADC_SOFTWARE_START;
  _ha->Init.DataAlign = ADC_DATAALIGN_RIGHT;
  _ha->Init.NbrOfConversion = chs;                   // <> _channel_num
  _ha->Init.DiscontinuousConvMode = DISABLE;
  _ha->Init.ScanConvMode = ENABLE; //DISABLE; // ENABLE
  _ha->Init.ContinuousConvMode = DISABLE;

  HUL_ADC_clk_enable(adc);

// multi, continues, use dma
#if defined(STM32F4) || defined(STM32F7)
  _ha->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  _ha->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  _ha->Init.Resolution = ADC_RESOLUTION_12B;
  _ha->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  if( _mode == eADC_DMA || _mode == eADC_DMAINTERRUPT ) {
    _ha->Init.DMAContinuousRequests = ENABLE;
  } else {
    _ha->Init.DMAContinuousRequests = DISABLE;
  }
#endif //STM32F4

  if (HAL_ADC_Init(_ha) != HAL_OK) // --> call HAL_ADC_MspInit()
  {
    ERROR_LOG(""); //Error_Handler();
  }

  _chs = ac;

  for(int i=0; i < _ha->Init.NbrOfConversion; i++) {
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
  sConfig.Rank = ++_channel_num; // 1 ~ ,  

  if (HAL_ADC_ConfigChannel(_ha, &sConfig) != HAL_OK) {
    ERROR_LOG("add adc channel"); //Error_Handler();
  }

  return  _channel_num; // rank is auto increase sequentially
}

int stm32adc::add_channel(struct adc_channels *ac) {
  ADC_ChannelConfTypeDef sConfig = {0};
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  sConfig.Channel = ac->ch;
  sConfig.Rank = ac->rank = _channel_num; // 0~ order by
  sConfig.SamplingTime = ac->sampling; //ADC_SAMPLETIME_3CYCLES;
  
  if (HAL_ADC_ConfigChannel(_ha, &sConfig) != HAL_OK) {
    ERROR_LOG("add adc channel"); //Error_Handler();
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
    GPIO_InitStruct.Pull = GPIO_NOPULL; //GPIO_PULLDOWN; //GPIO_NOPULL;
    HAL_GPIO_Init(ac->port, &GPIO_InitStruct);
  }

  return ++_channel_num;
}

void stm32adc::start() {
  if( ! isready() ) return;

  if(HAL_ADC_Start(_ha) != HAL_OK) {
    ERROR_LOG(""); //Error_Handler();
  }
}

void stm32adc::stop() {
  if( ! isready() ) return;

  HAL_ADC_Stop(_ha);
}

void stm32adc::conv() {
  if( isready() ) {
    if(HAL_ADC_PollForConversion(_ha, this->_timeout) != HAL_OK) {
      ERROR_LOG(""); //Error_Handler();
    }
  }
}

int stm32adc::read() {
  if( ! isready() ) { 
    return (-1);
  }
  if( _mode == eADC_POLLING ) {
    start();
    
    if(HAL_ADC_PollForConversion(_ha, this->_timeout) != HAL_OK) {
    ERROR_LOG(""); //Error_Handler();
    }
  }

  uint32_t val = HAL_ADC_GetValue(_ha);  // get adc value

  if( _mode == eADC_POLLING ) {
    stop();
  }
  
  return (int)(val & (uint32_t)0xfffl);
}

int stm32adc::read(uint16_t *buf) {
  start();

  int i = 0;
  for(; i < _channel_num; i++ ) {
    if(HAL_ADC_PollForConversion(_ha, 1) != HAL_OK) {
    ERROR_LOG(""); //Error_Handler();
    }
    
    uint32_t v = HAL_ADC_GetValue(_ha);  // get adc value
    buf[i] = (uint16_t)(v & 0x0fffU);
  }

  stop();

  return i;
}

/// @brief ////////////////////////////////////////////////////////////////////////////////////////////////////

stm32adcint::stm32adcint() 
: stm32adc() {
  _mode = eADC_INTERRUPT;
  HUL_ADC_nvic(get_handle()->Instance, 1);
}

stm32adcint::stm32adcint(ADC_TypeDef *adc, struct adc_channels *ac, void (*intrf)()) 
: stm32adc(adc, ac) {
  _mode = eADC_INTERRUPT;
  HUL_ADC_nvic(get_handle()->Instance, 1);
}

stm32adcint::~stm32adcint() {
  stop();
  
  HUL_ADC_nvic(get_handle()->Instance, 0);
  
  detach();
}

void stm32adcint::setup(ADC_TypeDef *adc, struct adc_channels *ac, void (*intrf)()) {
  stm32adc::setup(adc, ac);
  attach(intrf);
}

void stm32adcint::start() {
  if( HAL_ADC_Start_IT(this->_ha) != HAL_OK ) {
    ERROR_LOG(""); //Error_Handler();
  }
}

void stm32adcint::stop() {
  if( ! isready() ) return;

  HAL_ADC_Stop_IT(get_handle());
}

void stm32adcint::attach( void (*inf)()) {
  _stm32adc_callback = inf;
}

void stm32adcint::detach() {
  _stm32adc_callback = (void(*)())0;
}

int stm32adcint::read() {
  if( ! isready() ) return -1;

  int val = stm32adc::read();
  return val;
}

// -------------------------------------------------------------------------------------------------

stm32adcdma::stm32adcdma() 
: stm32adc() {
  _dmabuf = 0;
  _mode = eADC_DMAINTERRUPT;
}

stm32adcdma::stm32adcdma(ADC_TypeDef *adc, struct adc_channels *ac, void (*intrf)(), uint16_t *dmabuf) 
: stm32adc(adc, ac) {
  _mode = eADC_DMAINTERRUPT;
  _dmabuf = dmabuf;
}

void stm32adcdma::setup(ADC_TypeDef *adc, struct adc_channels *ac, void (*intrf)(), uint16_t *dmabuf) {
  if( !dmabuf ) {
    _dmabuf = new uint16_t[channel_num()*2];
    _outbuf = new uint16_t[channel_num()*2];
  }

  HUL_ADC_nvic(get_handle()->Instance, 1);
  HUL_ADC_clk_enable(adc);
  HUL_DMA_nvic(adc,1);

#if 1  // F1

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
    ERROR_LOG(""); //Error_Handler();
  }

#else  // F4

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
    ERROR_LOG(""); //Error_Handler();
  }

  hdma_adcp = &hdma_adc1;

#endif  

  __HAL_LINKDMA((_ha),DMA_Handle,hdma_adc1);

}

stm32adcdma::~stm32adcdma() {
  stop();
  if( _dmabuf ) {
    delete[]_dmabuf;
    _dmabuf = 0;
  }
}

void stm32adcdma::start() {
  if( ! isready() ) {
    ERROR_LOG("adc start not ready"); 
    return;
  }

#if 0
  stm32adcint::start(); 
  if( HAL_DMA_Start_IT(&hdma_adc1, (uint32_t)&ADC1->DR, (uint32_t)_dmabuf[0],  channel_num()) != HAL_OK ) {
      ERROR_LOG("adc dma start it nok"); 
  }
  if( HAL_ADC_Start(get_handle()) != HAL_OK ) {
    ERROR_LOG("adc start nok"); 
  }
 //  HAL_DMA_Start(&hdma_adc1, (uint32_t)&hadc1.Instance->DR, (uint32_t)_dmabuf, channel_num());
  if( HAL_ADC_Start(get_handle()) != HAL_OK ) {
    ERROR_LOG("adc start nok"); 
  }
  if( HAL_DMA_Start_IT(&hdma_adc1, (uint32_t)&ADC1->DR, (uint32_t)_dmabuf[0],  channel_num()) != HAL_OK ) {
      ERROR_LOG("adc dma start it nok"); 
  }
#else
 
  if( HAL_ADC_Start_DMA(get_handle(), (uint32_t*)_dmabuf, channel_num()) != HAL_OK ) {
    ERROR_LOG("adc start dma nok"); 
  };
#endif  
}

void stm32adcdma::stop() {
  if( ! isready() ) return;

  stm32adc::stop(); 
  HAL_ADC_Stop_DMA(get_handle());
}


void stm32adcdma::attach( void (*inf)()) {
  _stm32adc_callback = inf;
}

void stm32adcdma::detach() {
  _stm32adc_callback = (void(*)())0;
}

uint16_t *stm32adcdma::read() {

  return _dmabuf;
}

int stm32adcdma::read( uint16_t *bp) {
  if(adc_completed) {
    memmove(bp,_dmabuf,channel_num());
    adc_completed = 0;
    return channel_num();
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
  
  HAL_ADC_Start_IT(&hadc1); // _ha->Init.ContinuousConvMode = ENABLE; disable,. ... once or every time
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC1Result, ADC1_NUM); // perform only once when _ha->Init.DMAContinuousRequests = ENABLE ;  when disable perform every time
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
    ERROR_LOG(__FILE__, __LINE__); //Error_Handler();
    }

    __HAL_LINKDMA(_ha,DMA_Handle,hdma_adc1);


int main() {
  stm32adc adc(ADC1,0);
  while(1) {
    int val = adc.read();
  }
}
#endif

