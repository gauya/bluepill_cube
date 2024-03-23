/*
 * flash_hal.cpp
 *
 *  Created on: Mar 9, 2022
 *      Author: gau
 *
 *  ref. AN4826 Rev2
 */
#include "gflash.h"
#include "glog.h"

const uint32_t SFA = 0x08000000;

flash_map_t fmt_2m_s[] = {
	 { 0, 32, SFA }
	,{ 1, 32, SFA + 32*1*1024 }
	,{ 2, 32, SFA + 32*2*1024 }
	,{ 3, 32, SFA + 32*3*1024 }
	,{ 4,128, SFA + 32*4*1024 }
	,{ 5,256, SFA + 256*1*1024 }
	,{ 6,256, SFA + 256*2*1024 }
	,{ 7,256, SFA + 256*3*1024 }
	,{ 8,256, SFA + 256*4*1024 }
	,{ 9,256, SFA + 256*5*1024 }
	,{10,256, SFA + 256*6*1024 }
	,{11,256, SFA + 256*7*1024 }
};

flash_map_inf_t h7f_2m_s = {
		2048,0,
		1, 8,
		12,
		{12,0},
		{fmt_2m_s, 0 }
};

flash_map_t fmt_2m_d_1[] = {
	 { 0, 16, SFA }
	,{ 1, 16, SFA +  16*1*1024 }
	,{ 2, 16, SFA +  16*2*1024 }
	,{ 3, 16, SFA +  16*3*1024 }
	,{ 4, 64, SFA +  16*4*1024 }
	,{ 5,128, SFA + 128*1*1024 }
	,{ 6,128, SFA + 128*2*1024 }
	,{ 7,128, SFA + 128*3*1024 }
	,{ 8,128, SFA + 128*4*1024 }
	,{ 9,128, SFA + 128*5*1024 }
	,{10,128, SFA + 128*6*1024 }
	,{11,128, SFA + 128*7*1024 }
};

const uint32_t SFA2m_2 = SFA + 1024*1024;

flash_map_t fmt_2m_d_2[] = {
	 { 12, 16, SFA2m_2 }
	,{ 13, 16, SFA2m_2 +  16*1*1024 }
	,{ 14, 16, SFA2m_2 +  16*2*1024 }
	,{ 15, 16, SFA2m_2 +  16*3*1024 }
	,{ 16, 64, SFA2m_2 +  16*4*1024 }
	,{ 17,128, SFA2m_2 + 128*1*1024 }
	,{ 18,128, SFA2m_2 + 128*2*1024 }
	,{ 19,128, SFA2m_2 + 128*3*1024 }
	,{ 20,128, SFA2m_2 + 128*4*1024 }
	,{ 21,128, SFA2m_2 + 128*5*1024 }
	,{ 22,128, SFA2m_2 + 128*6*1024 }
	,{ 23,128, SFA2m_2 + 128*7*1024 }
};


flash_map_inf_t h7f_2m_d = {
		2048,0,
		0, 4,
		16,
		{8,8},
		{fmt_2m_d_1, fmt_2m_d_2 }
};

////////////////////////////////////////////////////////////////////////

flash_map_t fmt_1m_s[] = {
	 { 0, 32, SFA }
	,{ 1, 32, SFA +  32*1*1024 }
	,{ 2, 32, SFA +  32*2*1024 }
	,{ 3, 32, SFA +  32*3*1024 }
	,{ 4,128, SFA +  32*4*1024 }
	,{ 5,256, SFA + 256*1*1024 }
	,{ 6,256, SFA + 256*2*1024 }
	,{ 7,256, SFA + 256*3*1024 } // 0x80fffff
};

flash_map_inf_t h7f_1m_s = {
		1024,0,
		1, 4,
		8,
		{8,0},
		{fmt_1m_s, 0 }
};

flash_map_t fmt_1m_d_1[] = {
	 { 0, 16, SFA }
	,{ 1, 16, SFA +  16*1*1024 }
	,{ 2, 16, SFA +  16*2*1024 }
	,{ 3, 16, SFA +  16*3*1024 }
	,{ 4, 64, SFA +  16*4*1024 }
	,{ 5,128, SFA + 128*1*1024 }
	,{ 6,128, SFA + 128*2*1024 }
	,{ 7,128, SFA + 128*3*1024 } // 0x807ffff
};

const uint32_t SFA1m_2 = SFA + 512*1024;

