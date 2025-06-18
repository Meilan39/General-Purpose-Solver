#include "syn.hpp"

FILE* s_syntax_depth_map;
int s_minus_flag;
int s_divide_flag;

int s_syn(Node** head, Token* token) {
    /* open depth map */
    s_syntax_depth_map = fopen("./meta/depthmap.txt", "w");
    if(!s_syntax_depth_map) { printf("unable to open depthmap\n"); return -1; }
    /* parse */
    *head = s_file(&token, 0);
    /* close depth map */
    fclose(s_syntax_depth_map);
    /* print abstract syntax tree */
    if( (*head)==NULL ) return -1;
    if( token!=NULL ) return -1;
    return 0;
}

Node* s_compare(Token** token, int type) {
    if( (*token)==NULL ) return NULL;
    if( (*token)->type == type ) {
        Node* node = n_construct(type, (*token)->value);
        t_next(token);
        return node;
    } return NULL;
}

Node* s_file(Token** token, int depth) {
    PRINTMAP(depth, "file", token)
    Node* node = n_construct(nt_file, 0);
    Token* ptoken = *token;
    if(!n_push(node, s_ilp(token, depth+1))) goto c2;
    goto t;
c2: *token = ptoken; n_reset(node);
    if(!n_push(node, s_lp(token, depth+1))) goto c3;
    goto t;
c3: *token = ptoken; n_reset(node);
    if(!n_push(node, s_cnlp(token, depth+1))) goto c4;
    goto t;
c4: *token = ptoken; n_reset(node);
    if(!n_push(node, s_unlp(token, depth+1))) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node; 
}
Node* s_lp(Token** token, int depth) {
    PRINTMAP(depth, "LP", token)
    Node* node = n_construct(nt_lp, 0);
    Token* ptoken = *token;
    if(!n_push(node, s_objective(token, depth+1))) goto f;
    if(!n_push(node, s_linear_expression(token, depth+1))) goto f;
    if(!s_compare(token, lt_semicolon)) goto f;
    if(!n_push(node, s_compare(token, lt_constrain))) goto f;
    while(n_push(node, s_linear_constraint(token, depth+1)));
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node; 
}
Node* s_ilp(Token** token, int depth) {
    PRINTMAP(depth, "ILP", token)
    Node* node = n_construct(nt_ilp, 0);
    Token* ptoken = *token;
    if(!n_push(node, s_objective(token, depth+1))) goto f;
    if(!n_push(node, s_linear_expression(token, depth+1))) goto f;
    if(!s_compare(token, lt_semicolon)) goto f;
    if(!n_push(node, s_compare(token, lt_constrain))) goto f;
    while(n_push(node, s_linear_constraint(token, depth+1)));
    if(!n_push(node, s_compare(token, lt_integer))) goto f;
    while(n_push(node, s_variable_constraint(token, depth+1)));
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node; 
}
Node* s_unlp(Token** token, int depth) {
    PRINTMAP(depth, "UNLP", token)
    Node* node = n_construct(nt_unlp, 0);
    Token* ptoken = *token;
    if(!n_push(node, s_objective(token, depth+1))) goto f;
    if(!n_push(node, s_nonlinear_expression(token, depth+1))) goto f;
    if(!s_compare(token, lt_semicolon)) goto f;
    if(!n_push(node, s_compare(token, lt_bound))) goto f;
    while(n_push(node, s_variable_bound(token, depth+1)));
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node; 
}
Node* s_cnlp(Token** token, int depth) {
    PRINTMAP(depth, "CNLP", token)
    Node* node = n_construct(nt_cnlp, 0);
    Token* ptoken = *token;
    if(!n_push(node, s_objective(token, depth+1))) goto f;
    if(!n_push(node, s_nonlinear_expression(token, depth+1))) goto f;
    if(!s_compare(token, lt_semicolon)) goto f;
    if(!n_push(node, s_compare(token, lt_bound))) goto f;
    while(n_push(node, s_variable_bound(token, depth+1)));
    if(!n_push(node, s_compare(token, lt_constrain))) goto f;
    while(n_push(node, s_nonlinear_constraint(token, depth+1)));
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node; 
}
/* ----------------------- linear -----------------------  */
Node* s_linear_expression(Token** token, int depth) {
    PRINTMAP(depth, "linear expression", token)
    Node* node = n_construct(nt_linear_expression, 0);
    Token* ptoken = *token;
    if(!n_push(node, s_linear_additive(token, depth+1))) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;    
}
Node* s_linear_additive(Token** token, int depth) {
    PRINTMAP(depth, "linear additive", token)
    Node* node = n_construct(nt_linear_additive, 0);
    Token* ptoken = *token;
    s_minus_flag = 0;
    if(!n_push(node, s_linear_multiplicative(token, depth+1))) goto f;
    while(n_push(node, s_linear_additive_suffix(token, depth+1)));
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_linear_additive_suffix(Token** token, int depth) {
    PRINTMAP(depth, "linear additive suffix", token)
    Node* node = n_construct(nt_linear_additive_suffix, 0);
    Token* ptoken = *token;
    s_minus_flag = 0; // could be a problem
    if(!s_compare(token, lt_plus)) goto c2; s_minus_flag = 0;
    if(!n_push(node, s_linear_multiplicative(token, depth+1))) goto c2;
    goto t;
c2: *token = ptoken; n_reset(node);
    if(!s_compare(token, lt_minus)) goto f; s_minus_flag = 1;
    if(!n_push(node, s_linear_multiplicative(token, depth+1))) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_linear_multiplicative(Token** token, int depth) {
    PRINTMAP(depth, "linear multiplicative", token)
    Node* node = n_construct(nt_linear_multiplicative, 0);
    Token* ptoken = *token;
    int pflag = s_minus_flag;
    s_sign(token, depth+1); 
    n_push(node, s_real(token, depth+1));
    s_compare(token, lt_dot);
    if(!n_push(node, s_variable(token, depth+1))) goto c2;
    node->subtype = s_minus_flag ? 2 : 1;
    goto t;
c2: *token = ptoken; n_reset(node); s_minus_flag = pflag;
    s_sign(token, depth+1);
    if(!n_push(node, s_real(token, depth+1))) goto f;
    node->subtype = s_minus_flag ? 2 : 1;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
/* ----------------------- nonlinear -----------------------  */
Node* s_nonlinear_expression(Token** token, int depth) {
    PRINTMAP(depth, "nonlinear expression", token)
    Node* node = n_construct(nt_nonlinear_expression, 0);
    Token* ptoken = *token;
    if(!n_push(node, s_nonlinear_additive(token, depth+1))) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;    
}
Node* s_nonlinear_additive(Token** token, int depth) {
    PRINTMAP(depth, "nonlinear additive", token)
    Node* node = n_construct(nt_nonlinear_additive, 0);
    Token* ptoken = *token;
    s_minus_flag = 0;
    if(!n_push(node, s_nonlinear_multiplicative(token, depth+1))) goto f;
    while(n_push(node, s_nonlinear_additive_suffix(token, depth+1)));
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_nonlinear_additive_suffix(Token** token, int depth) {
    PRINTMAP(depth, "nonlinear additive suffix", token)
    Node* node = n_construct(nt_nonlinear_additive_suffix, 0);
    Token* ptoken = *token;
    s_minus_flag = 0;
    if(!s_compare(token, lt_plus)) goto c2; s_minus_flag = 0;
    if(!n_push(node, s_nonlinear_multiplicative(token, depth+1))) goto c2;
    goto t;
c2: *token = ptoken; n_reset(node);
    if(!s_compare(token, lt_minus)) goto f; s_minus_flag = 1;
    if(!n_push(node, s_nonlinear_multiplicative(token, depth+1))) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_nonlinear_multiplicative(Token** token, int depth) {
    PRINTMAP(depth, "nonlinear multiplicative", token)
    Node* node = n_construct(nt_nonlinear_multiplicative, 0);
    Token* ptoken = *token;
    s_divide_flag = 0;
    s_sign(token, depth+1); 
    if(!n_push(node, s_nonlinear_exponential(token, depth+1))) goto f;
    while(n_push(node, s_nonlinear_multiplicative_suffix(token, depth+1)));
    node->subtype = s_minus_flag ? 2 : 1;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_nonlinear_multiplicative_suffix(Token** token, int depth) {
    PRINTMAP(depth, "nonlinear multiplicative suffix", token)
    Node* node = n_construct(nt_nonlinear_multiplicative_suffix, 0);
    Token* ptoken = *token;
    if(!s_compare(token, lt_dot)) goto c2; s_divide_flag = 0;
    if(!n_push(node, s_nonlinear_exponential(token, depth+1))) goto c2;
    goto t;
c2: *token = ptoken; n_reset(node);
    if(!s_compare(token, lt_slash)) goto c3; s_divide_flag = 1;
    if(!n_push(node, s_nonlinear_exponential(token, depth+1))) goto c3;
    goto t;
c3: *token = ptoken; n_reset(node);
    s_divide_flag = 0;
    if(!n_push(node, s_nonlinear_exponential(token, depth+1))) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_nonlinear_exponential(Token** token, int depth) {
    PRINTMAP(depth, "nonlinear exponential", token)
    Node* node = n_construct(nt_nonlinear_exponential, 0);
    Token* ptoken = *token;
    if(!n_push(node, s_nonlinear_primary(token, depth+1))) goto f;
    while(n_push(node, s_nonlinear_exponential_suffix(token, depth+1)));
    node->subtype = s_divide_flag ? 2 : 1;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_nonlinear_exponential_suffix(Token** token, int depth) {
    PRINTMAP(depth, "nonlinear exponential suffix", token)
    Node* node = n_construct(nt_nonlinear_exponential_suffix, 0);
    Token* ptoken = *token;
    if(!s_compare(token, lt_caret)) goto f;
    if(!n_push(node, s_nonlinear_primary(token, depth+1))) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_nonlinear_primary(Token** token, int depth) {
    PRINTMAP(depth, "nonlinear primary", token)
    Node* node = n_construct(nt_nonlinear_primary, 0);
    Token* ptoken = *token;
    if(!n_push(node, s_real(token, depth+1))) goto c2;
    goto t;
c2: *token = ptoken; n_reset(node);
    if(!n_push(node, s_functions(token, depth+1))) goto c3;
    goto t;
c3: *token = ptoken; n_reset(node);
    if(!n_push(node, s_variable(token, depth+1))) goto c4;
    goto t;
c4: *token = ptoken; n_reset(node);
    if(!n_push(node, s_nonlinear_parenthesis(token, depth+1))) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_nonlinear_parenthesis(Token** token, int depth) {
    PRINTMAP(depth, "nonlinear parenthesis", token)
    Node* node = n_construct(nt_nonlinear_parenthesis, 0);
    Token* ptoken = *token;
    if(!s_compare(token, lt_h_parenthesis)) goto c2;
    if(!n_push(node, s_nonlinear_expression(token, depth+1))) goto c2;
    if(!s_compare(token, lt_t_parenthesis)) goto c2;
    goto t;
c2: *token = ptoken; n_reset(node);
    if(!s_compare(token, lt_h_bracket)) goto f;
    if(!n_push(node, s_nonlinear_expression(token, depth+1))) goto f;
    if(!s_compare(token, lt_t_bracket)) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_functions(Token** token, int depth) {
    PRINTMAP(depth, "functions", token)
    Node* node = n_construct(nt_functions, 0);
    Token* ptoken = *token;
    if(!n_push(node, s_root(token, depth+1))) goto c2;
    goto t;
c2: *token = ptoken; n_reset(node);
    if(!n_push(node, s_sqrt(token, depth+1))) goto c3;
    goto t;
c3: *token = ptoken; n_reset(node);
    if(!n_push(node, s_log(token, depth+1))) goto c4;
    goto t;
c4: *token = ptoken; n_reset(node);
    if(!n_push(node, s_ln(token, depth+1))) goto c5;
    goto t;
c5: *token = ptoken; n_reset(node);
    if(!n_push(node, s_sin(token, depth+1))) goto c6;
    goto t;
c6: *token = ptoken; n_reset(node);
    if(!n_push(node, s_cos(token, depth+1))) goto c7;
    goto t;
c7: *token = ptoken; n_reset(node);
    if(!n_push(node, s_tan(token, depth+1))) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_root(Token** token, int depth) {
    PRINTMAP(depth, "root", token)
    Node* node = n_construct(nt_root, 0);
    Token* ptoken = *token;
    if(!s_compare(token, lt_root)) goto f;
    if(!s_compare(token, lt_h_parenthesis)) goto f;
    if(!n_push(node, s_compare(token, ct_number))) goto f;
    if(!s_compare(token, lt_comma)) goto f;
    if(!n_push(node, s_nonlinear_expression(token, depth+1))) goto f;
    if(!s_compare(token, lt_t_parenthesis)) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_sqrt(Token** token, int depth) {
    PRINTMAP(depth, "square root", token)
    Node* node = n_construct(nt_sqrt, 0);
    Token* ptoken = *token;
    if(!s_compare(token, lt_sqrt)) goto f;
    if(!n_push(node, s_nonlinear_primary(token, depth+1))) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_log(Token** token, int depth) {
    PRINTMAP(depth, "log", token)
    Node* node = n_construct(nt_log, 0);
    Token* ptoken = *token;
    if(!s_compare(token, lt_log)) goto f;
    if(!s_compare(token, lt_h_parenthesis)) goto f;
    if(!n_push(node, s_compare(token, ct_number))) goto f;
    if(!s_compare(token, lt_comma)) goto f;
    if(!n_push(node, s_nonlinear_expression(token, depth+1))) goto f;
    if(!s_compare(token, lt_t_parenthesis)) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_ln(Token** token, int depth) {
    PRINTMAP(depth, "natural log", token)
    Node* node = n_construct(nt_ln, 0);
    Token* ptoken = *token;
    if(!s_compare(token, lt_ln)) goto f;
    if(!n_push(node, s_nonlinear_primary(token, depth+1))) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_sin(Token** token, int depth) {
    PRINTMAP(depth, "sin", token)
    Node* node = n_construct(nt_sin, 0);
    Token* ptoken = *token;
    if(!s_compare(token, lt_sin)) goto f;
    if(!n_push(node, s_nonlinear_primary(token, depth+1))) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_cos(Token** token, int depth) {
    PRINTMAP(depth, "cos", token)
    Node* node = n_construct(nt_cos, 0);
    Token* ptoken = *token;
    if(!s_compare(token, lt_cos)) goto f;
    if(!n_push(node, s_nonlinear_primary(token, depth+1))) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_tan(Token** token, int depth) {
    PRINTMAP(depth, "tan", token)
    Node* node = n_construct(nt_tan, 0);
    Token* ptoken = *token;
    if(!s_compare(token, lt_tan)) goto f;
    if(!n_push(node, s_nonlinear_primary(token, depth+1))) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
/* ----------------------- end -----------------------  */
Node* s_linear_constraint(Token** token, int depth) {
    PRINTMAP(depth, "linear constraint", token)
    Node* node = n_construct(nt_linear_constraint, 0);
    Token* ptoken = *token;
    if(!n_push(node, s_linear_expression(token, depth+1))) goto f;
    if(!n_push(node, s_comparison(token, depth+1))) goto f;
    if(!n_push(node, s_linear_expression(token, depth+1))) goto f;
    if(!s_compare(token, lt_semicolon)) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node; 
}
Node* s_nonlinear_constraint(Token** token, int depth) {
    PRINTMAP(depth, "nonlinear constraint", token)
    Node* node = n_construct(nt_nonlinear_constraint, 0);
    Token* ptoken = *token;
    if(!n_push(node, s_nonlinear_expression(token, depth+1))) goto f;
    if(!n_push(node, s_comparison(token, depth+1))) goto f;
    if(!n_push(node, s_nonlinear_expression(token, depth+1))) goto f;
    if(!s_compare(token, lt_semicolon)) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node; 
}
Node* s_variable_constraint(Token** token, int depth) {
    PRINTMAP(depth, "variable constraint", token)
    Node* node = n_construct(nt_variable_constraint, 0);
    Token* ptoken = *token;
    if(!n_push(node, s_variable(token, depth+1))) goto f;
    if(!s_compare(token, lt_semicolon)) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node; 
}
Node* s_variable_bound(Token** token, int depth) {
    PRINTMAP(depth, "variable bound", token)
    Node* node = n_construct(nt_variable_bound, 0);
    Token* ptoken = *token;
    s_minus_flag = 0; s_sign(token, depth+1);
    if(!n_push(node, s_real(token, depth+1))) goto f;
    node->next[0]->subtype = s_minus_flag ? 2 : 1;
    if(!s_compare(token, lt_less)) goto f;
    if(!n_push(node, s_variable(token, depth+1))) goto f;
    if(!s_compare(token, lt_less)) goto f;
    s_minus_flag = 0; s_sign(token, depth+1);
    if(!n_push(node, s_real(token, depth+1))) goto f;
    node->next[2]->subtype = s_minus_flag ? 2 : 1;
    if(!s_compare(token, lt_semicolon)) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node; 
}
Node* s_real(Token** token, int depth) {
    PRINTMAP(depth, "real number", token)
    Node* node = n_construct(nt_real, 0);
    Token* ptoken = *token;
    if(!n_push(node, s_rational(token, depth+1))) goto c2;
    goto t;
c2: *token = ptoken; n_reset(node);
    if(!n_push(node, s_irrational(token, depth+1))) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_rational(Token** token, int depth) {
    PRINTMAP(depth, "rational", token)
    Node* node = n_construct(nt_rational, 0);
    Token* ptoken = *token;
    if(!n_push(node, s_compare(token, ct_decimal))) goto c2;
    goto t;
c2: *token = ptoken; n_reset(node);
    if(!n_push(node, s_compare(token, ct_number))) goto c3;
    goto t;
c3: *token = ptoken; n_reset(node);
    if(!n_push(node, s_compare(token, ct_zero))) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_irrational(Token** token, int depth) {
    PRINTMAP(depth, "irrational", token)
    Node* node = n_construct(nt_irrational, 0);
    Token* ptoken = *token;
    if(!n_push(node, s_compare(token, lt_e))) goto c2;
    goto t;
c2: *token = ptoken; n_reset(node);
    if(!n_push(node, s_compare(token, lt_pi))) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_objective(Token** token, int depth) {
    PRINTMAP(depth, "objective", token)
    Node* node = n_construct(nt_objective, 0);
    Token* ptoken = *token;
    if(!n_push(node, s_compare(token, lt_minimize))) goto c2;
    goto t;
c2: *token = ptoken; n_reset(node);
    if(!n_push(node, s_compare(token, lt_maximize))) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;
}
Node* s_sign(Token** token, int depth) {
    PRINTMAP(depth, "sign", token)
    Node* node = n_construct(nt_sign, 0);
    Token* ptoken = *token;
    if(!n_push(node, s_compare(token, lt_plus))) goto c2; 
    goto t;
c2: *token = ptoken; n_reset(node);
    if(!n_push(node, s_compare(token, lt_minus))) goto f; 
    s_minus_flag = !s_minus_flag;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;   
}
Node* s_comparison(Token** token, int depth) {
    PRINTMAP(depth, "comparison", token)
    Node* node = n_construct(nt_comparison, 0);
    Token* ptoken = *token;
    if(!n_push(node, s_compare(token, lt_equal))) goto c2; 
    goto t;
c2: *token = ptoken; n_reset(node);
    if(!n_push(node, s_compare(token, lt_leq))) goto c3;
    goto t;
c3: *token = ptoken; n_reset(node);
    if(!n_push(node, s_compare(token, lt_geq))) goto c4;
    goto t;
c4: *token = ptoken; n_reset(node);
    if(!n_push(node, s_compare(token, lt_greater))) goto c5;
    goto t;
c5: *token = ptoken; n_reset(node);
    if(!n_push(node, s_compare(token, lt_less))) goto f; 
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;   
}
Node* s_variable(Token** token, int depth) {
    PRINTMAP(depth, "variable", token)
    Node* node = n_construct(nt_variable, 0);
    Token* ptoken = *token;
    if(!n_push(node, s_compare(token, lt_variable))) goto f;
    goto t;
f : *token = ptoken;
    return n_free(node);
t : return node;   
}