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
 *	Function to provide an everlasting loop that will take
 *	in a character and handle it according to the available
 *	options.
 **/
int dpu_start(){
    unsigned char memory[MEM_SIZE];
    unsigned char choice[CHOICE_SIZE];
    unsigned char flush[BYTE_SIZE];
    int bytes;

    // Print title and command list
    printf("    |=-=-=-=-=-=-=-=-=--->>DPU<<---=-=-=-=-=-=-=-=-=|\n"); 
    dpu_help();
    
    // Loop forever
    forever{
        // Prompt
        printf("> ");

        // Obtain a choice from the user/stdin
        fgets(choice, CHOICE_SIZE, stdin);
        
        // Flush input stream
        fgets(flush, strlen(flush), stdin);
        
        // Ensure the choice is lower-case
        tolower(choice);    
        
        // Switch to execture correct function 
        switch(choice[0]){
            case 'd':
                printf("\"dump memory\" not yet implemented.\n");
                break;
            case 'g':
                printf("\"go\" not yet implemented.\n");
                break;
            case 'l':
                bytes = dpu_LoadFile(memory, MEM_SIZE);
                if(bytes >= 0){
                    printf("0x%llx(%u) bytes have been loaded into memory from file.\n", bytes, bytes);        
                }    
                break;
            case 'm':
                printf("\"memory modify\" not yet implemented.\n");
                break;
            case 'q':
                printf("Goodbye.\n");
                return dpu_quit();
            case 'r':
                printf("\"display registers\" not yet implemented.\n");
                break;
            case 't':
                printf("\"trace\" not yet imlemeented.\n");
                break;
            case 'w':
                dpu_WriteFile(memory);
                break;
            case 'z':
                printf("\"reset registers\" not yet implemented.\n");
                break;
            // If the user selects '?' then the case will fall into 'h'
            case '?':
            case 'h':
                dpu_help();
                break;
            default:
                printf("%c is not an option.  Use H/h/? for help.\n", choice[0]);
        }

    }    
}

int dpu_go(){
    return 0;
}

int dpu_dump(){
    return 0;
}

/**
 *	Function to load data from a file into memory.
 **/
int dpu_LoadFile(void * memory, unsigned int max){
    FILE* file;
    unsigned int count;
    unsigned int nbytes;
    char buff[BUFF_SIZE];
    char filename[BUFF_SIZE];
    char error[BUFF_SIZE];
    char flush[CHOICE_SIZE];

    // Prompt for filename
    printf("\nEnter a filename: ");
    
    // Obtain filename
    fgets(filename, BUFF_SIZE, stdin);
    
    // Nullify the last byte
    filename[strlen(filename)-1] = '\0';

    // Open the file
    if((file = fopen(filename, "rb")) == NULL){
        sprintf(error, "write: %s", filename);
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

int dpu_quit(){
    return 0;    
}

int dpu_reg(){
    return 0;
}

int dpu_trace(){
    return 0;
}

/**
 *	Function to write bytes from memory to a file.
 **/
void dpu_WriteFile(void * memory){
    FILE* file;
    int nbytes;
    int wbytes;
    char filename[BUFF_SIZE];
    char error[BUFF_SIZE];
    char flush[CHOICE_SIZE];

    // Retrieve filename
    printf("\nEnter a filename: ");
    fgets(filename, BUFF_SIZE, stdin);
    
    // Nullify last byte
    filename[strlen(filename) -1] = '\0';

    // Retrienve number of bytes to write
    printf("\nEnter the amount of bytes to write: ");
    scanf("%d", &nbytes);
    
    // Flush input stream
    fgets(flush, CHOICE_SIZE, stdin);

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

    return;
}

int dpu_reset(){
    return 0;
}

/**
 *	Function to print DPU options to the user
 *	in the form of a menu.
 **/
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

