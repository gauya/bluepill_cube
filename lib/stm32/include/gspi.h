#include "main.h"
#include "ggpio.h"

#ifndef __GSPI_H
#define __GSPI_H

enum { eSPI_Master=0, eSPI_Slave=1  };
enum { eSPI_Duplex=0,eSPI_1line=1, eSPI_Rxonly=2};
enum { eSPI_Poll=0, eSPI_Interrupt, eSPI_Dma };

#define DEF_SPIRXBUFSIZE 64
#define MAX_SPIRXBUFSIZE (1024)
class gspi {
    ggpio _sck, _mosi, _miso;
    union {
        uint16_t _mode;
        struct {
            uint16_t _inited :1;
            uint16_t _ms     :1; // 0:master, 1:slave
            uint16_t _lines  :2; // 0:duplex, 
            uint16_t _dmaint :2; // 0:poll, 1:int, 2:dma
            uint16_t _remap  :1;
            //uint16_t :9;
        };
    };
    SPI_HandleTypeDef *_hs;

    uint8_t *_rxbuf;
    uint32_t _bufsize, _datalen;
public:
// constructor
    gspi(SPI_TypeDef *spi, uint16_t mode, gpio_t gclk, gpio_t gmiso, gpio_t gmosi, uint32_t bufsize, int speed=-1);
    gspi();
    ~gspi();

    int init();
    int init(SPI_TypeDef *spi, uint16_t mode, gpio_t gclk, gpio_t gmiso, gpio_t gmosi, uint32_t bufsize, int speed=-1);

    int start();

    friend void gspi_rxcallback(SPI_HandleTypeDef *hspi);

    int read(uint8_t *buf, uint32_t bsize);
    int read();
    int write(uint8_t *data, uint32_t len);

};

#endif // __GSPI_H
