/*
 Pass 1 of SIC Assembler
 Builds and displays Symbol Table
 Creates an intermediate file for Pass 2
 Checks for invalid labels, invalid mnenomics,  
 invalid operands for directives, missing START or END
*/

#include "assemble.h"

/* Dunno, compiler really wanted this here */
bool validLabel(char*);

/* ------------------------------------Global Variables---------------------------------- */
const int MAX_FILE_SIZE = 32768;
int LOCCTR = 0;
int STARTING_ADDR = 0;
unsigned int FILE_SIZE = 0;
char PROG_NAME[8];
char err_msg[80];
bool err = false;
bool hasErr = false;
bool endFound = false;
FILE *inter;

/* ----------------------------------Pass 1 of Assembler---------------------------------- */
void pass1(FILE *sourceFile, char *filename)
{
    int i = 0,
        tmp = 0;
    char line[256],
         *fields[3];

    inter = fopen(filename, "w");

    printf("Printing Symbol Table\n");

    fgets(line, 256, sourceFile);
    while (!feof(sourceFile))
    {
        fprintf(inter, "%s", line);

        /* Prints out comment line and loops again */
        if (line[0] == '.')
        {
            fprintf(inter, "\n\n\n\n");
            fgets(line, 256, sourceFile);
            continue;
        }

        /* Its a line with code */
        else
        {
            getTokens(line, fields);
            fprintf(inter, "%X\n", LOCCTR);
            
            /* Has something on the label field */
            if (!isspace(line[0]))
            {
                /* Check for START Directive */
                if (strcmp(fields[1], "START") == 0)
                {
                    /* Checks operand field for start address */
                    if (fields[2][0] == '\0')
                    {
                        printf("\nWarning: No address specified, zero assumed\n");
                    }
                    else
                    {
		        STARTING_ADDR = (int) strtol(fields[2], NULL, 16);
		        LOCCTR = STARTING_ADDR;
                        strcpy(PROG_NAME, fields[0]);
                    }
		    
                    /* Replace the zero with the starting address */
                    tmp = ftell(inter);
                    fseek(inter, tmp-2, 0); 
                    fprintf(inter, "%X\n", STARTING_ADDR);

                    /* Prints out newline for opcode field 
                       and starting address */
                    fprintf(inter, "s\n%X\n", STARTING_ADDR);
                }

                /* Checks for END Directive */
                else if (strcmp(fields[1], "END") == 0)
                {
                    endFound = true;
                    strcat(err_msg, "$");

                    /* Checks operand field for label/symbol */
                    if (fields[2][0] == '\0')
                    { 
                        err = true;
                        strcat(err_msg, "1");
                    }

                    FILE_SIZE = LOCCTR - STARTING_ADDR;

                    /* Determines if file too large */
                    if (MAX_FILE_SIZE - FILE_SIZE < 0)
                    {
                        err = true;
                        strcat(err_msg, "2");
                    }

                    fprintf(inter, "\n%s\n", fields[2]);
                    goto Le_End;
                }

                /* Handles label/symbol */
                else
                {
                    /* If valid label, add to symbol table */
                    if (validLabel(fields[0]))
                    {
                        ht_put(fields[0], LOCCTR); 
                        printf("%s-%X\n", fields[0], ht_get(fields[0]));
                    }
                    
                    /* Handles mnenomic */
                    tmp = handleInstruction(fields[1], fields[2]);
                    
                    /* Writes Op-code value of mnenomic */
                    if (tmp != 1)
                        fprintf(inter, "%02X\n", tmp);
                    /* Writes nothing for invalid mnenomic */
                    else 
                        fprintf(inter, "\n");

                    /* Writes out operand */
                    fprintf(inter, "%s\n", fields[2]);
                }
            }

            /* Nothing on the label field */
            else
            {
                /* Checks for END Directive */
                if (strcmp(fields[0], "END") == 0)
                {
                    endFound = true;
                    strcat(err_msg, "$");

                    /* Checks operand field for label/symbol */
                    if (fields[1][0] == '\0')
                    { 
                        err = true;
                        strcat(err_msg, "1");
                    }

                    FILE_SIZE = LOCCTR - STARTING_ADDR;

                    /* Determines if file too large */
                    if (MAX_FILE_SIZE - FILE_SIZE < 0)
                    {
                        err = true;
                        strcat(err_msg, "2");
                    }

                    fprintf(inter, "\n%s\n", fields[1]);
                    goto Le_End;
                }
                  
                /* Handles mnenomic */  
                tmp = handleInstruction(fields[0], fields[1]);


                /* Writes Op-code value of mnenomic */
                if (tmp != 1)
                    fprintf(inter, "%02X\n", tmp); 

                /* Writes nothing for invalid mnenomic */
                else 
                    fprintf(inter, "\n");

                 
                /* Writes out operand */
                fprintf(inter, "%s\n", fields[1]);
                }
        }
        
        Le_End:
        if (err)
            fprintf(inter, "%s\n", err_msg);
        else 
            fprintf(inter, "\n");

        if (endFound)
            break;

        err_msg[0] = '\0';
        err = false;
        free(*fields);
        fgets(line, 256, sourceFile);
    }
    
	
    fclose(inter);
}


