#ifndef MATRIX
#define MATRIX

#include <vector>
#include <cmath>

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a > b ? b : a)

class Matrix {
 private:
    std::vector<std::vector<double>> mat;
    bool transpose = false;
 public:
    int row, col;
 public:
    Matrix(int r, int c, bool identity);
    Matrix(double constant);
    Matrix();
    Matrix T() const;
    double norm() const;
    double  at(int r, int c) const;
    double& at(int r, int c);
    void print() const;
};

Matrix operator-(const Matrix& A);
Matrix operator+(const Matrix& A, const Matrix& B);
Matrix operator-(const Matrix& A, const Matrix& B);
Matrix operator*(const Matrix& A, const Matrix& B);
Matrix operator/(const Matrix& A, const Matrix& B);

#endif