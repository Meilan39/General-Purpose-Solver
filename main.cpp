#include <iostream>
#include <fstream>
#include "parser/lex.hpp"
#include "parser/tree.hpp"
#include "parser/lex.hpp"
#include "parser/syn.hpp"

int main(int argc, char *argv[]) {
    if(argc == 1) printf("file path unspecified\n");
    for(int i = 1; i < argc; i++) {
        // open file
        FILE *fptr = fopen(argv[i], "r");
        if(fptr == NULL) { printf("unable to open file\n"); return 1; } 
        // construct tokens and objects
        Token* tokens = t_construct();
        Node* head = NULL;
        // lexical analysis
        if(l_lex(tokens, fptr)) {printf("lexical error\n"); goto E;}
        // syntactical analysis  
        if(s_syn(&head, tokens->next)) {printf("syntax error\n"); goto E;}
        // semantic analysis
        n_simplify(head);
        // no errors
        printf("compiled successfully\n");
    E:  // close file
        fclose(fptr);
        t_destruct(tokens);
        // n_free(head);
    }
    return 0;
}