/**********************************************
 *  Author:     Dave Mariano
 *  Date:       January 30, 2015
 *  Filename:   dpu.h
 *  
 *  dpu.c function prototypes
 **********************************************/

#define MEM_SIZE    16000
#define BUFF_SIZE   512
#define CHOICE_SIZE 3
#define BYTE_SIZE   1
#define A           65
#define Z           90
#define forever     for(;;)

int dpu_start();

int dpu_go();

int dpu_dump();

int dpu_LoadFile(void * memory, unsigned int max);

int dpu_quit();

int dpu_reg();

int dpu_trace();

void dpu_WriteFile(void * memory);

int dpu_reset();

void dpu_help();
