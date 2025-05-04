#include <iostream>
#include <fstream>
#include "parser/lex.hpp"
#include "parser/tree.hpp"
#include "parser/token.hpp"
#include "parser/syn.hpp"
#include "simplex/simplex.hpp"

int main(int argc, char *argv[]) {
    bool v = false;
    if(argc == 1) printf("Error: file path unspecified\n");
    if(argc >  2) {
        for(int i = 2; i < argc; i++) {
            if(strcmp(argv[2], "-v") == 0) v = true;
        }
    }
    // open file
    FILE *fptr = fopen(argv[1], "r");
    if(fptr == NULL) { printf("Error: unable to open file\n"); return 1; } 
    // construct tokens and objects
    Token* tokens = t_construct();
    Node* head = NULL;
    // parse
    if(l_lex(tokens, fptr)) {printf("Error: lexical error\n"); goto E;}
    if(s_syn(&head, tokens->next)) {printf("Error: syntax error\n"); goto E;}
    n_simplify(head);
    // solve
    switch(head->next[0]->type) {
        case nt_lp: simplex::simplex(head, tokens->variables, argv[1], v); break;
        case nt_ilp:
        case nt_unlp:
        case nt_cnlp:
        default: break;
    }
E:  // close file
    fclose(fptr);
    t_destruct(tokens);
    n_free(head);
    return 0;
}