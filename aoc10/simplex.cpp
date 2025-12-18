//
// derived from an idea by CoPilot
//
#include <limits>
#include <stdexcept>

#include "simplex.h"

// simplex method with equal constraints, minimum objective
// 
// b the target, c picks off the objective function
// for AoC 2025 Day 10 pt2 example 1,
//
// std::vector<double>, double> constraints = {{0, 0, 0, 0, 1, 1},
//    { 0, 1, 0, 0, 0, 1 },
//    { 0, 0, 1, 1, 1, 0 },
//    { 1, 1, 0, 1, 0, 0 }
//    };
//    std::vector<double> b = { 3, 5, 4, 7 };
//    std::vector<double> c = { 1, 1, 1, 1, 1, 1 };
//
//   produces a different solution to the problem but same minimum
//   1 5 0 1 3 0 as opposed to 1 3 0 3 1 2
//

const double EPS = 1e-9; // Tolerance for floating-point comparisons

using T = double;

std::pair<std::vector<T>, T> solve_equal_min (constraint_t const& constraints,
                                                            target_t const& b,
                                                            objective_t const& c)
{
    std::vector<std::vector<T>> A; // Tableau
	std::vector<int> basis;        // Basic variable indices
    auto m = constraints.size();
    auto n = c.size();

    auto pivot = [&](int row, int col)
        {
            double pivotVal = A[row][col];
            for (auto& x : A[row])
                x /= pivotVal;
            for (int i = 0; i <= m; i++)
            {
                if (i != row)
                {
                    auto factor = A[i][col];
                    for (auto j = 0; j < A[i].size(); j++)
                        A[i][j] -= factor * A[row][j];
                }
            }
        };
    auto simplex = [&](int cols)
        {
            while (true)
            {
                // Find entering variable (most negative for minimization)
                int pivotCol = -1;
                for (int j = 0; j < cols; j++)
                {
                    if (A[m][j] < -EPS && (pivotCol == -1 || A[m][j] < A[m][pivotCol]))
                        pivotCol = j;
                }
                if (pivotCol == -1)
                    break; // Optimal

                // Find leaving variable (minimum ratio test)
                int pivotRow = -1;
                double minRatio = std::numeric_limits<double>::infinity();
                for (int i = 0; i < m; i++)
                {
                    if (A[i][pivotCol] > EPS)
                    {
                        double ratio = (double)A[i].back() / A[i][pivotCol];
                        if (ratio < minRatio)
                        {
                            minRatio = ratio;
                            pivotRow = i;
                        }
                    }
                }
                if (pivotRow == -1)
                    throw std::runtime_error("Unbounded solution.");

                pivot(pivotRow, pivotCol);
                basis[pivotRow] = pivotCol;
            }
        };

    // Build initial tableau for Phase 1
    // Columns: original vars + artificial vars + RHS
    A.assign(m + 1, std::vector<T>(n + m + 1, 0.0));

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

//    if (A[m].back() > EPS) {
//        throw std::runtime_error("No feasible solution (infeasible problem).");
//    }

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
    
    // extract solution
    std::vector<T> x(n, 0.0);
    for (int i = 0; i < m; i++)
        if (basis[i] < n)
            x[basis[i]] = A[i].back();
            

    return { x, -1 * A[m].back() };
}
