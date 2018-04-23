#ifndef CLI_H
#define CLI_H

#include "../main.h"
#include "../assembler/assemble.h"
#include "../sicengine/sicengine.h"


/* ------------------------------------Initialize commands-------------------------------- */
void initCMDS();


/* ------------------------------------c-string functions---------------------------------- */
bool not_ws(const char);
bool checkCmds(const char*, int *);
void getTokens(const char*, char**);


/* ------------------------------------Functions for CLI----------------------------------- */
int cli();
void load(char *);
void execute();
void debug();
void dump(char *, char *);
void help();
void assemble(char *);
void dir();

#endif
