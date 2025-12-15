#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>
#include <stdexcept>

using namespace std;

class Simplex {
private:
    vector<vector<double>> tableau;
    int numConstraints, numVariables;

    // Pivot operation
    void pivot(int pivotRow, int pivotCol) {
        double pivotVal = tableau[pivotRow][pivotCol];
        if (pivotVal == 0) throw runtime_error("Pivot value is zero — cannot proceed.");

        // Normalize pivot row
        for (int j = 0; j < tableau[0].size(); j++)
            tableau[pivotRow][j] /= pivotVal;

        // Eliminate pivot column in other rows
        for (int i = 0; i < tableau.size(); i++) {
            if (i != pivotRow) {
                double factor = tableau[i][pivotCol];
                for (int j = 0; j < tableau[0].size(); j++)
                    tableau[i][j] -= factor * tableau[pivotRow][j];
            }
        }
    }

    // Find entering variable (most negative in objective row for maximization)
    int findEnteringColumn() {
        int col = -1;
        double minVal = 0;
        for (int j = 0; j < numVariables + numConstraints; j++) {
            if (tableau[0][j] < minVal) {
                minVal = tableau[0][j];
                col = j;
            }
        }
        return col;
    }

    // Find leaving variable (minimum ratio test)
    int findLeavingRow(int pivotCol) {
        int row = -1;
        double minRatio = numeric_limits<double>::infinity();
        for (int i = 1; i <= numConstraints; i++) {
            if (tableau[i][pivotCol] > 1e-9) { // avoid division by zero
                double ratio = tableau[i].back() / tableau[i][pivotCol];
                if (ratio < minRatio) {
                    minRatio = ratio;
                    row = i;
                }
            }
        }
        return row;
    }

public:
    Simplex(const vector<vector<double>>& A, const vector<double>& b, const vector<double>& c) {
        numConstraints = A.size();
        numVariables = c.size();

        // Build tableau for maximization (convert minimization by negating c)
        tableau.assign(numConstraints + 1, vector<double>(numVariables + numConstraints + 1, 0));

        // Objective row
        for (int j = 0; j < numVariables; j++)
            tableau[0][j] = -c[j]; // Negate for minimization

        // Constraints
        for (int i = 0; i < numConstraints; i++) {
            for (int j = 0; j < numVariables; j++)
                tableau[i + 1][j] = A[i][j];
            tableau[i + 1][numVariables + i] = 1; // slack variable
            tableau[i + 1].back() = b[i];
        }
    }

    void solve() {
        while (true) {
            int pivotCol = findEnteringColumn();
            if (pivotCol == -1) break; // optimal

            int pivotRow = findLeavingRow(pivotCol);
            if (pivotRow == -1) throw runtime_error("Unbounded solution.");

            pivot(pivotRow, pivotCol);
        }
    }

    void printSolution() {
        vector<double> solution(numVariables, 0);
        for (int j = 0; j < numVariables; j++) {
            int pivotRow = -1;
            for (int i = 1; i <= numConstraints; i++) {
                if (abs(tableau[i][j] - 1) < 1e-9) {
                    bool isBasic = true;
                    for (int k = 0; k <= numConstraints; k++) {
                        if (k != i && abs(tableau[k][j]) > 1e-9) {
                            isBasic = false;
                            break;
                        }
                    }
                    if (isBasic) {
                        pivotRow = i;
                        break;
                    }
                }
            }
            if (pivotRow != -1) solution[j] = tableau[pivotRow].back();
        }

        cout << "\nOptimal solution found:\n";
        for (int j = 0; j < numVariables; j++)
            cout << "x" << j + 1 << " = " << solution[j] << "\n";

        cout << "Minimum value: " << tableau[0].back() << "\n";
    }
};

int main() {
    try {
#if 0
        int m, n;
        cout << "Enter number of constraints: ";
        if (!(cin >> m) || m <= 0) throw invalid_argument("Invalid number of constraints.");

        cout << "Enter number of variables: ";
        if (!(cin >> n) || n <= 0) throw invalid_argument("Invalid number of variables.");

        vector<vector<double>> A(m, vector<double>(n));
        vector<double> b(m), c(n);

        cout << "Enter constraint coefficients (A matrix):\n";
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                if (!(cin >> A[i][j])) throw invalid_argument("Invalid coefficient.");

        cout << "Enter RHS values (b vector):\n";
        for (int i = 0; i < m; i++)
            if (!(cin >> b[i])) throw invalid_argument("Invalid RHS value.");

        cout << "Enter objective function coefficients (c vector) for minimization:\n";
        for (int j = 0; j < n; j++)
            if (!(cin >> c[j])) throw invalid_argument("Invalid objective coefficient.");
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
        Simplex solver(A, b, c);
        solver.solve();
        solver.printSolution();

    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}