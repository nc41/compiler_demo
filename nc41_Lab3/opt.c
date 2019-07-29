#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include "instruction.h"



const char* opcode[] = {"nop", "add", "sub", "mult", "div", "addI", "subI", "multI",
    "divI", "lshift", "lshiftI", "rshift", "rshiftI",
    "and", "andI", "or", "orI", "not",
    "loadI", "load", "loadAI", "loadAO", "cload", "cloadAI",
    "cloadAO", "store", "storeAI", "storeAO", "cstore",
    "cstoreAI", "cstoreAO", "br", "cbr", "cmp_LT", "cmp_LE",
    "cmp_EQ", "cmp_NE", "cmp_GE", "cmp_GT", "i2i", "c2c", "i2c", "c2i",
    "output", "coutput", "read", "cread", "write", "cwrite", "halt"};


typedef struct IR_Node {
    Opcode_Name op;
    int srcs[2];
    int consts[2];
    int defs[2];
    char* labels[2];
    Instruction* targets[2];
    struct IR_Node* next;
    char* label_nop;
} iNode;

typedef struct Block {
    int id;
    struct Block* next;
} block;

static block* blocks_leader;
static block* blocks_last;
block* dummy_block_leader;
block* dummy_block_last;

FILE *outfile;

//struct IR_Node* dummy = NULL;
//dummy = (struct IR_Node*)malloc(sizeof(struct IR_Node));   

iNode* parseILOC() {
    struct IR_Node* dummy = NULL;
    dummy = (struct IR_Node*)malloc(sizeof(struct IR_Node));
    dummy -> next = (struct IR_Node*)malloc(sizeof(struct IR_Node));
    iNode* cur = dummy -> next;

    Instruction* instructPtr = parse();
    //printf("%s ", opcode[instructPtr -> operations -> opcode]);
    while(instructPtr) {

        cur -> op = instructPtr -> operations -> opcode;
        cur -> srcs[0] = INT_MIN;
        cur -> srcs[1] = INT_MIN;
        cur -> consts[0] = INT_MIN;
        cur -> consts[1] = INT_MIN;
        cur -> defs[0] = INT_MIN;
        cur -> defs[1] = INT_MIN;
        cur -> labels[0] = NULL;
        cur -> labels[1] = NULL;
        cur -> targets[0] = NULL;
        cur -> targets[1] = NULL;
        cur -> label_nop = NULL;
        cur -> next = NULL;

        //printf("%s ", opcode[instructPtr -> operations -> opcode]); 

        if(instructPtr -> operations -> opcode == 0) {
            //printf("label: ");
            iNode* tmp = dummy -> next;

            while(tmp != cur){
                if (tmp -> targets[0] != NULL) {
                    if (tmp -> targets[0] == instructPtr) {
                        cur -> label_nop = (char*) malloc((10)*sizeof(char));
                        strcpy(cur -> label_nop, tmp -> labels[0]);
                        //printf("%s \n", cur -> label_nop);
                        cur -> next = (struct IR_Node*)malloc(sizeof(struct IR_Node));
                        cur = cur -> next;
                        break;
                    }

                    if(tmp -> targets[1] != NULL) {
                        if (tmp -> targets[1] == instructPtr) {
                            cur -> label_nop = (char*) malloc((10)*sizeof(char));
                            strcpy(cur -> label_nop, tmp -> labels[1]);
                            //printf("%s \n", cur -> label_nop);
                            cur -> next = (struct IR_Node*)malloc(sizeof(struct IR_Node));
                            cur = cur -> next;
                            break;
                        }
                    }
                }

                tmp = tmp -> next;
            }

            instructPtr = instructPtr -> next;
            continue;
        }

        //printf("src: ");
        if(instructPtr -> operations -> srcs){
            //printf("%d ", instructPtr -> operations -> srcs -> value);
            cur -> srcs[0] = instructPtr -> operations -> srcs -> value; 
            if(instructPtr -> operations -> srcs -> next) {
                //printf("%d ", instructPtr -> operations -> srcs -> next -> value);
                cur -> srcs[1] = instructPtr -> operations -> srcs -> next -> value;
            }
        }

        //printf("const: ");
        if(instructPtr -> operations -> consts) {
            //printf("%d ", instructPtr -> operations -> consts -> value);
            cur -> consts[0] = instructPtr -> operations -> consts -> value;
            if(instructPtr -> operations -> consts -> next) {
                //printf(" %d ", instructPtr -> operations -> consts -> next -> value);
                cur -> consts[1] = instructPtr -> operations -> consts -> next -> value;
            }
        }

        //printf("def: ");
        if(instructPtr -> operations -> defs) {
            //printf("%d ", instructPtr -> operations -> defs -> value);
            cur -> defs[0] = instructPtr -> operations -> defs -> value;
            if(instructPtr -> operations -> defs -> next) {
                cur -> defs[1] = instructPtr -> operations -> defs -> next -> value;
                //printf(" %d ", instructPtr -> operations -> defs -> next -> value);
            }
        }

        //printf("label: ");
        if(instructPtr -> operations -> labels) {
            //printf("%s ", get_label(instructPtr -> operations -> labels -> value) -> string);
            cur -> labels[0] = (char*) malloc((10)*sizeof(char));
            //cur -> labels[0] = get_label(instructPtr -> operations -> labels -> value) -> string;
            strcpy( cur -> labels[0], get_label(instructPtr -> operations -> labels -> value) -> string);
            cur -> targets[0] = get_label(instructPtr -> operations -> labels -> value) -> target;
            //printf("%s ", cur -> labels[0]);
            //printf("%d ", get_label(instructPtr -> operations -> labels -> value) -> identifier);  
            //printf("%s ", opcode[get_label(instructPtr -> operations -> labels -> value) -> target -> operations -> opcode]);
            if(instructPtr -> operations -> labels -> next) {
                cur -> labels[1] = (char*) malloc((10)*sizeof(char));
                strcpy( cur -> labels[1], get_label(instructPtr -> operations -> labels -> next -> value) -> string);
                cur -> targets[1] = get_label(instructPtr -> operations -> labels -> next -> value) -> target;
                //printf("%s ", cur -> labels[1]);
                //printf("%s ", opcode[get_label(instructPtr -> operations -> labels -> next -> value) -> target -> operations -> opcode]);
            }

        }

        //printf("\n");
        instructPtr = instructPtr -> next;
        cur -> next = (struct IR_Node*)malloc(sizeof(struct IR_Node));
        cur = cur -> next;
    }

    return dummy;
}



