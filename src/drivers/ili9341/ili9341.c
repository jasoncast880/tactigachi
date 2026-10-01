#include "ili9341.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <hardware/spi.h>
#include <pico/time.h>

#include "pinout.h"  //TODO ensure that this is included via CMAKE

static uint8_t _ILI9341_CS;
static uint8_t _ILI9341_RST;
static uint8_t _ILI9341_DC;
static uint8_t _ILI9341_MOSI;
static uint8_t _ILI9341_SCLK;
static uint8_t _ILI9341_MISO;

//assume that the channel is claiming from an allocated memory region/hscanline buf
static void ili9341_dma_init(uint16_t* read_addr) { 
	read_addr = read_addr;
	screen_sector = 0 ;

	dma_chan = dma_claim_unused_channel(true);
	dma_channel_config c = dma_channel_get_default_config(dma_chan);
	static uint32_t tc = HSCANLINE_SIZE * 320;

	channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
	channel_config_set_write_increment(&c, false);
	channel_config_set_read_increment(&c, true);
	channel_config_set_dreq(&c, DREQ_SPI0_RX); //TODO hardcoded ; fix for portability
	
	/*
	 * technical challenges: 
	 * 1 ) need to sync the write increment ring wrap to the start of the hscanline
	 *
	 * 2 ) need to sync the end of the display packet with the end of the channel
	 * transmission..
	 *
	 * systemic challenges:
	 * 1 ) need to coordinate DMA channels across modules
	 * 2 ) need to cordinate DMA ISR, other ISR across driver files.
	 */

	dma_channel_configure(
		dma_chan,
		&c, 
		&spi0_hw->dr,
		read_addr, //TODO does this increment over transfer sequence ????
		tc, 
		false
	);

	dma_channel_set_irq0_enabled(dma_chan, true);

	irq_set_exclusive_handler(DMA_IRQ_0, hscanline_handler);
	irq_set_enabled(DMA_IRQ_0, true);
	
	//call dma_handler() once to kickstart the transfer. note that it needs to run on command 
}

void hscanline_handler() { //this needs to trigger at the end of dma chan's transfer
	dma_hw->ints0 = 1u << dma_chan;
	dma_channel_set_read_addr(dma_chan, read_addr, false); 
	//simply reconfigures the chan to map with the scanline region's start.
	//seperate protocol is needed to move the draw area to the right point.
	screen_sector++;
	if(screen_sector > MAX_SCREEN_SECTORS)
		screen_sector = 0 ;
}


// reconfigure the screen peripheral to the proper screen phase so DMA can do its mass-data transfer. At the end of the sequence start the DMA chan
int ili9341_reconfigure_draw_area(uint8_t hscanline_no ) {
	//hardware : set peripheral to appropriate sector
	ili9341_setAddrWindow( 0, hscanline_no * HSCANLINE_SIZE, 320, HSCANLINE_SIZE );

	//software side : update the engine's global pointer to calculate the right screen sector.
	dma_channel_start(dma_chan);
}

void write_screen() { 
	/*
	 * conditions to call: should have the engine up and running. 
	 * engine will have given a pointer handle for easy read pulls.
	 * 
	 */
	
}

//note after init, you still have to put cs low in order to write spi.
void ili9341_initialize(spi_inst_t* bus, int8_t cs, int8_t rst, int8_t dc) { 

	spi_init(bus, 8000 * 1000); //SPI hardcoded @ 8MHz
	gpio_set_function(SPI0_SCLK, GPIO_FUNC_SPI);
	gpio_set_function(SPI0_RX, GPIO_FUNC_SPI);
	gpio_set_function(SPI0_TX, GPIO_FUNC_SPI);

    _ILI9341_CS = cs;
    _ILI9341_RST = rst;
    _ILI9341_DC = dc;

    gpio_init(_ILI9341_CS);
    gpio_init(_ILI9341_RST);
    gpio_init(_ILI9341_DC);
   
    gpio_set_dir(_ILI9341_CS, GPIO_OUT);
    gpio_set_dir(_ILI9341_RST, GPIO_OUT);
    gpio_set_dir(_ILI9341_DC, GPIO_OUT);

    gpio_put(_ILI9341_DC, 0);
    gpio_put(_ILI9341_CS, 1);

    ili9341_hard_reset(); 
    ili9341_soft_reset();

    ili9341_writeCommand(0xC0); // Power Control 1
    ili9341_writeData(0x23);

    ili9341_writeCommand(0xC1); // Power Control 2
    ili9341_writeData(0x10);

    ili9341_writeCommand(0xC5); // VCOM control 1
    ili9341_writeData(0x3e);
    ili9341_writeData(0x28);

    ili9341_writeCommand(0xC7); // VCOM control 2
    ili9341_writeData(0x86);

    ili9341_writeCommand(0xB1); // Frame Rate Control
    ili9341_writeData(0x00);
    ili9341_writeData(0x18);

    ili9341_writeCommand(0xB6); // Display Function Control
    ili9341_writeData(0x08);
    ili9341_writeData(0x82);
    ili9341_writeData(0x27);


    //call to set controller's display orientation, pixel format
    ili9341_writeCommand(PIXSET);
    ili9341_writeData(0x55); //set the pixel format to RGB 5-6-5

    ili9341_writeCommand(MADCTL);
    ili9341_writeData(0xE8);
    ili9341_setAddrWindow(0,0,320,240); //recalibrate addressing to fit the whole frame

    gpio_put(_ILI9341_CS, 1);
		sleep_ms(500);
    gpio_put(_ILI9341_CS, 0);

    ili9341_writeCommand(DISPON);
    sleep_ms(100);
}

