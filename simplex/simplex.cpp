#include "simplex.hpp"

void simplex::simplex(Node* head, Variables* variables, FILE* fptr) {
    Tableau tableau(head, variables);
    // tableau.solve();
    tableau.print(fptr);
}