#include "gd.hpp"

gd::Flag gd::flag;
FILE* gd::fplot = NULL;
FILE* gd::fsample = NULL;

const double gd::h = 1e-6;
const int gd::maxDepth = 100;
const int gd::maxSearchDepth = 10;
const int gd::maxZoomDepth = 20;
const double gd::sampleDensity = 3; // samples per unit volume
const double gd::gradTolerance = 1e-4;
const double gd::sampleThreshold = 1e-2;
const double gd::clusterThreshold = 0.5;
const double gd::overlapThreshold = 1e-1;
const double gd::amax = 100;
const double gd::aepsilon = 1e-3;
bool gd::maximize = false;
int gd::sampleSize = 10; 
Matrix gd::c1 (1e-3); 
Matrix gd::c2 (0.9);
Matrix gd::half (0.5);

const double gd::augmentedDensity = 0.5; // samples per unit volume
const double gd::penaltyTolerance = 1e-1;
const double gd::minit = 0;
const int gd::maxAugmentDepth = 10;
double gd::r = 1;
bool gd::AL = false;
std::vector<gd::Penalty> gd::penalties;

void gd::gd(Node* head, Variables* variables, const std::string &path) {
    gd::AL = false;
    gd::maximize = head->next[0]->next[0]->type == lt_maximize; // flip the gd evaluation
    std::vector<Minima> minima;
    std::string sol = path;
    sol.replace(sol.rfind('.'), sol.back(), ".sol");

    FILE* fptr = fopen(sol.c_str(), "w");
    fplot = fopen("./PLOT/plot.txt", "w");
    fsample = fopen("./PLOT/sample.txt", "w");
    if(fptr == NULL) {printf("Error: file not found\n"); goto E; }
    if(fplot == NULL) { printf("Debug: plot file not-found\n"); goto E; }
    if(fsample == NULL) { printf("Debug: sample file not-found\n"); goto E; }

    fprintf(fplot, "Gradient Descent\n\n"); 
    minima = gd::solve(head, variables);

    /* no solution */
    if(minima.empty()) {
        printf("Warning: No solutions found\n");
        goto E;
    }

    /* sort minimums */
    std::sort(minima.begin(), minima.end(), [](auto const &a, auto const &b) {
        if(gd::maximize) return std::get<1>(a) > std::get<1>(b);
        else             return std::get<1>(a) < std::get<1>(b);
    });

    fprintf(fptr, "Solution file for: %s\n\n", path.c_str());
    for(size_t i = 0; i < minima.size(); i++) {
        fprintf(fptr, "Optimum : %lf {Convergence : %d%%}\n", std::get<1>(minima[i]),
                                        100 * std::get<2>(minima[i]) / gd::sampleSize);
        for(int j = 0; j < variables->len; j++)
            fprintf(fptr, "\t%-3s = %7.2f;\n", variables->arr[j], std::get<0>(minima[i]).at(j,0));
        fprintf(fptr, "\n");
    }
    
E:  fclose(fptr);
    fclose(fplot);
    fclose(fsample);
}

void gd::al(Node* head, Variables* variables, const std::string& path) {
    gd::AL = true;
    gd::maximize = head->next[0]->next[0]->type == lt_maximize; // flip the gd evaluation
    Node *mode = head->next[0];
    bool constraint = false;
    std::vector<Minima> minima;
    int depth = 0;
    std::string sol = path;
    sol.replace(sol.rfind('.'), sol.back(), ".sol");

    FILE* fptr = fopen(sol.c_str(), "w");
    fplot = fopen("./PLOT/plot.txt", "w");
    fsample = fopen("./PLOT/sample.txt", "w");
    if(fptr == NULL) {printf("Error: solution file cannot be created\n"); goto E; }
    if(fplot == NULL) { printf("Debug: plot file not-found\n"); goto E; }
    if(fsample == NULL) { printf("Debug: sample file not-found\n"); goto E; }

    for(int i = 0; i < mode->length; i++) {
        if(mode->next[i]->type == lt_constrain) {
            constraint = true;
            continue;
        } if(constraint) penalties.push_back({mode->next[i], gd::minit});
    }

    while(depth < gd::maxAugmentDepth) {
        fprintf(fplot, "Augmented Lagrangian r {%lf}\n\n", gd::r); 

        minima = gd::solve(head, variables);
        if(minima.empty()) continue;

        double temp;
        bool found = false;
        for(size_t i = 0; i < minima.size(); i++) { // delete violating optima
            bool valid = true;
            for(const auto &p : penalties) {
                gd::resolve(p.function, std::get<0>(minima[i]), temp);
                if(gd::penaltyTolerance < fabs(temp)) valid = false;
            } 
            if(valid) found = true; 
            else      minima.erase(minima.begin() + (i--));
        } if(found) break;

        gd::r *= 2;
        depth++;
    }

    /* no solution */
    if(minima.empty()) {
        printf("Warning: No solutions found\n");
        goto E;
    }

    /* sort minimums */
    std::sort(minima.begin(), minima.end(), [](auto const &a, auto const &b) {
        if(gd::maximize) return std::get<1>(a) > std::get<1>(b);
        else             return std::get<1>(a) < std::get<1>(b);
    });

    fprintf(fptr, "Solution file for: %s\n\n", path.c_str());
    for(size_t i = 0; i < minima.size(); i++) {
        fprintf(fptr, "Optimum : %lf {Convergence : %d%%}\n", std::get<1>(minima[i]),
                                        100 * std::get<2>(minima[i]) / gd::sampleSize);
        for(int j = 0; j < variables->len; j++)
            fprintf(fptr, "\t%-3s = %7.2f;\n", variables->arr[j], std::get<0>(minima[i]).at(j,0));
        fprintf(fptr, "\n");
    }

E:  fclose(fptr);
    fclose(fplot);
    fclose(fsample);
}

