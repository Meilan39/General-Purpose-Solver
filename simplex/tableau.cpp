#include "tableau.hpp"

int Tableau::solve() {
    /* temporary solution to RHS non-negative constraint */
    for(int r = 0; r < this->row - 2; r++) {
        if(cmp(this->mat[r][this->column - 1],"<",0)) {
            this->flag = sf_nonstandard;
            return -1;
        }
    }
    /* phase one */
    while(!optimal(this->row - 1)) {
        std::pair<int, int> piv;
        if(get_pivot(piv, this->row - 1) == -1) return -1;
        pivot(piv);
    }
    if(cmp(this->mat[this->row - 1][this->column - 1],"!=",0)) {
        this->flag = sf_infeasible; 
        return -1;
    }
    /* phase two */
    while(!optimal(this->row - 2)) {
        std::pair<int, int> piv;
        if(get_pivot(piv, this->row - 2) == -1) return -1;
        pivot(piv);
    }
    return 0;
}

std::vector<double> Tableau::solution(bool verbose) {
    int end = this->column - this->artificial - 1 - (verbose ? 0 : this->slack);
    std::vector<double> ret (end + 1);
    for(int c = 0; c < end; c++) { 
        bool basic = true;
        int oneIdx = -1;
        for(int r = 0; r < this->row - 2; r++) {
            if(cmp(this->mat[r][c],"==",1)) {
                if(oneIdx != -1) basic = false;
                oneIdx = r;
                continue;
            }
            if(cmp(this->mat[r][c],"!=",0)) {
                basic = false;
            }
        }
        ret[c] = (basic ? this->mat[oneIdx][this->column - 1] : 0);
    }
    ret[end] = (this->minimize ? -1 : 1) * this->mat[this->row - 2][this->column - 1];
    return ret;
}

bool Tableau::optimal(int obj_row) {
    bool minimize = obj_row == this->row - 1;
    int end = this->column - (minimize ? 1 : this->artificial + 1);
    for(int c = 0; c < end; c++) { // excluding b and artificial for phase 2
        double temp = this->mat[obj_row][c];
        if(( minimize && cmp(temp,">",0)) || 
           (!minimize && cmp(temp,"<",0))) {
            return 0;
        }
    }
    return 1;
}

void Tableau::pivot(const std::pair<int, int> &piv) {
    double value = this->mat[piv.first][piv.second];
    for(int c = 0; c < this->column; c++)
        mat[piv.first][c] /= value;
    for(int r = 0; r < this->row; r++) {
        if(r == piv.first) continue;
        double multiple = this->mat[r][piv.second];
        for(int c = 0; c < this->column; c++) {
            this->mat[r][c] -= multiple * this->mat[piv.first][c];
        }
    } 
}

int Tableau::get_pivot(std::pair<int, int> &piv, int obj_row) {
    bool minimize = obj_row == this->row - 1;
    double extrema, temp;
    /* column */
    extrema = this->mat[obj_row][0]; 
    piv.second = 0;
    int end = this->column - (minimize ? 1 : this->artificial + 1);
    for(int c = 1; c < end; c++) { // excluding b
        temp = this->mat[obj_row][c];
        if(( minimize && cmp(temp,">",extrema)) || 
           (!minimize && cmp(temp,"<",extrema))) {
            extrema = temp;
            piv.second = c;
        }
    }
    /* row */
    bool empty = true;
    for(int r = 0; r < this->row - 2; r++) { // excluding phase 1 and 2
        if(cmp(this->mat[r][this->column - 1],"<",0) || 
           cmp(this->mat[r][piv.second],"<=",0)) continue; // non-negative
        temp = this->mat[r][this->column - 1] / this->mat[r][piv.second];
        if(empty) { 
            extrema = temp; 
            piv.first = r; 
            empty = false;
            continue;
        }
        if(cmp(temp,"<",extrema)) {
            extrema = temp;
            piv.first = r;
        }
    } 
    if(empty) {
        if(minimize) this->flag = sf_infeasible;
        else         this->flag = sf_unbounded;
        return -1;
    }
    return 0;
}

