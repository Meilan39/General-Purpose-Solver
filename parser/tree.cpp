#include "tree.hpp"

FILE* s_abstract_syntax_tree;

void n_simplify(Node* head) {
    n_print(head, "./meta/pt.txt"); // print parse tree
    n_compress(head);
    n_print(head, "./meta/ast.txt"); // print abstract syntax tree
}

void n_compress(Node* head) {
    n_compress_suffix(head);
    n_compress_chain(head);
    for(int i = 0; i < head->length; i++)
        n_compress(head->next[i]);  
}

Node* n_free(Node* head) {
    if(!head) return NULL;
    if(head->length) {
        for(int i = 0; i < head->length; i++)
            if(head->next[i]) n_free(head->next[i]);
        if(head->next) free(head->next);
    }
    free(head);
    return NULL;
}

Node* n_reset(Node* head) {
    if(!head) return NULL;
    if(head->length) {
        for(int i = 0; i < head->length; i++)
            n_free(head->next[i]);
        if(head->next) free(head->next);
    }
    head->next = NULL;
    head->length = 0;
    head->subtype = 0;
    head->value = 0;
    return head; 
}

Node* n_construct(int type, double value) {
    Node* head = (Node*)malloc(sizeof(Node));
    if(!head) return NULL;
    head->next = NULL;
    head->length = 0;
    head->type = type;
    head->subtype = 0;
    head->value = value;
    return head;
}

Node** n_findd(Node* head, int type) {
    Node** ref = NULL;
    while(head->type != type){
        if(head->length > 2) return NULL;
        if(head->length == 2) {
            if(ct_terminator >= head->next[0]->type || head->next[0]->type >= lt_terminator)
                return NULL;
            head = head->next[1];
            ref = &(head->next[1]);           
        } else {
            head = head->next[0];
            ref = &(head->next[0]);            
        }
    } return head->length == 1 ? NULL : ref;
}

int n_push(Node* head, Node* node) {
    if(!node) return 0;
    if((head->length)++) head->next = (Node**)realloc(head->next, head->length * sizeof(Node*));
    else head->next = (Node**)malloc(head->length * sizeof(Node*));
    head->next[head->length - 1] = node;   
    return 1;
}

int n_pushfront(Node* head, Node* node) {
    if(!node) return 0;
    if((head->length)++) {
        Node** temp = (Node**)malloc(head->length * sizeof(Node*));
        for(int i = 0; i < head->length - 1; i++)
            temp[i+1] = head->next[i];
        free(head->next);
        head->next = temp;
    } else head->next = (Node**)malloc(head->length * sizeof(Node*));
    head->next[0] = node;
    return 1;
}

int n_delete(Node* head, int n, ...) {
    if(!(n > 0)) return 0;
    va_list args;
    va_start(args, n);
    int idx = 0, next = va_arg(args, int);
    for(int i = 0; i < head->length; i++) {
        if(next == i) {
            n_free(head->next[i]);
            next = (--n) ? va_arg(args, int) : next;
            continue;
        }
        head->next[idx++] = head->next[i];
    }
    head->length = idx + 1;
    va_end(args);
    return 1;
}

int n_emplace(Node* head, int length) {
    if(head->next) {
        if(head->length > length) goto E;
        head->next = (Node**)realloc(head->next, length * sizeof(Node*));
        for(int i = head->length; i < length; i++)
            head->next[i] = NULL;
        head->length = length;
    } else {
        head->length = length;
        head->next = (Node**)calloc(sizeof(Node*), head->length);
    }
    return 0;
E:  printf("...emplace failed\n");
    return -1;
}

void n_print(Node* head, const char* path) {
    s_abstract_syntax_tree = fopen(path, "w");
    if(!s_abstract_syntax_tree) {printf("unable to open %s\n", path); return;}
    int state[100] = {0};
    n_helper(head, 0, 0, state);
    fclose(s_abstract_syntax_tree);
} 

void n_helper(Node* head, int depth, int edge, int state[]) {
    if(depth > 0) {
        for(int i = 1; i < depth; i++)
            fprintf(s_abstract_syntax_tree, state[i] ? "│   " : "    ");
        fprintf(s_abstract_syntax_tree, "%s", (edge ? "└── " : "├── "));
    }
    /* print to file */
    fprintf(s_abstract_syntax_tree, "%s", n_typtostr(head->type));
    if(c_types(head->type) || head->type == lt_variable) {
        fprintf(s_abstract_syntax_tree, " {%lf}", head->value);
    }
    if(head->type == nt_linear_multiplicative || head->type == nt_nonlinear_multiplicative) {
        if(head->subtype == 1) fprintf(s_abstract_syntax_tree, " |%s|", "positive");
        if(head->subtype == 2) fprintf(s_abstract_syntax_tree, " |%s|", "negative");
    }
    if(head->type == nt_nonlinear_exponential) {
        if(head->subtype == 1) fprintf(s_abstract_syntax_tree, " |%s|", "product");
        if(head->subtype == 2) fprintf(s_abstract_syntax_tree, " |%s|", "quotient");
    }
    fprintf(s_abstract_syntax_tree, "\n");
    /* edge processing */
    if(head->length > 0) {
        state[depth] = !edge;
        for (int i = 0; i < head->length; i++) {
            n_helper(head->next[i], depth + 1, i == (head->length - 1), state);
        }
        state[depth] = 0;
    }
}

