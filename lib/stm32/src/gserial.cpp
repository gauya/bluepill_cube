/*
 * gserial.cpp
 *
 *  Created on: May 12, 2022
 *      Author: seu
 */


#include "stm32f1xx_it.h"
#include "gserial.h"
#include <stdarg.h>
#include "ggpio.h"
#include "glog.h"

serial_info_t __gsi[MAX_SERIAL_NUM] = {0,};

serial_info_t* get_console() {
	return (serial_info_t*)(&__gsi[0]); // first open, it will be console
}

static int _default_uart_id = -1;
static int _default_cdc_id = -1;

UART_HandleTypeDef *phuart_console=0;

UART_HandleTypeDef huart1,huart2,huart3;


void USART1_IRQHandler(void) {
  HAL_UART_IRQHandler(&huart1);
}


void USART2_IRQHandler(void) {
  HAL_UART_IRQHandler(&huart2);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	serial_info_t *s;
	if( (s = _get_serial(huart)) == 0 ) {
		ERROR_LOG("get serial nok");
		return ; // internal error
	}

	fifo_putc(s->_rx_buf,s->_rx_ch);
	if( s->_echo ) {
		HAL_UART_Transmit(huart,(uint8_t*)&(s->_rx_ch),1,100); // echo
	}
	HAL_UART_Receive_IT(huart,&(s->_rx_ch),1); // next recv interrupt enable
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if(huart->Instance == phuart_console->Instance) {
  	} else {
  	}
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(huart->Instance==USART1) {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */

   #if 1 // bluepill, blackpill
   	gpio_t tx={ GPIOA,9}, rx={GPIOA,10 };
   #else
   #endif

    GPIO_InitStruct.Pin = (1 << tx.pin); // GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(tx.port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = (1 << rx.pin); //GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(rx.port, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

static int stm32_uart_init(USART_TypeDef *uart, uint32_t speed=115200, gpio_t *tx=0, gpio_t *rx=0) {
  UART_HandleTypeDef *hu;

  hu = (uart == USART1)? &huart1 : (uart == USART2)? &huart2 : 0;

  if( hu == 0 ) return -1;

  hu->Instance = uart;
  hu->Init.BaudRate = speed;
  hu->Init.WordLength = UART_WORDLENGTH_8B;
  hu->Init.StopBits = UART_STOPBITS_1;
  hu->Init.Parity = UART_PARITY_NONE;
  hu->Init.Mode = UART_MODE_TX_RX;
  hu->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hu->Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(hu) != HAL_OK)
  {
    Error_Handler();
  }
  return 0;
}

int init_uart(int baud, USART_TypeDef *uart, int id) {
	stm32_uart_init(uart,baud);
	if( id >= MAX_SERIAL_NUM || id < -1 )
		return -2;

	if( id == -1 ) {
		for( int i=0; i<MAX_SERIAL_NUM; i++ ) {
			if( __gsi[i]._inited == 0 ) {
				id = i;
				break;
			}
		}
		if( id >= MAX_SERIAL_NUM ) {
			// overflow serial
			return -1;
		}
	} else if(__gsi[id].Handle) {
		return -3; // already installed
	}

	serial_info_t *s = &__gsi[id];
	s->_type = 0;
	s->Handle = (uart == USART1)? &huart1 : (uart == USART2)? &huart2 : 0;
	if( id==0 )
		phuart_console = s->Handle;

	s->_rx_buf = new (gfifo_t); //[sizeof(gfifo_t)];
	fifo_set(s->_rx_buf,0,0,0);

	s->_inited = 1;
	s->_fd = id;
	
	if( _default_uart_id == -1 ) 
		_default_uart_id = id;


	HAL_UART_Receive_IT(s->Handle,(uint8_t*)&(s->_rx_ch),1);

	return id;
}

int init_cdc(int id) {
	if( id >= MAX_SERIAL_NUM || id < -1 )
		return -2;

	if( id == -1 ) {
		for( int i=0; i<MAX_SERIAL_NUM; i++ ) {
			if( __gsi[i]._inited == 0 ) {
				id = i;
				break;
			}
		}
		if( id >= MAX_SERIAL_NUM ) {
			// overflow serial
			return -1;
		}
	}

	serial_info_t *s = &__gsi[id];
	s->_type = 1;
	s->Handle = 0;
	s->_inited = 1;
	s->_fd = id;
	if( id==0 )
		phuart_console = 0;

	if( _default_cdc_id == -1 ) 
		_default_cdc_id = id;

/*
	s->_rx_fifo = (gfifo_t*)new[sizeof(gfifo_t)];
	fifo_set(s->_rx_fifo,0,0,0);
*/

/*
	s->getc = cdc_getc;
	s->putc = cdc_putc;
	s->write = cdc_write;
*/
	return id;
}

int init_serial(int baud,USART_TypeDef *uart) {
	serial_info_t *s = &__gsi[0];

	if( get_console()->_inited == 0) {
		for(int i=0;i<MAX_SERIAL_NUM; i++, s++) { // set ECHO on, INTERRUPT mode set
			s->_conf = 0;
			s->Handle = 0;
			s->_rx_buf = 0;
			s->_mode = 1;
			s->_echo = 1;
		}
	}

	int id = init_uart(baud,uart);

	return id;
}

serial_info_t* _get_serial_byid(int id) {
	if((uint32_t)id < MAX_SERIAL_NUM) {
		return (serial_info_t*)(&__gsi[id]);
	}
	return (serial_info_t*)0;
}

serial_info_t* _get_serial(UART_HandleTypeDef *huart) {
	serial_info_t *s = __gsi;
	for(int i=0; i<MAX_SERIAL_NUM; i++, s++) {
		if(s->_inited && s->Handle == huart)
			return s;
	}
	return (serial_info_t*)0;
}

int __io_putchar(int ch) {
	if(get_console()->_type == 0) {
		if( HAL_UART_Transmit(get_console()->Handle, (uint8_t*)&ch,1,HAL_MAX_DELAY) != HAL_OK)
		return -1;
	} else { // USB
		get_console()->_tx_ch = ch;
		int to_cnt = 0;
		while( CDC_Transmit_FS((uint8_t*)&get_console()->_tx_ch, 1) == USBD_BUSY) {
			if(to_cnt++ > 100) return -1;
		}
	}
	return ch;
}

int __io_getchar() {
	int ch=0;
	if(get_console()->_type == 0) {
		HAL_UART_Receive_IT(get_console()->Handle, &get_console()->_rx_ch, 1);
		ch = fifo_getc(get_console()->_rx_buf);
#if 0
		while(HAL_UART_Receive_IT(get_console()->Handle, &get_console()->_rx_ch, 1) != HAL_OK);
		return get_console()->rx_ch;
#endif
	}
	else if(get_console()->_type == 1) {
		ch = cdc_getc();
	}
	return ch;
}

int _write(int32_t file, uint8_t *ptr, int32_t len) {
	if(get_console()->_type == 0) {
		if( HAL_UART_Transmit(get_console()->Handle, (uint8_t*)ptr,len,HAL_MAX_DELAY) != HAL_OK)
			return -1;
	}
	else if(get_console()->_type == 1) {
		CDC_Transmit_FS(ptr, len);
	}
	return len;
}

int ugetc() {
#if 0	
	int ch = ggetc(0);
	if(ch<1)
		uart_receive_it();
	return ch;
#else
	return fifo_getc(_get_serial(0)->_rx_buf);
#endif
}

int uputc(int ch) {
	return gputc(ch,0);
}

int uart_getc() {
	serial_info_t*s = _get_serial_byid( _default_uart_id );
	return fifo_getc(s->_rx_buf);
}

int uart_putc( int ch ) {
	serial_info_t*s = _get_serial_byid( _default_uart_id );
	if( HAL_UART_Transmit(s->Handle, (uint8_t*)&ch,1,HAL_MAX_DELAY) != HAL_OK)
		return -1;

	return fifo_getc(s->_rx_buf);
}

extern gfifo_t cdc_rx_fifo;

int cdc_getc() {
	// _default_cdc_id 
	return  fifo_getc(&cdc_rx_fifo);
}

int cdc_putc(int ch) {
	serial_info_t*s = _get_serial_byid( _default_cdc_id );
	int to_cnt=0;

	while( CDC_Transmit_FS((uint8_t*)&s->_tx_ch, 1) == USBD_BUSY) {
		if(to_cnt++ > 100) return -1;
	}
}

int ggetc(int fd) {
	serial_info_t*s = _get_serial_byid(fd);
	if(s && s->_inited) {
#if 0
		return fifo_getc(s->_rx_buf);
#else
		if(s->_type == 0) {
			return fifo_getc(s->_rx_buf);
		} else {
			return cdc_getc();
		}
#endif
	}
	return -1;
}

int gputc(int ch,int fd) {
	serial_info_t*s = _get_serial_byid(fd);
	if(s && s->_inited) {
		if(s->_type == 0) {
			s->_tx_ch = ch;
			if( HAL_UART_Transmit(get_console()->Handle, (uint8_t*)&s->_tx_ch,1,HAL_MAX_DELAY) != HAL_OK)
			return -1;
		} else {
			return cdc_putc(ch);
		}
	}
	return -1;
}

int gputs(const char* str,int fd) {
	serial_info_t*s = _get_serial_byid(fd);
	if(s && s->_inited) {
		if(s->_type == 0) {
			if( HAL_UART_Transmit(get_console()->Handle, (uint8_t*)str,strlen(str),HAL_MAX_DELAY) != HAL_OK)
				return -1;
		} else {
			int to_cnt = 0;
			while( CDC_Transmit_FS((uint8_t*)str, strlen(str)) == USBD_BUSY) {
				if(to_cnt++ > 100) return -1;
			}
//		return cdc_write((uint8_t*)str, strlen(str));
		}
		return 0;
	}
	return -1;
}

int gwrite(uint8_t *data, uint16_t len,int fd) {
	serial_info_t*s = _get_serial_byid(fd);
	if(s && s->_inited) {
		if(s->_type == 0) {
			if( HAL_UART_Transmit(get_console()->Handle, data,len,HAL_MAX_DELAY) != HAL_OK)
			return -1;
		} else {
			int to_cnt = 0;
			while( CDC_Transmit_FS(data,len) == USBD_BUSY) {
				if(to_cnt++ > 100) return -1;
			}
		}
	}
	return -1;
}

int gprintf(const char*fmt,...) {
	char buf[GDEBUG_BUFLEN];
	va_list ap;
	va_start(ap,fmt);
	int len = vsnprintf(buf,GDEBUG_BUFLEN-1,fmt,ap);
	va_end(ap);

	if(len > 0)
		return gwrite((uint8_t*)buf,(uint16_t)len,0);
	else
		return len;
}

int gdprintf(int fd,const char*fmt,...) {
	char buf[GDEBUG_BUFLEN];
	va_list ap;
	va_start(ap,fmt);
	int len = vsnprintf(buf,GDEBUG_BUFLEN-1,fmt,ap);
	va_end(ap);

	if(len > 0)
		return gwrite((uint8_t*)buf,(uint16_t)len,fd);
	else
		return len;
}

void uart_receive_it() {
	HAL_UART_Receive_IT(phuart_console,(uint8_t*)&get_console()->_rx_ch,1);
}

