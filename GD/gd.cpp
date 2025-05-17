#include "gd.hpp"

gd::Flag gd::flag;

const double gd::h = 1e-6;
int gd::maxDepth = 20;
bool gd::minimize = false;
double gd::gradTolerance = 1;
Matrix gd::c (1e-4); 
Matrix gd::tau (0.5); 

void gd::gd(Node* head, Variables* variables, const char* path) {
    gd::minimize = head->next[0]->next[0]->type == lt_minimize;
    Node* F = head->next[0]->next[1];

    int depth = 0;
    Matrix Hk(variables->len, variables->len, true);
    Matrix gk(variables->len, 1, false);
    Matrix gt(variables->len, 1, false);
    Matrix pk(variables->len, 1, false);
    Matrix sk(variables->len, 1, false);
    Matrix yk(variables->len, 1, false);
    Matrix xk(variables->len, 1, false);
    Matrix ak(0);
    Matrix skyk(0);
    if(gd::init(F, xk) == -1) return;
    if(gd::gradient(F, xk, gk) == -1) return;

    while(depth < gd::maxDepth && gk.norm() > gd::gradTolerance) {
        xk.T().print(); // to remove
        /* step direction and size */
        pk = -Hk * gk;
        if(gd::line_search(F, xk, pk, gk, ak) == -1) return;
        sk = ak * pk;
        /* step */
        xk = xk + sk;
        /* next gradient, update hessian */
        if(gd::gradient(F, xk, gt) == -1) return;
        yk = gt - gk;
        gk = gt;
        skyk = sk.T() * yk;
        Hk = Hk + (skyk + yk.T() * Hk * yk) * (sk * sk.T()) / (skyk * skyk) 
                - (Hk * yk * sk.T() + sk * yk.T() * Hk) / skyk;
        depth++;
    }
    xk.T().print(); // to remove
}

int gd::init(Node* F, Matrix &xk) {
    for(int r = 0; r < xk.row; r++)
        xk.at(r, 0) = 1;
    return 0;
}

int gd::gradient(Node* F, Matrix &xk, Matrix &gk) {
    double temp, Fm, Fp, h2 = 2 * gd::h;
    for(int r = 0; r < gk.row; r++) {
        /* xk is ultimately unchanged */
        temp = xk.at(r, 0);
        xk.at(r, 0) = temp + gd::h;
        if(gd::evaluate(F, xk, Fp) == -1) goto E;
        xk.at(r, 0) = temp - gd::h;
        if(gd::evaluate(F, xk, Fm) == -1) goto E;
        gk.at(r, 0) = (Fp - Fm) / h2;
        xk.at(r, 0) = temp;
    }
    return 0;
E:  return -1;
}

/* backtracking with Armijo's condition f(X) - f(X + aP) >= -acm (m = Pt grad(f(X))) */
int gd::line_search(Node* F, const Matrix &xk, const Matrix &pk, const Matrix &gk, Matrix &ak) {
    double fx, fxap;
    Matrix cm = -gd::c * pk.T() * gk; // 1x1
    ak.at(0, 0) = 2; // init a0 = 1
    do {
        ak = gd::tau * ak;
        if(gd::evaluate(F, xk, fx) == -1) goto E;
        if(gd::evaluate(F, xk + ak * pk, fxap) == -1) goto E;
    } while(fx - fxap < (cm * ak).at(0, 0));
    return 0;
E:  return -1;
}

int gd::evaluate(Node* head, const Matrix &replace, double &value) {
    if(replace.col != 1) goto E;
    if(gd::additive(head, replace, value) == -1) goto E;
    if(gd::minimize) value = -value;
    return 0;
E:  return -1;
}
int gd::additive(Node* head, const Matrix &replace, double &value) {
    double a = 0, b;
    for(int i = 0; i < head->length; i++) {
        Node* next = head->next[i];
        if(gd::multiplicative(next, replace, b) == -1) goto E;
        a = next->subtype == 1 ? a + b : a - b;
        if(!isfinite(a)) { flag = gd_overflow; goto E; }
    }
    value = a;
    return 0;
E:  return -1;
}
int gd::multiplicative(Node* head, const Matrix &replace, double &value) {
    double a = 1, b;
    for(int i = 0; i < head->length; i++) {
        Node* next = head->next[i];
        if(gd::exponential(next, replace, b) == -1) goto E;
        a = next->subtype == 1 ? a * b : a / b;
        if(!isfinite(a)) { flag = gd_overflow; goto E; }
    }
    value = a;
    return 0;
E:  return -1;
}
int gd::exponential(Node* head, const Matrix &replace, double &value) {
    double a, b = 1;
    /* from right to left */
    for(int i = head->length - 1; i >= 0; i--) {
        Node* next = head->next[i];
        if(gd::primary(next, replace, a) == -1) goto E;
        b = pow(a, b);
        if(!isfinite(b)) { flag = gd_overflow; goto E; }
    }
    value = b;
    return 0;
E:  return -1;
}
int gd::primary(Node* head, const Matrix &replace, double &value) {
    Node* next = head->next[0];
    switch(next->type) {
        case nt_real: 
            value = n_get_value(next); 
            break;
        case lt_variable:
            value = replace.at(next->value, 0); 
            break;
        case nt_functions:
            if(gd::functions(next->next[0], replace, value) == -1) goto E;
            break;
        case nt_nonlinear_parenthesis:
            if(gd::additive(next->next[0], replace, value) == -1) goto E;
            break;
        default:
            printf("default\n");
    }
    return 0;
E:  return -1;
}
int gd::functions(Node* head, const Matrix &replace, double &value) {
    double base, expression, a;
    switch(head->type) {
        case nt_root:
            base = n_get_value(head->next[0]);
            if(gd::additive(head->next[1], replace, expression) == -1) goto E;
            a = pow(expression, 1 / base);
            if(!isfinite(a)) {
                flag = gd_invalid_root; goto E;
            }
            break;
        case nt_sqrt:
            if(gd::additive(head->next[0], replace, expression) == -1) goto E;
            a = sqrt(expression);
            if(!isfinite(a)) { flag = gd_overflow; goto E; }
            break;
        case nt_log:
            base = n_get_value(head->next[0]);
            if(gd::additive(head->next[1], replace, expression) == -1) goto E;
            if(base != 2 && base != 10 && base != M_E) {
                flag = gd_invalid_log; goto E;
            }
            if(base == 2)   a = log2(expression);
            if(base == 10)  a = log10(expression);
            if(base == M_E) a = log(expression);
            if(!isfinite(a)) { flag = gd_overflow; goto E; }
            break;
        case nt_ln:
            if(gd::primary(head->next[0], replace, expression) == -1) goto E;
            a = log(expression);
            if(!isfinite(a)) { flag = gd_overflow; goto E; }
            break;
        case nt_sin:
            if(gd::primary(head->next[0], replace, expression) == -1) goto E;
            a = sin(expression);
            if(!isfinite(a)) { flag = gd_overflow; goto E; }
            break;
        case nt_cos:
            if(gd::primary(head->next[0], replace, expression) == -1) goto E;
            a = cos(expression);
            if(!isfinite(a)) { flag = gd_overflow; goto E; }
            break;
        case nt_tan:
            if(gd::primary(head->next[0], replace, expression) == -1) goto E;
            a = tan(expression);
            if(!isfinite(a)) { flag = gd_overflow; goto E; }
            break;
        default:
            goto E;
    }
    value = a;
    return 0;
E:  return -1;
}