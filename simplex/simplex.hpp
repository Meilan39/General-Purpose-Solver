#ifndef SIMPLEX
#define SIMPLEX

#include "../parser/token.hpp"
#include "../parser/tree.hpp"
#include "tableau.hpp"

namespace simplex {

    void simplex(Node* head, Variables* variables, const char* path, bool verbose); 

    /* @brief simplex solver or bnb algorithm   */
    /* @return                                  */
    /* empty vector : infeasiable or unbounded  */
    /* vector :  */
    std::vector<double> bnb(Node* head, Variables* variables);

}

#endif