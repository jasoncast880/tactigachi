#include "display.hpp"

#include <hardware/dma.h>
#include <hardware/irq.h>
#include <hardware/structs/spi.h>

#include "graphics_conf.h"

//for assets ; 
#include "ampalaya_tileset_16.h"
#include "tilemaps.h"

namespace Display{

static uint32_t spi0_dma_chan; //hardware

void setup(Engine* eng) {
	//how to verfy e works?? TODO
	if(!eng) {
		//break and shii
	}

	static uint8_t MAX_SCREEN_SECTORS = 240/HSCANLINE_SIZE;

	
#ifdef RTOS_MODE
	render_token = xSemaphoreCreateBinary();
#endif
}

volatile uint32_t tile_count = 0; //TODO: build engine api to give easy data

#ifdef RTOS_MODE

void push_pixels( void* pvParameters ) { 
	for(;;) {
		xSemaphoreTake(render_token, portMAX_DELAY);
		ili9341_setCS_LO();

		//reconfigure the display draw area
		y0 = h_scanline_counter * HSCANLINE_SIZE
		ili9341_setAddrWindow(0,y0,DEFAULT_TILE_LEN*DEFAULT_SCREEN_TILES_X, HSCANLINE_SIZE);
		ili9341_writeCommand(RAM_WR);

		dma_channel_start(spi0_dma_chan); //dma isr will return the semphr, channel is on ring so dont worry about resetting the pointer.
		//refer to the handler/isr for more detail

#if HSCANLINE_RENDER
		e->h_scanline_counter++;
#endif
	}
}

void render( void* pvParameters ) {
	for(;;) {
		xSemaphoreTake(render_token, portMAX_DELAY);

#if HSCANLINE_RENDER
		if( e->h_scanline_counter < HSCANLINE_MAX ) {
			engine_render(e); //TODO how much time does this take?
			h_scanline_counter++;
		} else {
			h_scanline_counter = 0; //TODO give update entities access to run
		}
#endif

		xSemaphoreGive(render_token);
	}
}

void update_entities(void* pvParams ) {
	for(;;) {
	}
}

#endif 

}
