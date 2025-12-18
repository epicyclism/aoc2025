#include <bits/stdc++.h>
using namespace std;

struct Simplex {
    vector<vector<double>> A; // Constraint matrix
    vector<double> b;         // RHS
    vector<double> c;         // Objective coefficients
    vector<int> basis;        // Basis variable indices
    int m, n;                 // m constraints, n variables

    Simplex(const vector<vector<double>>& A_, const vector<double>& b_, const vector<double>& c_)
        : A(A_), b(b_), c(c_), m(A_.size()), n(A_[0].size()) {
        basis.resize(m);
    }

    // Solve LP using simplex (assumes feasible start)
    double solve(vector<double>& x) {
        vector<vector<double>> tableau(m + 1, vector<double>(n + m + 1, 0.0));

        // Build tableau
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) tableau[i][j] = A[i][j];
            tableau[i][n + i] = 1; // slack/artificial
            tableau[i].back() = b[i];
            basis[i] = n + i;
        }
        for (int j = 0; j < n; j++) tableau[m][j] = -c[j];

        // Simplex iterations
        while (true) {
            int pivotCol = -1;
            for (int j = 0; j < n + m; j++)
                if (tableau[m][j] < -1e-9) { pivotCol = j; break; }
            if (pivotCol == -1) break; // optimal

            int pivotRow = -1;
            double minRatio = 1e18;
            for (int i = 0; i < m; i++) {
                if (tableau[i][pivotCol] > 1e-9) {
                    double ratio = tableau[i].back() / tableau[i][pivotCol];
                    if (ratio < minRatio) {
                        minRatio = ratio;
                        pivotRow = i;
                    }
                }
            }
            if (pivotRow == -1) throw runtime_error("Unbounded LP");

            pivot(tableau, pivotRow, pivotCol);
            basis[pivotRow] = pivotCol;
        }

        x.assign(n, 0.0);
        for (int i = 0; i < m; i++)
            if (basis[i] < n) x[basis[i]] = tableau[i].back();
        return tableau[m].back();
    }

    void pivot(vector<vector<double>>& tab, int r, int c) {
        double div = tab[r][c];
        for (double& val : tab[r]) val /= div;
        for (int i = 0; i < m + 1; i++) {
            if (i != r) {
                double factor = tab[i][c];
                for (int j = 0; j < n + m + 1; j++)
                    tab[i][j] -= factor * tab[r][j];
            }
        }
    }
};

// Branch and Bound for Integer LP
double branchAndBound(Simplex& lp, vector<double>& bestX, double bestVal) {
    vector<double> x;
    double val = lp.solve(x);

    // If worse than best, prune
    if (val >= bestVal - 1e-9) return bestVal;

    // Check if integer
    bool isInt = true;
    for (double xi : x)
        if (fabs(xi - round(xi)) > 1e-9) { isInt = false; break; }

    if (isInt) {
        bestX = x;
        return val;
    }

    // Branch on first fractional variable
    for (int i = 0; i < (int)x.size(); i++) {
        if (fabs(x[i] - round(x[i])) > 1e-9) {
            // Left branch: xi <= floor(xi)
            {
                auto A2 = lp.A;
                auto b2 = lp.b;
                auto c2 = lp.c;
                vector<double> newRow(lp.n, 0.0);
                newRow[i] = 1;
                A2.push_back(newRow);
                b2.push_back(floor(x[i]));
                Simplex left(A2, b2, c2);
                bestVal = branchAndBound(left, bestX, bestVal);
            }
            // Right branch: xi >= ceil(xi)
            {
                auto A2 = lp.A;
                auto b2 = lp.b;
                auto c2 = lp.c;
                vector<double> newRow(lp.n, 0.0);
                newRow[i] = -1;
                A2.push_back(newRow);
                b2.push_back(-ceil(x[i]));
                Simplex right(A2, b2, c2);
                bestVal = branchAndBound(right, bestX, bestVal);
            }
            break;
        }
    }
    return bestVal;
}

int main() {
    // Example: Minimize z = x1 + 2x2
    // Subject to:
    // x1 + x2 = 4
    // x1, x2 >= 0, integer

    vector<vector<double>> A = { {1, 1} };
    vector<double> b = { 4 };
    vector<double> c = { 1, 2 }; // Minimize

    // Convert minimization to maximization by negating c
    for (double& ci : c) ci = -ci;

    Simplex lp(A, b, c);
    vector<double> bestX;
    double bestVal = 1e18;

    bestVal = branchAndBound(lp, bestX, bestVal);

    cout << "Optimal integer solution:\n";
    for (size_t i = 0; i < bestX.size(); i++)
        cout << "x" << i + 1 << " = " << round(bestX[i]) << "\n";
    cout << "Minimum value = " << bestVal << "\n";
    return 0;
}