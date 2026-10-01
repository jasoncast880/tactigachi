file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

add_library(ili9341_rp2040 STATIC
  ${SOURCES}
)

#pico_generate_pio_header(ili9341_rp2040 ${CMAKE_CURRENT_LIST_DIR}/ili9341.pio)

target_link_libraries(ili9341_rp2040 PUBLIC
    pico_stdlib 
    hardware_spi
	hardware_dma
	#hardware_pio

	tactigachi_cfg
)

target_include_directories(ili9341_rp2040 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(ili9341_rp2040 PRIVATE -mcpu=cortex-m0plus -mthumb) #why compiler specs here? TODO clarify
