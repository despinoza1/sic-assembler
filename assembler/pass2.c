/*
 Pass 2 of SIC Assembler
 Creates a listing file and object file if valid
 Checks for errors primarily in the operand field
*/

#include "assemble.h"

#define SOURCE interInfo[0]
#define LOCATION interInfo[1]
#define OPCODE interInfo[2]
#define OPERAND interInfo[3]
#define ERROR interInfo[4]

/* ------------------------------------Global Variables----------------------------------- */
FILE *inter = NULL;
FILE *objFile = NULL;
FILE *lstFile = NULL;
bool rmObj = false;
/* Used for creating Text Records */
bool startAddress = false;
bool doneText = false;
/* Used for Header Record */
char FILE_NAME[8];
unsigned int FILE_SIZE;

/* ------------------------------------Pass 2 of Assembler--------------------------------- */
void pass2(char *intermediate)
{
    int size = strlen(intermediate),
        index = 0,
        numOfInstructions = 1,
        textSize = 0;
    char temp;
    char *name = malloc(size);
    char *interInfo[5];
    char machineCode[34] = { '\0' };
    /* Variables for object file */
    char headerRecord[80],
         textRecord[120],
         endRecord[80];

     
    SOURCE = malloc(128);   // Stores source code line
    LOCATION = malloc(10);     // Stores current location
    OPCODE = malloc(4);     // Stores opcode value of mnenomic
    OPERAND = malloc(10);    // Stores operand
    ERROR = malloc(50);    // Stores any error messages
    
    /* Open intermediate file for reading */
    inter = fopen(intermediate, "r");

    /* Create listing file */
    strcpy(name, intermediate);
    name[size-3] = 'l';
    name[size-2] = 's';
    name[size-1] = 't';
    lstFile = fopen(name, "w");

    /* Create object file */
    name[size-3] = 'o';
    name[size-2] = 'b';
    name[size-1] = 'j';
    if (!hasErrs()) 
        objFile = fopen(name, "w");

   /* Prime Read */
   fgets(SOURCE, 128, inter);
   fgets(LOCATION, 10, inter);
   fgets(OPCODE, 4, inter);
   fgets(OPERAND, 10, inter);
   fgets(ERROR, 50, inter);
   while (!feof(inter))
   {
   /* --------------------------------Listing File------------------------------------- */
       /* Writes out Comments */
       if (SOURCE[0] == '.')
       {
           fprintf(lstFile, "\t\t%s", SOURCE);
       }
       /* Writes out LOCCTR - Machine Code - Source Line */
       else
       {
           getMachineCode(OPCODE, OPERAND, machineCode);

           /* Remove trailing newline character */
           index = strlen(LOCATION);
           if (LOCATION[index-1] == '\n')
               LOCATION[index-1] = '\0';

           /* Writes out line to listing file */
           if (ERROR[0] == '\n')
           {
               if (OPCODE[0] == '\n')
                   fprintf(lstFile, "%-6s\t%-6s\t%s", " ", machineCode, SOURCE);
               else
                   fprintf(lstFile, "%-6s\t%-6s\t%s", LOCATION, machineCode, SOURCE);

               /* Writes out blank line for no errors */
               fprintf(lstFile, "\n");
           }
           /* Write out errors */
           else
           { 
               fprintf(lstFile, "%-6s\t%-6s\t%s", LOCATION, machineCode, SOURCE);

               writeErrors(ERROR, machineCode);  
           }     
 

           /* --------------------------Object File------------------------------- */
           if (objFile != NULL)
           {
               /* Create Header Record */
               if (OPCODE[0] == 's')
               {
                   FILE_SIZE = headerInfo(FILE_NAME);
                   sprintf(headerRecord, "H%-6s%06X%06X",
                           FILE_NAME, (unsigned int)strtol(OPERAND, NULL, 16), FILE_SIZE);
                   fprintf(objFile, "%s\n", headerRecord);
               }
               /* Create End Record and close the file */
               else if (OPCODE[0] == '\n')
               {
                   index = strlen(OPERAND);
                   if (OPERAND[index-1] == '\n')
                       OPERAND[index-1] = '\0';

                   /* I reused FILE_NAME 'cause to lazy to 
                      make a variable to hold the value */
                   sprintf(FILE_NAME, "%02X", textSize); 
                                  
                   /* Fix Text Record Length */
                   textRecord[7] = FILE_NAME[0];
                   textRecord[8] = FILE_NAME[1];

                   fprintf(objFile, "%s\n", textRecord);

                   index = ht_get(OPERAND);
                   sprintf(endRecord, "E%06X", index);
                   fprintf(objFile, "%s", endRecord);
                   fclose(objFile);
               }
               /* Create Text Record */
               else
               {
                   if (!doneText)
                   {
                       if (!startAddress)
                       {
                           Create:
                           /* Begins Creating Text Record */
                           sprintf(textRecord, "T%06X00", 
                                   (unsigned int)strtol(LOCATION, NULL, 16));
                           strcat(textRecord, machineCode);
                           textSize += strlen(machineCode) / 2;
                           
                           startAddress = true; //Address and corresponding machine code added
                       }

                       else
                       {
                           /* If machine code not empty add to Text Record */
                           if (machineCode[0] != '\0')
                           {
                               /* Check if theres space */
                               if (numOfInstructions < 10) 
                               {
                                   /* If size of text record is zero, it means 
                                      a reserve came before it and new record must
                                      be made */
                                   if (textSize == 0)
                                       goto Create;

                                   /* Catenate and update info of record */
                                   strcat(textRecord, machineCode);
                                   ++numOfInstructions;
                                   textSize += strlen(machineCode) / 2;    
                               }
                               /* If no space write out record and make a new one */
                               else
                               { 
                                   WriteOut:
                                   /* I reused FILE_NAME 'cause to lazy to 
                                      make a variable to hold the value */
                                   sprintf(FILE_NAME, "%02X", textSize); 
                                  
                                   /* Fix Text Record Length */
                                   textRecord[7] = FILE_NAME[0];
                                   textRecord[8] = FILE_NAME[1];

                                   fprintf(objFile, "%s\n", textRecord);
                                
                                   goto Clean;
                               }
                           }

                           /* The machine code is probably a reserve directive */
                           else
                           {
                               /* It was preceded by another reserve */
                               if (textSize == 0)
                                   goto Clean;
                               /* Otherwise write out valid text record */
                               else
                                   goto WriteOut;
                           }
                       }
                   }

                   /* Resets out all variables and jumps to 
                      create a new text record */
                   else
                   {
                       Clean:
                       doneText = false;
                       startAddress = false;
                       textSize = 0;
                       numOfInstructions = 1;
                       textRecord[0] = '\0';
             
                       goto Create; 
                   }
               }
           }
          /* --------------------------End Object File----------------------------- */
       }

       Bottom:
       machineCode[0] = '\0';

       /* Update */
       fgets(SOURCE, 128, inter);
       fgets(LOCATION, 10, inter);
       fgets(OPCODE, 4, inter);
       fgets(OPERAND, 10, inter);
       fgets(ERROR, 50, inter);
   }
   
   if (!foundEnd())
      fprintf(lstFile, "ERROR:\tNo END Directive found\n");

   if (rmObj)
      remove(name);

   fclose(inter);
   fclose(lstFile);
}