void ili9341_writeCommand(uint8_t commandByte){
    gpio_put(_ILI9341_CS, 0);

    gpio_put(_ILI9341_DC, 0);
    spi_write_blocking(bus, &commandByte, 1);

    gpio_put(_ILI9341_CS, 1);
}

void ili9341_writeData(uint8_t dataByte){
    gpio_put(_ILI9341_CS, 0);

    gpio_put(_ILI9341_DC, 1);
    spi_write_blocking(bus, &dataByte, 1);

    gpio_put(_ILI9341_CS, 1);
}

void ili9341_writeDataBuffer8(uint8_t* dataBuf, size_t len){ 
    gpio_put(_ILI9341_CS, 0);

    gpio_put(_ILI9341_DC, 1);
    spi_write_blocking(bus, dataBuf, len);

    gpio_put(_ILI9341_CS, 1);
}

//commands abstracted
void ili9341_setScrollWindow(uint16_t tfa, uint16_t vsa, uint16_t bfa){
    ili9341_setCS_HI();

    ili9341_writeCommand(VSCR_DEF);
    ili9341_writeData((uint8_t)(tfa>>8));
    ili9341_writeData((uint8_t)(tfa&0xFF));
    ili9341_writeData((uint8_t)(vsa>>8));
    ili9341_writeData((uint8_t)(vsa&0xFF));
    ili9341_writeData((uint8_t)(bfa>>8));
    ili9341_writeData((uint8_t)(bfa&0xFF));
    sleep_ms(10);
}

void ili9341_setScrollPtr(uint16_t vsp){
		ili9341_setCS_HI();

    ili9341_writeCommand(VSCR_ADD);
    ili9341_writeData((uint8_t)(vsp>>8));
    ili9341_writeData((uint8_t)(vsp&0xFF));
    sleep_ms(10);
}

void ili9341_setAddrWindow(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h) { 
    uint16_t x1 = x0+w-1;
    uint16_t y1 = y0+h-1;
    ili9341_setCS_HI();

    ili9341_writeCommand(CASET);
    ili9341_writeData((uint8_t)(x0>>8));
    ili9341_writeData((uint8_t)(x0&0xFF));
    ili9341_writeData((uint8_t)(x1>>8));
    ili9341_writeData((uint8_t)(x1&0xFF));

    ili9341_writeCommand(RASET);
    ili9341_writeData((uint8_t)(y0>>8));
    ili9341_writeData((uint8_t)(y0&0xFF));
    ili9341_writeData((uint8_t)(y1>>8));
    ili9341_writeData((uint8_t)(y1&0xFF));

} //YOU MUST FOLLOW WITH A RAMWR, THEN DO A 16 bit write

static void ili9341_hard_reset(){
    gpio_put(_ILI9341_RST, 1);
    sleep_ms(10);
    gpio_put(_ILI9341_RST, 0);
    sleep_ms(10);
    gpio_put(_ILI9341_RST, 1);
    sleep_ms(120);
}

static void ili9341_soft_reset(){
    //call to set power & electrical presets
    ili9341_writeCommand(SWRESET);
    sleep_ms(150);
    ili9341_writeCommand(SLPOUT);
    sleep_ms(120);
}

void ili9341_setCS_HI() { 
	gpio_put(_ILI9341_CS, 1);
}
void ili9341_setCS_LO() {
	gpio_put(_ILI9341_CS, 0);
}

