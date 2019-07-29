#ifndef ILOC_EMITTER_H
#define ILOC_EMITTER_H

#define size_virtual_register 4096

extern FILE *outfile;

typedef enum OP {nop=0, write, cwrite, add, sub, subI, cload, loadI, loadAI, load, 
                addI, mult, multI, store, storeAI, storeAO, 
                cstore, i2i, or, orI, not, and, andI, divide, divI, c2c, i2c, c2i,
                br, cbr, cmp_LT, cmp_LE, cmp_GT, cmp_GE, cmp_EQ, cmp_NE, halt,
                read, cread} OP;

int nexReg();
int nextLabel();
void emit(int label, OP op, int r1, int r2, int r3);
void emit_cbr(int label, int label2, int r1);
int getBase(int, int, int);
int getOffset(int, int*, int*);
int getBase4Two(int low1, int high1,  int low2, int high2, int size);


#endif
