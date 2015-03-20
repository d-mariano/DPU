/*********************************************************
 *  Author:     Dave Mariano
 *  Date:       January 30, 2015
 *  Filename:   dpu.c
 *      
 *  Functions that make up the DPU - Dave Processing Unit
 *
 *********************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dpu.h"

/**
 *	DPU startup function that initializes memory and provides 
 *	an everlasting loop.  An input character  is taken and 
 *	handled according to the available options.
 */
int dpu_start(){
    unsigned char memory[MEM_SIZE];
    unsigned char choice[BUFF_SIZE];
    unsigned char flush[BUFF_SIZE];
    unsigned int offset, length, i;
    int bytes;

    /* Reset registers */
    dpu_reset();
    
    /*  Print title and command list */
    printf("    |=-=-=-=-=-=-=-=-=--->>DPU<<---=-=-=-=-=-=-=-=-=|\n"); 
    dpu_help();
    
    /*  Loop forever */
    forever{
        // Prompt
        printf("> ");

        // Obtain a choice from the user/stdin
        fgets(choice, BUFF_SIZE, stdin);
        
        // Ensure the choice is lower-case
        for(i = 0; i < strlen(choice); i++){
            choice[i] = tolower(choice[i]);    
        }    
        
        // Switch to execture correct function 
        switch(choice[0]){
            case 'd':
                printf("Enter offset in hex:\t");
                // Test for a valid intake
                if(scanf("%x", &offset) == 0){
                    printf("Not a valid offset.\n");
                    break;
                }    
                // Test for an offset in range
                if(offset >= MEM_SIZE){
                    printf("Not a valid offset.\n");
                    // Flush input buffer
                    fgets(flush, BUFF_SIZE, stdin);
                    break;
                }
                fgets(flush,BUFF_SIZE, stdin);
                printf("Enter length in hex:\t");
                if(scanf("%x", &length) == 0){
                    printf("Not a valid length.\n");
                    break;
                }    
                // Flush input 
                fgets(flush, BUFF_SIZE, stdin);
                
                dpu_dump(memory, offset, length);
                break;
            case 'g':
                /*
                 * while(!flag_stop){
                 *     dpu_instCycle(memory);
                 * }
                 */

                printf("\"go\" not yet implemented.\n");
                break;
            case 'l':
                bytes = dpu_LoadFile(memory, MEM_SIZE);
                if(bytes >= 0){
                    printf("0x%x(%d) bytes have been loaded into memory from file.\n", (unsigned int)bytes, (unsigned int)bytes);        
                }    
                break;
            case 'm':
                printf("Enter offset in hex:\t");
                // Ensure proper value is taken in
                if(scanf("%x", &offset) == 0){
                    printf("Not a valid offset.\n");
                    break;
                }    
                // Flush input
                fgets(flush, BUFF_SIZE, stdin);
                // Test for an offset in range
                if(offset >= MEM_SIZE){
                    printf("Not a valid offset.\n");
                    break;
                }    
                       
                dpu_modify(memory, offset);
                break;
            case 'q':
                printf("Goodbye.\n");
                return dpu_quit();
            case 'r':
                dpu_reg();
                break;
            case 't':
                dpu_instCycle(memory);  
                dpu_reg();
                break;
            case 'w':
                dpu_WriteFile(memory);
                break;
            case 'z':
                dpu_reset();
                printf("Registers have been reset.\n");
                break;
            // If the user selects '?' then the case will fall into 'h'
            case '?':
            case 'h':
                dpu_help();
                break;
            default:
                printf("%c is not an option.  Enter H/h/? for help.\n", choice[0]);
        }
    }    
}

int dpu_go(){
    return 0;
}


/**
 *  Memory Dump:  Dump length amount of memory, beginning  at offset
 */