void buildBlock(iNode* dummy){
    blocks_leader -> next = (block* )malloc(sizeof(block));

    blocks_last -> next = (block* )malloc(sizeof(block));

    iNode* cur = dummy -> next;
    int block_id = 0;

    blocks_leader = blocks_leader -> next;
    blocks_last = blocks_last -> next;
    //printf("%d\n", cur -> op);
    blocks_leader -> id = block_id;
    blocks_leader -> next = (block* )malloc(sizeof(block));
    blocks_leader = blocks_leader -> next;
    //printf("%d\n", cur -> op);

    while (cur -> next) {
        if(cur -> label_nop != NULL) {
            // printf("%d %s\n", block_id - 1, cur -> label_nop);
            blocks_last -> id = block_id - 1;
            blocks_leader -> id = block_id;
            blocks_leader -> next = (block* )malloc(sizeof(block));
            blocks_leader = blocks_leader -> next;
            blocks_last -> next = (block* )malloc(sizeof(block));
            blocks_last = blocks_last -> next;

        }
        block_id++;
        cur = cur -> next;
    }

    blocks_leader -> next = NULL;
    blocks_last -> id = block_id - 1;
    blocks_last -> next = NULL;
    //printf("%d\n", block_id - 1);
}

typedef struct VN {
    int id;
    char* expression;

    struct VN* next;
} Value_Number;

int searchVN(char* target, Value_Number* vn) {
    //printf("target:%s\n", target);
    if(vn -> expression == NULL || vn -> id == -1) {
        return -1;
    }

    while (vn) {
        if(vn -> expression && !strcmp(vn -> expression, target)) {
            return vn -> id;
        }
        //printf("searching: %s\n", vn -> expression);
        vn = vn -> next;
    }

    // not found
    return -1;
}

Value_Number* searchNode(char* target, Value_Number* vn) {
    //printf("target:%s\n", target);
    if(vn -> expression == NULL || vn -> id == -1) {
        return NULL;
    }

    while (vn) {
        if(vn -> expression && !strcmp(vn -> expression, target)) {
            return vn;
        }
        //printf("searching: %s\n", vn -> expression);
        vn = vn -> next;
    }

    // not found
    return NULL;
}

char* searchName(int target, Value_Number* vn){
    if(vn -> expression == NULL || vn -> id == -1) {
        return NULL;
    }

    while(vn) {
        if(vn -> id == target) {
            return vn -> expression;
        }
        vn = vn -> next;
    }

    return NULL;
} 


Value_Number* getNew(Value_Number* vn) {
    vn -> next = (struct VN*)malloc(sizeof(struct VN));
    vn = vn -> next;
    vn -> id = -1;
    vn -> expression = NULL;
    vn -> next = NULL;
    return vn;
}

void printVN(Value_Number* vn) {
    while (vn -> next) {
        //printf("%s : %d\n", vn -> expression, vn -> id);
        vn = vn -> next;
    }

}

char* getReg(int i) {
    char* tmp = (char*)malloc(50*sizeof(char));
    char digit[100];
    strcpy(tmp, "r");
    sprintf(digit, "%d", i);
    strcat(tmp, digit);
    return tmp;
}


