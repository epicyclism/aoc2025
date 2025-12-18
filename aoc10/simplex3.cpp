#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <limits>

using namespace std;

const double EPS = 1e-9; // Tolerance for floating-point comparisons

class Simplex {
private:
    int m, n; // m = constraints, n = variables
    vector<vector<double>> A; // Tableau
    vector<int> basis;        // Basic variable indices

public:
    Simplex(const vector<vector<double>>& constraints, const vector<double>& b, const vector<double>& c) {
        m = constraints.size();
        n = c.size();

        // Build initial tableau for Phase 1
        // Columns: original vars + artificial vars + RHS
        A.assign(m + 1, vector<double>(n + m + 1, 0.0));

        // Fill constraint coefficients
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                A[i][j] = constraints[i][j];
            }
        }

        // Add artificial variables for equality constraints
        for (int i = 0; i < m; i++) {
            A[i][n + i] = 1.0; // Artificial variable
            basis.push_back(n + i);
        }

        // RHS
        for (int i = 0; i < m; i++) {
            A[i].back() = b[i];
        }

        // Phase 1 objective: minimize sum of artificial vars
        for (int j = n; j < n + m; j++) {
            A[m][j] = 1.0;
        }

        // Make Phase 1 objective row consistent
        for (int i = 0; i < m; i++) {
            for (int j = 0; j <= n + m; j++) {
                A[m][j] -= A[i][j];
            }
        }

        // Run Phase 1
        simplex(n + m);

        if (A[m].back() > EPS) {
            throw runtime_error("No feasible solution (infeasible problem).");
        }

        // Remove artificial variables and set original objective
        A[m].assign(n + m + 1, 0.0);
        for (int j = 0; j < n; j++) {
            A[m][j] = c[j]; // Original minimization objective
        }

        // Adjust objective row for current basis
        for (int i = 0; i < m; i++) {
            if (basis[i] < n) {
                for (int j = 0; j <= n + m; j++) {
                    A[m][j] -= A[i][j] * c[basis[i]];
                }
            }
        }

        // Run Phase 2
        simplex(n);
    }

    void simplex(int cols) {
        while (true) {
            // Find entering variable (most negative for minimization)
            int pivotCol = -1;
            for (int j = 0; j < cols; j++) {
                if (A[m][j] < -EPS && (pivotCol == -1 || A[m][j] < A[m][pivotCol])) {
                    pivotCol = j;
                }
            }
            if (pivotCol == -1) break; // Optimal

            // Find leaving variable (minimum ratio test)
            int pivotRow = -1;
            double minRatio = numeric_limits<double>::infinity();
            for (int i = 0; i < m; i++) {
                if (A[i][pivotCol] > EPS) {
                    double ratio = A[i].back() / A[i][pivotCol];
                    if (ratio < minRatio) {
                        minRatio = ratio;
                        pivotRow = i;
                    }
                }
            }
            if (pivotRow == -1) throw runtime_error("Unbounded solution.");

            pivot(pivotRow, pivotCol);
            basis[pivotRow] = pivotCol;
        }
    }

    void pivot(int row, int col) {
        double pivotVal = A[row][col];
        for (double& x : A[row]) x /= pivotVal;
        for (int i = 0; i <= m; i++) {
            if (i != row) {
                double factor = A[i][col];
                for (int j = 0; j < (int)A[i].size(); j++) {
                    A[i][j] -= factor * A[row][j];
                }
            }
        }
    }

    vector<double> getSolution() {
        vector<double> x(n, 0.0);
        for (int i = 0; i < m; i++) {
            if (basis[i] < n) {
                x[basis[i]] = A[i].back();
            }
        }
        return x;
    }

    double getObjectiveValue() {
        return A[m].back();
    }
};

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

void t1()
{
    vector<vector<double>> constraints = {
    {0, 0, 0, 0, 1, 1},
    {0, 1, 0, 0, 0, 1},
    {0, 0, 1, 1, 1, 0},
    {1, 1, 0, 1, 0, 0}
    };
    vector<double> b = { 3, 5, 4, 7 };
    vector<double> c = { 1, 1, 1, 1, 1, 1 }; // output

    Simplex solver(constraints, b, c);

    vector<double> sol = solver.getSolution();
    double obj = solver.getObjectiveValue();

    cout << fixed << setprecision(6);
    cout << "Optimal solution:\n";
    for (size_t i = 0; i < sol.size(); i++) {
        cout << "x" << (i + 1) << " = " << sol[i] << "\n";
    }
    cout << "Minimum value of objective function: " << obj << "\n";
}

void t2()
{
    vector<vector<double>> constraints = {
    { 1, 0, 1, 1, 0 },
    { 0, 0, 0, 1, 1 },
    { 1, 1, 0, 1, 1 },
    {1, 1, 0, 0, 1},
    {1, 0, 1, 0, 1}
    };
    vector<double> b = { 7, 5, 12, 7, 2 };
    vector<double> c = { 1, 1, 1, 1, 1 }; // output

    Simplex solver(constraints, b, c);

    vector<double> sol = solver.getSolution();
    double obj = solver.getObjectiveValue();

    cout << fixed << setprecision(6);
    cout << "Optimal solution:\n";
    for (size_t i = 0; i < sol.size(); i++) {
        cout << "x" << (i + 1) << " = " << sol[i] << "\n";
    }
    cout << "Minimum value of objective function: " << obj << "\n";
}

void t3()
{
    vector<vector<double>> constraints = {
    {1, 1, 1, 0},
        {1, 0, 1, 1 },
        {1, 0, 1, 1},
{1, 1, 0, 0},
{1, 1, 1, 0},
{0, 0, 1, 0}
    };
    vector<double> b = { 10, 11, 11, 5, 10, 5 };
    vector<double> c = { 1, 1, 1, 1 }; // output

    Simplex solver(constraints, b, c);

    vector<double> sol = solver.getSolution();
    double obj = solver.getObjectiveValue();

    cout << fixed << setprecision(6);
    cout << "Optimal solution:\n";
    for (size_t i = 0; i < sol.size(); i++) {
        cout << "x" << (i + 1) << " = " << sol[i] << "\n";
    }
    cout << "Minimum value of objective function: " << obj << "\n";
}

int main() {
    t1();
    t2();
    t3();

    return 0;
}