std::vector<gd::Minima> gd::solve(Node* head, Variables* variables) {
    gd::maximize = head->next[0]->next[0]->type == lt_maximize;
    gd::AL = head->next[0]->type == nt_cnlp;
    Node* F = head->next[0]->next[1];
    
    std::vector<gd::Minima> minima;
    std::vector<gd::Point> points;
    uniform Udist;
    std::vector<Bound> bounds(variables->len);
    std::vector<uniform> dists(variables->len, Udist);

    /* read bound information */
    double sampleVolume = 1;
    for(int i = 3; i < head->next[0]->length; i++) {
        Node* next = head->next[0]->next[i];
        if(next->type == lt_constrain) break;
        Bound bound = {n_get_value(next->next[0]) * (next->next[0]->subtype==2 ? -1 : 1), 
                       n_get_value(next->next[2]) * (next->next[2]->subtype==2 ? -1 : 1)};
        sampleVolume *= abs(bound.max - bound.min);
        bounds[n_get_value(next->next[1])] = bound;
        dists[n_get_value(next->next[1])] = uniform(bound.min, bound.max);
    }
    /* check for incorrectly bounded variables */
    bool unbounded = false;
    for(size_t i = 0; i < dists.size(); i++) {
        if(dists[i] == Udist) {
            printf("Error: %s is unbounded\n", variables->arr[i]);
            unbounded = true;
        }
    } if(unbounded) return minima;

    gd::sampleSize = sampleVolume * (gd::AL ? gd::augmentedDensity : gd::sampleDensity);
    points = gd::mesh(F, variables, dists, gd::sampleSize);
    minima.reserve(sampleSize);

    for(auto &point : points) {
        double minimum;
        bool cluster = false;
        if(BFGS(F, variables, point, bounds) == -1) continue;
        for(auto &m : minima) {
            if(cmp((std::get<0>(m) - point.xk).norm(),">",gd::overlapThreshold)) continue;
            cluster = true;
            std::get<2>(m)++;
            break;
        }
        if(cluster) continue;
        gd::evaluate(F, point.xk, minimum);
        minima.emplace_back(std::make_tuple(point.xk, minimum, 1));
    }

    return minima;
}

int gd::BFGS(Node* F, Variables* variables, Point &point, std::vector<Bound> &bounds) {
    int depth = 0;
    Matrix &xk = point.xk;
    Matrix &gk = point.gk;
    Matrix Hk(variables->len, variables->len, true);
    Matrix pk(variables->len, 1, false);
    Matrix gt(variables->len, 1, false);
    Matrix sk(variables->len, 1, false);
    Matrix yk(variables->len, 1, false);
    Matrix skyk(0);
    Matrix ak(0);

    while(cmp(gk.norm(),">",gd::gradTolerance)) {
        if(depth > gd::maxDepth) goto E;
        fprintf(fplot, "%lf %lf\n", xk.at(0,0), xk.at(1,0));
        /* step direction */
        // pk = -gk;
        pk = -Hk * gk;
        /* step size */
        if(gd::line_search(F, xk, pk, gk, ak) == -1) goto E;
        sk = ak * pk;
        /* step */
        xk = xk + sk;
        if(gd::outbound(bounds, xk)) goto E;
        /* pre-update */
        if(gd::gradient(F, xk, gt) == -1) goto E;
        yk = gt - gk;
        skyk = sk.T() * yk;
        if(depth == 0) Hk = Hk * (skyk / (yk.T() * yk));
        /* update hessian */
        Hk = Hk + (skyk + yk.T() * Hk * yk) * (sk * sk.T()) / (skyk * skyk) 
                - (Hk * yk * sk.T() + sk * yk.T() * Hk) / skyk;
        /* update */
        gk = gt;
        depth++;
    }

    fprintf(fplot, "\n");
    return 0;
E:  fprintf(fplot, "\n");
    return -1;
}

