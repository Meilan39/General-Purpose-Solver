#include "token.hpp"

Token* t_construct() {
    Token* head = (Token*)malloc(sizeof(Token));
    head->next = NULL;
    head->type = 0;
    head->value = 0;
    v_construct(head);
    return head;
}

Token* t_destruct(Token* head) {
    while(head) {
        Token* curr = head->next;
        free(head);
        head = curr;
    } return NULL;
}

int t_push(Token* head, int type, double value) {
    if(! (head)) goto E;
    for(; head->next; head = head->next);
    if(! (head->next = t_construct())) goto E;
    head->next->type = type;
    head->next->value = value;
    return 0;
 E: printf("token push allocation failed\n"); return -1;
}

int t_next(Token** current) {
    *current = (*current)->next;
    return *current ? 0 : -1;
}

void t_print(Token* head) {
    if(head) head = head->next;
    for(; head; head = head->next) {
        printf("%d: %f\n", head->type, head->value);
    }
}

void v_construct(Token* _head) {
    _head->variables = (Variables*)malloc(sizeof(Variables)); 
    Variables* head = _head->variables; 
    head->arr = NULL;
    head->len = 0;
}

void v_destruct(Token* _head) {
    Variables* head = _head->variables;
    if(head == NULL) return;
    for(int i = 0; i < head->len; i++) {
        free(head->arr[i]);
    } free(head->arr);
    free(head);
    _head->variables = NULL;
}

int v_push(Token* _head, char* s, int num) {
    Variables* head = _head->variables;
    if(head == NULL) return -1;
    for(int i = 0; i < head->len; i++) {
        if(strncmp(s, head->arr[i], strlen(head->arr[i]))==0) return i;
    }
    if((head->len)++) head->arr = (char**)realloc(head->arr, head->len * sizeof(char*));
    else head->arr = (char**)malloc(head->len * sizeof(char*));
    head->arr[head->len - 1] = strndup(s, num);  
    return head->len - 1;
}

void v_print(Token* _head) {
    Variables* head = _head->variables;
    if(head == NULL) return;
    for(int i = 0; i < head->len; i++) {
        printf("%s ", head->arr[i]);
    }
}