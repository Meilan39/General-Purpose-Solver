#ifndef GD
#define GD

#include <algorithm>
#include <vector>
#include <random>
#include "../global/global.hpp"
#include "../parser/tree.hpp"
#include "matrix.hpp"

namespace gd {
    using uniform = std::uniform_real_distribution<>;

    struct Bound {
        double min;
        double max;
    };
    struct Point {
        Matrix xk;
        Matrix gk;
        double gnorm;
        Point(int len) : xk(len, 1, false), gk(len, 1, false) {}
    };
    /// @brief 
    typedef enum {gd_overflow, gd_invalid_root, gd_invalid_log} Flag; 
    extern Flag flag;
    
    extern FILE *fplot;
    extern FILE *fsample;

    extern const double h; // (machine epsilon 1e-16)^{1/3}
    extern const int maxDepth;
    extern const int maxSearchDepth;
    extern const int maxZoomDepth;
    extern const double sampleDensity; // sample size for N-d mesh
    extern const double gradTolerance; // terminating condition for gradient norm
    extern const double sampleThreshold; // sampling condition for gradient
    extern const double clusterThreshold; // clustering condition for diff * grad
    extern const double overlapThreshold;
    extern const double amax;
    extern const double aepsilon;
    extern bool minimize;
    extern Matrix c1;
    extern Matrix c2;
    extern Matrix half;

    void gd(Node* head, Variables* variables, const std::string& path);
    int BFGS(Node* F, Variables* variables, Point &point, std::vector<Bound> &bounds);

    std::vector<Point> mesh(Node* F, Variables* variables, std::vector<uniform> dists, int sampleSize);
    bool outbound(const std::vector<Bound> &bounds, const Matrix &xk);

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