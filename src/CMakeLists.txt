add_executable( tactigachi_bm ) 
add_executable( tactigachi_rtos )

add_subdirectory(drivers) #for pulling in the core libs; fatfs, engines, etc.

if(TARGET tactigachi_bm)
set(CMAKE_BUILD_TYPE Debug)
target_sources( tactigachi_bm PRIVATE
	dummy.cpp
	#add on peripherals as you go
	${CMAKE_CURRENT_SOURCE_DIR}/modules/display/display.cpp
	#${CMAKE_CURRENT_SOURCE_DIR}/modules/sdc/sdc.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/modules/buttons/buttons.c
	#${CMAKE_CURRENT_SOURCE_DIR}/modules/usb/usb.cpp

)

target_include_directories(tactigachi_bm PRIVATE
	#for all modules visibility into the executable
	${CMAKE_CURRENT_SOURCE_DIR}/modules/display
	#${CMAKE_CURRENT_SOURCE_DIR}/modules/sdc
	#${CMAKE_CURRENT_SOURCE_DIR}/modules/buttons
	#${CMAKE_CURRENT_SOURCE_DIR}/modules/usb
)


target_link_libraries(tactigachi_bm PRIVATE
	# macro config
	tactigachi_cfg

	# core HAL as required
	pico_stdlib
	hardware_irq
	hardware_gpio
	hardware_dma
	hardware_pio
	hardware_pwm

	# drivers/other HAL
	ili9341_rp2040
	usb_rp2040 # pulls in bsp/board api, tusb??

	# core dependencies
	tile_engine 
	#fatfs

	# misc.
	assets
)

target_compile_definitions(tactigachi_bm PRIVATE configNUMBER_OF_CORES=1)

pico_enable_stdio_usb(tactigachi_bm 0) #perhaps disable if not using/debugging via gdb
pico_enable_stdio_uart(tactigachi_bm 1)
pico_add_extra_outputs(tactigachi_bm)

add_custom_command(TARGET tactigachi_bm POST_BUILD # command to enable static mem/sanity check.
    COMMAND arm-none-eabi-size $<TARGET_FILE:tactigachi_bm>
    COMMENT "--- Manual Static Memory Report ---"
)

endif() #tactigachi_bm

####################################################################################################################################################################################

if(TARGET tactigachi_rtos)
target_compile_definitions(tactigachi_rtos PRIVATE PICO_CXX_DISABLE_ALLOCATION_OVERRIDES=1) #allow for override of new, delete.

target_compile_definitions(tactigachi_rtos PRIVATE RTOS_MODE)

####MODULES#####

target_sources( tactigachi_rtos PRIVATE
	main.cpp
	#mem_override.cpp 

	${CMAKE_CURRENT_SOURCE_DIR}/modules/display/display.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/modules/sdc/sdc.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/modules/usb/usb.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/modules/buttons/buttons.c
)

target_include_directories(tactigachi_rtos PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/modules #for common.h

  ${CMAKE_CURRENT_SOURCE_DIR}/modules/display
	${CMAKE_CURRENT_SOURCE_DIR}/modules/sdc

  ${CMAKE_CURRENT_SOURCE_DIR}/modules/usb
	${CMAKE_CURRENT_SOURCE_DIR}/modules/buttons
)

#### DEPENDENCIES #####

target_link_libraries(tactigachi_rtos PRIVATE
	# macro config
	tactigachi_cfg

	# core HAL as required
	pico_stdlib
  pico_cyw43_arch_none
	hardware_irq
	hardware_gpio
	hardware_dma
	hardware_pio

	# 3rd party SDK lib presets (in sdk w.p)
	tinyusb_device

	# drivers/other HAL
	ili9341_rp2040
	sd_rp2040 
	usb_rp2040 # pulls in bsp/board api, tusb??

	# core dependencies
	FreeRTOS-Kernel-Heap4
	tile_engine 
	fatfs

	# misc.
	assets
)

target_compile_definitions(tactigachi_rtos PRIVATE configNUMBER_OF_CORES=1) #perhaps add on multicore support if possible...

pico_enable_stdio_usb(tactigachi_rtos 1) #perhaps disable if not using/debugging via gdb
#pico_enable_stdio_uart(tactigachi_rtos 1)
pico_add_extra_outputs(tactigachi_rtos)

add_custom_command(TARGET tactigachi_rtos POST_BUILD # command to enable static mem/sanity check.
    COMMAND arm-none-eabi-size $<TARGET_FILE:tactigachi_rtos>
    COMMENT "--- Manual Static Memory Report ---"
) 

endif() #tactigachi_rtos

