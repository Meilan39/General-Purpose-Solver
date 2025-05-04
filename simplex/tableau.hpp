#ifndef TABLEAU
#define TABLEAU

#define TOLERANCE 0.001

#include <vector>
#include <iostream>
#include "../parser/tree.hpp"
#include "../parser/token.hpp"

class Tableau {
public:
  int row, column, defined, slack, artificial;
  bool minimize;
  std::vector<std::vector<double>> mat;
public:
  Tableau(Node* head, Variables* variables);
  int solve();
  void solution(const char* path, Variables* variables, bool verbose);
private:
  int get_pivot(std::pair<int, int> &piv, int obj_row);
  bool optimal(int obj_row);
  void pivot(const std::pair<int, int> &piv);
  void print();
  bool cmp(const double &a, const char* s, const double &b);
};

#endif