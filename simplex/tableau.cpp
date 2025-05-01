#include "tableau.hpp"

Tableau::Tableau(Node* head, Variables* variables) {
    Node* mode = head->next[0];
    if(mode->next[0]->type == lt_minimize) make_minimize(head, variables);
    if(mode->next[0]->type == lt_maximize) make_maximize(head, variables);
}

int Tableau::solve() {
    return 1;
}
void Tableau::print(FILE* fptr) {
    for(auto v : mat) {
        for(auto d : v)
            std::cout << d << " ";
        std::cout << std::endl;
    }
}
void Tableau::make_minimize(Node* head, Variables* variables) {
    this->column = 1; // target function
    this->row = variables->len + 1; // column b
    Node* mode = head->next[0];
    for(int i = 0; i < mode->length; i++) {
        if(mode->next[i]->type != nt_linear_constraint) continue;
        if(mode->next[i]->next[1]->type != lt_equal) this->column++;
        this->row++;
    }
    this->mat = std::vector<std::vector<double>>
                (this->row, std::vector<double>(this->column, 0.0));
}
void Tableau::make_maximize(Node* head, Variables* variables) {
    /* determine matrix size */
    this->row = 1; // target function
    this->column = variables->len + 1 + 1; // slack + Z + b
    this->varcount = variables->len;
    Node* mode = head->next[0];
    for(int i = 0; i < mode->length; i++) {
        if(mode->next[i]->type != nt_linear_constraint) continue;
        if(mode->next[i]->next[1]->type != lt_equal) this->column++;
        this->row++;
    }
    /* size matrix */
    this->mat = std::vector<std::vector<double>>
                (this->row, std::vector<double>(this->column, 0.0));
    /* define objective and constraint row */
    int obj_row = this->row - 1; // objective function row
    int con_row = 0;
    /* poplulate matrix */
    for(int i = 0; i < mode->length; i++) {
        Node* element = mode->next[i];
        /* populate objective function row */
        if(element->type == nt_linear_additive) {
            Node* additive = element;
            /* for each linear multiplication */
            for(int j = 0; j < additive->length; j++) {
                Node* multiplicative = additive->next[j];
                Node *coefficient = nullptr, *variable = nullptr;
                for(int k = 0; k < multiplicative->length; k++) {
                    if(multiplicative->next[k]->type == nt_rational ||
                       multiplicative->next[k]->type == nt_irrational) {
                        coefficient = multiplicative->next[k];
                    }
                    if(multiplicative->next[k]->type == lt_variable) {
                        variable = multiplicative->next[k];
                    }
                }
                /* populate matrix */
                if(variable) {
                    double value = coefficient ? n_get_value(coefficient) : 1;
                    this->mat[obj_row][(int)n_get_value(variable)] += -value; 
                } else {
                    double value = n_get_value(coefficient);
                    this->mat[obj_row][this->column - 1] += value;
                }
            }
            /* Z */
            this->mat[obj_row][this->column - 2] = 1; 
        }
        /* populate constraint rows */
        if(element->type == nt_linear_constraint) {
            bool negate = false; 
            bool less, inequality;
            for(int l = 0; l < element->length; l++) {
                Node* additive = element->next[l];
                /* inequality and sign fliping */
                if(additive->type != nt_linear_additive) {
                    negate = true;
                    less = additive->type == lt_less || additive->type == lt_leq;
                    inequality = additive->type != lt_equal;
                    continue;
                }
                /* for each linear multiplication */
                for(int j = 0; j < additive->length; j++) {
                    Node* multiplicative = additive->next[j];
                    Node *coefficient = nullptr, *variable = nullptr;
                    for(int k = 0; k < multiplicative->length; k++) {
                        if(multiplicative->next[k]->type == nt_rational ||
                           multiplicative->next[k]->type == nt_irrational) {
                            coefficient = multiplicative->next[k];
                        }
                        if(multiplicative->next[k]->type == lt_variable) {
                            variable = multiplicative->next[k];
                        }
                    }
                    /* populate matrix */
                    if(variable) {
                        double value = coefficient ? n_get_value(coefficient) : 1;
                        this->mat[con_row][(int)n_get_value(variable)] += negate ? -value : value; 
                    } else {
                        double value = n_get_value(coefficient);
                        this->mat[con_row][this->column - 1] += negate ? value : -value;
                    }
                }
            }
            /* slack variable */
            if(inequality) this->mat[con_row][this->varcount + con_row] = less ? 1 : -1;
            con_row++; // next constraint row
        }
    }
}