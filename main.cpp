#include <iostream>
#include <fstream>
#include <vector>
#include "tabulate.hpp"

using namespace std;

class Table {
private:
    int n, m;
    vector<double> C, delta, l;
    vector<vector<double>> A;
    bool highlight = false;
    int currX = 0, currY = 0;
public:
    Table(int n, int m) : n(n), m(m), C(n), delta(n + 1), l(m), A(m, vector<double>(n + 1)) {};

    void fromFile(const string &filename) {
        ifstream file(filename);
        for (int i = 0; i < n; ++i)
            file >> C[i];
        for (int i = 0; i < m; ++i) {
            file >> l[i];
            for (int j = 0; j <= n; ++j)
                file >> A[i][j];
        }
        file.close();
    }

    friend ostream &operator<<(ostream &out, const Table &table) {
        tabulate::Table res_table;
        tabulate::Table::Row_t row(table.n + 2);

        row[0] = "C";
        for (int i = 0; i < table.n; ++i)
            row[i + 1] = to_string(table.C[i]);
        row[table.n + 1] = "b";
        res_table.add_row(row);

        for (int i = 0; i < table.m; ++i) {
            row[0] = to_string(table.l[i]);
            for (int j = 0; j <= table.n; ++j) {
                row[j + 1] = to_string(table.A[i][j]);
            }
            res_table.add_row(row);
        }

        row[0] = "delta";
        for (int i = 0; i <= table.n; ++i)
            row[i + 1] = to_string(table.delta[i]);
        res_table.add_row(row);

        if (table.highlight) {
            res_table[table.currY + 1][table.currX + 1].format()
                    .border_top("=")
                    .border_left("#");
            res_table[table.currY + 1][table.currX + 2].format()
                    .border_left("#");
            res_table[table.currY + 2][table.currX + 1].format()
                    .border_top("=");
        }

        return out << res_table;
    }

    void calculateDelta() {
        for (int i = 0; i < n; ++i) {
            delta[i] = -C[i];
        }
        delta[n] = 0;
        for (int i = 0; i <= n; ++i) {
            for (int j = 0; j < m; ++j) {
                delta[i] += l[j] * A[j][i];
            }
        }
    }

    [[nodiscard]] bool isFinished() const {
        for (int i = 0; i < n; ++i) {
            if (delta[i] > 0)
                return false;
        }
        return true;
    }

    void findCell() {
        double max = -1;
        for (int i = 0; i < n; ++i)
            if (delta[i] > max) {
                currX = i;
                max = delta[i];
            }
        int i;
        double min;
        for (i = 0; i < m; ++i) {
            if (A[i][currX] > 0) {
                min = A[i][n] / A[i][currX];
                currY = i;
                break;
            }
        }
        for (; i < m; ++i)
            if (A[i][currX] > 0 && A[i][n] / A[i][currX] < min) {
                min = A[i][n] / A[i][currX];
                currY = i;
            }
        highlight = true;
    }

    void doSmth() {
        vector<vector<double>> oldA = A;
        for (int i = 0; i < currY; ++i) {
            for (int j = 0; j <= n; ++j) {
                A[i][j] -= oldA[i][currX] * oldA[currY][j] / oldA[currY][currX];
            }
        }
        for (int i = 0; i <= n; ++i) {
            A[currY][i] /= oldA[currY][currX];
        }
        for (int i = currY + 1; i < m; ++i) {
            for (int j = 0; j <= n; ++j) {
                A[i][j] -= oldA[i][currX] * oldA[currY][j] / oldA[currY][currX];
            }
        }
        l[currY] = C[currX];
        highlight = false;
    }

    double getResult() {
        return delta[n];
    }
};

int main() {
    cout << "Hello, World!" << endl;
    Table table(5, 3);
    table.fromFile("input.txt");
    table.calculateDelta();

    while (!table.isFinished()) {
        table.findCell();
        cout << table << endl;
        table.doSmth();
        table.calculateDelta();
    }
    cout << table << endl;

    double res = table.getResult();
    cout << endl << "min is " << res << endl;

    return 0;
}
