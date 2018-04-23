/*
 Debugger for an SIC program 
 Allows for stepping through one instruction at a time
 Display content of all the Registers
 Change the content of the 
  -Index Register, Linkage Register, and the Accumulator
 As well change the content of any memory location
*/

#define REG_A regs[0]
#define REG_X regs[1]
#define REG_L regs[1]

#include "debug.h"
#include "cli.h"
#include "../sicengine/sicengine.h"


void RunDebug(unsigned long address)
{
    char instruction[20];
    char cmd[20];
    WORD regs[6];

    printf("\n\t\tSIC Debugger\n\n");

    while (true)
    {
        printf(COLOR_GREEN "$ " COLOR_RESET);
        fgets(cmd, 20, stdin);

        if (cmd[strlen(cmd)-1] == '\n')
            cmd[strlen(cmd)-1] = '\0';

        if (strlen(cmd) > 1)
        {
            printf(COLOR_RED "\nUnrecognized command\n\n" COLOR_RESET);
            continue;
        }

        switch(toupper(cmd[0]))
        {
            /* Displays Help */
            case 'H': printf(COLOR_CYAN "\n\t\tDebugger Help\n" COLOR_RESET);
            printf(COLOR_GREEN "\ns\t -Step to next instruction"
                   "\nr\t -Display the contents of registers"
                   "\ne\t -Enter edit mode"
                   "\n\t  Allows change of the content of registers and memory"
                   "\n\t  Use h while in edit mode for help"
                   "\nh\t -This message"
                   "\nc\t -Quit debugger and continue execution"
                   "\nq\t -Quit debugger\n\n" COLOR_RESET);
            break;

            /* Step through onto the next instruction */
            case 'S': printf("\nExecuting instruction at %04X \n", (unsigned int)address); 
            SICRun(&address, 1);
            printf("\n");
            break;

            /* Display Registers */
            case 'R': GetReg(&regs);
            GetIR(address, instruction);
            printf("\nRegisters: A:%02X X:%02X L:%02X PC:%04X SW:%c\n"
                   "Instruction: %s\n\n", 
                   *REG_A, *REG_X, *REG_L, GetPC(), GetCC(), instruction);
            break;

            /* Edit Content */
            case 'E': edit();
            break;

            /* Quit and continue execution */
            case 'C': printf("\nProgram will continue to execute\n\n");
            SICRun(&address, 0);
            return;

            /* Quit */
            case 'Q': PutPC(address); 
            return;

            default: printf(COLOR_RED "Unrecognized Command\n" COLOR_RESET);
            break;
        }
    }
}

/* Edit mode, allows the editting of registers
   and memory content */
void edit()
{
    char cmd[30];
    WORD regs[6];
    int i, j;

    printf("\t\tEdit Mode\n");

    while(true)
    {
        printf(COLOR_CYAN "# " COLOR_RESET);
        fgets(cmd, 30, stdin);

        switch(toupper(cmd[0]))
        {
            /* Help for Edit Mode */
            case 'H': printf(COLOR_CYAN "\n\t\tEditor Help\n" COLOR_RESET);
                printf(COLOR_GREEN
                "\nTo edit a register:"  
                "\na\t -Accumulator Register"
                "\nx\t -Index Register"
                "\nl\t -Linkage Register"
                "\n\t  Followed by it new content in hexadecimal"
                "\n\nTo edit a memory location:"
                "\nb\t -Edit a byte at memory location"
                "\nw\t -Edit a word at memory location"
                "\n\t  Followed by the memory location and new content in hexadecimal"
                "\n\nTo display the content of registers and memory:"
                "\nr\t -Display Registers"
                "\nd\t -Dump memory from START to END"
                "\nh\t -This message"
                "\nq\t -Quit Editor\n\n" COLOR_RESET);
            break;

            /* Edit Registers */
            case 'A': editReg(cmd, 1);
            break;
            case 'X': editReg(cmd, 2);
            break;
            case 'L': editReg(cmd, 3);
            break;

            /* Edit memory location */
            case 'B': editMem(cmd, 1);
            break;
            case 'W': editMem(cmd, 2);
            break;

            /* Display Registers */
            case 'R': GetReg(&regs);
            printf("\nEditable Registers: A:%02X X:%02X L:%02X\n\n", 
                   *REG_A, *REG_X, *REG_L);
            break;

            /* Dump Memory */
            case 'D': 
            if (cmd[strlen(cmd)-1] == '\n')
                cmd[strlen(cmd)-1] = '\0';
            /* Cheap fix number 3, the ordering of everything
               is on point */
            for (i = 1; isspace(cmd[i]); ++i);    // Set i to start of first operand
            for (j = i+1; !isspace(cmd[j]); ++j); // Set the end of first operand to NULL Char
            cmd[j] = '\0';
            for (j = j+1; isspace(cmd[j]); ++j);  // Set j to start of second operand
            dump(cmd+i, cmd+j);
            break;

            /* Quit Edit Mode */
            case 'Q': printf("Exiting edit mode\n\n");
            return;

            default: printf(COLOR_RED "\nUnrecognized command\n" COLOR_RESET);
            break;
        }

        cmd[0] = '\0';
    }
}

/* Edits the A, X or L Register */
void editReg(const char line[], int mode)
{
    WORD regs[6],
         newReg;
    unsigned long temp;
    int i;

    GetReg(&regs);
    
    /* My Tokenizer function acts weird in this file
       so here's a very cheap fix */
    for (i = 1; !isspace(line[i]); ++i);

    temp = strtol(line+i, NULL, 16);

    switch(mode)
    {
        /* Edit Accumulator Register */
        case 1: *REG_A = temp;
        PutReg(regs);
        break;

        /* Edit Index Register */
        case 2: *REG_X = temp;
        PutReg(regs);
        break;

        /* Edit Linkage Register */
        case 3: *REG_L = temp;
        PutReg(regs);
        break;

        default: printf("\nDunno how this happened\n");
        break;
    }
}

/* Edit a memory location */
void editMem(const char *line, int mode)
{
    ADDRESS location;
    int i = 0;
    char *operand1,
         *operand2;
    unsigned int temp = 0;
    BYTE byte = 0;
    BYTE *word;

    /* Cheap fix number 2, it gets worse */
    for (i = 1; !isspace(line[i]); ++i);
    operand1 = line+i;

    location = strtol(operand1, &operand2, 16);
    
    for (i = 0; !isspace(operand2[i]); ++i);
    temp = strtol(operand2+i, NULL, 16);


    switch(mode)
    {
        /* Edit a byte of memory */
        case 1: byte = (unsigned char)temp;
        PutMem(location, &byte,0);
        break;
   
        /* Edit a word of memory */
        case 2: word = (unsigned char *) &temp;
        //PutMem(location, word, 1);  Left for historic purposes
        /* Almost forgot about edianness */
        PutMem(location, &word[2], 0);
        PutMem(location+1, &word[1], 0);
        PutMem(location+2, &word[0], 0);
        break;

        default: printf("\nI'm a non-profit student plz donate to fix.\n");
        break;
    }
}
