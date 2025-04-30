#include <iostream>
#include <fstream>
#include "parser/lex.hpp"
#include "parser/tree.hpp"
#include "parser/token.hpp"
#include "parser/syn.hpp"
#include "simplex/simplex.hpp"

int main(int argc, char *argv[]) {
    if(argc == 1) printf("file path unspecified\n");
    for(int i = 1; i < argc; i++) {
        // open file
        FILE *fptr = fopen(argv[i], "r");
        if(fptr == NULL) { printf("unable to open file\n"); return 1; } 
        // construct tokens and objects
        Token* tokens = t_construct();
        Node* head = NULL;
        // parse
        if(l_lex(tokens, fptr)) {printf("lexical error\n"); goto E;}
        if(s_syn(&head, tokens->next)) {printf("syntax error\n"); goto E;}
        n_simplify(head);
        // solve
        switch(head->next[0]->type) {
            case nt_lp: simplex(head, tokens->variables); break;
            case nt_ilp:
            case nt_unlp:
            case nt_cnlp:
        }
    E:  // close file
        fclose(fptr);
        t_destruct(tokens);
        // n_free(head);
    }
    return 0;
}