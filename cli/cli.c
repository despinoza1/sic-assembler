/*
 Command Line interpeter for the SIC machine
 Has eight commands: 
   LOAD, EXECUTE, DEBUG, DUMP, HELP, DIRECTORY, ASSEMBLE, EXIT
*/

#include "cli.h"
#include "debug.h"

#define BUFFER_MAX 256

/* ------------------------------------Global Variables----------------------------------- */
unsigned long startLocation = 0; // Holds the starting address for execute function


/* -----------------------------Command line interpeter/interface-------------------------- */
int cli()
{
    char buffer[BUFFER_MAX],
        *greeting = "SIC CLI v.75",
	*cmd[3];
    int index = 0;
    bool flag;

    printf("  \t\t\t%s\n", greeting);

    /* Infinite loop for command line interface/interpeter */
    while(true)
    {
        printf("user@computer> ");
	fgets(buffer, BUFFER_MAX, stdin);

        /* Retrieves tokens of user input */
	getTokens(buffer, cmd);

        /* Determine if command exist */
	flag = checkCmds(cmd[0], &index);

	if (!flag)
	    printf(COLOR_RED "%s: Command not found.\n" COLOR_RESET,
                   cmd[0]);
	else
	{
	    switch(index)
	    {
	     case 0: load(cmd[1]);
	         break;
	     case 1: execute();
		 break;
	     case 2: debug();
		 break;
	     case 3: dump(cmd[1], cmd[2]);
	         break;
	     case 4: help();
		 break;
	     case 5: assemble(cmd[1]);
		 break;
	     case 6: dir();
		 break;
	     case 7: return 0;
		 break;
	     default:
		 break;
	    }
	}

        free(*cmd);
    } 

    return 0;
}


/* -------------------------------Command STRUCT & ARRAY---------------------------------- */

/* Structure for commands */
typedef struct
{
   char *cmd;
   char *format;
   char *doc;
} cmd_t;

/* Array of structure commands */
cmd_t commands[8];

/* Initializes commands */
void initCMDS()
{
    int i;
	
    for (i = 0; i < 8; ++i)
    {
        commands[i].cmd = malloc(10);
	commands[i].format = malloc(30);
	commands[i].doc = malloc(30);
    }
    
    /* In hindsight I should I put
       all this info in a file */
    commands[0].cmd = "LOAD\0";
    commands[0].format = "LOAD <filename>\0";  
    commands[0].doc = "--Loads a SIC object file into memory\n";
	
    commands[1].cmd = "EXECUTE\0";
    commands[1].format = commands[1].cmd;
    commands[1].doc = "--Executes previously loaded object file\n";
	
    commands[2].cmd = "DEBUG\0";
    commands[2].format = commands[2].cmd;
    commands[2].doc = "--Executes in debug mode\n";
	
    commands[3].cmd = "DUMP\0";
    commands[3].format = "DUMP <start end>\0";
    commands[3].doc = "--Dump memory content from specified range"
                      "\n\t\t\t\t(range must be hexadecimal digits)\n";
	
    commands[4].cmd = "HELP\0";
    commands[4].format = commands[4].cmd;
    commands[4].doc = "--This message.\n";
	
    commands[5].cmd = "ASSEMBLE\0";
    commands[5].format = "ASSEMBLE <filename>\0";
    commands[5].doc = "--Assemle SIC .asm file\n";
	
    commands[6].cmd = "DIRECTORY\0";
    commands[6].format = commands[6].cmd;
    commands[6].doc  = "--Displays current directory\n";
	
    commands[7].cmd = "EXIT\0"; 
    commands[7].format = commands[7].cmd;
    commands[7].doc = "--Exits SIC CLI v.75\n";
}


/* -------------------------------------C string Functions-------------------------------- */

/* Checks for whitespace */
bool not_ws(const char test)
{
    if (test != ' ' && test != '\t' && test != '\n') 
        return true;
    else 
	    return false;
}

/* "Better than strtok" - No one */ 
void getTokens(const char *str, char **tokens)
{
    int i = 0,
        j = 0,
        k = 0;
    for (; i < 3; ++i)
    {
        tokens[i] = malloc(50);
        tokens[i][0] = '\0';
    }	
 
    i = 0;
    while (str[i] != '\0' && k < 3)
    {
	if (not_ws(str[i]))
	{
	    for (j = 0; not_ws(str[i]); ++j)
	    {
		if (k == 0)
		    tokens[k][j] = (char)toupper(str[i]);
                else 
		    tokens[k][j] = str[i];

		i++;
	    }

	    tokens[k][j] = '\0';
	    k++;
	}

	i++;
    }
}

/* Checks if user input is a valid command */
bool checkCmds(const char *cmd, int *index)
{
   int i;
   char shortCmd[4];
   shortCmd[3] = '\0';

   for (i = 0; i < 8; ++i)
   {		
       if (strcmp(cmd, commands[i].cmd) == 0)
       {
           *index = i;
	   return true;
       }
       /* Checks if user inputs the first three letters
          of a command */
       else if (strlen(cmd) < 4)
       {
	   strncpy (shortCmd, commands[i].cmd, 3);
	   if (strcmp(cmd, shortCmd) == 0)
	   {
	       *index = i;
	       return true;
	   }
       }
    }
	
    return false;
}