int dpu_dump(void * memptr, unsigned int offset, unsigned int length){
    unsigned int i;
    unsigned char line[LINE_LENGTH];
    unsigned int lineLength = LINE_LENGTH;
    unsigned int count = 0;

    while(count < length){
    
        if(offset == MEM_SIZE){
            break;
        } 

        /* Print the offset/block number */
        printf("%4.4X\t", offset);
        /* Create the line */
        for(i = 0; i < LINE_LENGTH; i++, offset++, count++){
            /* Ensure that the pointer does not go out of bounds */
            if(offset == MEM_SIZE){
                lineLength = i;
                break;
            }
            /* Ensure that the line length does not exceed desired length */
            if(count == length){
                lineLength = i;
                break;
            }      
            line[i] = *((char*)memptr + offset);
            printf("%2.2X ", line[i]);
        }
        /* Move to a newline and continue to ASCII representation of the 
         * line dumped.
         */   
        printf("\n\t");
        for(i = 0; i < lineLength; i++){
            if(isprint(line[i])){
                printf(" %c ", line[i]);
            }else{
                printf(" . ");
            }    
        }
        /* Newline for next block */
        printf("\n");
    }

    return 0;
}


/**
 *	Function to load data from a file into memory.
 */
int dpu_LoadFile(void * memory, unsigned int max){
    FILE* file;
    int nbytes;
    unsigned char buff[BUFF_SIZE];
    unsigned char filename[BUFF_SIZE];
    unsigned char error[BUFF_SIZE];
    unsigned char flush[BUFF_SIZE];

    // Prompt for filename
    printf("\nEnter a filename: ");
    
    // Obtain filename
    fgets(filename, BUFF_SIZE, stdin);
    
    // Nullify the last byte
    filename[strlen(filename)-1] = '\0';

    // Open the file
    if((file = fopen(filename, "rb")) == NULL){
        sprintf(error, "load: %s", filename);
        perror(error);
        return -1;
    }

    printf("Loading %s to memory.\n", filename);
    // Set number of bytes loaded
    nbytes = 0;

    // Write file to memory
    while(fgets(buff, BUFF_SIZE, file)){
        // Check if memory has enough space for the buffer
        if(max - strlen(memory) >= strlen(buff)){
            strcat(memory, buff);
            nbytes += strlen(buff);
        }else{
            buff[(max - strlen(memory))] = '\0'; 
            // Load exact amount of bytes possible 
            strcat(memory,buff);
            nbytes += strlen(buff);
            // Anounce that the file has been truncated
            printf("Input from file has been truncated to fit into memory.\n");
            fclose(file);
            return nbytes;
        }
    }

    fclose(file);
    
    return nbytes;
}


/**
 *  Memory Modify:  
 *      Modify bytes of memory, in hex, beginning at offset.
 */
int dpu_modify(void * memptr, unsigned int offset){
    unsigned char input[BUFF_SIZE];
    unsigned char flush[BUFF_SIZE];
    unsigned int byte;
    unsigned int i;
    
    printf("*All byte values accepted in hex.*\nEnter '.' to stop.\n\n");

    forever{
        // Print current offset and value
        printf("%4.4X : %2.2X > ", offset, *((unsigned char*)memptr + offset));
        // Get input
        fgets(input, BUFF_SIZE, stdin);
        // Nullify newline element
        input[2] ='\0';

        // Check if input is quit
        if(input[0] == '.'){
            break;    
        }
        // Check if input is null
        if(strcmp(input, "") == 0){
            // Ignore invalid input and continue
            continue;
        }
        // Check if input is enter
        if(strcmp(input, "\n") == 0){
            // Ignore invalid input continue
            continue;
        }    

        // Ensure the string is lowercase
        for(i = 0; i < strlen(input); i++){
            input[i] = tolower(input[i]);
        }

        // Capture input as a hex-byte
        if(sscanf(input, "%x", &byte) == 0){
            continue;
        }

        // Assign offset with new byte value
        *((unsigned char*)memptr + offset) = byte;
        // Increment offset to next byte
        ++offset;
        
        if(offset == MEM_SIZE){
            printf("End of memory.\n");
            break;
        }    
    }
    return 0;
}    