std::vector<gd::Point> gd::mesh(Node* F, Variables* variables, std::vector<uniform> dists, int sampleSize) {
    std::vector<gd::Point> points; 
    points.reserve(sampleSize); 
    /* initialize uniform-real mersenne twister */
    std::random_device rd;
    std::mt19937 MT (rd());

    for(int i = 0; i < sampleSize; i++) {
        gd::Point point (variables->len);

        /* generate random N-d coordinate */
        for(size_t j = 0; j < dists.size(); j++)
            point.xk.at(j,0) = dists[j](MT);        
        
        /* discard if gradient is uncomputable */
        if(gd::gradient(F, point.xk, point.gk) == -1) continue;
        point.gnorm = point.gk.norm();

        /* discard if gradient is almost flat */
        if(cmp(point.gnorm,"<",gd::sampleThreshold)) continue; 

        /* cluster if proximal */
        Matrix t (variables->len, 1, false);
        bool cluster = false;
        for(size_t j = 0; j < points.size(); j++) {
            t = points[j].xk - point.xk;
            if(t.norm() * max(points[j].gnorm, point.gnorm) < gd::clusterThreshold) {
                points[j].xk = half * (points[j].xk + point.xk);
                cluster = true; 
                break;
            }
        } 
        if(cluster) continue;

        fprintf(fsample, "%lf %lf\n\n", point.xk.at(0,0), point.xk.at(1,0));
        points.emplace_back(point);
    }

    return points;
}

bool gd::outbound(const std::vector<Bound> &bounds, const Matrix &xk) {
    for(size_t i = 0; i < bounds.size(); i++) {
        if(cmp(xk.at(i, 0),"<",bounds[i].min) || cmp(bounds[i].max,"<",xk.at(i, 0)))
            return true;
    } return false;
}

int gd::gradient(Node* F, const Matrix &xk, Matrix &gk) {
    double temp, Fm, Fp, h2 = 2 * gd::h;
    Matrix x = xk;
    for(int r = 0; r < gk.row; r++) {
        temp = x.at(r, 0);
        x.at(r, 0) = temp + gd::h;
        if(gd::evaluate(F, x, Fp) == -1) goto E;
        x.at(r, 0) = temp - gd::h;
        if(gd::evaluate(F, x, Fm) == -1) goto E;
        gk.at(r, 0) = (Fp - Fm) / h2;
        x.at(r, 0) = temp;
    }
    return 0;
E:  return -1;
}

int gd::line_search(Node* F, const Matrix &xk, const Matrix &pk, const Matrix &gk, Matrix &ak) {
    int depth = 0;
    double f0, fa = 0, fap = 0, gaf;
    double ainit = 1 / max(gd::aepsilon, pk.norm()); // epsilon for divide by zero
    double amax = gd::amax * ainit; // amax is gd::amax * 1 / pk.norm()
    Matrix p = pk.T(); // transpose directional vector
    Matrix g0 = p * gk, ga = gk;
    Matrix a(0), ap(0); 
    if(gd::evaluate(F, xk, f0) == -1) goto E;

    while(true) {
        if(depth > gd::maxSearchDepth) goto E;

        ap = a; fap = fa;
        a = (depth == 0) ? (ainit) : (2.0 * a);
        if(a.at(0,0) > amax) goto E;
        if(gd::evaluate(F, xk + a*pk, fa) == -1) goto E;

        if((fa > f0 + (c1*a*g0).at(0,0)) || (fa >= fap && depth != 0)) {
            if(gd::zoom(F, xk, pk, gk, ap, a, ak) == -1) goto E;
            break;
        }

        if(gd::gradient(F, xk + a*pk, ga) == -1) goto E; 
        gaf = (p * ga).at(0,0); // pk.T() * ga

        if(abs(gaf) <= (-gd::c2 * g0).at(0,0)) {
            ak = a;
            break;
        }
        if(gaf >= 0) {
            if(gd::zoom(F, xk, pk, gk, a, ap, ak) == -1) goto E;
            break;
        }
        
        depth++;
    }
    return 0;
E:  return -1;
}

