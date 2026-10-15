#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "pico/stdlib.h"
#include <stddef.h>
#include <stdint.h>

#include "pinout.h"       
#include "hardware/dma.h"
#include "hardware/irq.h"
#include <hardware/regs/intctrl.h>
#include <hardware/spi.h>
#include <pico/platform/common.h>

#include "engine_api.h"
#include "tile_engine.hpp"

namespace Graphics {

extern "C" {

Engine* setup();

void start();

void render(); //main time-crit thread work


static void render_handler();

}

}