int dpu_quit(){
    return 0;    
}


/**
 * Register dump:   
 *      Display all registers and flags with their current values.
 */
int dpu_reg(){
    unsigned int i;

    /* Print regsiter file */
    for(i = 0; i < RF_SIZE; i ++){
        if(i % LINE_BREAK == 0){
            printf("\n");
        }
        if(i == RF_SP){
            printf(" SP:%8.8X ", SP);
        }else if(i == RF_LR){
            printf(" LR:%8.8X ",  LR);
        }else if(i == RF_PC){
            printf(" PC:%8.8X ",  PC);
        }else{    
            printf("r%2.2d:%8.8X ", i,  regfile[i]);
        }    
    }    
    
    /* Print flags */
    printf("\t SZC:%d%d%d", flag_sign, flag_zero, flag_carry);

    /* Print non-visible registers */
    printf("\n   MAR:%8.8X   MBR:%8.8X   IR0:%4.4X   IR1:%4.4X   Stop:%d   IR Flag:%d\n",  mar,  mbr, IR0, IR1, flag_stop, flag_ir);

    return 0;
}


/**
 *	Write to File:
 *	    Function to write bytes from memory to a file.
 */
void dpu_WriteFile(void * memory){
    FILE* file;
    int nbytes;
    int wbytes;
    unsigned char filename[BUFF_SIZE];
    unsigned char error[BUFF_SIZE];
    unsigned char flush[BUFF_SIZE];

    // Retrieve filename
    printf("\nEnter a filename: ");
    fgets(filename, BUFF_SIZE, stdin);
    
    // Nullify last byte
    filename[strlen(filename) -1] = '\0';

    // Retrieve number of bytes to write
    printf("\nEnter the amount of bytes to write: ");
    if(scanf("%d", &nbytes) == 0){
        printf("Invalid input.\n");
        return;
    }    
    // Flush input stream
    fgets(flush, BUFF_SIZE, stdin);
    
    /** Check if number of bytes specified is greater than memory, 
     *  less than 0, or 0.  
     *  In gcc, non-numbers equate to 0. 
     *  In c++ (Visual Studio), non-numbers are less than 0.
     */
    if(nbytes > MEM_SIZE){
        printf("File not written.  Cannot write more bytes than in memory.\n");
        return;
    
    }else if(nbytes <= 0){
        printf("0 bytes written.\n");
        return;
    }

    // Open the file
    if((file = fopen(filename, "wb")) == NULL){
        sprintf(error, "write: %s", filename);
        perror(error);
        return;
    }

    // Write memory to file
    if((wbytes = fwrite(memory, BYTE_SIZE, nbytes, file)) < 0){
        perror("dpu: write: ");
    }else{
        printf("%d bytes have been written to %s.\n", wbytes, filename);
    }
    
    // Close the file
    fclose(file);

    return;
}


/**
 *  Reset: Reset all registers to 0.
 */
int dpu_reset(){
    unsigned int i;
    
    // Reset visible registers
    for(i = 0; i < RF_SIZE; i++){
        regfile[i] = 0;
    }
    // Reset flags
    flag_sign = 0;
    flag_zero = 0;
    flag_carry = 0;
    flag_stop = 0;
    flag_ir = 0;
    // Non-visible registers
    mar = 0;
    mbr = 0;
    ir = 0;
    // Unofficial current instruction register
    cir = 0;
    
    return 0;
}


/**
 * Help:  Function to print DPU options to the user
 *	      in the form of a menu.
 */
void dpu_help(){
    printf("\td\tdump memory\n"
            "\tg\tgo - run the entire program\n"
            "\tl\tload a file into memory\n"
            "\tm\tmemory modify\n"
            "\tq\tquit\n"
            "\tr\tdisplay registers\n"
            "\tt\ttrace - execute one instruction\n"
            "\tw\twrite file\n"
            "\tz\treset all registers to zero\n"
            "\t?, h\tdisplay list of commands\n");
}


