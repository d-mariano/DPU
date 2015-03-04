/**********************************************
 *  Author:     Dave Mariano
 *  Date:       January 30, 2015
 *  Filename:   dpu.h
 *  
 *  dpu.c function prototypes
 **********************************************/

/*  Sizes */
#define MEM_SIZE        0x4000
#define BUFF_SIZE       0x200
#define CHOICE_SIZE     0x3
#define BYTE_SIZE       ((int)sizeof(char))
#define LINE_LENGTH     0x10
#define REG_SIZE_BITS   0x20
#define REG_SIZE        0x4
#define RF_SIZE         0x10

/* Special Registers in Register File Offsets */
#define REG_SP  0xD
#define REG_LR  0xE
#define REG_PC  0xF

/* Flag settings */
#define HIGH    '1'
#define LOW     '0'

/* Forever loop */
#define forever for(;;)

/* Prototypes */
int dpu_start();

int dpu_go();

int dpu_dump(void * memptr, unsigned int offset, unsigned int length);

int dpu_LoadFile(void * memory, unsigned int max);

int dpu_modify(void * memptr, unsigned int offset);

int dpu_quit();

int dpu_reg();

int dpu_trace();

void dpu_WriteFile(void * memory);

int dpu_reset();

void dpu_help();

void dpu_fetch(void * memory);