void valueNumber(iNode* dummy) {
    //printf("-----------value numbering-----------\n");

    block* leader = dummy_block_leader -> next;
    block* last = dummy_block_last -> next;
    Value_Number* vn, *exprVn;    

    Value_Number* dummy_vn, *dummy_exprVn;



    Value_Number* vn2name;
    Value_Number* dummy_vn2name;

    iNode* cur_instr = dummy -> next;

    int valueNum = 0;

    while (leader && last) {
        int start = leader -> id;
        int end = last -> id;
        //printf("%d %d \n", start, end);

        dummy_vn = (struct VN*)malloc(sizeof(struct VN));

        exprVn = (struct VN*)malloc(sizeof(struct VN));
        dummy_exprVn = exprVn;

        vn2name = (struct VN*)malloc(sizeof(struct VN));
        dummy_vn2name = vn2name;


        vn = (struct VN*)malloc(sizeof(struct VN));
        vn = getNew(vn);

        dummy_vn -> next = vn;

        exprVn = getNew(exprVn);
        vn2name = getNew(vn2name);

        valueNum = 0;

        for(;start <= end; ++start) {
            //printf("value number: %d\n", valueNum); 
            char* op = (char*)malloc(20*sizeof(char));
            strcpy(op, opcode[cur_instr ->op]);

            //printf("%s\n", op);

            if(!strcmp(opcode[cur_instr ->op], "i2i")){
                char left[50];
                char digit[100];
                strcpy(left, "r");
                sprintf(digit, "%d", cur_instr -> srcs[0]);
                strcat(left, digit);

                //printf(" i2i src1 insert %s %s %s into VN table\n", opcode[cur_instr ->op], left, digit);

                if(searchVN(left, dummy_vn -> next) == -1) {
                    vn -> expression = (char*)malloc(100*sizeof(char));

                    strcpy(vn -> expression, left);

                    vn -> id = valueNum;
                    //printf("new vn %s\n", vn -> expression);
                    vn = getNew(vn);
                    //printf("%s\n", dummy_vn -> next -> expression);

                    ++valueNum;
                    //printf(" i2i src1 insert %s  %s into VN table\n", opcode[cur_instr ->op], left);
                }
                else{
                    //Value_Number* nodeL = searchNode(left, dummy_vn -> next);
                    char right[50];
                    strcpy(right, "r");
                    sprintf(digit, "%d", cur_instr -> defs[0]);
                    strcat(right, digit);

                    Value_Number* nodeR = searchNode(right, dummy_vn -> next);

                    if(nodeR){
                        nodeR -> id = valueNum++;
                    }
                    else{
                        vn -> expression = (char*)malloc(100*sizeof(char));

                        strcpy(vn -> expression, right);

                        vn -> id = valueNum;
                        //printf("new vn %s\n", vn -> expression);
                        vn = getNew(vn);
                        //printf("%s\n", dummy_vn -> next -> expression);

                        ++valueNum;
                        //printf(" i2i def1 insert %s %s %s into VN table\n", opcode[cur_instr ->op], right, digit);
                    }

                }
                cur_instr = cur_instr -> next;
                continue;
            }

            else if(strcmp(opcode[cur_instr ->op], "add") && strcmp(opcode[cur_instr ->op], "sub") && strcmp(opcode[cur_instr ->op], "mult") && strcmp(opcode[cur_instr ->op], "div") && strcmp(opcode[cur_instr ->op], "addI") && strcmp(opcode[cur_instr ->op], "subI") && strcmp(opcode[cur_instr ->op], "multI") && strcmp(opcode[cur_instr ->op], "divI")) {

                if (strcmp(opcode[cur_instr ->op], "write") && strcmp(opcode[cur_instr ->op], "cwrite") && strcmp(opcode[cur_instr ->op], "br") && strcmp(opcode[cur_instr ->op], "cbr") && strcmp(opcode[cur_instr ->op], "nop") && strcmp(opcode[cur_instr ->op], "halt")){
                    char right[50];
                    char digit[100];
                    strcpy(right, "r");
                    sprintf(digit, "%d", cur_instr -> defs[0]);
                    strcat(right, digit);
                    Value_Number* nodeR = searchNode(right, dummy_vn -> next);
                    if(nodeR){
                        nodeR -> id = valueNum++;
                    }
                    else{
                        vn -> expression = (char*)malloc(100*sizeof(char));

                        strcpy(vn -> expression, right);

                        vn -> id = valueNum;
                        //printf("new vn %s\n", vn -> expression);
                        vn = getNew(vn);
                        //printf("%s\n", dummy_vn -> next -> expression);

                        ++valueNum;
                        //printf(" else def1 insert %s %s %s into VN table\n", opcode[cur_instr ->op], right, digit);
                    }

                }

                cur_instr = cur_instr -> next;
                continue;
            }

            //printf("%s \n", opcode[cur_instr ->op]);


            char left[50];

            if(cur_instr -> srcs[0] != INT_MIN) { 
                strcpy(left, "r");
                char digit[100];
                sprintf(digit, "%d", cur_instr -> srcs[0]);
                strcat(left, digit);
                //printf("%s\n", left);  
                if(searchVN(left, dummy_vn -> next) == -1) {
                    //printf("notfound %s\n", left);
                    vn -> expression = (char*)malloc(100*sizeof(char));

                    strcpy(vn -> expression, left);

                    vn -> id = valueNum;
                    //printf("new vn %s\n", vn -> expression);
                    vn = getNew(vn);
                    //printf("%s\n", dummy_vn -> next -> expression);

                    ++valueNum;
                    //printf(" src1 insert %s  %s into VN table\n", opcode[cur_instr ->op], left);
                }

                if(cur_instr -> srcs[1] != INT_MIN) {
                    strcpy(left, "r");
                    sprintf(digit, "%d", cur_instr -> srcs[1]);
                    strcat(left, digit);
                    if(searchVN(left, dummy_vn -> next) == -1) {
                        //printf("notfound %s\n", left);
                        vn -> expression = (char*)malloc(100*sizeof(char));

                        strcpy(vn -> expression, left);

                        vn -> id = valueNum;
                        //printf("new vn %s : %d\n", vn -> expression, valueNum);
                        vn = getNew(vn);
                        //printf("%s\n", dummy_vn -> next -> expression);
                        ++valueNum;
                        //printf(" src2 insert %s %s into VN table\n", opcode[cur_instr ->op], left);
                    }
                }

            }

            if(cur_instr -> consts[0] != INT_MIN) {
                char digit[100];
                sprintf(digit, "%d", cur_instr -> consts[0]);

                if(searchVN(digit, dummy_vn -> next) == -1) {
                    //printf("notfound %s\n", digit);
                    vn -> expression = (char*)malloc(100*sizeof(char));

                    strcpy(vn -> expression, digit);

                    vn -> id = valueNum;
                    //printf("new vn %s\n", vn -> expression);
                    vn = getNew(vn);
                    //printf("%s\n", dummy_vn -> next -> expression);
                    ++valueNum;
                }

                //printf(" const1 insert %s %s into VN table\n", opcode[cur_instr ->op], digit);
            }

            //printf("left: %s\n", left);
            char expr[50];

            strcpy(expr, opcode[cur_instr ->op]);

            if(cur_instr -> srcs[0] != INT_MIN) {
                char digit[100];
                sprintf(digit, " %d", searchVN(getReg(cur_instr -> srcs[0]), dummy_vn -> next));
                strcat(expr, digit);
                if(cur_instr -> srcs[1] != INT_MIN) {
                    sprintf(digit, " %d", searchVN(getReg(cur_instr -> srcs[1]), dummy_vn -> next));
                    strcat(expr, digit);
                    // printf("%s\n", expr);
                }
                //printf("expr: %s\n", expr);
            }

            if(cur_instr -> consts[0] != INT_MIN) {
                char digit[100];
                sprintf(digit, "%d", cur_instr -> consts[0]);
                sprintf(digit, " %d", searchVN(digit, dummy_vn -> next));
                strcat(expr, digit);
                if (cur_instr -> consts[1] != INT_MIN) {
                    sprintf(digit, "%d", cur_instr -> consts[1]);
                    sprintf(digit, " %d", searchVN(digit, dummy_vn -> next));
                    strcat(expr, digit);
                }

            }


            //printf("%s\n", expr);
            int expr2Vn = searchVN(expr, dummy_exprVn -> next); 
            char right[50];
            strcpy(right, "r");
            char digit[100];
            sprintf(digit, "%d", cur_instr -> defs[0]);
            strcat(right, digit);


            if(expr2Vn != -1) {
                //printf("find: %s %s %d\n", expr, right, expr2Vn);

                char name[50];
                char* tmp_name = searchName( expr2Vn, dummy_vn2name -> next);
                if(tmp_name){
                    strcpy(name, tmp_name);
                    //printf("name: %s\n", name);
                }
                //printf("check: %d %d\n", searchVN(name, dummy_vn -> next), searchVN(expr, dummy_exprVn -> next));

                if(tmp_name && searchVN(name, dummy_vn -> next) == searchVN(expr, dummy_exprVn -> next)) {
                    //printf("reduce: %s %s\n", expr, right);

                    int name_reg = 0, i;
                    int len = strlen(name);
                    for(i=1; i<len; i++){
                        name_reg = name_reg * 10 + ( name[i] - '0' );
                    }

                    //printf("%d %d\n", name_reg, len);

                    cur_instr -> op = 39;

                    cur_instr -> srcs[0] = name_reg;
                    cur_instr -> srcs[1] = INT_MIN;
                    cur_instr -> consts[0] = INT_MIN;
                    cur_instr -> consts[1] = INT_MIN;

                    //printf("%s\n", name);

                }

                Value_Number* tmp = searchNode(right, dummy_vn -> next);

                if (tmp) {
                    //printf("update tmp: %d %d\n", tmp -> id, expr2Vn);
                    tmp -> id = expr2Vn;
                }
                else {
                    //printf("update tmp: %d\n", expr2Vn);
                    vn -> expression = (char*)malloc(100*sizeof(char));
                    strcpy(vn -> expression, expr);
                    vn -> id = expr2Vn;
                    vn = getNew(vn);
                }

            }
            else{

                Value_Number* tmp = searchNode(expr, dummy_exprVn -> next);   
                if (tmp) {
                    //printf("update tmp: %d %d\n", tmp -> id, expr2Vn);
                    tmp -> id = valueNum;
                }
                else{
                    exprVn -> expression = (char*)malloc(100*sizeof(char));
                    strcpy(exprVn -> expression, expr);
                    exprVn -> id = valueNum;
                    exprVn = getNew(exprVn);
                }

                tmp = searchNode(right, dummy_vn -> next);

                if (tmp){
                    //printf("update tmp: %d %d\n", tmp -> id, expr2Vn);
                    tmp -> id = valueNum;
                }
                else {
                    //printf("update tmp: %d\n", expr2Vn);
                    vn -> expression = (char*)malloc(100*sizeof(char));
                    strcpy(vn -> expression, right);
                    vn -> id = valueNum;
                    vn = getNew(vn);
                }

                tmp = searchNode( right, dummy_vn2name -> next);
                if(tmp){
                    tmp -> id = valueNum;
                }
                else{
                    vn2name -> id = valueNum;
                    vn2name -> expression = (char*)malloc(100*sizeof(char));
                    strcpy(vn2name -> expression, right);
                    vn2name = getNew(vn2name);
                    //printf("right: %s\n", right);
                }

                ++valueNum;
            }

            cur_instr = cur_instr -> next;
        }
        /*
           printf("\n");

           printf("=======Table======\n");

           printf("\n");

           printf("=======VN table======\n");

           printVN(dummy_vn -> next);

           printf("=======expr2VN table======\n");

           printVN(dummy_exprVn -> next);

           printf("=======Name table======\n");

           printVN(dummy_vn2name -> next);

           printf("=======Next Block======\n");

           printf("\n");
           */
        leader = leader -> next;
        last = last -> next;
    }

}


