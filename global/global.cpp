#include "global.hpp"

bool cmp(const double &a, const char* s, const double &b) {
    if(strcmp(s, "<" )==0) return (b - a) > TOLERANCE;
    if(strcmp(s, "<=")==0) return (b - a) > -TOLERANCE;
    if(strcmp(s, "==")==0) return abs(a - b) < TOLERANCE; 
    if(strcmp(s, "!=")==0) return abs(a - b) > TOLERANCE;
    if(strcmp(s, ">=")==0) return (a - b) > -TOLERANCE; 
    if(strcmp(s, ">" )==0) return (a - b) > TOLERANCE;
    return 0;
}

bool isInt(const double &d) {
    return cmp(d,"==",round(d));
}