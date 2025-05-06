#ifndef GD
#define GD

#include <vector>
#include "../parser/tree.hpp"

namespace gd {

    using Replace = std::vector<std::pair<int, double>>;

    void gd(Node* head, Variables* variables, const char* path);

    int evaluate(Node* head, const Replace &replace, double& value);
    int additive(Node* head, const Replace &replace, double& value);
    int multiplicative(Node* head, const Replace &replace, double& value);
    int exponential(Node* head, const Replace &replace, double& value);
    int primary(Node* head, const Replace &replace, double& value);
    int parenthesis(Node* head, const Replace &replace, double& value);
    int fraction(Node* head, const Replace &replace, double& value);

}

#endif GD