int get_new_label(iNode* dummy){
    iNode* cur_instr = dummy -> next;
    int newLabel = -1;

    while(cur_instr -> next) {

        if(cur_instr -> label_nop != NULL) {
            //printf("%s \n", cur_instr -> label_nop);
            int name_label = 0, i;
            int len = strlen(cur_instr -> label_nop);
            for(i=1; i<len; i++){
                name_label = name_label * 10 + ( cur_instr -> label_nop[i] - '0' );
            }
            if(name_label > newLabel) {
                newLabel = name_label;
            }
        }


        cur_instr = cur_instr -> next;
    }

    //printf("%d \n", newLabel);

    return newLabel;
}

int get_new_reg(iNode* dummy){
    iNode* cur_instr = dummy -> next;
    int newReg = -1;

    while(cur_instr -> next) {
        if(cur_instr -> srcs[0] != INT_MIN) {
            //printf("%s \n", cur_instr -> label_nop);
            if(cur_instr -> srcs[0] > newReg) {
                newReg = cur_instr -> srcs[0];
            }
        }

        if(cur_instr -> srcs[1] != INT_MIN) {
            //printf("%s \n", cur_instr -> label_nop);
            if(cur_instr -> srcs[1] > newReg) {
                newReg = cur_instr -> srcs[1];
            }
        }

        if(cur_instr -> defs[0] != INT_MIN) {
            //printf("%s \n", cur_instr -> label_nop);
            if(cur_instr -> defs[0] > newReg) {
                newReg = cur_instr -> defs[0];
            }
        }

        if(cur_instr -> defs[1] != INT_MIN) {
            //printf("%s \n", cur_instr -> label_nop);
            if(cur_instr -> defs[1] > newReg) {
                newReg = cur_instr -> defs[1];
            }
        }

        cur_instr = cur_instr -> next;
    }

    //printf("%d \n", newReg);

    return newReg;

}

