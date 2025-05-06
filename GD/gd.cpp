#include "gd.hpp"


void gd::gd(Node* head, Variables* variables, const char* path);

int gd::evaluate(Node* head, const Replace &replace, double &value) {
    if(head->type != nt_nonlinear_additive) return 1;
    if(gd::additive(head, replace, value) == -1) goto E;
    return 0;
E:  return -1;
}
int gd::additive(Node* head, const Replace &replace, double &value) {
    double a = 0, b;
    for(int i = 0; i < head->length; i++) {
        Node* next = head->next[i];
        if(gd::multiplicative(next, replace, b) == -1) goto E;
        a = next->subtype == 1 ? a + b : a - b;
        if(!isfinite(a)) goto E;
    }
    value = a;
    return 0;
E:  return -1;
}
int gd::multiplicative(Node* head, const Replace &replace, double &value) {
    double a = 1, b;
    for(int i = 0; i < head->length; i++) {
        Node* next = head->next[i];
        if(gd::exponential(next, replace, b) == -1) goto E;
        a = next->subtype == 1 ? a * b : a / b;
        if(!isfinite(a)) goto E;
    }
    value = a;
    return 0;
E:  return -1;
}
int gd::exponential(Node* head, const Replace &replace, double &value) {
    double a, b = 1;
    /* from right to left */
    for(int i = head->length - 1; i >= 0; i--) {
        Node* next = head->next[i];
        if(gd::parenthesis(next, replace, a) == -1) goto E;
        b = pow(a, b);
        if(!isfinite(b)) goto E;
    }
    value = b;
    return 0;
E:  return -1;
}
int gd::primary(Node* head, const Replace &replace, double &value) {
    Node* next = head->next[0];
    return 0;
E:  return -1;
}
int gd::parenthesis(Node* head, const Replace &replace, double &value) {

}
int gd::fraction(Node* head, const Replace &replace, double &value);



// int nonvariable_function(Node* head, double* value) {
//     double base, expression, a;
//     switch(head->type) {
//         case nt_root:
//             if(get_numeric(head->next[2], &base) == -1) goto E;
//             if(nonvariable_addition(head->next[5], &expression) == -1) goto E;
//             a = pow(expression, 1 / base);
//             if(!isfinite(a)) {
//                 printf("...unsupported root base\n");
//                 return -1;
//             }
//             break;
//         case nt_sqrt:
//             if(nonvariable_addition(head->next[2], &expression) == -1) goto E;
//             a = sqrt(expression);
//             if(!isfinite(a)) goto E;
//             break;
//         case nt_log:
//             if(get_numeric(head->next[2], &base) == -1) goto E;
//             if(nonvariable_addition(head->next[5], &expression) == -1) goto E;
//             if(base != 2 && base != 10 && base != GLOBAL_E) {
//                 printf("...unsupported log base\n");
//                 return -1;
//             }
//             if(base == 2)        a = log2(expression);
//             if(base == 10)       a = log10(expression);
//             if(base == GLOBAL_E) a = log(expression);
//             if(!isfinite(a)) goto E;
//             break;
//         case nt_ln:
//             if(get_numeric(head->next[2], &expression) == -1) goto E;
//             a = log(expression);
//             if(!isfinite(a)) goto E;
//             break;
//         case nt_sin:
//             if(get_numeric(head->next[2], &expression) == -1) goto E;
//             a = sin(expression);
//             if(!isfinite(a)) goto E;
//             break;
//         case nt_cos:
//             if(get_numeric(head->next[2], &expression) == -1) goto E;
//             a = cos(expression);
//             if(!isfinite(a)) goto E;
//             break;
//         case nt_tan:
//             if(get_numeric(head->next[2], &expression) == -1) goto E;
//             a = tan(expression);
//             if(!isfinite(a)) goto E;
//             break;
//         case nt_asin:
//             if(get_numeric(head->next[2], &expression) == -1) goto E;
//             a = asin(expression);
//             if(!isfinite(a)) {
//                 printf("...unsupported arcsin range (-1 ~ 1)");
//                 return -1;
//             }
//             break;
//         case nt_acos:
//             if(get_numeric(head->next[2], &expression) == -1) goto E;
//             a = acos(expression);
//             if(!isfinite(a)) {
//                 printf("...unsupported arccosine range (-1 ~ 1)");
//                 return -1;
//             }
//             break;
//         case nt_atan:
//             if(get_numeric(head->next[2], &expression) == -1) goto E;
//             a = atan(expression);
//             if(!isfinite(a)) goto E;
//             break;
//         case nt_sinh:
//             if(get_numeric(head->next[2], &expression) == -1) goto E;
//             a = atan(expression);
//             if(!isfinite(a)) goto E;
//             break;
//         case nt_cosh:
//             if(get_numeric(head->next[2], &expression) == -1) goto E;
//             a = atan(expression);
//             if(!isfinite(a)) goto E;
//             break;
//         case nt_tanh:
//             if(get_numeric(head->next[2], &expression) == -1) goto E;
//             a = atan(expression);
//             if(!isfinite(a)) goto E;
//             break;
//         default:
//             printf("...invalid input for nonvariable resolve\n");
//             goto E;
//     }
//     return 0;
// E:  printf("...double overflow\n");
//     return -1;
// }
