#include "gspi.h"
#include "ggpio.h"
#include "ghul.h"
#include "glog.h"
#include <string.h>

#define MAX_SPI_NUM 3

SPI_HandleTypeDef __hspi[MAX_SPI_NUM];
gspi __gspi[MAX_SPI_NUM];

int spi_speed_idx[] = {
    SPI_BAUDRATEPRESCALER_2,SPI_BAUDRATEPRESCALER_4,SPI_BAUDRATEPRESCALER_8,SPI_BAUDRATEPRESCALER_16,
    SPI_BAUDRATEPRESCALER_32,SPI_BAUDRATEPRESCALER_64,SPI_BAUDRATEPRESCALER_128,SPI_BAUDRATEPRESCALER_256
};

void gspi_rxcallback(SPI_HandleTypeDef *hspi) {
  gspi *s;
  //s = &__gspi[(hspi->Instance - SPI1) / (SPI2-SPI1)];
  int idx = HUL_SPI_idx(hspi->Instance);
  if( idx == -1 ) {
    return;
  }
  s = &__gspi[idx];
  
  // save data

  // 
  if(s->_dmaint == eSPI_Interrupt) {
    HAL_SPI_Receive_IT(s->_hs, s->_rxbuf, s->_bufsize);    
  } else
  if(s->_dmaint == eSPI_Dma) {
    HAL_SPI_Receive_DMA(s->_hs, s->_rxbuf, s->_bufsize);
  }
}

int spi_tx_cnt=0, spi_rx_cnt=0;
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  spi_tx_cnt++;

#if defined(SPI1) 
  if( hspi->Instance == SPI1 ) {
  }
#endif
#if defined(SPI2)  
  else if( hspi->Instance == SPI2 ) {
  }
#endif
#if defined(SPI3)  
  else if( hspi->Instance == SPI3 ) {
  }
#endif
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
  spi_rx_cnt++;

  gspi_rxcallback(hspi);


#if defined(SPI1) 
  if( hspi->Instance == SPI1 ) {

  }
#endif
#if defined(SPI2)  
  else if( hspi->Instance == SPI2 ) {
  }
#endif
#if defined(SPI3)  
  else if( hspi->Instance == SPI3 ) {
  }
#endif  
}

#if 0
void HAL_SPI_IRQHandler(SPI_HandleTypeDef *hspi)
{
  if (hspi->Instance == SPI2)
  {
    if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE)) {

    }
  }
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

void SPI1_IRQHandler(void)
{
}

#if defined(SPI2)  
void SPI2_IRQHandler(void)
{
}
#endif

#if defined(SPI3)  
void SPI3_IRQHandler(void)
{
}
#endif

#ifdef __cplusplus
}
#endif

#ifdef SPI_TEST
uint8_t rx1_data[64];

void spi1_master_init() {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

#if 0
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PB3     ------> SPI1_SCK
    PB4     ------> SPI1_MISO
    PB5     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif

    gpio_t gs1sck={GPIOB,3}, gs1miso={GPIOB,4}, gs1mosi={GPIOB,5};

    ggpio s1sck( &gs1sck, eGPIO_AFPP, 0, 3),
      s1mosi( &gs1mosi, eGPIO_AFPP, 0, 3),
      s1miso( &gs1miso, eGPIO_INPUT, 0, 3);

    __HAL_AFIO_REMAP_SPI1_ENABLE();

    /* SPI1 DMA Init */
    /* SPI1_RX Init */
    hdma_spi1_rx.Instance = DMA1_Channel2;
    hdma_spi1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_spi1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi1_rx.Init.Mode = DMA_CIRCULAR;
    hdma_spi1_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_spi1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hspi,hdmarx,hdma_spi1_rx);

    /* SPI1_TX Init */
    hdma_spi1_tx.Instance = DMA1_Channel3;
    hdma_spi1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_spi1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi1_tx.Init.Mode = DMA_NORMAL;
    hdma_spi1_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_spi1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hspi,hdmatx,hdma_spi1_tx);
    /* SPI1 interrupt Init */
    HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SPI1_IRQn);

  __hspi1.Instance = SPI1;
  __hspi1.Init.Mode = SPI_MODE_MASTER; // SPI_MODE_SLAVE
  __hspi1.Init.Direction = SPI_DIRECTION_2LINES; //
  __hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  __hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  __hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  __hspi1.Init.NSS = SPI_NSS_SOFT;
  __hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; 
  __hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  __hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  __hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  __hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&__hspi1) != HAL_OK)
  {
    Error_Handler();
  }    
}

uint8_t rx2_data[64];

void spi2_slave_init() {
    __HAL_RCC_SPI2_CLK_ENABLE();
HAL_SPI_Receive_IT(&__hspi2, rx2_data, sizeof(rx2_data));
    //    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI2 GPIO Configuration
    PB13     ------> SPI2_SCK
    PB14     ------> SPI2_MISO
    PB15     ------> SPI2_MOSI
    */
    gpio_t gs2sck={GPIOB,13}, gs2miso={GPIOB,14}, gs2mosi={GPIOB,15};

    ggpio 
    s2sck( &gs2sck, eGPIO_INPUT, 0, 3),
    s2mosi( &gs2mosi, eGPIO_INPUT, 0, 3),
    s2miso( &gs2miso, eGPIO_AFPP, 0, 3);

#if 0
  GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif

    /* SPI2 interrupt Init */
    HAL_NVIC_SetPriority(SPI2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SPI2_IRQn);

  __hspi2.Instance = SPI2;
  __hspi2.Init.Mode = SPI_MODE_SLAVE;
  __hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  __hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  __hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  __hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  __hspi2.Init.NSS = SPI_NSS_SOFT;
  __hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  __hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  __hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  __hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&__hspi2) != HAL_OK)
  {
    Error_Handler();
  }    
}
#endif // SPI_TEST

