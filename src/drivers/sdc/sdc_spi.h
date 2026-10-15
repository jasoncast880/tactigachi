#ifndef SDC_H
#define SDC_H

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#define CMD_GO_IDLE_ST  0x00 
#define CMD0 CMD_GO_IDLE_ST

#define SEND_IF_COND    0x08 
#define CMD8 SEND_IF_COND    

#define READ_OCR        0x3A 
#define CMD58 READ_OCR        

#define STOP_TRANS		0x0c 
#define CMD12 STOP_TRANS		  

#define SET_BLOCKLEN    0x10 
#define CMD16 SET_BLOCKLEN		

#define RD_SINGLE_BLOCK 0x11
#define CMD17 RD_SINGLE_BLOCK 

#define RD_MULT_BLOCK   0x12 
#define CMD18 RD_MULT_BLOCK

#define WR_SINGLE_BLOCK 0x18 
#define CMD24 WR_SINGLE_BLOCK 
                             
#define WR_MULT_BLOCK   0x19 
#define CMD25 WR_MULT_BLOCK   

//APP_COMMAND Defines: Note must be preceded with APP_CMD (CMD55)
#define APP_CMD         0x37 //cmd55

#define SD_SEND_OP_COND 0x29 
#define ACMD41 SD_SEND_OP_COND 

static uint64_t sdc_cmd;
int sdc_initialize(uint8_t cs, spi_inst_t* spi);

int send_cmd(uint8_t idx, uint32_t arg);

static uint8_t recv_r1(); //wait for r1 response ; only needed for follow up on cmd's 
static uint8_t recv_data_token_blocking();

int recv_cmd_blocking(uint8_t* buf, size_t size); //receive response from cmd
int recv_data_blocking(uint8_t* buf); //receive 512 by data block (1)

int sdc_CS_LO();
int sdc_CS_HI();

#endif //SDC_H
