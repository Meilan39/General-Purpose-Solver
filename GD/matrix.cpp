#include "matrix.hpp"


Matrix::Matrix(int r, int c, bool identity) {
    this->row = r;
    this->col = c;
    this->mat = std::vector<std::vector<double>>(r, std::vector<double>(c, 0));
    if(!identity) return;
    for(int r = 0; r < row; r++)
        for(int c = 0; c < col; c++)
            if(r==c) this->mat[r][c] = 1;
}

Matrix::Matrix(double constant) {
    this->row = 1;
    this->col = 1;
    this->mat = std::vector<std::vector<double>>(1, std::vector<double>(1, constant));
}

Matrix::Matrix() {
    this->row = 0;
    this->col = 0;
    this->mat = std::vector<std::vector<double>>(0, std::vector<double>(0));
}

Matrix Matrix::T() const { 
    Matrix R = *this;
    R.transpose = !R.transpose;
    R.row = this->col;
    R.col = this->row;
    return R;
}

double Matrix::norm() const {
    double sum = 0;
    for(int r = 0; r < this->row; r++)
        for(int c = 0; c < this->col; c++)
            sum += this->mat[r][c] * this->mat[r][c];
    return sqrt(sum);
}

double Matrix::at(int r, int c) const {
    return this->transpose ? this->mat.at(c).at(r) : this->mat.at(r).at(c); 
}

double& Matrix::at(int r, int c) {
    return this->transpose ? this->mat.at(c).at(r) : this->mat.at(r).at(c);
}

Matrix operator-(const Matrix& A) {
    Matrix R (A.row, A.col, false);
    for(int r = 0; r < A.row; r++)
        for(int c = 0; c < A.col; c++)
            R.at(r, c) = - A.at(r, c);
    return R;
}

void Matrix::print() const {
    for(int r = 0; r < this->row; r++) {
        for(int c = 0; c < this->col; c++) {
            printf("%lf ", this->at(r, c));
        } printf("\n");
    } printf("\n");
}

Matrix operator+(const Matrix& A, const Matrix& B) {
    bool unit = (A.row==1 && A.col==1) || (B.row==1 && B.col==1);
    if(((A.row != B.row) || (A.col != B.col)) && !unit) return Matrix(0, 0, false);
    Matrix R (max(A.row, B.row), max(A.col, B.col), false);
    double a, b;
    for(int r = 0; r < R.row; r++) {
        for(int c = 0; c < R.col; c++) {
            a = (A.row==1 && A.col==1) ? A.at(0, 0) : A.at(r, c);
            b = (B.row==1 && B.col==1) ? B.at(0, 0) : B.at(r, c);
            R.at(r, c) = a + b;
        }
    }
    return R;
}
Matrix operator-(const Matrix& A, const Matrix& B) {
    bool unit = (A.row==1 && A.col==1) || (B.row==1 && B.col==1);
    if(((A.row != B.row) || (A.col != B.col)) && !unit) return Matrix(0, 0, false);
    Matrix R (max(A.row, B.row), max(A.col, B.col), false);
    double a, b;
    for(int r = 0; r < R.row; r++) {
        for(int c = 0; c < R.col; c++) {
            a = (A.row==1 && A.col==1) ? A.at(0, 0) : A.at(r, c);
            b = (B.row==1 && B.col==1) ? B.at(0, 0) : B.at(r, c);
            R.at(r, c) = a - b;
        }
    }
    return R;
}
Matrix operator*(const Matrix& A, const Matrix& B) {
    bool unit = (A.row==1 && A.col==1) || (B.row==1 && B.col==1);
    if((A.col != B.row) && !unit) return Matrix(0, 0, false);
    double a, b;
    Matrix R (!unit ? A.row : max(A.row, B.row), !unit ? B.col : max(A.col, B.col), false);
    for(int r = 0; r < R.row; r++) {
        for(int c = 0; c < R.col; c++) {
            if(!unit) {
                for(int k = 0; k < A.col; k++) { // A.col or B.row
                    R.at(r, c) += A.at(r, k) * B.at(k, c);
                }
            } else {
                a = (A.row==1 && A.col==1) ? A.at(0, 0) : A.at(r, c);
                b = (B.row==1 && B.col==1) ? B.at(0, 0) : B.at(r, c);
                R.at(r, c) = a * b;
            }
        }
    }
    return R;
}
/* element wise division, same conditions as addition and subtraction */
Matrix operator/(const Matrix& A, const Matrix& B) {
    bool unit = (A.row==1 && A.col==1) || (B.row==1 && B.col==1);
    if(((A.row != B.row) || (A.col != B.col)) && !unit) return Matrix(0, 0, false);
    Matrix R (max(A.row, B.row), max(A.col, B.col), false);
    double a, b;
    for(int r = 0; r < R.row; r++) {
        for(int c = 0; c < R.col; c++) {
            a = (A.row==1 && A.col==1) ? A.at(0, 0) : A.at(r, c);
            b = (B.row==1 && B.col==1) ? B.at(0, 0) : B.at(r, c);
            R.at(r, c) = a / b;
        }
    }
    return R;
}