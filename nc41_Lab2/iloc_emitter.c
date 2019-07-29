#include <stdio.h>
#include <stdlib.h>
#include "iloc_emitter.h"

static int reg = 0;
static int offset = 4;
static int label = 0;

int nextLabel(){
    return label++;
}

int nextReg(){
    //printf("Get registers. \n");
	if (reg < size_virtual_register){
		return reg++;
	}
	else{
		printf("Running out of registers.");
		exit(0);
	}
}

int getOffset(int dim, int* start, int* end){
    return 0;
}


int getBase(int u, int range, int size){
	int cur_offset = offset;
	offset += size * range + 4;
    //printf("newBase: %d\n", offset);
	return cur_offset + size * u;
}

int getBase4Two(int low1, int high1,  int low2, int high2, int size){
    int cur_offset = offset;
    offset += (high1 - low1 + 1) * (high2 - low2 + 1) * size + 4;
    //printf("newBase: %d  newOffset : %d\n", offset, offset - cur_offset); 
    return cur_offset - (low1*(high2 - low2 + 1) * size + (low2 * size));
}


void emit(int label, OP op, int r1, int r2, int r3){
    if(label < -1){
        printf("Error: No such label exists.\n");
        return;
    } 
    
    switch (op) {
        case nop:
            fprintf(outfile, "L%d:\tnop\n", label);
            break;
        case write:
            fprintf(outfile, "\twrite r%d\n", r1);
            break;
        case cwrite:
             fprintf(outfile, "\tcwrite r%d\n", r1);
             break;
        case loadI:
            fprintf(outfile, "\tloadI %d => r%d\n", r1, r2);
            break;
        case load:
             fprintf(outfile, "\tload r%d => r%d\n", r1, r2);
             break;
        case cload:
             fprintf(outfile, "\tcload r%d => r%d\n", r1, r2);
             break;
        case addI:
            fprintf(outfile, "\taddI r%d, %d => r%d\n", r1, r2, r3);
            break;
        case i2i:
            fprintf(outfile, "\ti2i r%d => r%d\n", r1, r2);
            break;
        case add:
            fprintf(outfile, "\tadd r%d, r%d => r%d\n", r1, r2, r3);
            break;
        case mult:
            fprintf(outfile, "\tmult r%d, r%d => r%d\n", r1, r2, r3);
            break;
        case store:
             fprintf(outfile, "\tstore r%d => r%d\n", r1, r2);
             break;
        case multI:
             fprintf(outfile, "\tmultI r%d, %d => r%d\n", r1, r2, r3);
             break;
        case cstore:
             fprintf(outfile, "\tcstore r%d => r%d\n", r1, r2);
             break;
        case sub:
            fprintf(outfile, "\tsub r%d, r%d => r%d\n", r1, r2, r3);
            break;
        case subI:
            fprintf(outfile, "\tsubI r%d, %d => r%d\n", r1, r2, r3);
            break;
        case divide:
             fprintf(outfile, "\tdiv r%d, r%d => r%d\n", r1, r2, r3);
             break;
        case divI:
              fprintf(outfile, "\tdivI r%d, %d => r%d\n", r1, r2, r3);
              break;
        case c2c:
            fprintf(outfile, "\tc2c r%d => r%d\n", r1, r2);
            break;
        case i2c:
            fprintf(outfile, "\ti2c r%d => r%d\n", r1, r2);
            break;
        case c2i:
            fprintf(outfile, "\tc2i r%d => r%d\n", r1, r2);
            break;
        case and:
            fprintf(outfile, "\tand r%d, r%d => r%d\n", r1, r2, r3);
            break;
        case andI:
            fprintf(outfile, "\tandI r%d, %d => r%d\n", r1, r2, r3);
            break;
        case or:
            fprintf(outfile, "\tor r%d, r%d => r%d\n", r1, r2, r3);
            break;
        case cmp_LT:
            fprintf(outfile, "\tcmp_LT r%d, r%d => r%d\n", r1, r2, r3);
            break;
        case cmp_LE:
            fprintf(outfile, "\tcmp_LE r%d, r%d => r%d\n", r1, r2, r3);
            break;
        case cmp_GT:
             fprintf(outfile, "\tcmp_GT r%d, r%d => r%d\n", r1, r2, r3);
             break;
        case cmp_GE:
             fprintf(outfile, "\tcmp_GE r%d, r%d => r%d\n", r1, r2, r3);
             break;
        case cmp_EQ:
             fprintf(outfile, "\tcmp_EQ r%d, r%d => r%d\n", r1, r2, r3);
             break;
        case cmp_NE:
             fprintf(outfile, "\tcmp_NE r%d, r%d => r%d\n", r1, r2, r3);
             break;
        case br:
             fprintf(outfile, "\tbr => L%d\n", label);
             break;
        case read:
             fprintf(outfile, "\tread => r%d\n", r1);
             break;
        case cread:
              fprintf(outfile, "\tcread => r%d\n", r1);
              break;
        case not:
             fprintf(outfile, "\tnot r%d => r%d\n", r1, r2);
             break;
        
    }
}
void emit_cbr(int label, int label2, int r1){
    if(label < -1 || label < -1){
         printf("Error: No such label exists.\n");
         return;
     }
    fprintf(outfile, "\tcbr r%d => L%d, L%d\n", r1, label, label2);
}





