#ifndef MAIN_H
#define MAIN_H

/* ----------------------------------Standard libraries------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>


/* ----------------------------------Boolean data type------------------------------------- */
#define bool char
#define true 1
#define false 0


/*
 I got bored, so I started to search
 up output formatting and I found these
 ANSI escape codes for color.
*/
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_PURPLE  "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

#endif
