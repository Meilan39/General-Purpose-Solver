#ifndef TREE
#define TREE

#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include "token.hpp"
#include "lex.hpp"

extern FILE* s_abstract_syntax_tree;

typedef struct NODE{
   struct NODE** next;
   int length;
   int type;
   int subtype;
   double value;
} Node;


Node* n_free(Node* head);
Node* n_reset(Node* head);
Node* n_construct(int type, double value);
Node** n_findd(Node* head, int type);
int n_push(Node* head, Node* node);
int n_pushfront(Node* head, Node* node);
int n_insert(Node* head, Node* node, int index);
int n_delete(Node* head, int index);
int n_emplace(Node* head, int length);
int n_replace(Node** head, Node* next);
void n_print(Node* head, const char* path);
void n_helper(Node* head, int depth, int endge, int state[]);
double n_get_value(Node* head);
/* simplify */
void n_simplify(Node* head);
void n_compress(Node* head);
void n_refactor(Node* head);
void n_compress_symbol(Node* head);
void n_compress_suffix(Node* head);
void n_compress_chain(Node* head);
/* exceptions */
const char* n_typtostr(int type);
int n_suffix_exception(int type);
int n_chain_exception(int type);

enum n_Types {
   nt_file = lt_terminator + 1,
   nt_lp,
   nt_ilp,
   nt_unlp,
   nt_cnlp,
   nt_linear_expression,
   nt_linear_additive,
   nt_linear_additive_suffix,
   nt_linear_multiplicative,
   nt_nonlinear_expression,
   nt_nonlinear_additive,
   nt_nonlinear_additive_suffix,
   nt_nonlinear_multiplicative,
   nt_nonlinear_multiplicative_suffix,
   nt_nonlinear_exponential,
   nt_nonlinear_exponential_suffix,
   nt_nonlinear_primary,
   nt_nonlinear_parenthesis,
   nt_linear_constraint,
   nt_nonlinear_constraint,
   nt_variable_constraint,
   nt_equality_constraint,
   nt_variable_bound,
   nt_functions,
   nt_root,
   nt_sqrt,
   nt_log,
   nt_ln,
   nt_sin,
   nt_cos,
   nt_tan,
   nt_real,
   nt_rational,   
   nt_irrational,
   nt_objective,
   nt_sign,
   nt_comparison,
   nt_variable,
   nt_terminator
};

/* exception map */
extern const char* const n_typtostr_map[nt_terminator];
extern const char n_suffix_exception_map[nt_terminator];
extern const char n_chain_exception_map[nt_terminator];

#endif
