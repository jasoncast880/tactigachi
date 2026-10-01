#pragma once

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/gpio.h"

#include "graphics_conf.h"

#ifdef __cplusplus
extern "C"{
#endif

//gp defines; 
//ili9341 modes, reg addressing refer to strionix ili9341 10.1 table


//general-purpose registers
#define NOOP    0x00
#define SWRESET 0x01

//power&display modes
#define SLPIN   0x10 /* Sleep ON */
#define SLPOUT  0x11 /* Sleep OFF */
#define PTLON   0x12 /* Partial mode ON */
#define NORON   0x13 /* Normal mode ON */

#define INVOFF  0x20 /* Display Inversion ON */
#define INVON   0x21 /* Display Inversion OFF */
#define DISPOFF 0x28 /* Display OFF */
#define DISPON  0x29 /* Display ON */

/* Mem Access CTL Register: MY-MX-MV-ML-BGR-MH-X-X
 * MY - Row Addr Order
 * MX - Column Addr Order
 * MV - Row/Column Order
 * ML - Vertical Refresh Order
 * BGR - RGB/BGR Order: 
 * MH - Horizontal Refresh Order
 * X - DC, assume 0!!!!
 * for ampalaya purposes
 * screen is rotated 90deg CCW && RGB-big endian
 * thus set:
 * MADCTL = 0 1 1 0 1 1 / 0 0 == 0x68
 */
#define MADCTL  0x36 /* see above.. */

//scrolling-related registers here
static volatile bool NORMAL_MODE = true;

#define VSCR_DEF 0x33 /* config vert. scrolling */
#define VSCR_ADD 0x37 /* assign vert. scrolling pointer */

#define CASET   0x2A /* Column Address SET */
#define RASET   0x2B /* Row Address SET */

#define RAM_WR  0x2C /* Write to VRAM */

#define FRMCTR1 0xB1 /* frame rate control */
#define PIXSET  0x3A /* config color format */

static void ili9341_hard_reset();
static void ili9341_soft_reset(); 

static spi_inst_t* bus;
void ili9341_initialize(spi_inst_t* bus, int8_t cs,int8_t rst,int8_t dc); //running on spi0 bus w . dma enabled

static uint dma_chan;
static uint16_t* read_addr;
static uint8_t screen_sector;
static uint8_t MAX_SCREEN_SECTORS;
static void ili9341_dma_init(uint16_t* read_addr);
void hscanline_handler();

//wrapped in CS toggling.
void ili9341_writeCommand(uint8_t commandByte);
void ili9341_writeData(uint8_t dataByte);
void ili9341_writeDataBuffer8(uint8_t* dataBuf, size_t len);
void ili9341_writeDataBuffer16(uint16_t* dataBuf, size_t len); //consider ridding

//CS toggling handled
void ili9341_setAddrWindow(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h); 
void ili9341_setScrollWindow(uint16_t tfa, uint16_t vsa, uint16_t bfa); 
void ili9341_setScrollPtr(uint16_t vsp); //page 123 of strionix manual

//for locking access to the spi0 bus
void ili9341_setCS_HI();
void ili9341_setCS_LO();

void ili9341_dma_mem_to_disp( volatile void* write_addr, uint16_t read_addr, uint32_t tc ) ;
void ili9341_dma_periph_to_disp( volatile void* write_addr, uint16_t read_addr, uint32_t tc ) ;  //IDK HOW THIS ONE WILL WORK

#ifdef __cplusplus
}
#endif                                                       
