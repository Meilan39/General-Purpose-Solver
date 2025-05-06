#ifndef LEX
#define LEX

#include <string.h>
#include "token.hpp"
#include "constants.hpp"

#define FILE_LINE_LENGTH 256
extern char l_filebuffer[FILE_LINE_LENGTH];

typedef struct l_Map l_Map;
typedef enum l_Types {
    /* common symbols */
    lt_plus = ct_terminator + 1, 
    lt_minus, 
    lt_dot, 
    lt_slash,
    lt_caret,
    lt_equal,
    lt_leq,
    lt_geq,
    lt_greater,
    lt_less,
    lt_comma,
    lt_semicolon,
    lt_h_parenthesis,
    lt_t_parenthesis,
    lt_h_bracket,
    lt_t_bracket,
    /* specific symbols */
    lt_e,
    lt_pi,
    lt_variable, 
    /* commands */
    lt_maximize,
    lt_minimize,
    lt_constrain,
    lt_general,
    lt_solution,
    /* fuinctions */
    lt_root,
    lt_sqrt,
    lt_log,
    lt_ln,
    lt_sin,
    lt_cos,
    lt_tan,
    /* terminator */
    lt_terminator
} l_Types;

struct l_Map {
    const char* value;
    const int key;
};
extern const l_Map l_map[];

int l_lex(Token* head, FILE* fptr);

int l_hash(char** s);
const char* l_unhash(int key);

int l_variable(char** s, Token* head, double* value);

#endif