/**
 * Fetch:  Fetch an instruction from memory, at the address
 *         of the program counter.  Memory is 8 bits, and so
 *         4 bytes must be collected into the MBR, starting at the
 *         memory offset specified by MAR, as the Memory Buffer 
 *         Register is 32 bits.  This requires bit shifting 8 bits 
 *         to the left for each byte.  Doing this 4 times will equate 
 *         to 32 bits read.  The PC is incremented by the size of one
 *         register/instruction after the contents of MBR are stored in
 *         the Instruction Register.
 */         
void dpu_fetch(void * memory){
    unsigned int i;

    /* MAR <- PC */
    mar = PC;
    
    /* MBR <- memory[MAR] */
    for(i = 0; i < CYCLES; i++){
        mbr = mbr << BITS;
        /* Add memory at mar to mbr */
        mbr += *((unsigned char*)memory + (mar + i));
    }    

    /* IR <- MBR */
    ir = mbr;

    /* PC <- PC + 1 instruction */
    PC += REG_SIZE;
}


/**
 * Execute: Recognize instruction type, acknowledge instruction 
 *          fields, execute instruction based on the instruction fields.
 */
void dpu_execute(void *memory){
    /* Recognize instruction type: */
    
    /* 
     * Data Processing 
     */
    if(DATA_PROC){
        /* Acknowledge Operation field */
        if(DATA_AND){
            regfile[RD] &= RN;
        }else if(DATA_EOR){
            regfile[RD] ^= RN;
        }else if(DATA_SUB){
            regfile[RD] -= RN;
        }else if(DATA_SXB){
            regfile[RD] = (signed)RN;
        }else if(DATA_ADD){
            regfile[RD] += RN;
        }else if(DATA_ADC){
            regfile[RD] += RN + flag_carry; 
        }else if(DATA_LSR){
            regfile[RD] = regfile[RD] >> RN;
        }else if(DATA_LSL){
            regfile[RD] = regfile[RD] << RN;
        }else if(DATA_TST){
            if(regfile[RD] & RN){ 
                
            }    
        }else if(DATA_TEQ){
            if(regfile[RD] ^ RN){

            }
        }else if(DATA_CMP){
            if(regfile[RD] - CMP == 0){
                
            }
        }else if(DATA_ROR){
            
        }else if(DATA_ORR){
            regfile[RD] |= RN;
        }else if(DATA_MOV){
            regfile[RD] = RN;
        }else if(DATA_BIC){
            regfile[RD] &= !RN;
        }else if(DATA_MVN){
            regfile[RD] != RN;
        }
    }
    /* 
     * Load/Store 
     */
    else if(LOAD_STORE){
        /* Ackknoledge instruction fields */
        if(LOAD_BIT){
            /*Load Byte*/
            if(BYTE_BIT){
                regfile[RD] = *((unsigned char *)memory + RN);
            }
            /*Load Word*/
            else{
                /* Load first byte */
                regfile[RD] = *((unsigned char *)memory + RN);
                /* Shift bits to the left */
                regfile[RD] = regfile[RD] << BITS;
                /* Load second byte */
                regfile[RD] += *((unsigned char *)memory + (RN + 1));
            }
        }else{
            /*Store Byte*/
            if(BYTE_BIT){
                *((unsigned char *)memory + RN) = (unsigned char)regfile[RD];     
            }else{
                *((unsigned char *)memory + RN) = (unsigned char)(regfile[RD] >> BITS & 0xFF);
                *((unsigned char *)memory + (RN +1)) = (unsigned char)(regfile[RD] & 0xFF);
            }
        } 
    /* 
     * Immediate Operations 
     */
    }else if(IMMEDIATE){
        /* Move immediate value into regfile at RD */
        if(MOV){
            regfile[RD] = IMM_VALUE;    
            /* Check moved value is 0 */
            if(IMM_VALUE == 0 ){
                flag_zero = 1;
            }else{ 
                flag_zero = 0; 
            }
            /* Check if moved value is below zero */
            if(IMM_VALUE < 0){
                flag_sign = 1;
            }else{ 
                flag_sign = 0; 
            }
        }else if(CMP){
            /* Test if the values are equal */
            if((regfile[RD] - IMM_VALUE) == 0){
                flag_zero = 1;
            }else{
                flag_zero = 0;
            }
            /* Test if the value of register RD is less */
            if((regfile[RD] - IMM_VALUE) < 0){
                flag_sign = 1;
            }else{
                flag_sign = 0;
            }    
            /* Test if the value of register Rd is less*/
            if((regfile[RD] - IMM_VALUE) > 0){
                flag_carry = 1;
            }else{
                flag_carry =0;
            }    
        }else if(ADD){
            regfile[RD] += IMM_VALUE;
            /* Test if result after add is below 0 */
            if(regfile[RD] + IMM_VALUE < 0){
                flag_sign = 1;
            }else{
                flag_sign = 0;
            }
            /* Test if result after add is 0 */
            if(regfile[RD] + IMM_VALUE == 0){
                flag_zero = 1;
            }else{
                flag_zero = 0;
            }
            /* Test if result after add is greater than 32 bits */
            if(regfile[RD] + IMM_VALUE > 0xFFFFFFFF){
                regfile[RD] = IMM_VALUE - 0x100000000;
                flag_carry = 1;
            }else{
                flag_carry = 0;
                regfile[RD] += IMM_VALUE;
            }
        }else if(SUB){
            regfile[RD] -= IMM_VALUE;
        }    
    /* 
     * Conditonal Branch 
     */
    }else if(COND_BRANCH){
        /* Check condition codes */
        if(EQ){
            if(flag_zero){
                PC += (signed)COND_ADDR;
            }    
        }else if(NE){
            if(!flag_zero){
                PC += (signed)COND_ADDR;
            }
        }else if(CS){
            if(flag_carry){
                PC += (signed)COND_ADDR;
            }
        }else if(CC){
            if(!flag_carry){
                PC += (signed)COND_ADDR;
            }
        }else if(MI){
            if(flag_sign){
                PC += (signed)COND_ADDR;      
            }    
        }else if(PL){
            if(!flag_sign){
                PC += (signed)COND_ADDR;
            }
        }else if(HI){
            if(flag_carry && !flag_sign){
                PC += (signed)COND_ADDR;   
            }
        }else if(LS){
            if(!flag_carry && flag_sign){
                PC += (signed)COND_ADDR;
            }
        }else if(AL){
            PC += (signed)COND_ADDR;
        }
    /* 
     * Push / Pull 
     */
    }else if(PUSH_PULL){
               
    }
    /* 
     * Unonditional Branch 
     */
    else if(BRANCH){
        if(LINK_BIT){

        }    
        PC = OFFSET12;
    /* 
     * Stop 
     */
    }else if(STOP){
        flag_stop = 1;
    }    
    
    return;
}    


/**
 * Instruction Cycle:   
 *      A function cycle consists of a fetch, which provides two 
 *      instructions, and an execute for each of said instructions.  
 *      An IR flag is uses to determine the which instruction is to be
 *      executed from either 16-bit IR0 or IR1 of the 32-bit 
 *      Instruction Register.  If the flag is high, IR1 will be executed,
 *      then the flag will be set low.  If the flag is low, a fetch is made,
 *      providing IR0 and IR1 with the new instructions, and the flag is 
 *      then set high.  
 *
 */
void dpu_instCycle(void * memory){
    /* Determine which IR to use via IR Active flag */
    if(flag_ir == 0){
        /* Fetch new set of instructions */
        dpu_fetch(memory);
        /*   */
        cir = IR0;
        dpu_execute(memory);
        flag_ir = 1;
    }else{
        cir = IR1;
        dpu_execute(memory);
        flag_ir = 0;
    }     

    return;
}



