#include "simplex.hpp"

void simplex::simplex(Node* head, Variables* variables, const char* path, bool verbose) {
    Tableau tableau(head, variables);
    if(tableau.solve() == -1) return;
    tableau.solution(path, variables, verbose);
}