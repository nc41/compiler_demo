#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "att.h"

// typedef enum types {TYPE_INT = 0, TYPE_BOOL, TYPE_CHAR} Types;

typedef struct{	
	char* name;
	int offset;
	Types type;
    int reg;
    int isScalar;
    int dim;
    int base;
    int array_start[2];
    int array_end[2];

} sym_entry;

typedef struct {
     int dim;
     int arraySize[4];

 } mulDim;

typedef struct regVal {
	Types type;
	int targetReg;
	int arraySize;
    	
} regVal;

void newHashTable();

sym_entry* search(char* key);

void insert(char* key, Types type, int base, int reg);

void printTable();


#endif