iNode* getNode(iNode* dummy, int idx) {
    iNode* cur = dummy -> next;

    while(cur -> next) {
        if(idx == 0){
            return cur;
        }
        idx -= 1;
        cur = cur -> next;
    }

    return NULL;
}

bool check_op(iNode* dummy, char* op, int start, int end) {
    iNode* cur = dummy;

    while(cur -> next && start <= end) {
        if(!strcmp(opcode[cur -> op], op)){
            return true;
        }

        start += 1;
        cur = cur -> next;
    }


    return false;
}

void init_node(iNode* node, int op) {
    node -> op = op;
    node -> srcs[0] = INT_MIN;
    node -> srcs[1] = INT_MIN;
    node -> consts[0] = INT_MIN;
    node -> consts[1] = INT_MIN;
    node -> defs[0] = INT_MIN;
    node -> defs[1] = INT_MIN;
    node -> labels[0] = NULL;
    node -> labels[1] = NULL;
    node -> targets[0] = NULL;
    node -> targets[1] = NULL;
    node -> label_nop = NULL;
    node -> next = NULL;
}

void instructCpy(iNode* node, iNode* ori) {
    node -> op = ori -> op;
    node -> srcs[0] = ori -> srcs[0];
    node -> srcs[1] = ori -> srcs[1];
    node -> consts[0] = ori -> consts[0];
    node -> consts[1] = ori -> consts[1];
    node -> defs[0] = ori -> defs[0];
    node -> defs[1] = ori -> defs[1];

    node -> labels[0] = ori -> labels[0];
    node -> labels[1] = ori -> labels[1];
    node -> targets[0] = ori -> targets[1];
    node -> targets[1] = ori -> targets[1];
    node -> label_nop = ori -> label_nop;
    node -> next = NULL;
}

