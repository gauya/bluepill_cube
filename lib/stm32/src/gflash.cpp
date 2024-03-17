#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_flash.h"
#include "string.h"
#include "gflash.h"

int flash_erase(uint32_t addr, int bank, int pages) {
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError;

    EraseInitStruct.Banks = bank; //FLASH_BANK_1, FLASH_BANK_BOTH, FLASH_BANK_2;
    EraseInitStruct.NbPages = pages;
    EraseInitStruct.PageAddress = addr;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES; //FLASH_TYPEERASE_MASSERASE

    HAL_FLASH_Unlock();
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
        HAL_FLASH_Lock();
        return -1;
    }

    HAL_FLASH_Lock();
    return 0;
}

int flash_write(uint32_t addr, uint32_t *data, size_t len) {
    uint16_t *dp = (uint16_t *)data;

    flash_erase(addr,1);

    HAL_FLASH_Unlock();

    for( size_t i=0; i < len; i += 2, addr += 2 ) {
        // *(__IO uint16_t*)Address = *dp++;
        //FLASH_Program_HalfWord(addr, *dp++);
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr, *dp++ ) != HAL_OK) {
            HAL_FLASH_Lock();
        return -1;
        }
    }

    HAL_FLASH_Lock();

    return len;
}

int flash_read(void *buf, uint32_t addr, size_t len) {
    memcpy(buf, (void*)addr, len);
}