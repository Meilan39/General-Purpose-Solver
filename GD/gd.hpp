#ifndef GD
#define GD

#include <vector>
#include "../global/global.hpp"
#include "../parser/tree.hpp"
#include "matrix.hpp"

namespace gd {

    typedef enum {gd_overflow, gd_invalid_root, gd_invalid_log} Flag; 
    extern Flag flag;

    extern const double h; // (machine epsilon 1e-16)^{1/3}
    extern const int maxDepth;
    extern const int maxSearchDepth;
    extern const int maxZoomDepth;
    extern const double gradTolerance;
    extern const double amax;
    extern const double aepsilon;
    extern bool minimize;
    extern Matrix c1;
    extern Matrix c2;

    void gd(Node* head, Variables* variables, const char* path);

    int init(Node* F, Matrix &xk);
    int gradient(Node* F, const Matrix &xk, Matrix &gk);
    int line_search(Node* F, const Matrix &xk, const Matrix &pk, const Matrix& gk, Matrix &ak);
    int zoom(Node* F, const Matrix &xk, const Matrix &pk, const Matrix &gk, Matrix &bl, Matrix &br, Matrix& ak);

    int evaluate(Node* head, const Matrix &replace, double& value);
    int additive(Node* head, const Matrix &replace, double& value);
    int multiplicative(Node* head, const Matrix &replace, double& value);
    int exponential(Node* head, const Matrix &replace, double& value);
    int primary(Node* head, const Matrix &replace, double& value);
    int functions(Node* head, const Matrix &replace, double& value);
}

#endif