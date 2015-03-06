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
 *	an everlasting loop that will take in a character and 
 *	handle it according to the available options.
 */
int dpu_start(){
    unsigned char memory[MEM_SIZE];
    unsigned char choice[BUFF_SIZE];
    unsigned char flush[BUFF_SIZE];
    unsigned int offset, length, i;
    int bytes;

    // Reset registers
    dpu_reset();
    
    // Print title and command list
    printf("    |=-=-=-=-=-=-=-=-=--->>DPU<<---=-=-=-=-=-=-=-=-=|\n"); 
    dpu_help();
    
    // Loop forever
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
                dpu_fetch(memory);
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

        // Print the offset/block number
        printf("%4.4X\t", offset);
        // Create the line
        for(i = 0; i < LINE_LENGTH; i++, offset++, count++){
            // Ensure that the pointer does not go out of bounds
            if(offset == MEM_SIZE){
                lineLength = i;
                break;
            }
            // Ensure that the line length does not exceed desired length
            if(count == length){
                lineLength = i;
                break;
            }      
            line[i] = *((char*)memptr + offset);
            printf("%2.2X ", line[i]);
        }
        // Move to a newline and continue to ASCII representation
        printf("\n\t");
        for(i = 0; i < lineLength; i++){
            if(isprint(line[i])){
                printf(" %c ", line[i]);
            }else{
                printf(" . ");
            }    
        }
        // Newline for next block
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
 *  Memory Modify:  Modify bytes of memory, in hex, beginning at offset.
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
 * Register dump:   Display all registers and flags with their current values.
 */
int dpu_reg(){
    unsigned short ir0, ir1;
    unsigned int i;
    unsigned int linebreak = 6;

    /* Print regsiter file */
    for(i = 0; i < RF_SIZE; i ++){
        if(i % linebreak == 0){
            printf("\n");
        }
        if(i == REG_SP){
            printf(" SP:%8.8X ", regfile[REG_SP]);
        }else if(i == REG_LR){
            printf(" LR:%8.8X ",  regfile[REG_LR]);
        }else if(i == REG_PC){
            printf(" PC:%8.8X ",  regfile[REG_PC]);
        }else{    
            printf("r%2.2d:%8.8X ", i,  regfile[i]);
        }    
    }    
    
    /* Print flags */
    printf("\t SZC:%c%c%c", flag_sign, flag_zero, flag_carry);
    
    /* Parse IR0 and IR1 */
    ir0 = getir0(ir);
    ir1 = getir1(ir);

    /* Print non-visible registers */
    printf("\n   MAR:%8.8X   MBR:%8.8X   IR0:%4.4X   IR1:%4.4X   Stop:%c   IR Flag:%c\n",  mar,  mbr, ir0, ir1, flag_stop, flag_ir);

    return 0;
}

/**
 * Trace:   Single step through instruction after instruction, beginning with 
 *          the program counter's position.  Register values will be displayed 
 *          after each trace.
 */
int dpu_trace(){
    return 0;
}

/**
 *	Function to write bytes from memory to a file.
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
 *  Reset all registers to 0.
 */
int dpu_reset(){
    unsigned int i;
    
    // Reset visible registers
    for(i = 0; i < RF_SIZE; i++){
        regfile[i] = 0;
    }
    // Reset flags
    flag_sign = LOW;
    flag_zero = LOW;
    flag_carry = LOW;
    flag_stop = LOW;
    flag_ir = LOW;
    // Non-visible registers
    mar = 0;
    mbr = 0;
    ir = 0;
    
    return 0;
}

/**
 *	Function to print DPU options to the user
 *	in the form of a menu.
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
    unsigned int i, cycles;
   
    /**  
     *  Amount of cycles needed for bitshifting bytes
     *  into a 32 bit register from 8 bit memory
     */
    cycles = REG_SIZE / BYTE_SIZE;  

    /* MAR <- PC */
    mar = regfile[REG_PC];
    
    /* MBR <- memory[MAR] */
    for(i = 0; i < cycles; i++){
        mbr = mbr << 8;
        /* Add memory at mar to mbr */
        mbr += *((unsigned char*)memory + (mar + i));
    }    

    /* IR <- MBR */
    ir = mbr;

    /* PC <- PC + 1 instruction */
    regfile[REG_PC] += REG_SIZE;
}

/* Process to parse the first IR register, IR0 */
unsigned short getir0(unsigned long ir){
    return ir >> 16;
}    

/* Process to parse the second IR register, IR1 */
unsigned short getir1(unsigned long ir){
    return ir & 0x0000FFFF;
}