void unrolling(iNode* dummy) {
    //printf("=====unrolling=====\n");
    int cur_reg = get_new_reg(dummy);
    int cur_label = get_new_label(dummy);

    block* leader = dummy_block_leader -> next;
    block* last = dummy_block_last -> next;
    iNode* cur_un = NULL, *pre = NULL, *app = NULL;
    while(leader && last) {
        int start = leader -> id, end = last -> id;
        iNode* first_instr = getNode(dummy, start);
        iNode* last_instr = getNode(dummy, end);
        //printf( "%d: %s %d: %s \n", start, opcode[getNode(dummy, start) -> op], end, opcode[getNode(dummy, end) -> op]);
        if(!strcmp("cbr", opcode[last_instr -> op]) && !check_op(first_instr, "store", start, end) && !check_op(first_instr, "load", start, end)){
            //if(!strcmp("cbr", opcode[last_instr -> op])){
            //printf("%s \n", first_instr -> label_nop);
            //printf("%s %s\n", last_instr -> labels[0], last_instr -> labels[1]);
            if(first_instr -> label_nop && (!strcmp(first_instr -> label_nop, last_instr -> labels[0]) || !strcmp(first_instr -> label_nop, last_instr -> labels[1]))){
                //printf("find loop \n");
                iNode* addI = getNode(dummy, end-2);
                if(!strcmp(opcode[addI -> op], "addI") && addI -> consts[0] == 1) {
                    //printf("find loop: addI 1 \n");

                    iNode* tmp = dummy -> next;
                    bool inside = false;

                    char* l1 = (char*)malloc(50*sizeof(char));
                    char* l2 = (char*)malloc(50*sizeof(char));
                    strcpy(l1, "L");
                    strcpy(l2, "L");

                    char digit[100];
                    sprintf(digit, "%d", cur_label + 1);
                    strcat(l1, digit);

                    sprintf(digit, "%d", cur_label + 2);
                    strcat(l2, digit);


                    int r1 = cur_reg + 1;
                    int r2 = cur_reg + 2;

                    cur_label += 3;
                    cur_reg += 3;

                    while(tmp -> next) {
                        if(tmp == first_instr) {
                            inside = true;
                            tmp = tmp -> next;
                            continue;
                        }    
                        if(tmp == last_instr) {
                            inside = false;
                            tmp = tmp -> next;
                            continue;
                        }

                        if(!inside) {
                            if(!strcmp(opcode[tmp -> op], "cbr")) {
                                if(!strcmp(tmp -> labels[0], first_instr -> label_nop)){
                                    //printf("change cbr\n"); 
                                    strcpy(tmp -> labels[0], l1);
                                }
                                if(!strcmp(tmp -> labels[1], first_instr -> label_nop)){
                                    //printf("change cbr\n"); 
                                    strcpy(tmp -> labels[1], l1);
                                }
                            }
                        }

                        tmp = tmp -> next;
                    }


                    iNode* new_instr1 = (struct IR_Node*)malloc(sizeof(struct IR_Node));
                    init_node(new_instr1, 0);
                    new_instr1 -> label_nop = (char*)malloc(50*sizeof(char));
                    strcpy(new_instr1 -> label_nop, l1);

                    if(!app) {
                        app = new_instr1;
                    }

                    iNode* new_instr2 = (struct IR_Node*)malloc(sizeof(struct IR_Node));
                    init_node(new_instr2, 5);
                    //printf("%s \n", opcode[getNode(dummy, end - 2) -> op]);
                    new_instr2 -> srcs[0] = getNode(dummy, end - 2) -> srcs[0];
                    new_instr2 -> consts[0] = 4;
                    new_instr2 -> defs[0] = r1;

                    iNode* cmp = getNode(dummy, end - 1);
                    iNode* new_instr3 = (struct IR_Node*)malloc(sizeof(struct IR_Node));
                    init_node(new_instr3, cmp -> op);
                    new_instr3 -> srcs[0] = r1;
                    new_instr3 -> srcs[1] = cmp -> srcs[1];
                    new_instr3 -> defs[0] = r2;

                    iNode* new_instr4 = (struct IR_Node*)malloc(sizeof(struct IR_Node));
                    init_node(new_instr4, last_instr -> op);
                    new_instr4 -> srcs[0] = r2;
                    new_instr4 -> labels[1] = (char*)malloc(50*sizeof(char));
                    new_instr4 -> labels[0] = (char*)malloc(50*sizeof(char));
                    strcpy(new_instr4 -> labels[1], first_instr -> label_nop);
                    strcpy(new_instr4 -> labels[0], l2);

                    iNode* new_instr5 = (struct IR_Node*)malloc(sizeof(struct IR_Node));
                    init_node(new_instr5, 0);
                    new_instr5 -> label_nop = (char*)malloc(50*sizeof(char));
                    strcpy(new_instr5 -> label_nop, l2);

                    new_instr1 -> next = new_instr2;
                    new_instr2 -> next = new_instr3;
                    new_instr3 -> next = new_instr4;
                    new_instr4 -> next = new_instr5;

                    cur_un = new_instr5;

                    int times;
                    for(times = 0; times < 4; ++times) {
                        int j;
                        for(j = start + 1; j < end - 2; ++j) {
                            iNode* new_instr = (struct IR_Node*)malloc(sizeof(struct IR_Node));
                            iNode* ori = getNode(dummy, j);
                            //share the address except next.
                            instructCpy(new_instr, ori);
                            cur_un -> next = new_instr;
                            cur_un = new_instr;
                        }
                    }

                    iNode* new_instr6 = (struct IR_Node*)malloc(sizeof(struct IR_Node));
                    init_node(new_instr6, 5);
                    //printf("%s \n", opcode[getNode(dummy, end - 2) -> op]);
                    new_instr6 -> srcs[0] = getNode(dummy, end - 2) -> srcs[0];
                    new_instr6 -> consts[0] = 4;
                    new_instr6 -> defs[0] = getNode(dummy, end - 2) -> srcs[0];
                    cur_un -> next = new_instr6;
                    cur_un = new_instr6;

                    iNode* new_instr7 = (struct IR_Node*)malloc(sizeof(struct IR_Node));
                    init_node(new_instr7, 31);
                    //printf("%s \n", opcode[getNode(dummy, end - 2) -> op]);
                    new_instr7 -> labels[0] = l1;
                    cur_un -> next = new_instr7;
                    cur_un = new_instr7;

                }
            }

        }

        leader = leader -> next;
        last = last -> next;
        pre = last_instr;
        }
        //printf("end unrolling\n");
        if(cur_un && app) {
            pre -> next = app;
            cur_un -> next= (struct IR_Node*)malloc(sizeof(struct IR_Node));
            //printf("%s \n", opcode[pre -> op]);
            //
        }
    }



    void printInstruction(iNode* dummy) {
        //printf("--------------new instructions----------------\n");
        iNode* cur_instr = dummy -> next;

        char ins[100];

        while (cur_instr -> next) {
            if(cur_instr -> label_nop != NULL) {
                strcpy(ins, cur_instr -> label_nop);
                strcat(ins, ":\t");

                if(!strcmp(opcode[cur_instr -> op], "nop") || !strcmp(opcode[cur_instr -> op], "halt")) {
                    strcat(ins, opcode[cur_instr -> op]);
                    cur_instr = cur_instr->next;
                    printf("%s\n", ins);
                    fprintf(outfile, "%s\n", ins);
                    continue;
                }

            }
            else{
                strcpy(ins, "\t");
            }

            if(!strcmp(opcode[cur_instr -> op], "nop") || !strcmp(opcode[cur_instr -> op], "halt")) {
                strcat(ins, opcode[cur_instr -> op]);
                cur_instr = cur_instr->next;
                printf("%s\n", ins);
                fprintf(outfile, "%s\n", ins);
                continue;
            }

            strcat(ins, opcode[cur_instr -> op]);

            if(cur_instr -> srcs[0] != INT_MIN) {
                strcat(ins, " r");
                char digit[100];
                sprintf(digit, "%d", cur_instr -> srcs[0]);
                strcat(ins, digit);
            }

            if (cur_instr -> srcs[1] != INT_MIN) {
                strcat(ins, ", r");
                char digit[100];
                sprintf(digit, "%d", cur_instr -> srcs[1]);
                strcat(ins, digit);    
            }

            if(cur_instr -> consts[0] != INT_MIN) {
                char digit[100];
                if(cur_instr -> srcs[0] != INT_MIN || cur_instr -> srcs[1] != INT_MIN){
                    sprintf(digit, ", %d", cur_instr -> consts[0]);
                }
                else{
                    sprintf(digit, " %d", cur_instr -> consts[0]);
                }
                strcat(ins, digit);
            }
            if(!strcmp(opcode[cur_instr -> op], "write") || !strcmp(opcode[cur_instr -> op], "cwrite") || !strcmp(opcode[cur_instr -> op], "ouput") || !strcmp(opcode[cur_instr -> op], "coutput")){

            }
            else if(!strcmp(opcode[cur_instr -> op], "cbr") || !strcmp(opcode[cur_instr -> op], "br")) {
                strcat(ins, " ->");
            }
            else {
                strcat(ins, " =>");
            }

            if(cur_instr -> labels[0] != NULL) {
                strcat(ins, " ");
                strcat(ins, cur_instr -> labels[0]);
            }

            if(cur_instr -> labels[1] != NULL) {
                strcat(ins, ", ");
                strcat(ins, cur_instr -> labels[1]);
            }


            if(cur_instr -> defs[0] != INT_MIN) {
                strcat(ins, " r");
                char digit[100];
                sprintf(digit, "%d", cur_instr -> defs[0]);
                strcat(ins, digit);
            }

            if (cur_instr -> defs[1] != INT_MIN) {
                strcat(ins, ", r");
                char digit[100];
                sprintf(digit, "%d", cur_instr -> defs[1]);
                strcat(ins, digit);
            }

            printf("%s\n", ins);
            fprintf(outfile, "%s\n", ins);
            cur_instr = cur_instr -> next;
        }

    }


    int countfactorial(int i) {
        int fact = 0;    
        while(i % 2 == 0) {
            i /= 2;
            fact += 1;
        }
        if(i != 1){
            //printf("%d\n", fact);
            return -1;;
        }

        return fact;
    }

    void shift(iNode* dummy) {
        //printf("=======shift======\n");

        iNode* cur = dummy -> next;

        while(cur -> next) {
            if(cur -> op == 7) {
                int factor = countfactorial(cur -> consts[0]);
                //        printf("%d %d\n", cur -> consts[0], factor);
                if(factor != -1) {
                    cur -> op = 10;
                    cur -> consts[0] = factor;
                }
            }
            else if(cur -> op == 8){
                int factor = countfactorial(cur -> consts[0]);
                //printf("%d %d\n", cur -> consts[0], factor);
                if(factor != -1) {
                    cur -> op = 12;
                    cur -> consts[0] = factor;
                }
            }
            cur = cur -> next;
        }

    }

    int main(int argc, char** argv) {
        //Instruction* instructPtr = parse();
        static FILE *ilocFile = NULL;
        /*
           ilocFile = fopen(argv[1],"r");
           stdin = ilocFile;
           iNode* dummy = parseILOC();
           */
        outfile = fopen("output.i", "w"); 
        //printf("%s \n", opcode[dummy->next->op]);

        if(argc == 3) {
            ilocFile = fopen(argv[2], "r");
            stdin = ilocFile;
            iNode* dummy = parseILOC();
            blocks_leader = (block*)malloc(sizeof(block));
            blocks_last = (block*)malloc(sizeof(block));
            dummy_block_leader = blocks_leader;
            dummy_block_last = blocks_last; 

            if(!strcmp("-u", argv[1])) {
                buildBlock(dummy);
                unrolling(dummy);
                shift(dummy);
                printInstruction(dummy);            
            }
            else if(!strcmp("-v", argv[1])) {
                buildBlock(dummy);
                valueNumber(dummy);     
                shift(dummy);
                printInstruction(dummy);
            }
            else{
                printf("This optimizer only support Value Numbering and Loop unrolling.\n");
            }
        }
        else if(argc == 4) {
            ilocFile = fopen(argv[3], "r");
            stdin = ilocFile;
            iNode* dummy = parseILOC();

            if(!strcmp("-u", argv[1]) && !strcmp("-v", argv[2])){
                blocks_leader = (block*)malloc(sizeof(block));
                blocks_last = (block*)malloc(sizeof(block));

                dummy_block_leader = blocks_leader;
                dummy_block_last = blocks_last;

                buildBlock(dummy);
                unrolling(dummy);

                blocks_leader = (block*)malloc(sizeof(block));
                blocks_last = (block*)malloc(sizeof(block));

                dummy_block_leader = blocks_leader;
                dummy_block_last = blocks_last;
                buildBlock(dummy);
                valueNumber(dummy);
                shift(dummy);
                printInstruction(dummy);    
            }
            else if(!strcmp("-u", argv[2]) && !strcmp("-v", argv[1])){
                blocks_leader = (block*)malloc(sizeof(block));
                blocks_last = (block*)malloc(sizeof(block));

                dummy_block_leader = blocks_leader;
                dummy_block_last = blocks_last;

                buildBlock(dummy);
                valueNumber(dummy); 

                blocks_leader = (block*)malloc(sizeof(block));
                blocks_last = (block*)malloc(sizeof(block));

                dummy_block_leader = blocks_leader;
                dummy_block_last = blocks_last;
                buildBlock(dummy);
                unrolling(dummy);
                shift(dummy);
                printInstruction(dummy);       
            }
            else {
                 printf("This optimizer only support Value Numbering and Loop unrolling.\n");
            }
        }
        else{
                printf("The format is wrong.");
                printf("This optimizer only support Value Numbering and Loop unrolling.\n");
                printf("The format is ./opt [-v] [-u] output.i \n");
        }

        /*
           blocks_leader = (block*)malloc(sizeof(block));
           blocks_last = (block*)malloc(sizeof(block));

           dummy_block_leader = blocks_leader;
           dummy_block_last = blocks_last;

           buildBlock(dummy);
           unrolling(dummy);

           blocks_leader = (block*)malloc(sizeof(block));
           blocks_last = (block*)malloc(sizeof(block));

           dummy_block_leader = blocks_leader;
           dummy_block_last = blocks_last;
           buildBlock(dummy);
        //valueNumber(dummy);
        shift(dummy);
        printInstruction(dummy);
        */
        return 0;
    }
