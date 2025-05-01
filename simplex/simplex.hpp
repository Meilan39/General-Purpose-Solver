#ifndef SIMPLEX
#define SIMPLEX

#include "../parser/token.hpp"
#include "../parser/tree.hpp"
#include "tableau.hpp"

namespace simplex {

    void simplex(Node* head, Variables* variables, FILE* fptr); 

}

#endif