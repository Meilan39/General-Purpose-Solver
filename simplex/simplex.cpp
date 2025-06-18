#include "simplex.hpp"

void simplex::simplex(Node* head, Variables* variables, const std::string& path) {
    Tableau tableau(head, variables);
    if(tableau.solve() == -1) {
        if(tableau.flag == sf_unbounded)   printf("Error: unbounded constraints\n");
        if(tableau.flag == sf_infeasible)  printf("Error: infeasible constraints\n");
        if(tableau.flag == sf_nonstandard) printf("Error: non-standard input\n");
        return;
    }
    std::vector<double> solution = tableau.solution();
    /* print solution */
    std::string sol = path;
    sol.replace(sol.rfind('.'), sol.back(), ".sol");
    FILE* fptr = fopen(sol.c_str(), "w");

    if(fptr == NULL) {
        printf("Error: unable to open file");
        return;
    }
    fprintf(fptr, "Solution file for : %s\n\n", path.c_str());
    fprintf(fptr, "Optimum : %lf\n", solution.back());
    for(int i = 0; i < tableau.defined; i++) 
        fprintf(fptr, "\t%-3s = %7.2lf;\n", variables->arr[i], solution[i]);
    fprintf(fptr, "Slack Variables\n");
    for(int i = 0; i < tableau.slack; i++) 
        fprintf(fptr, "\ts%d  = %7.2lf;\n", i + 1, solution[tableau.defined + i]);
    fclose(fptr);
}

std::vector<double> simplex::bnb(Node* head, Variables* variables) {
    std::vector<double> ret;
    Tableau tableau(head, variables);
    if(tableau.solve() == -1) {
        // if(tableau.flag == sf_unbounded)   printf("Error: unbounded constraints\n");
        // if(tableau.flag == sf_infeasible)  printf("Error: infeasible constraints\n");
        // if(tableau.flag == sf_nonstandard) printf("Error: non-standard input\n");
        return ret;
    }
    return tableau.solution();
}