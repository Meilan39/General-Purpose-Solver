#ifndef TABLEAU
#define TABLEAU

#include <vector>
#include "../parser/tree.hpp"
#include "../parser/token.hpp"

class Tableau {
 public:
    int row, column;
    std::vector<std::vector<int>> arr;
 public:
    Tableau(Node* head, Variables* variables);
    int solve();
    void print(FILE* fptr);
 private:
    
}

#endif