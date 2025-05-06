#ifndef TABLEAU
#define TABLEAU

#include <vector>
#include <iostream>
#include "../global/global.hpp"
#include "../parser/tree.hpp"
#include "../parser/token.hpp"

typedef enum simplex_flag {sf_unbounded = 1, sf_infeasible = 2} Simplex_Flag; 

class Tableau {
public:
  int row, column, defined, slack, artificial;
  int flag;
  bool minimize;
  std::vector<std::vector<double>> mat;
public:
  Tableau(Node* head, Variables* variables);
  int solve();
  std::vector<double> solution(bool verbose);
private:
  int get_pivot(std::pair<int, int> &piv, int obj_row);
  bool optimal(int obj_row);
  void pivot(const std::pair<int, int> &piv);
  void print();
};

#endif