/* -----------------------------------Command Functions----------------------------------- */

void load(char *arg)
{
    char objRecord[80] = { '\0' };
    char fileName[10] = { '\0' };
    char xdigits[3] = { '\0' };
    int index;
    unsigned int size = 0;
    unsigned long currAddr = 0;
    unsigned char byte = 0; 

    if (arg[0] == '\0')
        printf(COLOR_YELLOW "Format: %s, use help\n" COLOR_RESET, 
               commands[0].format);
    else
    {
       FILE *objFile = NULL;

       if ((objFile = fopen(arg, "r")) == NULL)
          printf(COLOR_RED "File specified does not exist.\n" COLOR_RESET);
       else
       {
           /* Get the Header Record */
           fgets(objRecord, 80, objFile);
           sscanf(objRecord, "H%6s%6X%6X", fileName, &startLocation, &size);

           /* Display information of program */
           printf(COLOR_GREEN "\nNow loading:\n\n" COLOR_RESET);
           printf("Program Name:\t   %s\nStarting Location: %06X\nSize:\t\t   %06X\n\n", 
                   fileName, (unsigned int)startLocation, size);

          /* Handles Text Records */
          fgets(objRecord, 80, objFile);
          while (objRecord[0] != 'E')
          {
              sscanf(objRecord, "T%6X%2X", &currAddr, &size);
               
              /* Loops through current Text Record */
              for (index = 9; objRecord[index] != '\n'; index+=2)
              {
                    /* Convert the string into digits */
                    strncpy(xdigits, objRecord+index, 2);
                    byte = (unsigned char) strtol(xdigits, NULL, 16);

                    /* Write out to memory */
                    PutMem(currAddr, &byte, 0);
                    ++currAddr;
              }

              fgets(objRecord, 80, objFile);
          }
          
          /* Handles End Record and sets starting location
             to its value for EXECUTE command */
          sscanf(objRecord, "E%6X", &startLocation);
       }
       
    }
}

void execute()
{
    SICRun(&startLocation, false);
}

void debug()
{
    RunDebug(startLocation);
}

void dump(char *arg1, char *arg2)
{
    unsigned char byte;
    unsigned long i, start, end;

    if (arg1[0] == '\0' || arg2[0] == '\0')
        printf(COLOR_YELLOW"Format: %s, use help\n" COLOR_RESET,
               commands[3].format);
    else
    {
	/* Check if arg1's content are hexadecimal digits */
        for (i = 0; i < strlen(arg1); ++i)
        {
            if (!isxdigit(arg1[i]))
            {
                printf(COLOR_RED "\nSTART must be hexadecimal digit\n\n" COLOR_RESET);
                return;
            }
        }
        /* Check if arg2's content are hexadecimal digits */
        for (i = 0; i < strlen(arg2); ++i)
        {
            if (!isxdigit(arg2[i]))
            {
                printf(COLOR_RED "\nEND must be hexadecimal digit\n\n" COLOR_RESET);
                return;
            }
        }

        /* Convert */
        start = strtol(arg1, NULL, 16);
        end = strtol(arg2, NULL, 16); 
        
        printf("\nDumping memory content from %04X to %04X\n",
               (unsigned int)start, (unsigned int)end);

        /* Print out memory in columns of 16 */
        for (i = start, start = 0; i <= end; ++i)
        {
            if (start % 16 == 0)
                printf("\n%04X: ", (unsigned int)i);

            GetMem(i, &byte, 0);
            printf("%02X ", (unsigned int)byte);
            start++;
        }
        
        printf("\n\n");
   }
}

void help()
{
    int i;
       printf("\n\t\t\t   Help\n\n");
	
    for (i = 0; i < 8; ++i)
        printf(COLOR_CYAN "%20s\t\t" COLOR_GREEN "%s" COLOR_RESET, 
                commands[i].format, commands[i].doc);
    putchar('\n');
}

void assemble(char *filename)
{
    if (filename[0] == '\0')
        printf(COLOR_YELLOW "Format: %s, use help\n" COLOR_RESET, 
               commands[5].format);
    else
    {
        FILE *source;
        char *interfile;
        int size = strlen(filename);

        if ((source = fopen(filename, "r")) == NULL)
        {
            printf(COLOR_RED "Source file could not be opened.\n" COLOR_RESET);
            return;
        }

        /* Copies source file name and changes extension 
           it assumes that its in the old Windows style of
           three letter extension */
        interfile = malloc(size);
        strcpy(interfile, filename);
        interfile[size-3] = 'i';
        interfile[size-2] = 'n';
        interfile[size-1] = 't';


        printf(COLOR_GREEN "\nAssembling\n" COLOR_RESET);
        pass1(source, interfile);
        
        fclose(source);
        
        pass2(interfile);   
        printf(COLOR_GREEN "Done Assembling\n\n" COLOR_RESET);
    }
}

void dir()
{
    system("ls -aF --color=auto");
}
