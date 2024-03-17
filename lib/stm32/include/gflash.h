
#ifndef __GFLASH_H__
#define __GFLASH_H__

#include <stdint.h>
#include <stddef.h>
#include <stm32f1xx_hal.h>
#include <stm32f1xx_hal_flash.h>

int flash_erase(uint32_t addr, int pages=1, int bank=FLASH_BANK_1);
int flash_write(uint32_t addr, uint32_t *data, size_t len);
int flash_read(void *buf, uint32_t addr, size_t len);

#endif // __GFLASH_H__