flash_map_t fmt_1m_d_2[] = {
	 { 12, 16, SFA1m_2 }              // 0x808ffff
	,{ 13, 16, SFA1m_2 +  16*1*1024 }
	,{ 14, 16, SFA1m_2 +  16*2*1024 }
	,{ 15, 16, SFA1m_2 +  16*3*1024 }
	,{ 16, 64, SFA1m_2 +  16*4*1024 }
	,{ 17,128, SFA1m_2 + 128*1*1024 }
	,{ 18,128, SFA1m_2 + 128*2*1024 }
	,{ 19,128, SFA1m_2 + 128*3*1024 } // 0x80fffff
};


flash_map_inf_t h7f_1m_d = {
		2048,0,
		0, 4,
		16,
		{8,8},
		{fmt_1m_d_1, fmt_1m_d_2 }
};

/////////////////////////////////////////////////////////////////////
// 32kbyte all page( 0 ~ 31 ) : 1kbyte   --low density
//
flash_map_t _fm_1k = { 0, 1, SFA };
flash_map_t _fm_2k = { 0, 2, SFA };

flash_map_inf_t fm32k32page = { 32,1,2,4,32,{32,0},{&_fm_1k,} };
/////////////////////////////////////////////////////////////////////
// 128kbyte all page( 0 ~ 127 ) : 1kbyte -- medium density
flash_map_inf_t fm128128page = { 128,1,2,4,128,{128,0},{&_fm_1k,} };
/////////////////////////////////////////////////////////////////////
// 512kbyte all page( 0 ~ 255 ) : 2kbyte -- high density
flash_map_inf_t fm512k256page = { 512,2,2,4,256,{256,0},{&_fm_2k,} };
/////////////////////////////////////////////////////////////////////
// 256kbyte all page( 0 ~ 127 ) : 2kbyte -- high density
flash_map_inf_t fm256k128page = { 256,2,2,4,128,{128,0},{&_fm_2k,} };
/////////////////////////////////////////////////////////////////////
// 1Mbyte all page( 0 ~ 511 ) : 2kbyte -- XL high density
flash_map_inf_t fm1024k512page = { 1024,2,2,4,512,{512,0},{&_fm_2k,} };
/////////////////////////////////////////////////////////////////////


#if 0 // flash memory struct ,map

BASE 0x0800 0000

-- RM0468 Reference manual  stm32h723
sector 8  128k

-- RM0008  STM32F103xB
page  32  1k	low-density devices
page 128  1k    medium-density devices
page 256  2k    high-density devices
page 128  2k    connectivity line devices)
page 512  2k    XL-density Flash module organization

-- RM0383 stm32f411 
sector 4  16k
sector 1  64k
sector 3 128k

-- RM0090 STM32F405/415, STM32F407/417, STM32F427/437 STM32F429/439

STM32F40x and STM32F41x - single bank
sector 4  16k
sector 1  64k
sector 7 128k

STM32F42xxx and STM32F43xxx - dual bank sector(0 ~ 23)
bank1 sector 4  16k
      sector 1  64k
      sector 7 128k
bank2 sector 4  16k
      sector 1  64k
      sector 3 128k

(STM32F42xxx and STM32F43xxx) . STM32F72xxx and STM32F732xx/F733xx
bank1 sector 4  16k
      sector 1  64k
      sector 7 128k
bank2 sector 4  16k
      sector 1  64k
      sector 3 128k

(STM32H750xB)
bank1 128k

(STM32H742xI/743xI/753xI devices)
bank1 sector 8 128k
bank2 sector 8 128k

(STM32H742xG/743xG)
bank1 sector 4 128k
bank2 sector 4 128k

#endif

flash_map_inf_t *__default_flash_map_inf_t = &h7f_2m_s;  // 2Mbyte single mode

// return bank (0 or 1) on success
int get_flash_inf(uint32_t addr, flash_map_t* inf) {
	if( addr < SFA ) return -1;
//#if defined (DUAL_BANK)

	flash_map_inf_t *mi = __default_flash_map_inf_t;
	flash_map_t *m;

	if( mi->type == 2 ) {
		inf->page = (addr - SFA) / (1024 * mi->page_size);
		inf->size = mi->page_size;
		inf->address = SFA + (inf->page * 1024 * inf->size);
		return 0;
	}
	int bank = (mi->type == 0)? 2 : 1;
	for(int i=0; i<bank ;i++) {
		m = mi->page_inf[i];
		if(!m) break;
		for(int k=0;k<(int)mi->page_per_bank[i];k++) {
			if(addr >= m[k].address && addr < m[k+1].address) {
				*inf = m[k];
				return i;
			}
			gdebug(5,">>addr=0x%x != 0x%x / 0x%x page=%d size=%dk\n",addr, m[k].address
					, m[k+1].address,m[k].page,m[k].size);
		}
	}
	return -3;
}


