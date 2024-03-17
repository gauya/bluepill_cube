/*
 * flash_hal.h
 *
 *  Created on: Mar 9, 2022
 *      Author: seu
 */

#ifndef __FLASH_HAL_H__
#define __FLASH_HAL_H__

#if 0
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_flash.h"
#include "stm32h7xx_hal_flash_ex.h"
#else
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"
#endif

typedef union {
	uint32_t reg;
	struct {
		uint32_t iwdg_stop :1;
		uint32_t iwdg_stdby:1;
		uint32_t ndbank    :1;  // 1:single bank mode (8x 256bit)
		uint32_t ndboot    :1;  // 1:disable dual boot(default)
		uint32_t nwrp      :11; // real:7
		uint32_t rdp       :7;
		uint32_t nrst_stdby:1;
		uint32_t nrst_stop :1;
		uint32_t iwdg_sw   :1;
		uint32_t wwdg_sw   :1;
		uint32_t bor_lev   :2;
		uint32_t optstrt   :1;
		uint32_t optlook   :1;
	};
} FLASH_OPTCR_t;

typedef struct {
	uint16_t page   ; // page id
	uint16_t size   ; // kbyte
	uint32_t address; // start address
} flash_map_t;

typedef struct {
//	char name[9]; // h + 7 + 4 + 3 + v + i + t + 6
	uint32_t size; // total size
	int page_size; // kbyte per sector , type in (0 or 1)
	int type; // 0: dual bank, 1: single bank , 2: simple_array
	int dsize; // read data size, single bank: 8x 32bits, dual : 4x x 32bits
	uint16_t pages;
	uint16_t page_per_bank[2]; // page nr for bank
	flash_map_t *page_inf[2]; // page info. per bank
} flash_map_inf_t;

extern const uint32_t SFA;

#ifdef __cplusplus
extern "C" {
#endif

int flash_erase(uint32_t addr, int pages=1, int bank=FLASH_BANK_1);
int get_flash_inf(uint32_t addr, flash_map_t* inf);
uint32_t flash_write(uint32_t flash_addr, uint32_t *data, uint32_t len);
void flash_read(uint32_t flash_addr, uint32_t *data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_HAL_H__ */
