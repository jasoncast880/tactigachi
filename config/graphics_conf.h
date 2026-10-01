#ifndef GRAPHICS_CONF_H
#define GRAPHICS_CONF_H


#define DEFAULT_TILE_LEN 16
#define DEFAULT_SCREEN_TILES_Y 15
#define DEFAULT_SCREEN_TILES_X 20
#define MAX_LAYERS 2
#define MAX_SPRITES_PER_LAYER 5

//#define DIRTY_RENDER 0
//#define FULLSCREEN_RENDER 0
#define HSCANLINE_RENDER  1


//h-scanline
#define HSCANLINE_SIZE 16
#if HSCANLINE_RENDER
static const int HSCANLINE_MAX = (DEFAULT_SCREEN_TILES_Y*DEFAULT_TILE_LEN)/HSCANLINE_SIZE;
#endif //TODO Define other render types and their allocated size

//engine preferences
#define SPRITE_RESERVE_SIZE 5

#endif //GRAPHICS_CONF_H

