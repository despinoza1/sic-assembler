#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include "../main.h"
#include "../cli/cli.h"

/* ---------------------------------------OPTAB--------------------------------------------- */
void initOPTAB();
unsigned int getOpcode(char*);


/* ---------------------------------------SYMTAB-------------------------------------------- */
void initSYMTAB();
static unsigned int ht_hash(const char*);
void ht_put(char*, int);
unsigned int ht_get(char*);
bool duplicateKey(char*);

 
/* ---------------------------------------Pass 1------------------------------------------- */
void pass1(FILE*, char*);
bool validLabel(char*);
int handleInstruction(char*, char*);
bool hasErrs();
bool foundEnd();
unsigned int headerInfo(char*);


/* ---------------------------------------Pass 2-------------------------------------------- */
void pass2(char*);
void getMachineCode(char*, char*, char*);
void writeErrors(char *, char *);

#endif