void n_compress_chain(Node* head) {
    if(head->length==0) return;
    for(int i = 0; i < head->length; i++) {
        while(1) {
            Node* next = head->next[i];
            if(n_chain_exception(next->type)) break;
            if(next->length!=1) break;
            head->next[i] = next->next[0];
            free(next->next);
            free(next);                 
        }
    }
}

void n_compress_suffix(Node* head) {
    if(head->length==0) return;
    if( ! n_suffix_exception(head->type)) return;
    for(int i = 0; i < head->length; i++) {
        Node* next = head->next[i];
        if(head->type + 1 != next->type) continue;
        head->length += next->length - 1;
        Node** temp = (Node**)malloc(head->length * sizeof(Node*));
        int j = 0, k = 0, l = 0;
        for(; j < head->length; j++) {
            if(j < i || j >= i + next->length) {
                temp[j] = head->next[k++];
                continue;
            } else temp[j] = next->next[l++];
            if(j==i) k++;
        }
        free(head->next);
        head->next = temp;
        free(next->next);
        free(next);
    }
}

const char* n_typtostr(int type) {
    if(type < 1 || nt_terminator <= type) return n_typtostr_map[0];
    return n_typtostr_map[type] ? n_typtostr_map[type] : n_typtostr_map[0];
}
int n_suffix_exception(int type) {
    if(type < 1 || nt_terminator <= type) return 0;
    return n_suffix_exception_map[type];
}
int n_chain_exception(int type) {
    if(type < 1 || nt_terminator <= type) return 0;
    return n_chain_exception_map[type];
}

const char* const n_typtostr_map[nt_terminator] = {
    [0] = "",
    /* constants */
    [ct_number] = "number",
    [ct_decimal] = "decimal",
    [ct_zero] = "zero",
    /* lex */
    [lt_plus] = "plus",
    [lt_minus] = "minus",
    [lt_dot] = "dot", 
    [lt_slash] = "slash",
    [lt_caret] = "caret",
    [lt_equal] = "equal",
    [lt_leq] = "<=",
    [lt_geq] = ">=",
    [lt_greater] = ">",
    [lt_less] = "<",
    [lt_comma] = "comma",
    [lt_semicolon] = "semicolon",
    [lt_h_parenthesis] = "head parenthesis",
    [lt_t_parenthesis] = "tail parenthesis",
    [lt_h_bracket] = "head bracket",
    [lt_t_bracket] = "tail bracket",
    [lt_e] = "e",
    [lt_pi] = "pi",
    [lt_variable] = "variable",
    [lt_lp] = "LP",
    [lt_ilp] = "ILP",
    [lt_unlp] = "UNLP",
    [lt_cnlp] = "CNLP",
    [lt_maximize] = "Maximize",
    [lt_minimize] = "Minimize",
    [lt_constrain] = "Constraint",
    [lt_general] = "General",
    [lt_root] = "root",   
    [lt_sqrt] = "square root",  
    [lt_log] = "logarithm",   
    [lt_ln] = "natural logarithm",  
    [lt_sin] = "sin",  
    [lt_cos] = "cos",   
    [lt_tan] = "tan",  
    /* syn */
    [nt_file] = "file",
    [nt_lp] = "LP",
    [nt_ilp] = "ILP",
    [nt_unlp] = "UNLP",
    [nt_cnlp] = "CNLP",
    [nt_linear_expression] = "linear expression",
    [nt_linear_additive] = "linear additive",
    [nt_linear_additive_suffix] = "linear additive suffix",
    [nt_linear_multiplicative] = "linear multiplicative",
    [nt_nonlinear_expression] = "nonlinear expression",
    [nt_nonlinear_additive] = "nonlinear additive",
    [nt_nonlinear_additive_suffix] = "nonlinear additive suffix",
    [nt_nonlinear_multiplicative] = "nonlinear multiplicative",
    [nt_nonlinear_multiplicative_suffix] = "nonlinear multiplicative suffix",
    [nt_nonlinear_exponential] = "nonlinear exponential",
    [nt_nonlinear_exponential_suffix] = "nonlinear exponential suffix",
    [nt_nonlinear_primary] = "nonlinear primary",
    [nt_nonlinear_parenthesis] = "nonlinear parenthesis",
    [nt_linear_constraint] = "linear constraint",
    [nt_nonlinear_constraint] = "nonlinear constraint",
    [nt_variable_constraint] = "variable constraint",
    [nt_functions] = "functions",
    [nt_root] = "root",
    [nt_sqrt] = "sqrt",
    [nt_log] = "log",
    [nt_ln] = "ln",
    [nt_sin] = "sin",
    [nt_cos] = "cos",
    [nt_tan] = "tan",
    [nt_real] = "real number",
    [nt_rational] = "rational",   
    [nt_irrational] = "irrational",
    [nt_objective] = "objective",
    [nt_sign] = "sign",
    [nt_comparison] = "comparison",
    [nt_variable] = "variable",
};

const char n_chain_exception_map[] = {
    [nt_linear_additive] = 1,
    [nt_nonlinear_additive] = 1,
    [nt_linear_multiplicative] = 1,
    [nt_nonlinear_multiplicative] = 1,
    [nt_nonlinear_exponential] = 1,
    [nt_nonlinear_parenthesis] = 1,
    [nt_irrational] = 1,
    [nt_rational] = 1,
};

const char n_suffix_exception_map[] = {
    [nt_linear_additive] = 1,
    [nt_nonlinear_additive] = 1,
    [nt_linear_multiplicative] = 1,
    [nt_nonlinear_multiplicative] = 1,
    [nt_nonlinear_exponential] = 1,
};
