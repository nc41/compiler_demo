#ifndef ATTR_H
#define ATTR_H
#include <stddef.h>
#include <stdlib.h>

typedef enum types {TYPE_INT=0, TYPE_BOOL, TYPE_CHAR} Types;
typedef union {int num; char* c;} var;


typedef struct {
        Types type;
        int val;
        char* valC;
        int target;
        int target_addr;
        int arraysize;
        int array_start[2];
        int array_end[2];
        int lhs;
        int rhs;
        char* name;
        char* vars[100];
        int numVars;
        int dim;
        int bounds[10];
        int label[5];
        } regInfo;

#endif
