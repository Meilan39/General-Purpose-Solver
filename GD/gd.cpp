#include "gd.hpp"

gd::Flag gd::flag;

const double gd::h = 1e-6;
int gd::maxDepth = 20;
int gd::maxSearchDepth = 10;
int gd::maxZoomDepth = 10;
bool gd::minimize = false;
double gd::gradTolerance = 1e-4;
Matrix gd::a1 (0.9); 
Matrix gd::a2 (1e-4);
Matrix gd::bmax (0.5); 

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
        /* step direction and size */
        pk = -gk; // -Hk * gk;
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
        xk.T().print();
        printf("%lf\n", gk.norm());
    }
}

int gd::init(Node* F, Matrix &xk) {
    for(int r = 0; r < xk.row; r++)
        xk.at(r, 0) = 1;
    return 0;
}

int gd::gradient(Node* F, const Matrix &xk, Matrix &gk) {
    double temp, Fm, Fp, h2 = 2 * gd::h;
    Matrix x = xk;
    for(int r = 0; r < gk.row; r++) {
        temp = x.at(r, 0);
        x.at(r, 0) = temp + gd::h;
        if(gd::evaluate(F, x, Fp) == -1) goto E;
        x.at(r, 0) = temp - gd::h;
        if(gd::evaluate(F, x, Fm) == -1) goto E;
        gk.at(r, 0) = (Fp - Fm) / h2;
        x.at(r, 0) = temp;
    }
    return 0;
E:  return -1;
}

int gd::line_search(Node* F, const Matrix &xk, const Matrix &pk, const Matrix &gk, Matrix &ak) {
    int depth = 0;
    double f0, fb, fbp;
    Matrix g0 = gk, gb = gk, gbl = gk;
    Matrix b(0), bp = b;
    if(gd::evaluate(F, xk, f0) == -1) goto E;
    while(depth < gd::maxSearchDepth) {
        bp = b; 
        b = 0.5 * (b + gd::bmax);

        if(gd::evaluate(F, xk + b*pk, fb) == -1) goto E;
        if(gd::evaluate(F, xk + bp*pk, fbp) == -1) goto E;
        if(gd::gradient(F, xk + b*pk, gb) == -1) goto E; 

        if((fb > f0 + (a1*b*g0.T()*pk).at(0,0)) || (fb >= fbp)) {
            if(gd::zoom(F, xk, pk, gk, bp, b, ak) == -1) goto E;
            break;
        } 
        if(abs((gb.T()*pk).at(0,0)) <= abs((gd::a2*g0.T()*pk).at(0, 0))) {
            ak = b;
            break;
        }
        if((gb.T()*pk).at(0,0) >= 0) {
            if(gd::zoom(F, xk, pk, gk, b, gd::bmax, ak) == -1) goto E;
            break;
        }
    }
    return 0;
E:  return -1;
}

int gd::zoom(Node* F, const Matrix &xk, const Matrix &pk, const Matrix &gk, Matrix &bl, Matrix &br, Matrix& ak) {
    int depth = 0; 
    double f0, fb, fbl;
    Matrix g0 = gk, gb = gk, gbl = gk;
    Matrix b(0);
    if(gd::evaluate(F, xk, f0) == -1) goto E;
    while(depth < gd::maxZoomDepth) {
        b = 0.5 * (bl + br);
        if(gd::evaluate(F, xk + b*pk, fb) == -1) goto E;
        if(gd::evaluate(F, xk + bl*pk, fbl) == -1) goto E;
        if(gd::gradient(F, xk + b*pk, gb) == -1) goto E;
        if(gd::gradient(F, xk + bl*pk, gbl) == -1) goto E;
        if((fb > f0 + (gd::a1*b*g0.T()*pk).at(0, 0)) || (fb >= fbl)) {
            br = b;
        } else {
            if(abs((gb.T()*pk).at(0,0)) <= abs((gd::a2*g0.T()*pk).at(0, 0))) {
                ak = b;
                break;
            }
            if(((br-bl)*(gbl.T()*pk)).at(0, 0) >= 0) {
                br = bl;
            }
            bl = br;
        }
        depth++;
    }
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

/*
int gd::zoom(Node* F, const Matrix &xk, const Matrix &pk, const Matrix &gk, Matrix &bl, Matrix &br, Matrix& ak) {
    int depth = 0; 
    double f0, fb, fbl;
    Matrix g0 = gk, gb = gk;
    if(gd::evaluate(F, xk, f0) == -1) goto E;
    while(depth < gd::maxZoomDepth) {
        Matrix b = 0.5 * (bl + br);
        if(gd::evaluate(F, xk + b*pk, fb) == -1) goto E;
        if(gd::evaluate(F, xk + bl*pk, fbl) == -1) goto E;
        if(gd::gradient(F, xk + b*pk, gb) == -1) goto E;

        // Armijo condition check
        if(fb > f0 + gd::a1*b*(g0.T()*pk).at(0,0) || fb >= fbl) {
            br = b;
        } else {
            // Check curvature condition
            if(std::abs((gb.T()*pk).at(0,0) <= gd::a2 * std::abs((g0.T()*pk).at(0,0)) {
                ak = b; // Assign the found step
                break;
            }
            // Update interval based on gradient at b
            if((gb.T()*pk).at(0,0) >= 0) {
                br = b;
            } else {
                bl = b;
            }
        }
        depth++;
    }
    return 0;
E:  return -1;
}

int gd::line_search(Node* F, const Matrix &xk, const Matrix &pk, const Matrix &gk, Matrix &ak) {
    int depth = 0;
    double f0, fb, fbp;
    Matrix g0 = gk, gb;
    Matrix b(0), bp;

    if(gd::evaluate(F, xk, f0) == -1) return -1;

    while(depth < gd::maxSearchDepth) {
        bp = b; 
        b = 0.5 * (b + gd::bmax);

        if(gd::evaluate(F, xk + b*pk, fb) == -1 || 
           gd::evaluate(F, xk + bp*pk, fbp) == -1 ||
           gd::gradient(F, xk + b*pk, gb) == -1) return -1;
        // Check Armijo or if fb not better than previous
        if(fb > f0 + gd::a1*b*(g0.T()*pk).at(0,0) || fb >= fbp) {
            if(gd::zoom(F, xk, pk, gk, bp, b, ak) == -1) return -1;
            break; // Exit after zoom
        }
        // Check curvature condition
        if(std::abs((gb.T()*pk).at(0,0)) <= gd::a2 * std::abs((g0.T()*pk).at(0,0))) {
            ak = b;
            break;
        }
        // Check gradient sign for interval update
        if((gb.T()*pk).at(0,0) >= 0) {
            if(gd::zoom(F, xk, pk, gk, bp, b, ak) == -1) return -1;
            break; // Exit after zoom
        }
        depth++;
    }
    return 0;
}

*/