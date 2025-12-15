#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>
#include <stdexcept>

using namespace std;

class SimplexSolver {
private:
    vector<vector<double>> tableau;
    int numConstraints, numVariables;

    // Find pivot column (most negative in objective row)
    int findPivotColumn() {
        int pivotCol = -1;
        double minVal = 0.0;
        for (int j = 0; j < numVariables + numConstraints; j++) {
            if (tableau[numConstraints][j] < minVal) {
                minVal = tableau[numConstraints][j];
                pivotCol = j;
            }
        }
        return pivotCol;
    }

    // Find pivot row using minimum ratio test
    int findPivotRow(int pivotCol) {
        int pivotRow = -1;
        double minRatio = numeric_limits<double>::infinity();
        for (int i = 0; i < numConstraints; i++) {
            if (tableau[i][pivotCol] > 1e-9) { // avoid division by zero
                double ratio = tableau[i].back() / tableau[i][pivotCol];
                if (ratio < minRatio) {
                    minRatio = ratio;
                    pivotRow = i;
                }
            }
        }
        return pivotRow;
    }

    // Perform pivot operation
    void pivot(int pivotRow, int pivotCol) {
        double pivotVal = tableau[pivotRow][pivotCol];
        if (pivotVal == 0) throw runtime_error("Pivot value is zero!");

        // Normalize pivot row
        for (double& val : tableau[pivotRow]) {
            val /= pivotVal;
        }

        // Eliminate pivot column in other rows
        for (int i = 0; i <= numConstraints; i++) {
            if (i != pivotRow) {
                double factor = tableau[i][pivotCol];
                for (int j = 0; j < tableau[i].size(); j++) {
                    tableau[i][j] -= factor * tableau[pivotRow][j];
                }
            }
        }
    }

public:
#if 0
    SimplexSolver(const vector<vector<double>>& A, const vector<double>& b, const vector<double>& c) {
        numConstraints = A.size();
        numVariables = c.size();

        // Build initial tableau
        tableau.assign(numConstraints + 1, vector<double>(numVariables + numConstraints + 1, 0.0));

        // Fill constraint coefficients
        for (int i = 0; i < numConstraints; i++) {
            for (int j = 0; j < numVariables; j++) {
                tableau[i][j] = A[i][j];
            }
            tableau[i][numVariables + i] = 1.0; // slack variable
            tableau[i].back() = b[i];
        }

        // Fill objective function row
        for (int j = 0; j < numVariables; j++) {
            tableau[numConstraints][j] = -c[j]; // maximize
        }
    }
#else
    SimplexSolver(const vector<vector<double>>& A, const vector<double>& b, const vector<double>& c) {
        numConstraints = A.size();
        numVariables = c.size();

        // Build initial tableau
        tableau.assign(numConstraints + 1, vector<double>(numVariables + numConstraints + 1, 0.0));

        // Fill constraint coefficients
        for (int i = 0; i < numConstraints; i++) {
            for (int j = 0; j < numVariables; j++) {
                tableau[i][j] = A[i][j];
            }
            tableau[i][numVariables + i] = 0.0; // slack variable
            tableau[i].back() = b[i];
        }

        // Fill objective function row
        for (int j = 0; j < numVariables; j++) {
            tableau[numConstraints][j] = -c[j]; // maximize
        }
    }
#endif
    void solve() {
        while (true) {
            int pivotCol = findPivotColumn();
            if (pivotCol == -1) break; // optimal

            int pivotRow = findPivotRow(pivotCol);
            if (pivotRow == -1) throw runtime_error("Unbounded solution!");

            pivot(pivotRow, pivotCol);
            printSolution();
        }
    }

    void printSolution() {
        vector<double> solution(numVariables, 0.0);

        for (int j = 0; j < numVariables; j++) {
            int pivotRow = -1;
            bool isBasic = true;
            for (int i = 0; i < numConstraints; i++) {
                if (abs(tableau[i][j] - 1.0) < 1e-9) {
                    if (pivotRow == -1) pivotRow = i;
                    else { isBasic = false; break; }
                }
                else if (abs(tableau[i][j]) > 1e-9) {
                    isBasic = false;
                    break;
                }
            }
            if (isBasic && pivotRow != -1) {
                solution[j] = tableau[pivotRow].back();
            }
        }

        cout << "Optimal solution found:\n";
        for (int j = 0; j < numVariables; j++) {
            cout << "x" << j + 1 << " = " << solution[j] << "\n";
        }
        cout << "Max Z = " << tableau[numConstraints].back() << "\n";
    }
};

int main() {
    try {
#if 0
        // Example: Maximize Z = 3x1 + 5x2
        // Subject to:
        // 2x1 + 3x2 <= 8
        // 2x1 +   x2 <= 4
        // x1, x2 >= 0

        vector<vector<double>> A = {
            {2, 3},
            {2, 1}
        };
        vector<double> b = { 8, 4 };
        vector<double> c = { 3, 5 };
#else
        vector<vector<double>> A = {
            {0, 0, 0, 0, 1, 1},
            {0, 1, 0, 0, 0, 1 },
            {0, 0, 1, 1, 1, 0 },
            {1, 1, 0, 1, 0, 0 }
        };
        vector<double> b = { 3, 5, 4, 7 };
        vector<double> c = { 1, 1, 1, 1, 1, 1 }; // output
#endif
        SimplexSolver solver(A, b, c);
        solver.solve();
        solver.printSolution();
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
    }
    return 0;
}