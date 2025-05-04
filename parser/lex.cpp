#include "lex.hpp"

char l_buffer[FILE_LINE_LENGTH];

int l_lex(Token* head, FILE* fptr) {
    int type;
    double value;
    while(fgets(l_buffer, FILE_LINE_LENGTH, fptr) != NULL) {
        // replace new line with null-terminator
        l_buffer[strcspn(l_buffer, "\n")] = '\0';
        // lexical analysis
        char* s = l_buffer;
        while(*s!='\0') {
            /* pass spaces */
            while(*s==' ') {s++;}
            if(*s=='\0') {break;}
            /* general init */
            type = 0;
            value = 0;
            /* numerical constants */
            if( (type = l_hash(&s)) )                   {goto P;}
            if( (type = c_constant(&s, &value)) )       {goto P;} 
            if( (type = l_variable(&s, head, &value)) ) {goto P;}
            /* lexical error */
            return -1;
        P:  /* pushback */
            t_push(head, type, value);
        }
    }
    return 0;
}

int l_hash(char** s) {
    const char* value;
    for(int i = 0; (value = l_map[i].value); i++) {
        if(strncmp(*s, value, strlen(value)) == 0) {
            *s += strlen(value);
            return l_map[i].key;
        }
    } return 0;
}

const char* l_unhash(int key) {
    for(int i = 0; l_map[i].key; i++) {
        if(key == l_map[i].key) {
            return l_map[i].value;
        }
    } return NULL;
}

int l_variable(char** s, Token* head, double* value) {
    char* ps = *s;
    if(('a' > **s || **s > 'z') && 
       ('A' > **s || **s > 'Z')) goto f;
    (*s)++;
    c_number(s);
    *value = v_push(head, ps, (int)(*s - ps));
    goto t;
f : *s = ps;
    return 0;
t : return lt_variable;
}

const l_Map l_map[] = {
    /* common symbols */
    {"+", lt_plus},
    {"-", lt_minus},
    {"*", lt_dot},
    {"/", lt_slash},
    {"^", lt_caret},
    {"=", lt_equal},
    {"<=", lt_leq},
    {">=", lt_geq},
    {">", lt_greater},
    {"<", lt_less},
    {",", lt_comma},
    {";", lt_semicolon},
    {"(", lt_h_parenthesis},
    {")", lt_t_parenthesis},
    {"{", lt_h_bracket},
    {"}", lt_t_bracket},
    /* specific symbols */
    {"e", lt_e},
    {"pi", lt_pi},
    {"variable", lt_variable},
    /* Modes */
    {"LP", lt_lp},
    {"ILP", lt_ilp},
    {"UNLP", lt_unlp},
    {"CNLP", lt_cnlp},
    /* commands */
    {"Maximize", lt_maximize},
    {"Minimize", lt_minimize},
    {"Constrain", lt_constrain},
    {"General", lt_general},
    {"Solution", lt_solution},
    /* functions */
    {"root", lt_root},
    {"sqrt", lt_sqrt},
    {"log", lt_log},
    {"ln", lt_ln},
    {"sin", lt_sin},
    {"cos", lt_cos},
    {"tan", lt_tan},
    /* end */
    {NULL, 0}
};