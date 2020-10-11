#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "symtab.h"
#include "util.h"

#define SIZE 551
#define SHIFT 4

static int hash ( char * key ){ 
    int temp = 0;
    int i = 0;
    while (key[i] != '\0'){ 
        temp = ((temp << SHIFT) + key[i]) % SIZE;
        ++i;
    }
    return temp;
}

typedef struct LineListRec
   { int lineno;
     struct LineListRec * next;
   } * LineList;

typedef struct BucketListRec
   { 
    char * name;
    char * scope;
    IdType id;
    DataType data;
     LineList lines;
     int memloc ;
     struct BucketListRec * next;
   } * BucketList;

static BucketList hashTable[SIZE];

void st_initialize(){
    int i;
    for(i = 0; i < SIZE; ++i)
        hashTable[i] = NULL;
}

void st_insert( char * name, char * scope, IdType id, DataType data, int lineno, int loc) { 
    int h = hash(name), first = 1;
    BucketList l =  hashTable[h], p = NULL;
    while(l != NULL) {
        if((strcmp(name, l->name) == 0) && (strcmp(scope, l->scope) == 0))
            break;
        p = l;
        l = l->next;
        first = 0;
    }

    if(l == NULL) { 
        l = (BucketList) malloc(sizeof(struct BucketListRec));
        l->name = name;
        l->scope = scope;
        l->id = id;
        l->data = data;
        l->lines = (LineList) malloc(sizeof(struct LineListRec));
        l->lines->lineno = lineno;
        l->memloc = loc;
        l->lines->next = NULL;
        l->next = NULL;
        if(first)
            hashTable[h] = l;
        else{
            p->next = l; 
        }
    }

    else { 
        LineList t = l->lines;
        while(t->next != NULL)
            t = t->next;
        t->next = (LineList) malloc(sizeof(struct LineListRec));
        t->next->lineno = lineno;
        t->next->next = NULL;
    }
} 

int st_lookup ( char * name, char * scope )
{ 	
    int h = hash(name);
    BucketList l =  hashTable[h];
    while(l != NULL){
        if((strcmp(name, l->name) == 0) && (strcmp(scope, l->scope) == 0))
            break;
        l = l->next;
    }
    if(l == NULL)
        return -1;
    else
        return l->memloc;
}

int st_getDataType(char * name, char * scope){
    int h = hash(name);
    BucketList l =  hashTable[h];
    while(l != NULL){
        if((strcmp(name, l->name) == 0) && (strcmp(scope, l->scope) == 0))
            break;
        l = l->next;
    }
    if(l == NULL)
        return -1;
    else
        return l->data;
}

int st_getIdType(char * name, char * scope){
    int h = hash(name);
    BucketList l =  hashTable[h];
    while(l != NULL){
        if((strcmp(name, l->name) == 0) && (strcmp(scope, l->scope) == 0))
            break;
        l = l->next;
    }
    if(l == NULL)
        return -1;
    else
        return l->id;
}

int getQuantidadeParametros(TreeNode * functionNode) {
    int qtd = 0;
    TreeNode * temp = functionNode->child[0];
    if(temp != NULL) {
        ++qtd;
        while(temp->sibling != NULL) {
            temp = temp->sibling;
            ++qtd;
        }
    }
    return qtd;
}

void printSymTab(FILE * listing)
{ int i;
  fprintf(listing,"Variavel         Escopo       Tipo         Tipo do Dado     Linha\n");
  fprintf(listing,"--------        --------     ------       --------------   -------\n");
  
for(i = 0; i < SIZE; ++i){
        if(hashTable[i] != NULL){
            BucketList l = hashTable[i];
            while(l != NULL){
                LineList t = l->lines;
                fprintf(listing, "%-16s ", l->name);
                fprintf(listing, "%-10s ", l->scope);
                switch(l->id){
                    case variableT:
                        fprintf(listing, "%-16s ", "variable");
                        break;
                    case vectorT:
                        fprintf(listing, "%-16s ", "vector");
                        break;
                    case functionT:
                        fprintf(listing, "%-16s ", "function");
                        break;
                }
                switch(l->data){
                    case voidT:
                        fprintf(listing, "%-12s ", "void");
                        break;
                    case integerT:
                        fprintf(listing, "%-12s ", "integer");
                        break;
                }
                while (t != NULL){
                    fprintf(listing, "%4d ", t->lineno);
                    t = t->next;
                }
                fprintf(listing, "\n");
                l = l->next;
            }
        }
    }
} /* printSymTab */
