#include "bnb.hpp"

bool bnb::minimize;
bool bnb::maxReached = false;
int bnb::insidx;
int const bnb::maxDepth = 50;

void bnb::bnb(Node* head, Variables* variables, const char* path) {
    ivec general;
    fvec optimal;
    Node* mode = head->next[0];
    bnb::minimize = mode->next[0]->type == lt_minimize;

    for(int i = 0; i < mode->length; i++) {
        if(mode->next[i]->type == lt_integer) insidx = i;
        if(mode->next[i]->type == nt_variable_constraint) {
            general.push_back(mode->next[i]->next[0]->value);
        }
    }

    bnb::branch(head, variables, general, optimal, 0);
    if(optimal.empty()) {
        printf("Error: unable to find mixed integer solution\n");
        return;
    }

    /* print solution */
    std::string sol = path;
    sol.replace(sol.find('.'), sol.back(), ".sol");
    FILE* fptr = fopen(sol.c_str(), "w");
    
    if(fptr == NULL) {
        printf("Error: unable to open file");
        return;
    }
    fprintf(fptr, "Solution file for : %s\n\n", path);
    fprintf(fptr, "Optimum : %lf\n", optimal.back());
    for(int i = 0; i < (int)(optimal.size()-1); i++) 
        fprintf(fptr, "\t%-3s = %7.2f;\n", variables->arr[i], optimal[i]);
    fclose(fptr);
}

void bnb::branch(Node* head, Variables* variables, const ivec &general, fvec &optimal, int depth) {
    if(depth > bnb::maxDepth) {
        if(maxReached) return;
        printf("Warning: branch limit reached - results may be unoptimal\n");
        maxReached = true; return;
    }

    fvec solution = simplex::bnb(head, variables);
    if(solution.empty()) {
        return;
    }

    if(!optimal.empty()) {
        if(cmp(solution.back(), bnb::minimize ? ">=" : "<=" ,optimal.back())) {
            return;
        }
    }

    double diffmax = 0;
    int idx = -1;
    for(int i : general) {
        double diff = abs(solution[i] - round(solution[i]));
        if(cmp(diff,"!=",0)) {
            if(diffmax < diff) {
                diffmax = diff;
                idx = i;
            }
        }
    }

    if(idx == -1) {
        printf("%d : adding %lf\n", depth, solution.back());
        if(optimal.empty()) {
            optimal = solution; 
            return;
        }
        if(( bnb::minimize && cmp(solution.back(),"<",optimal.back())) || 
           (!bnb::minimize && cmp(solution.back(),">",optimal.back()))) {
            optimal = solution;
        }
    } else {
        Node* mode = head->next[0];
        /* left side */
        if(!bnb::exists(head, idx, floor(solution[idx]), true)) {
            Node* node = bnb::new_constraint(idx, floor(solution[idx]), true);
            n_insert(mode, node, bnb::insidx);
            bnb::branch(head, variables, general, optimal, depth + 1);
            n_delete(mode, bnb::insidx);
        }
        /* right side */
        if(!bnb::exists(head, idx, ceil(solution[idx]), false)) {
            Node* node = bnb::new_constraint(idx, ceil(solution[idx]), false);
            n_insert(mode, node, bnb::insidx);
            bnb::branch(head, variables, general, optimal, depth + 1);
            n_delete(mode, bnb::insidx);
        }
    }
}

/* dangerous function */
bool bnb::exists(Node* head, int idx, double number, bool left) {
    for(int i = bnb::insidx; head->next[0]->next[i]->type != lt_integer; i++) {
        Node* constraint = head->next[0]->next[i];
        if((cmp(constraint->next[0]->next[0]->next[0]->value,"==",idx)) &&
            constraint->next[1]->type == (left ? lt_leq : lt_geq) &&
           (cmp(constraint->next[2]->next[0]->next[0]->next[0]->next[0]->value,"==",number))) {
            return true;
        }
    } return false; 
}

Node* bnb::new_constraint(int idx, double number, bool left) {
    /* variable */
    Node* linear_constraint = n_construct(nt_linear_constraint, 0);
    Node *linear_additive = n_construct(nt_linear_additive, 0),
         *linear_multiplicative = n_construct(nt_linear_multiplicative, 0),
         *linear_term = n_construct(lt_variable, idx),
         *linear_rational;
    linear_multiplicative->subtype = 1;
    n_push(linear_multiplicative, linear_term);
    n_push(linear_additive, linear_multiplicative);
    n_push(linear_constraint, linear_additive);
    /* inequality */
    if(left) n_push(linear_constraint, n_construct(lt_leq, 0));
    else     n_push(linear_constraint, n_construct(lt_geq, 0));
    /* constant */
    linear_additive = n_construct(nt_linear_additive, 0);
    linear_multiplicative = n_construct(nt_linear_multiplicative, 0);
    linear_term = n_construct(nt_real, 0);
    linear_rational = n_construct(nt_rational, 0);
    if(cmp(number,"<",0)) {
        linear_multiplicative->subtype = 2;
        number = -number;
    } else linear_multiplicative->subtype = 1;
    n_push(linear_rational, n_construct(ct_number, number));
    n_push(linear_term, linear_rational);
    n_push(linear_multiplicative, linear_term);
    n_push(linear_additive, linear_multiplicative);
    n_push(linear_constraint, linear_additive);
    /* return */
    return linear_constraint;
}