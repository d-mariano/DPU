/**********************************************
 *  Author:     Dave Mariano
 *  Date:       January 30, 2015
 *  Filename:   dpu.h
 *  
 *  dpu.c function prototypes
 **********************************************/

#define MEM_SIZE        (16 * 1024)
#define BUFF_SIZE       512
#define CHOICE_SIZE     3
#define BYTE_SIZE       ((int)sizeof(char))
#define LINE_LENGTH     0x10
#define INPUT_SIZE      4
#define ZERO            48
#define NINE            57
#define forever         for(;;)

int dpu_start();

int dpu_go();

int dpu_dump(void * memptr, unsigned int offset, unsigned int length);

int dpu_LoadFile(void * memory, unsigned int max);

int dpu_modify(void * memory, unsigned int offset);

int dpu_quit();

int dpu_reg();

int dpu_trace();

void dpu_WriteFile(void * memory);

int dpu_reset();

void dpu_help();
