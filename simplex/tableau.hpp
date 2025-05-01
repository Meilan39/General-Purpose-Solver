#ifndef TABLEAU
#define TABLEAU

#include <vector>
#include <iostream>
#include "../parser/tree.hpp"
#include "../parser/token.hpp"

class Tableau {
 public:
   int row, column, varcount, transpose;
   std::vector<std::vector<double>> mat;
 public:
   Tableau(Node* head, Variables* variables);
   int solve();
   void print(FILE* fptr);
 private:
   void make_minimize(Node* head, Variables* variables);
   void make_maximize(Node* head, Variables* variables);
};

#endif