/* Return the machine code equivalent of the opcode and operand */
void getMachineCode(char *opcode, char *operand, char *machineCode)
{
    char address[5],
         tempHex[3],
         symbol[10];
    int i;

    /* Handles RSUB */
    if (opcode[0] == '4' && opcode[1] == 'C')
    {
        /* Remove trailing newline */
        i = strlen(opcode);
        if (opcode[i-1] == '\n')
            opcode[i-1] = '\0';

        sprintf(machineCode, "%2s0000", opcode);
    }
    /* Handles the BYTE and WORD Directive */
    else if (opcode[0] == '0' && opcode[1] == '2')
    {
        /* Fill out machine code with hex digits */
        if (toupper(operand[0]) == 'X' && operand[1] == '\'')
        {
            char *temp;
            temp = malloc(strlen(operand) - 2);
            for(i = 2; operand[i] != '\''; ++i)
                temp[i-2] = operand[i];
            temp[i-2] = '\0';

            sprintf(machineCode, "%s",  temp);  
        }
        /* Fill out with hex representation of ASCII char */
        else if (toupper(operand[0] == 'C') && operand[1] == '\'')
        {
            for (i = 2; operand[i] != '\''; ++i)
            {
                sprintf(tempHex, "%02X", operand[i]);
                strcat(machineCode, tempHex);
            }
        }
        /* WORD Directive */
        else 
        {
             i = atoi(operand);
             sprintf(machineCode, "%06X", i);
        }
    }
    /* Handles the Reserve directives */
    else if (opcode[0] == '0' && opcode[1] == '3')
    {
        machineCode[0] = '\0';
    }
    /* Handles START and END */
    else if (opcode[0] == 's' || opcode[0] == '\n')
    {
        machineCode[0] = '\0';
    } 
    /* Handle Mnenomic and its operand */
    else 
    { 
        /* Make operand all uppercase */
        for (i = 0; i < strlen(operand); ++i)
            operand[i] = toupper(operand[i]);
  
        /* Remove trailing newline character */
        if (opcode[2] == '\n')
            opcode[2] = '\0';
        i = strlen(operand);
        if (operand[i-1] == '\n')
            operand[i-1] = '\0';
        
        for (i = 0; operand[i] != ',' && i < strlen(operand); ++i)
            ;
 
        /* Check if in ',X' format */
        if (operand[i] == ',')
        {
            if (operand[i+1] == 'X')
            {
                for (i = 0; i < strlen(operand) - 2; ++i)
                    symbol[i] = operand[i];

                i = ht_get(symbol);

                /* Mask the x bit */
                i = 0x8000 | i;

                sprintf(address, "%04X", i);
            }
            /* Error */
            else
            {
                sprintf(address, "````");
                rmObj = true;
            }
        }
        else
        {
            i = ht_get(operand);

            if (i == 1)
            {
                sprintf(address, "____");
                rmObj = true;
            }
            else
                sprintf(address, "%04X", i);
        }
 
        sprintf(machineCode, "%2s%4s", opcode, address); 
    }
}