int get_flash_inf_page(int page, flash_map_t* inf) {
	flash_map_inf_t *mi = __default_flash_map_inf_t;

	if(mi->type == 2) { // array pages
		if( (uint32_t)page > (mi->size / mi->pages)) {
			return -1;
		}
		inf->page = page;
		inf->size = mi->page_size;
		inf->address = SFA + (page * mi->page_size * 1024);
		return 0;
	}

	int bank = (mi->type == 0)? 2 : 1;

	flash_map_t *m;
	for(int i=0; i<bank ;i++) {
		m = mi->page_inf[i];
		if(!m) break;
		for(int k=0;k<(int)mi->page_per_bank[i];k++) {
			if(page == m[k].page) {
				*inf = m[k];
				return i;
			}
		}
	}
	return -1; //
}

#if 0 // H7
int flash_erase_page(int page, int num) {
	uint32_t err = 0;
	FLASH_EraseInitTypeDef pei;
	pei.Banks = 3; // 1, 2, 3
	pei.Sector = page;
	pei.NbSectors = num;
	pei.TypeErase = FLASH_TYPEERASE_SECTORS; // mass or sector FLASH_TYPEERASE_MASSERASE
	pei.VoltageRange = FLASH_VOLTAGE_RANGE_3; // when 32bit
	if( HAL_FLASHEx_Erase(&pei,&err) != HAL_OK ) {
		gdebug(5,"flash erase error(%d)\n",err);
		return -5;
	}
	return 0;
}
#else
int flash_erase(uint32_t addr, int pages, int bank ) {
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
#endif

#if 0
uint32_t flash_write(uint32_t flash_addr, uint32_t *data, uint32_t len) {
	HAL_FLASH_Unlock();

	//void FLASH_Erase_Sector(uint32_t Sector, uint32_t Banks, uint32_t VoltageRange);
	//HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *SectorError)

	flash_map_t mt;
	int sectors = 1;
	int rt = get_flash_inf( flash_addr, &mt);
	if( rt != 0 ) {
		gdebug(5,"%s -> get_flash_inf() addr=%x not(%d)\n", __FUNCTION__, flash_addr,rt);
		return 0;
	} else {
		if( len > mt.size ) {
			sectors = (len / mt.size) + 1; // Roughly... not excute value
			gdebug(5,"request size (%u:%x) > (%x) not been fully processed yet.\n"
					,len,len, mt.size);
		}
		gdebug(5,"page=%d addr=%x nr=%d\n",mt.page, mt.address, mt.size);
	}
	flash_erase_page(mt.page, sectors); //	FLASH_PageErase( flash_addr );

	uint32_t i;
	for(i=0;i<len; i+=FLASH_NB_32BITWORD_IN_FLASHWORD) { // __default_flash_map_inf_t.dsize
		HAL_FLASH_Program( FLASH_TYPEPROGRAM_FLASHWORD, flash_addr, (uint32_t)data); // FLASH_NB_32BITWORD_IN_FLASHWORD 32*8=256bit
		data += FLASH_NB_32BITWORD_IN_FLASHWORD;
		flash_addr += FLASH_NB_32BITWORD_IN_FLASHWORD;
	}
	HAL_FLASH_Lock();

	return i;
}
#else
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
#endif

int flash_read(uint32_t addr, uint32_t *data, size_t len) {
	HAL_FLASH_Unlock();
	for(uint32_t i=0;i<len; i+=4) {
		*data++ = *(__IO uint32_t*)addr;
		addr += 4;
	}
	HAL_FLASH_Lock();

	return len;
}


void flash_test() {
	uint32_t saddr = 0x8000000 + 1024 * 60; // 60k~

	struct test {
		uint8_t ver;
		uint32_t addr;
		char name[16];
		int flag;
	} test = { 32, saddr, "I am a boy", 2024 };

	struct test test2;

	int i1 = flash_write(saddr, (uint32_t*)&test, sizeof(struct test));
	int i2 = flash_read(saddr, (uint32_t*)&test2, sizeof(struct test));

	gdebug(2,"flash test %d, %d\n",i1,i2);

	gdebug(2,"--==> %u,%lx, %s, %d\n", test2.ver, test2.addr, test2.name, test2.flag);
}