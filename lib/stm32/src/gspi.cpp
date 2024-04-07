#include "gspi.h"
#include "ggpio.h"

SPI_HandleTypeDef __hspi1, __hspi2, __hspi3;

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  // 전송 완료 처리
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
  // 수신 완료 처리
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

void SPI2_IRQHandler(void)
{

}

#ifdef __cplusplus
}
#endif

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

    /* SPI1 interrupt Init */
    HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SPI1_IRQn);

  __hspi1.Instance = SPI1;
  __hspi1.Init.Mode = SPI_MODE_MASTER;
  __hspi1.Init.Direction = SPI_DIRECTION_2LINES;
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

void prepare_spi() {
    HAL_SPI_Receive_IT(&__hspi2, rx2_data, sizeof(rx2_data));    
}