int gd::zoom(Node* F, const Matrix &xk, const Matrix &pk, const Matrix &gk, Matrix &al, Matrix &ar, Matrix& ak) {
    int depth = 0; 
    double f0, fa, fal, gaf;
    Matrix p = pk.T(); // transpose of unit directional vector
    Matrix g0 = p*gk, ga = gk;
    Matrix a(0);
    if(gd::evaluate(F, xk, f0) == -1) goto E;
    while(true) {
        if(depth > gd::maxZoomDepth) goto E;

        a = 0.5 * (al + ar);
        if(gd::evaluate(F, xk + a*pk, fa) == -1) goto E;
        if(gd::evaluate(F, xk + al*pk, fal) == -1) goto E;
        
        if((fa > f0 + (gd::c1*a*g0).at(0, 0)) || (fa >= fal)) {
            ar = a;
        } else {        
            if(gd::gradient(F, xk + a*pk, ga) == -1) goto E;
            gaf = (p * ga).at(0,0);

            if(abs(gaf) <= (-gd::c2 * g0).at(0, 0)) {
                ak = a;
                break;
            }
            if(gaf * (ar - al).at(0,0) >= 0) {
                ar = al;
            }
            al = a;
        }
        
        depth++;
    }
    return 0;
E:  return -1;
}

int gd::evaluate(Node* head, const Matrix &replace, double &value) {
    double lagrange = 0, augment = 0, a;
    if(replace.col != 1) goto E;
    if(gd::additive(head, replace, value) == -1) goto E;
    if(gd::AL) {
        for(const auto &p : gd::penalties) {
            if(gd::additive(p.function, replace, a) == -1) goto E;
            lagrange += p.multiplier * a;
            augment  += gd::r * (a * a);
        }
        value += lagrange + augment;
        if(!isfinite(value)) { flag = gd_overflow; goto E; }
    }
    if(gd::maximize) value = -value;
    return 0;
E:  return -1;
}
int gd::resolve(Node* head, const Matrix& replace, double &value) {
    if(replace.col != 1) goto E;
    if(gd::additive(head, replace, value) == -1) goto E;
    if(gd::maximize) value = -value;
    return 0;
E:  return -1;
}
int gd::additive(Node* head, const Matrix &replace, double &value) {
    double a = 0, b;
    for(int i = 0; i < head->length; i++) {
        Node* next = head->next[i];
        if(gd::multiplicative(next, replace, b) == -1) goto E;
        a = next->subtype == 1 ? a + b : a - b;
        if(!isfinite(a)) { flag = gd_overflow; goto E; }
    }
    value = a;
    return 0;
E:  return -1;
}
int gd::multiplicative(Node* head, const Matrix &replace, double &value) {
    double a = 1, b;
    for(int i = 0; i < head->length; i++) {
        Node* next = head->next[i];
        if(gd::exponential(next, replace, b) == -1) goto E;
        a = next->subtype == 1 ? a * b : a / b;
        if(!isfinite(a)) { flag = gd_overflow; goto E; }
    }
    value = a;
    return 0;
E:  return -1;
}
int gd::exponential(Node* head, const Matrix &replace, double &value) {
    double a, b = 1;
    /* from right to left */
    for(int i = head->length - 1; i >= 0; i--) {
        Node* next = head->next[i];
        if(gd::primary(next, replace, a) == -1) goto E;
        b = pow(a, b);
        if(!isfinite(b)) { flag = gd_overflow; goto E; }
    }
    value = b;
    return 0;
E:  return -1;
}
int gd::primary(Node* head, const Matrix &replace, double &value) {
    Node* next = head->next[0];
    switch(next->type) {
        case nt_real: 
            value = n_get_value(next); 
            break;
        case lt_variable:
            value = replace.at(next->value, 0); 
            break;
        case nt_functions:
            if(gd::functions(next->next[0], replace, value) == -1) goto E;
            break;
        case nt_nonlinear_parenthesis:
            if(gd::additive(next->next[0], replace, value) == -1) goto E;
            break;
        default:
            printf("%d default\n", head->type);
    }
    return 0;
E:  return -1;
}
int gd::functions(Node* head, const Matrix &replace, double &value) {
    double base, expression, a;
    switch(head->type) {
        case nt_root:
            base = n_get_value(head->next[0]);
            if(gd::additive(head->next[1], replace, expression) == -1) goto E;
            a = pow(expression, 1 / base);
            if(!isfinite(a)) {
                flag = gd_invalid_root; goto E;
            }
            break;
        case nt_sqrt:
            if(gd::primary(head->next[0], replace, expression) == -1) goto E;
            a = sqrt(expression);
            if(!isfinite(a)) { flag = gd_overflow; goto E; }
            break;
        case nt_log:
            base = n_get_value(head->next[0]);
            if(gd::additive(head->next[1], replace, expression) == -1) goto E;
            if(base != 2 && base != 10 && base != M_E) {
                flag = gd_invalid_log; goto E;
            }
            if(base == 2)   a = log2(expression);
            if(base == 10)  a = log10(expression);
            if(base == M_E) a = log(expression);
            if(!isfinite(a)) { flag = gd_overflow; goto E; }
            break;
        case nt_ln:
            if(gd::primary(head->next[0], replace, expression) == -1) goto E;
            a = log(expression);
            if(!isfinite(a)) { flag = gd_overflow; goto E; }
            break;
        case nt_sin:
            if(gd::primary(head->next[0], replace, expression) == -1) goto E;
            a = sin(expression);
            if(!isfinite(a)) { flag = gd_overflow; goto E; }
            break;
        case nt_cos:
            if(gd::primary(head->next[0], replace, expression) == -1) goto E;
            a = cos(expression);
            if(!isfinite(a)) { flag = gd_overflow; goto E; }
            break;
        case nt_tan:
            if(gd::primary(head->next[0], replace, expression) == -1) goto E;
            a = tan(expression);
            if(!isfinite(a)) { flag = gd_overflow; goto E; }
            break;
        default:
            goto E;
    }
    value = a;
    return 0;
E:  return -1;
}

