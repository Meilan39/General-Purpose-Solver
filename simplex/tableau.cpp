#include "tableau.hpp"

Tableau::Tableau(Node* head, Variables* variables) {
    this->row = 1; // target function
    this->column = variables->len + 1; // column b
    Node* mode = head->next[0];
    for(int i = 0; i < mode->length; i++) {
        if(mode->next[i]->type != nt_linear_constraint) continue;
        if(mode->next[i]->next[1]->type != lt_equal) this->column++;
        this->row++;
    }
    arr = std::vector(this->row, std::vector<int>(this->column));

    for(int i = 0; i < mode->length; i++) {
        Node* additive = mode->next[i];
        if(additive->type == nt_linear_additive) {
            for(int j = 0; j < additive->length; j++) {
                Node* multiplicative = additive->next[j];
                Node* coefficient = n_findb(multiplicative, nt_)
                
                arr[this->row - 1][varidx] = 
            }
        }
    }
}
int Tableau::solve();
void Tableau::print(FILE* fptr);