/* -----------------------------Supporting Functions for Pass1----------------------------- */
/* Determines if label is valid */
bool validLabel(char *label)
{
    strcat(err_msg, "@");
    int i;
    bool valid = true;
    
    /* Check if label starts with a letter */
    if (!isalpha(label[0]))
    {
        strcat(err_msg, "1");
        hasErr = true;
        err = true;
        valid = false;
    }

    /* Checks if label only has alphanumeric characters */
    for (i = 0; i < strlen(label); i++)
    {
        if (!isalnum(label[i]))
        {
            strcat(err_msg, "2");
            valid = false;
            hasErr = true;
            err = true;
            break;
        }
    }

    /* Checks if label too long */
    if (strlen(label) > 6)
    {
        strcat(err_msg, "3");
        err = true;
        hasErr = true;
        valid = false;
    }

    /* Checks if label is already in symbol table */
    if (duplicateKey(label))
    {
        strcat(err_msg, "4");
        err = true;
        hasErr = true;
        valid = false;
    }

    return valid;
}

/* Increments Location Counter */
int handleInstruction(char *instruction, char *operand)
{
    int opcode,
    operandValue = 0;

    strcat(err_msg, "%");


    /* Increments location counter 3 bytes */		
    if (strcmp(instruction, "WORD") == 0)
    {
        LOCCTR += 3;
        return 2;
    }

    /* Increments location counter according to operand */
    else if (strcmp(instruction, "RESB") == 0)
    {
        operandValue = atoi(operand);
        LOCCTR += operandValue;
        return 3;
    }

    /* Increments location counter according to operand times three */
    else if (strcmp(instruction, "RESW") == 0)
    {
        operandValue = atoi(operand) * 3;
        LOCCTR += operandValue;
        return 3;
    } 
 
    /* Increments location counter according to operand size */
    else if (strcmp(instruction, "BYTE") == 0)
    {
        int i = 0,
        length = strlen(operand);


        /* Handles hexadecimal values */
        if (operand[0] == 'X')
        {
            strcat(err_msg, "~");


            /* Checks if starts and ends with single quotes */
            if (operand[1] != '\'' || operand[length-1] != '\'')
            {
                strcat(err_msg, "2");
                err = true;
                hasErr = true;
                return 1;
            }

            /* Checks if longer than 32 hexadecimal digits */
            else if ((length - 3) > 32)
            {
                strcat(err_msg, "4");
                err = true;
                hasErr = true;
                return 1;
            }

            /* Checks if even number of hexadecimal digits */
            else if ((length - 3) % 2 != 0)
            {
                strcat(err_msg, "5");
                err = true;
                hasErr = true;
                return 1;
            }

            /* Removes single quotes and X from string length */
            else
            {
                i = length - 3;
                i /= 2;
            }
        }

        /* Handles ASCII characters */
        else if (operand[0] == 'C')
        {
            strcat(err_msg, "~");

            /* Checks if starts and ends with single quotes */
            if (operand[1] != '\'' || operand[length-1] != '\'')
            {
                strcat(err_msg, "2");
                err = true;
                hasErr = true;
                return 1;
            }

            /* Checks if longer than 30 characters */
            else if ((length - 3) > 30)
            {
                strcat(err_msg, "3");
                err = true;
                hasErr = true;
                return 1;
            }

            /* Removes single quotes and C from string length */
            else
            {
                i = length - 3;
            }
        }

        /* Invalid use of BYTE Directive */
        else
        {
            strcat(err_msg, "2");
            err = true;
            hasErr = true;
            i = 0;
            return 1;
        }

        /* Increments location counter by length of operand */
        LOCCTR += i;
        return 2;
    }

    else
    {
        /* Gets Op-code value of mnenomic */
        opcode = getOpcode(instruction);
      
        /* Increments location counter by 3 bytes */
        if (opcode != 1)
        {
            LOCCTR += 3;
            return opcode;
        }
        /* If 1, invalid mnenomic */
        else
        {
            strcat(err_msg, "1");
            err = true;
            hasErr = true;
            LOCCTR += 0;
        }
    }

    return 1;
}

/* Returns if there were errors during Pass 1 */
bool hasErrs()
{
    return hasErr;
}

/* Returns if the END directive was found */
bool foundEnd()
{
    return endFound;
}

/* Returns START information */
unsigned int headerInfo(char *name)
{
    int i;

    for (i = 0; i < strlen(PROG_NAME); ++i)
        name[i] = PROG_NAME[i];

    return FILE_SIZE;
}