int gspi::init(SPI_TypeDef *spi, uint16_t mode, gpio_t gsck, gpio_t gmiso, gpio_t gmosi,uint32_t bufsize, int speed)
{
  if( _rxbuf ) {
    delete[] _rxbuf;
  }
  if( bufsize == 0 || bufsize > MAX_SPIRXBUFSIZE) {
    error_log("spi rxbuf size wrong");
  }
   
  int idx;
  if( (idx=HUL_SPI_clk_enable(spi,_remap)) == -1 ) {
    error_log("spi is wrong");
    return -1;
  }

  if( speed < 0 || speed > 7 ) speed = 2;

  _sck.init(&gsck,eGPIO_AFPP, 0, 3);
  _mosi.init(&gmosi,eGPIO_AFPP, 0, 3);
  _miso.init(&gmiso,eGPIO_INPUT, 0, 3);
  
  _mode = mode;

  if( _dmaint != eSPI_Poll ) {
    if( !(_rxbuf = new uint8_t[bufsize])) {
        return -1;
    }
    if(HUL_SPI_nvic(spi,1) == -1) {
      return -1;
    };
  }

  _hs = &__hspi[idx];

  _hs->Instance = spi;
  _hs->Init.Mode = (_ms == eSPI_Master)? SPI_MODE_MASTER : SPI_MODE_SLAVE;
  _hs->Init.Direction = (_lines==eSPI_Duplex)? SPI_DIRECTION_2LINES : 
      (_lines==eSPI_1line)? SPI_DIRECTION_1LINE : SPI_DIRECTION_2LINES_RXONLY;
  _hs->Init.DataSize = SPI_DATASIZE_8BIT;
  _hs->Init.CLKPolarity = SPI_POLARITY_LOW;
  _hs->Init.CLKPhase = SPI_PHASE_1EDGE;
  _hs->Init.NSS = SPI_NSS_SOFT;
  _hs->Init.BaudRatePrescaler = ((speed) << 3U); //SPI_BAUDRATEPRESCALER_8 2,4,8,16,32,64,128,256
  _hs->Init.FirstBit = SPI_FIRSTBIT_MSB;
  _hs->Init.TIMode = SPI_TIMODE_DISABLE;
  _hs->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  _hs->Init.CRCPolynomial = 10;

  if (HAL_SPI_Init(_hs) != HAL_OK) {
    error_log("spi init fail");
    return -1;
  }    

  _inited = 1;

  return 0;
}

gspi::gspi(SPI_TypeDef *spi, uint16_t mode, gpio_t gsck, gpio_t gmiso, gpio_t gmosi, uint32_t bufsize, int speed)
{
  _datalen = 0;
  _rxbuf = 0;
  _mode = 0;
  _hs = 0;

  init(spi,mode,gsck,gmiso,gmosi,bufsize,speed);
}

gspi::gspi() {
  _datalen = 0;
  _rxbuf = 0;
  _mode = 8; // Intr, master, duplex mode
  _hs = 0;
}

gspi::~gspi() {
  if(_rxbuf) {
    delete[] _rxbuf;
    _rxbuf = 0;
    _datalen = 0;
  }
}

int gspi::start() {
  if( !_hs ) return -1;

  if(_dmaint == 1) {
    HAL_SPI_Receive_IT(_hs, _rxbuf, _bufsize);    
  } else
  if(_dmaint == 2) {
    HAL_SPI_Receive_DMA(_hs, _rxbuf, _bufsize);
  }

  return 0;
}

int gspi::read() {
  uint8_t ch;
  if (HAL_SPI_Receive(_hs, &ch, 1, HAL_MAX_DELAY) == HAL_OK) {
    return ch;
  }
  return -1;
}

int gspi::read(uint8_t *buf, uint32_t bsize) {
  if( !_inited || _datalen == 0 ) return -1;

  uint32_t ln=0;
  if( _datalen > bsize ) {
    memcpy( _rxbuf, buf, bsize );
    _datalen -= bsize;
    ln = bsize;
    memcpy( _rxbuf+bsize, _rxbuf, _datalen);
  } else {
    memcpy( _rxbuf, buf, _datalen );
    ln = _datalen;
    _datalen = 0;
  }

  return ln;
}

int gspi::write(uint8_t *data, uint32_t len) {
  if( !_inited || len == 0 ) return -1;
  
  switch(_dmaint) {
    case eSPI_Poll:
      if(HAL_SPI_Transmit(_hs, data, len, 100) != HAL_OK) {
        return -1;
      };
      break;
    case eSPI_Interrupt:
      if(HAL_SPI_Transmit_IT(_hs,data,len) != HAL_OK) {
        return -1;
      }
      break;
    case eSPI_Dma:
      if(HAL_SPI_Transmit_DMA(_hs,data,len) != HAL_OK) {
        return -1;
      }
      break;
    default:
      return -1;
  }

  return len;
}