/*
int gd::zoom(Node* F, const Matrix &xk, const Matrix &pk, const Matrix &gk, Matrix &bl, Matrix &br, Matrix& ak) {
    int depth = 0; 
    double f0, fb, fbl;
    Matrix g0 = gk, gb = gk;
    if(gd::evaluate(F, xk, f0) == -1) goto E;
    while(depth < gd::maxZoomDepth) {
        Matrix b = 0.5 * (bl + br);
        if(gd::evaluate(F, xk + b*pk, fb) == -1) goto E;
        if(gd::evaluate(F, xk + bl*pk, fbl) == -1) goto E;
        if(gd::gradient(F, xk + b*pk, gb) == -1) goto E;

        // Armijo condition check
        if(fb > f0 + gd::a1*b*(g0.T()*pk).at(0,0) || fb >= fbl) {
            br = b;
        } else {
            // Check curvature condition
            if(std::abs((gb.T()*pk).at(0,0) <= gd::a2 * std::abs((g0.T()*pk).at(0,0)) {
                ak = b; // Assign the found step
                break;
            }
            // Update interval based on gradient at b
            if((gb.T()*pk).at(0,0) >= 0) {
                br = b;
            } else {
                bl = b;
            }
        }
        depth++;
    }
    return 0;
E:  return -1;
}

int gd::line_search(Node* F, const Matrix &xk, const Matrix &pk, const Matrix &gk, Matrix &ak) {
    int depth = 0;
    double f0, fb, fbp;
    Matrix g0 = gk, gb;
    Matrix b(0), bp;

    if(gd::evaluate(F, xk, f0) == -1) return -1;

    while(depth < gd::maxSearchDepth) {
        bp = b; 
        b = 0.5 * (b + gd::bmax);

        if(gd::evaluate(F, xk + b*pk, fb) == -1 || 
           gd::evaluate(F, xk + bp*pk, fbp) == -1 ||
           gd::gradient(F, xk + b*pk, gb) == -1) return -1;
        // Check Armijo or if fb not better than previous
        if(fb > f0 + gd::a1*b*(g0.T()*pk).at(0,0) || fb >= fbp) {
            if(gd::zoom(F, xk, pk, gk, bp, b, ak) == -1) return -1;
            break; // Exit after zoom
        }
        // Check curvature condition
        if(std::abs((gb.T()*pk).at(0,0)) <= gd::a2 * std::abs((g0.T()*pk).at(0,0))) {
            ak = b;
            break;
        }
        // Check gradient sign for interval update
        if((gb.T()*pk).at(0,0) >= 0) {
            if(gd::zoom(F, xk, pk, gk, bp, b, ak) == -1) return -1;
            break; // Exit after zoom
        }
        depth++;
    }
    return 0;
}

*/