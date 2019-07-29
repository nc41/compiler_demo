#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"


#define TABLE_SIZE 1024


static sym_entry **hashtable;

int hash(char *str){
	unsigned long hash = 5381;
	int c;
	while (c = *str++){
		hash = ((hash << 5) + hash) + c;
	}
	return hash % TABLE_SIZE;
}

void newHashTable(){
	int i = 0;
	hashtable = (sym_entry **) malloc(sizeof(sym_entry* ) *TABLE_SIZE);	
	for(i = 0; i < TABLE_SIZE; ++i){
		hashtable[i] = NULL;
	}
}


sym_entry* search(char* key){
	int entry = hash(key);
	int visited = 0;
	while(hashtable[entry] != NULL && visited < TABLE_SIZE){
		if(!strcmp(hashtable[entry] -> name, key))
			return hashtable[entry];
		entry++;
		entry %= TABLE_SIZE;
		visited++;
	}
	printf("Warning: The data does not exist in hashtable.\n");
	return NULL;
}

void insert(char* key, Types type, int base, int reg){
	int hashIdx = hash(key);
	int visited = 0;
	while(hashtable[hashIdx] != NULL){
		if(!strcmp(hashtable[hashIdx] -> name, key)){
			printf("Warning: The data exists in hashtable.\n");
			return;
		}
		++hashIdx;
		hashIdx %= TABLE_SIZE;
		visited++;
		if(visited == TABLE_SIZE){
			printf("Warning: The Hashtable is full of data.\n");
			return;
		}
	}
	
	sym_entry *data = (sym_entry *) malloc(sizeof(sym_entry));
	data -> name = (char *) malloc(strlen(key) + 5);
	strcpy(data->name, key);
	data -> type = type;
    data -> base = base;
    data -> isScalar = 1;
    data -> dim = 0;
    data -> reg = reg;
	hashtable[hashIdx] = data;
}

char* typestr(Types s){
    switch (s) {
        case TYPE_INT: 
            return "int"; 
            break;
        case TYPE_BOOL:
             return "bool";
             break;
        case TYPE_CHAR:
             return "char";
             break;
        default:
             return "Undefinded Type";
    }
}

void printTable(){
    int i = 0;
    printf("\t**********Stmbol Table**********\n");
    for(i = 0; i < TABLE_SIZE; ++i){
        if (hashtable[i] != NULL) {
            printf("variable name:%10s, type:%4s, FakeZeroBase:%10d, register:r%d, dimensions:%3d \n", hashtable[i]->name, typestr(hashtable[i]->type), hashtable[i] -> base, hashtable[i] -> reg, hashtable[i] -> dim);
        }
    }
    printf("\t**********Stmbol Table End**********\n");

}



