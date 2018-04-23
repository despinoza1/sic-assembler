/*
 Opcode table (OPTAB) for the 
 25 SIC mnenomics and their machine code equivalent
*/

#include "assemble.h"

/* Structure for SIC instructions */
typedef struct
{
   unsigned int opcode;
   char *mnenomic;
} opcode_t;

/* Op Table */
opcode_t OPTAB[25];

void initOPTAB()
{
    int i;
    for (i = 0; i < 25; ++i)
        OPTAB[i].mnenomic = malloc(5);

    OPTAB[0].opcode = 24;
    OPTAB[0].mnenomic = "ADD";
    OPTAB[1].opcode = 88;
    OPTAB[1].mnenomic = "AND"; 
    OPTAB[2].opcode = 40;
    OPTAB[2].mnenomic = "COMP";
    OPTAB[3].opcode = 36;
    OPTAB[3].mnenomic = "DIV";
    OPTAB[4].opcode = 60;
    OPTAB[4].mnenomic = "J";
    OPTAB[5].opcode = 48;
    OPTAB[5].mnenomic = "JEQ";
    OPTAB[6].opcode = 52;
    OPTAB[6].mnenomic = "JGT";
    OPTAB[7].opcode = 56;
    OPTAB[7].mnenomic = "JLT";
    OPTAB[8].opcode = 72;
    OPTAB[8].mnenomic = "JSUB";
    OPTAB[9].opcode = 0;
    OPTAB[9].mnenomic = "LDA";
    OPTAB[10].opcode = 80;
    OPTAB[10].mnenomic = "LDCH";
    OPTAB[11].opcode = 8;
    OPTAB[11].mnenomic = "LDL";
    OPTAB[12].opcode = 4;
    OPTAB[12].mnenomic = "LDX";
    OPTAB[13].opcode = 32;
    OPTAB[13].mnenomic = "MUL";
    OPTAB[14].opcode = 68;
    OPTAB[14].mnenomic = "OR";
    OPTAB[15].opcode = 216;
    OPTAB[15].mnenomic = "RD";
    OPTAB[16].opcode = 76;
    OPTAB[16].mnenomic = "RSUB";
    OPTAB[17].opcode = 12;
    OPTAB[17].mnenomic = "STA";
    OPTAB[18].opcode = 84;
    OPTAB[18].mnenomic = "STCH";
    OPTAB[19].opcode = 20;
    OPTAB[19].mnenomic = "STL";
    OPTAB[20].opcode = 16;
    OPTAB[20].mnenomic = "STX";
    OPTAB[21].opcode = 28;
    OPTAB[21].mnenomic = "SUB";
    OPTAB[22].opcode = 224;
    OPTAB[22].mnenomic = "TD";
    OPTAB[23].opcode = 44;
    OPTAB[23].mnenomic = "TIX";
    OPTAB[24].opcode = 220;
    OPTAB[24].mnenomic = "WD";
}

/* Returns Op-code for mnenomic or 1 if failed */
unsigned int getOpcode(char *key)
{
    int first = 0,
        last = 24,
        mid;
    char *mnenomic = malloc(6);

    while (first <= last)
    {
        mid = (first + last) / 2;
        strcpy(mnenomic, OPTAB[mid].mnenomic);
        if (strncmp(mnenomic, key, strlen(mnenomic)) == 0)
            return OPTAB[mid].opcode;
        else if (strncmp(mnenomic, key, strlen(mnenomic)) > 0)
            last = mid - 1;
        else
            first = mid + 1;
    }

    return 1;
}
