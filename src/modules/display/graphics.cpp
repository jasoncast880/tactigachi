#include "graphics.hpp"

Engine* Graphics::setup() {
	
	struct Layer* sys_layer = add_layer(...);
	engine_init(sys_layer);

}

void Graphics::start() {
}

void Graphics::render() {
	


} //needs to throw ISR 
