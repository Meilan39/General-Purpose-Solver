#include "constants.hpp"

char c_buffer[256];

int c_constant(char** s, double* value) {
    errno = 0;
    int type = 0;
    char *ps = *s, *end;
    if(c_decimal(s)) {type = ct_decimal; goto P;}
    if(c_number(s)) {type = ct_number; goto P;}
    if(c_compare(s, c_zero, 1)) {type = ct_zero; goto P;}
    return 0;
P:  /* convert value */
    strncpy(c_buffer, ps, (*s)-ps);
    c_buffer[(*s)-ps] = '\0';
    *value = strtold(c_buffer, &end);
    if(errno == ERANGE) {
        printf("...double overflow of a input literal\n");
        return 0;
    }
    return type;
}
int c_types(int type) {
    return (ct_number <= type && type <= ct_zero);
}

int c_compare(char** s, char hash[][3], int keyCount) {
    for(int i = 0; i < keyCount; i++) {
        if(strncmp(*s, hash[i], strlen(hash[i]))==0) {
            *s += strlen(hash[i]);
            return 1;
        }
    } return 0;
}

int c_number(char** s) {
    char* ps = *s;
    if(!c_compare(s, c_nonzero, 9)) goto f;
    while(c_digit(s));
    goto t;
f : *s = ps;
    return 0;
t : return 1;
}
int c_decimal(char** s) {
    char* ps = *s;
    if(!c_compare(s, c_nonzero, 9)) goto f;
    while(c_digit(s));
    if(!c_compare(s, c_period, 1)) goto f;
    while(c_digit(s));  
    goto t;
f : *s = ps;
    return 0;
t : return 1;
}
int c_digit(char** s) {
    char* ps = *s;
    if(!c_compare(s, c_zero, 1)) goto c2;
    goto t;
c2: *s = ps;
    if(!c_compare(s, c_nonzero, 9)) goto f;
    goto t;
f : *s = ps;
    return 0;
t : return 1;
}

char c_period[1][3] = {"."};
char c_nonzero[9][3] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};
char c_zero[1][3] = {"0"};