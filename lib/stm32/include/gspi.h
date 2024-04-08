#include "main.h"
#include "ggpio.h"

#ifndef __GSPI_H
#define __GSPI_H

enum { eSPI_Slave=0, eSPI_Master=1 };
enum { eSPI_Duplex=0,eSPI_1line=1, eSPI_Rxonly=2};

#define DEF_SPIRXBUFSIZE 64
#define MAX_SPIRXBUFSIZE (32*1024)
class gspi {
    ggpio _sck, _mosi, _miso;
    union {
        uint16_t _mode;
        struct {
            uint16_t _master :1;
            uint16_t _lines  :2; // 0:duplex, 
        };
    };
    SPI_HandleTypeDef *_hs;

    uint8_t *_rxbuf;
    uint32_t _bufsize;
public:
// constructor
    gspi(SPI_TypeDef *spi, uint16_t mode, gpio_t gclk, gpio_t gmiso, gpio_t gmosi, uint32_t bufsize, int speed=-1);
    gspi();
    ~gspi();

// 
    int init();
    int init(SPI_TypeDef *spi, uint16_t mode, gpio_t gclk, gpio_t gmiso, gpio_t gmosi, uint32_t bufsize, int speed=-1);

    int start();

    int read();
    int write();

};

#endif // __GSPI_H
