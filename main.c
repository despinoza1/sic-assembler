/************************************************************
 * Daniel Espinoza                                          *
 *                                                          *
 *  Command Line Interpeter                                 *
 *  Two Pass Assembler                                      *
 *  Absolute Loader                                         *
 *  Debugger                                                *
 * for Simplified Instruction Computer (SIC)                * 
 * from Leland L. Beck's book:                              *
 * System Software - An Introduction to Systems Programming *
 *************************************************************/

#include "main.h"
#include "cli/cli.h"

int main()
{
    /* Initialze SIC machine */
    SICInit();

    /* Initialze command line interpeter commnands */
    initCMDS();

    /* Initialize Opcode Table and a table for symbols */
    initOPTAB();
    initSYMTAB();

    /* Runs command line interperter */
    cli();

    return 0;
}
