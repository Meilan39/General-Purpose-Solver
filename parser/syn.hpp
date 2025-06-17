#ifndef SYNTAX
#define SYNTAX

#include <stdio.h>
#include "token.hpp"
#include "lex.hpp"
#include "tree.hpp"

extern FILE* s_syntax_depth_map;

extern int s_minus_flag;
extern int s_divide_flag;

#define DEPTHMAP 1
#ifdef DEPTHMAP
    #define PRINTMAP(depth, name, token)                                        \
        if(depth>0) {                                                           \
            for (int i = 1; i < depth; i++)                                     \
                fprintf(s_syntax_depth_map, "│   ");                            \
            fprintf(s_syntax_depth_map, "├── ");                                \
        }                                                                       \
        fprintf(s_syntax_depth_map, "%s\n", name);        
        // if((*token) && c_types((*token)->type))                              
        //     fprintf(s_syntax_depth_map, "%s {%Lf}\n", name, (*token)->value); 
        // else                                                                  
#else
    #define PRINTMAP(fmt, ...)
#endif

int s_syn(Node** head, Token* token);
Node* s_compare(Token** token, int type);

Node* s_file(Token** token, int depth);
Node* s_lp(Token** token, int depth);
Node* s_ilp(Token** token, int depth);
Node* s_unlp(Token** token, int depth);
Node* s_cnlp(Token** token, int depth);
Node* s_linear_expression(Token** token, int depth);
Node* s_linear_additive(Token** token, int depth);
Node* s_linear_additive_suffix(Token** token, int depth);
Node* s_linear_multiplicative(Token** token, int depth);
Node* s_nonlinear_expression(Token** token, int depth);
Node* s_nonlinear_additive(Token** token, int depth);
Node* s_nonlinear_additive_suffix(Token** token, int depth);
Node* s_nonlinear_multiplicative(Token** token, int depth);
Node* s_nonlinear_multiplicative_suffix(Token** token, int depth);
Node* s_nonlinear_exponential(Token** token, int depth);
Node* s_nonlinear_exponential_suffix(Token** token, int depth);
Node* s_nonlinear_primary(Token** token, int depth);
Node* s_nonlinear_parenthesis(Token** token, int depth);
Node* s_functions(Token** token, int depth);
Node* s_root(Token** token, int depth);
Node* s_sqrt(Token** token, int depth);
Node* s_log(Token** token, int depth);
Node* s_ln(Token** token, int depth);
Node* s_sin(Token** token, int depth);
Node* s_cos(Token** token, int depth);
Node* s_tan(Token** token, int depth);
Node* s_linear_constraint(Token** token, int depth);
Node* s_nonlinear_constraint(Token** token, int depth);
Node* s_variable_constraint(Token** token, int depth);
Node* s_variable_bound(Token** token, int depth);
Node* s_real(Token** token, int depth);
Node* s_rational(Token** token, int depth);
Node* s_irrational(Token** token, int depth);
Node* s_objective(Token** token, int depth);
Node* s_sign(Token** token, int depth);
Node* s_comparison(Token** token, int depth);
Node* s_variable(Token** token, int depth);

#endif