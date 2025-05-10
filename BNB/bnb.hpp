#ifndef BNB
#define BNB

#include "../simplex/simplex.hpp"

namespace bnb {
    using fvec = std::vector<double>;
    using ivec = std::vector<int>;

    extern bool minimize;
    extern bool maxReached;
    extern int insidx;
    extern const int maxDepth;

    void bnb(Node* head, Variables* variables, const char* path);
    void branch(Node* head, Variables* variables, const ivec &general, fvec &optimal, int depth);
    bool exists(Node* head, int idx, double number);
    Node* new_constraint(int idx, double number, bool left);
}

#endif