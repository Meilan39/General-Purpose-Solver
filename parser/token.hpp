#ifndef TOKEN
#define TOKEN

#include "stdlib.h"
#include "string.h"
#include "stdio.h"

typedef struct _Variables {
    char** arr;
    int len;
} Variables;

typedef struct _Token {
    Variables* variables;
    double value;
    int type;
    struct _Token* next;
} Token;

Token* t_construct();
Token* t_destruct(Token* head);
int t_push(Token* head, int type, double value);
int t_next(Token** curent);
void t_print(Token* head);

void v_construct(Token* _head);
void v_destruct(Token* _head);
int v_push(Token* _head, char* s, int num);
void v_print(Token* _head);

#endif