void Tableau::print() {
    for(auto v : mat) {
        for(auto d : v)
            printf("%+3.3f ", d);
        printf("\n");
    } printf("\n");
}

Tableau::Tableau(Node* head, Variables* variables) {
    Node* mode = head->next[0];
    if(mode->next[0]->type == lt_minimize) this->minimize = true;
    if(mode->next[0]->type == lt_maximize) this->minimize = false;
    /* determine matrix size */
    this->defined = variables->len;
    this->row = 0;
    this->slack = 0;
    this->artificial = 0;
    for(int i = 0; i < mode->length; i++) {
        if(mode->next[i]->type != nt_linear_constraint) continue;
        if(mode->next[i]->next[1]->type == lt_less || 
           mode->next[i]->next[1]->type == lt_leq) {
            this->slack++;
        }
        if(mode->next[i]->next[1]->type == lt_equal) {
            this->artificial++;
        }
        if(mode->next[i]->next[1]->type == lt_greater || 
           mode->next[i]->next[1]->type == lt_geq) {
            this->slack++;
            this->artificial++;
        }
        this->row++;
    }
    this->row += 2; // ... phase II + phase I
    this->column = this->defined + this->slack + this->artificial + 1; // ... b
    /* size matrix */
    this->mat = std::vector<std::vector<double>>
                (this->row, std::vector<double>(this->column, 0.0));
    /* define objective and constraint row */
    int phase_two = this->row - 2; // objective function row
    int phase_one = this->row - 1;
    int con_row = 0;
    int countSlack = 0, countArtificial = 0;
    bool negative;
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
                negative = multiplicative->subtype == 2;
                for(int k = 0; k < multiplicative->length; k++) {
                    if(multiplicative->next[k]->type == nt_real)
                        coefficient = multiplicative->next[k];
                    if(multiplicative->next[k]->type == lt_variable)
                        variable = multiplicative->next[k];
                }
                /* populate matrix */
                if(variable) {
                    double value = coefficient ? n_get_value(coefficient) : 1;
                    if(negative ^ !this->minimize) value = -value;
                    this->mat[phase_two][(int)n_get_value(variable)] += value;
                } else {
                    double value = n_get_value(coefficient);
                    if(negative ^ !this->minimize) value = -value;
                    this->mat[phase_two][this->column - 1] += value;
                }
            }
        }
        /* populate constraint rows */
        if(element->type == nt_linear_constraint) {
            bool negate = false; 
            bool less, isSlack, isArtificial;
            for(int l = 0; l < element->length; l++) {
                Node* additive = element->next[l];
                /* inequality and sign fliping */
                if(additive->type != nt_linear_additive) {
                    negate = true;
                    less = additive->type == lt_less || additive->type == lt_leq;
                    isSlack = additive->type != lt_equal;
                    isArtificial = !less;
                    continue;
                }
                /* for each linear multiplication */
                for(int j = 0; j < additive->length; j++) {
                    Node* multiplicative = additive->next[j];
                    Node *coefficient = nullptr, *variable = nullptr;
                    negative = multiplicative->subtype == 2;
                    for(int k = 0; k < multiplicative->length; k++) {
                        if(multiplicative->next[k]->type == nt_real)
                            coefficient = multiplicative->next[k];
                        if(multiplicative->next[k]->type == lt_variable)
                            variable = multiplicative->next[k];
                    }
                    /* populate matrix */
                    if(variable) {
                        double value = coefficient ? n_get_value(coefficient) : 1;
                        if(negative ^ negate) value = -value;
                        this->mat[con_row][(int)n_get_value(variable)] += value;
                    } else {
                        double value = n_get_value(coefficient);
                        if(negative ^ !negate) value = -value; // constants are !negate
                        this->mat[con_row][this->column - 1] += value;
                    }
                }
            }
            /* slack variable */
            if(isSlack) 
                this->mat[con_row][this->defined + (countSlack++)] = less ? 1 : -1;
            if(isArtificial) {
                for(int j = 0; j < this->column; j++)
                    this->mat[phase_one][j] += this->mat[con_row][j];
                this->mat[con_row][this->defined + this->slack + (countArtificial++)] = 1;
            }
            con_row++; // next constraint row
        }
    }
}