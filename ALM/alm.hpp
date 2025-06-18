#ifndef ALM
#define ALM

#include <algorithm>
#include <vector>
#include <random>
#include "../global/global.hpp"
#include "../parser/tree.hpp"
#include "../gd/matrix.hpp"
#include "../gd/gd.hpp"

namespace alm {



    void alm(Node* head, Variables* variables, const std::string& path);

}

#endif