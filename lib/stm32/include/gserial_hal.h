/*
 * gserial.h
 *
 *  Created on: May 12, 2022
 *      Author: seu
 */

#ifndef _GSERIAL_H_
#define _GSERIAL_H_

#include "app.h"
#include "gfifo.h"
//#include "guart_hal.h"
#include "usbd_cdc_if.h"

#define GDEBUG_BUFLEN	256
/////////////////////////////////////////////////////////////////////

typedef struct {
	union {
		uint16_t _conf;
		struct {
			uint16_t  Lock:1; // _lock
			uint16_t _type:3; // 0: uart, 1: cdc, 2:
			uint16_t _mode:2; // 0: polling, 1: rxinterrupt, 2: ,4: DMA
			uint16_t _echo:1;
			uint16_t _inited:1;
			uint16_t _fd:3;
			uint16_t :6;
		};
	};
	UART_HandleTypeDef *Handle;

	gfifo_t *_rx_buf;
//	gfifo_t *_tx_buf;

	uint8_t _rx_ch; // uart interrupt
	uint8_t _tx_ch;

	/*
	int (*putc)(int);
	int (*getc)();
	int (*write)(uint8_t *, uint16_t );

	void (*lock)();
	void (*unlock)();
*/
} serial_info_t;

#define MAX_SERIAL_NUM	6

#ifdef __cplusplus
serial_info_t* _get_serial(int id);
serial_info_t* _get_serial(UART_HandleTypeDef *huart);

serial_info_t* get_console();

int ggetc(int fd=0);
int gputc(int ch,int fd=0);
int gputs(const char* str,int fd=0);
int gwrite(uint8_t *data, uint16_t len,int fd=0);

int init_uart(UART_HandleTypeDef *phuart, int fd=-1);
int init_cdc(int id=-1);
int init_serial(UART_HandleTypeDef *phuart);

extern "C" {
#endif

int ugetc();
int uputc(int ch);

int cdc_getc();
int cdc_putc(int ch);

int __io_putchar(int ch);
int __io_getchar();
int _write(int32_t file, uint8_t *ptr, int32_t len);
int gprintf(const char*format,...);
int gdprintf(int fd,const char*format,...);

#ifdef __cplusplus
}
#else

int gputs(const char* str);

#endif

#endif /* _GSERIAL_H_ */
