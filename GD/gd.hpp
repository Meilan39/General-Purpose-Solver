#ifndef GD
#define GD

#include <vector>
#include "../parser/tree.hpp"

namespace gd {

    typedef enum {gd_overflow, gd_invalid_root, gd_invalid_log} Flag; 
    extern Flag flag;

    void gd(Node* head, Variables* variables, const char* path);

    int evaluate(Node* head, const std::vector<double> &replace, double& value);
    int additive(Node* head, const std::vector<double> &replace, double& value);
    int multiplicative(Node* head, const std::vector<double> &replace, double& value);
    int exponential(Node* head, const std::vector<double> &replace, double& value);
    int primary(Node* head, const std::vector<double> &replace, double& value);
    int functions(Node* head, const std::vector<double> &replace, double& value);

}

#endif