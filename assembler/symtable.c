/*
 Symbol Table for Assembler
 Orginized in a dynamic Hash Table
 with each bucket with a c-string for label,
 unsinged integer for address and 
 a pointer to the next bucket
*/

#include "assemble.h"
#define MAX 500


/* --------------------------------Structures for Symbols---------------------------------- */
/* Bucket for Hash Table */
typedef struct symbol_s
{
    unsigned int address;
    char *label;
    struct symbol_s *next;
} symbol_t;

/* Hash Table of Symbols */
typedef struct
{
    int size;
    symbol_t **table;
} ht_symbols;

/* Symbol Table */
ht_symbols *SYMTAB = NULL;


/* --------------------------Supporting Functions for Hash Table-------------------------- */
/* Initialize Symbol Table */
void initSYMTAB()
{
    int i;
    
    SYMTAB = malloc(sizeof(ht_symbols));
    SYMTAB->table = malloc(sizeof(symbol_t) * MAX);

    for (i = 0; i < MAX; i++)
        SYMTAB->table[i] = NULL;

    SYMTAB->size = MAX;
}

/* Hash Function - Implementation of djb2 algorithm by Dan Bernstein */
static unsigned int ht_hash(const char *key)
{
    unsigned int hashval = 5381;

    while (*(key++))
        hashval = ((hashval << 5) + hashval + (*key));

    return hashval;
}

/* Inserts a new bucket into table */
void ht_put(char *key, int data)
{
    unsigned int i = 0,
        j;
    symbol_t *newSym = NULL;
    symbol_t *next = NULL;
    symbol_t *prev = NULL;

    i = ht_hash(key);
    i %= SYMTAB->size;
    next = SYMTAB->table[i];

    while (next != NULL && strcmp(key, next->label) > 0)
    {
        prev = next;
        next = next->next;
    }

    newSym = malloc(sizeof(symbol_t));
    newSym->label = malloc(10);
    
    strcpy(newSym->label, key);

    newSym->address = data;

    if (next == SYMTAB->table[i])
    {
       newSym->next = next;
       SYMTAB->table[i] = newSym;
    }
    else if (next == NULL)
    {
        prev->next = newSym;
    }
    else
    {
        newSym->next = next;
        prev->next = newSym;
    }
}

/* Retrieves data to corresponding key */
unsigned int ht_get(char *key)
{
    unsigned int i = 0;
    symbol_t *symbol;
    i = ht_hash(key);
    i %= SYMTAB->size;

    symbol = SYMTAB->table[i];

    if (symbol != NULL && symbol->label != NULL && strcmp(key, symbol->label) == 0)
        return symbol->address;    

    while (symbol != NULL && symbol->label != NULL && strcmp(key, symbol->label) > 0)
        symbol = symbol->next;

    if (symbol == NULL || symbol->label == NULL || strcmp(key, symbol->label) != 0)
        return 1;
    else
        return symbol->address;
}

/* Determines if key is duplicate of another */
bool duplicateKey(char *key)
{
    symbol_t *test = NULL;
    unsigned int i = 0;

    i = ht_hash(key);
    i %= SYMTAB->size;

    test = SYMTAB->table[i];

    if (test == NULL)
        return false;
    else if (strcmp(test->label, key) == 0)
        return true;

    while (test->label != NULL && test->next != NULL)
    {
        test = test->next;
        if (strcmp(test->label, key) == 0)
            return true;
    } 

    return false;
}