/* Writes out errors to listing file */
void writeErrors(char *errors, char *machineCode)
{
    int i;

    i = strlen(errors);
    if (errors[i-1] == '\n')
       errors[i-1] = '\0';

    fprintf(lstFile, "ERROR:");

    for (i = 0; i < strlen(errors); ++i)
    {
       if (errors[i] == '@')
       {
           while (errors[i] != '%' && errors[i] != '~' && errors[i] != '$')
           {
               if (errors[i] == '1')
               {
                   fprintf(lstFile, "\tLabel must start with a letter\n");
               }
               else if (errors[i] == '2')
               {
                   fprintf(lstFile, "\tLabel can only be alphanumeric\n");
               }
               else if (errors[i] == '3')
               {
                   fprintf(lstFile, "\tLabel cannot be longer than 6 alphanumeric characters\n");
               }
               else if (errors[i] == '4')
               {
                   fprintf(lstFile, "\tLabel is a duplicate\n");
               }
               else if (errors[i] == '5')
               {
                   fprintf(lstFile, "\tToo many symbols in symbol table\n");
               }

               ++i;
           }
       }
       if (errors[i] == '%')
       {
           while (errors[i] != '@' && errors[i] != '~' && errors[i] != '$')
           {
               if (errors[i] == '1')
               {
                   fprintf(lstFile, "\tNot a valid mnenomic or directive\n");
               }
               else if (errors[i] == '2')
               {
                   fprintf(lstFile, "\tOperand required\n");
               }

               ++i;
           }
       }
       if (errors[i] == '~')
       {
           while (errors[i] != '%' && errors[i] != '@' && errors[i] != '$')
           {
               if (errors[i] == '1')
               {
                   fprintf(lstFile, "\tOperand for BYTE must start with either an X or C\n");
               }
               else if (errors[i] == '2')
               {
                   fprintf(lstFile, "\tValue must be encased in single quotes\n");
               }
               else if (errors[i] == '3')
               {
                   fprintf(lstFile, "\tCannot have more than 30 characters\n");
               }
               else if (errors[i] == '4')
               {
                   fprintf(lstFile, "\tCannot have more than 32 hexadecimal digits\n");
               }
               else if (errors[i] == '5')
               {
                   fprintf(lstFile, "\tHexadecimal digits must be even\n");
               }

               ++i;
           }
 
           if (machineCode[0] == '`')
           {
               fprintf(lstFile, "\tInvalid format for indexing\n");
               machineCode[0] = '\0';
           }
           else if (machineCode[0] == '_')
           {
               fprintf(lstFile, "\tUndfined symbol in operand\n");
               machineCode[0] = '\0';
           }
       }
       if (errors[i] == '$')
       {
          while (errors[i] != '%' && errors[i] != '~' && errors[i] != '@')
           {
               if (errors[i] == '1')
               {
                   fprintf(lstFile, "\tMissing operand for END directive\n");
               }
               else if (errors[i] == '2')
               {
                   fprintf(lstFile, "\tFile is too long, download more RAM\n");
               }
               else if (errors[i] == '3')
               {
                   fprintf(lstFile, "\tNo END Directive found\n");
               }
               else if (errors[i] == '4')
               {
                   fprintf(lstFile, "\tInvalid Operand\n");
               }

               ++i;
           }
       }
    }
    
    fprintf(lstFile, "\n");
}
