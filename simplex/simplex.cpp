#include "simplex.hpp"

void simplex::simplex(Node* head, Variables* variables, const char* path, bool verbose) {
    Tableau tableau(head, variables);
    if(tableau.solve() == -1) {
        if(tableau.flag == sf_unbounded)   printf("Error: unbounded constraints\n");
        if(tableau.flag == sf_infeasible)  printf("Error: infeasible constraints\n");
        if(tableau.flag == sf_nonstandard) printf("Error: non-standard input\n");
        return;
    }
    /* print solution */
    FILE* fptr = fopen(path, "a");
    if(fptr == NULL) {
        printf("Error: unable to open file");
        return;
    }
    fprintf(fptr, "\nSolution\n");
    std::vector<double> solution = tableau.solution(verbose);
    for(int i = 0; i < tableau.defined; i++) 
        fprintf(fptr, "\t%-3s = %7.2f;\n", variables->arr[i], solution[i]);
    for(int i = 0; verbose && (i < tableau.slack); i++) 
        fprintf(fptr, "\ts%d  = %7.2f;\n", i + 1, solution[tableau.defined + i]);
    /* print Z */
    fprintf(fptr, "\t%-3s = %7.2f;\n", "Z", solution.back());
    fclose(fptr);
}

std::vector<double> simplex::bnb(Node* head, Variables* variables) {
    std::vector<double> ret;
    Tableau tableau(head, variables);
    if(tableau.solve() == -1) {
        if(tableau.flag == sf_unbounded)   printf("Error: unbounded constraints\n");
        if(tableau.flag == sf_infeasible)  printf("Error: infeasible constraints\n");
        if(tableau.flag == sf_nonstandard) printf("Error: non-standard input\n");
        return ret;
    }
    return tableau.solution(false);
}