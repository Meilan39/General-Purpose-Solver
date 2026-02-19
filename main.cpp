#include <iostream>
#include <fstream>
#include "parser/lex.hpp"
#include "parser/tree.hpp"
#include "parser/token.hpp"
#include "parser/syn.hpp"
#include "simplex/simplex.hpp"
#include "BNB/bnb.hpp"
#include "GD/gd.hpp"

int main(int argc, char *argv[]) {
    if(argc == 1) { printf("Error: file path unspecified\n"); return 0; }
    if(argc >  2) { printf("Error: too many arguments\n"); return 0; }
    // open file
    FILE *fptr = fopen(argv[1], "r");
    if(fptr == NULL) { printf("Error: unable to open file\n"); return 1; } 
    // extract path
    std::string path(argv[1]);
    path = path.substr(0, path.find_last_of('.'));
    // construct tokens and objects
    Token* tokens = t_construct();
    Node* head = NULL;
    // parse
    if(l_lex(tokens, fptr)) {printf("Error: lexical error\n"); goto E;}
    if(s_syn(&head, tokens->next)) {printf("Error: syntax error\n"); goto E;}
    n_simplify(head);
    // solve
    printf("Solving...\n");
    switch(head->next[0]->type) {
        case nt_lp: simplex::simplex(head, tokens->variables, argv[1]); break;
        case nt_ilp: bnb::bnb(head, tokens->variables, argv[1]); break;
        case nt_unlp: gd::gd(head, tokens->variables, argv[1]); break;
        case nt_cnlp: gd::al(head, tokens->variables, argv[1]); break;
        default: break;
    }
    printf("Finished. Output to %s.sol\n", path.c_str());
E:  // close file
    fclose(fptr);
    t_destruct(tokens);
    n_free(head);
    return 0;
}