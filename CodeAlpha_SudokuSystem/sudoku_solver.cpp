#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

// ─── ANSI Colors ─────────────────────────────────────────────────────────────

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define WHITE   "\033[37m"
#define DIM     "\033[2m"

// ─── Grid ────────────────────────────────────────────────────────────────────

const int SIZE = 9;

// A sample puzzle  (0 = empty cell)
int puzzle[SIZE][SIZE] = {
    {5, 3, 0,  0, 7, 0,  0, 0, 0},
    {6, 0, 0,  1, 9, 5,  0, 0, 0},
    {0, 9, 8,  0, 0, 0,  0, 6, 0},

    {8, 0, 0,  0, 6, 0,  0, 0, 3},
    {4, 0, 0,  8, 0, 3,  0, 0, 1},
    {7, 0, 0,  0, 2, 0,  0, 0, 6},

    {0, 6, 0,  0, 0, 0,  2, 8, 0},
    {0, 0, 0,  4, 1, 9,  0, 0, 5},
    {0, 0, 0,  0, 8, 0,  0, 7, 9}
};

// Keep a copy of the original to distinguish given vs solved cells
int original[SIZE][SIZE];

// ─── Display ─────────────────────────────────────────────────────────────────

void printBanner() {
    cout << "\n";
    cout << CYAN << BOLD;
    cout << "  ╔══════════════════════════════════╗\n";
    cout << "  ║       SUDOKU SOLVER  (C++)       ║\n";
    cout << "  ║    Backtracking Algorithm        ║\n";
    cout << "  ╚══════════════════════════════════╝\n";
    cout << RESET << "\n";
}

void printGrid(int grid[SIZE][SIZE], bool solved = false) {
    cout << "\n";
    cout << BOLD << DIM << "      1   2   3    4   5   6    7   8   9\n" << RESET;
    cout << "    " << DIM << "┌───────────┬───────────┬───────────┐\n" << RESET;

    for (int r = 0; r < SIZE; r++) {
        // Row label
        cout << BOLD << "  " << (r + 1) << " " << RESET;

        for (int c = 0; c < SIZE; c++) {
            // Left border
            if (c == 0)
                cout << DIM << "│ " << RESET;
            else if (c % 3 == 0)
                cout << DIM << "│ " << RESET;

            if (grid[r][c] == 0) {
                cout << DIM << "·" << RESET << " ";           // empty cell
            } else if (original[r][c] != 0) {
                cout << BOLD << YELLOW << grid[r][c] << RESET << " ";  // given
            } else {
                cout << (solved ? GREEN : CYAN) << BOLD
                     << grid[r][c] << RESET << " ";            // solved
            }
        }

        cout << DIM << "│\n" << RESET;

        // Horizontal dividers
        if (r == 2 || r == 5)
            cout << "    " << DIM << "├───────────┼───────────┼───────────┤\n" << RESET;
    }

    cout << "    " << DIM << "└───────────┴───────────┴───────────┘\n" << RESET;

    // Legend
    cout << "\n  " << YELLOW << BOLD << "■" << RESET << " Given   ";
    cout << GREEN  << BOLD << "■" << RESET << " Solved  ";
    cout << DIM    << "·" << RESET << " Empty\n\n";
}

// ─── Sudoku Rules ────────────────────────────────────────────────────────────

// Check if 'num' can be placed at grid[row][col]
bool isSafe(int grid[SIZE][SIZE], int row, int col, int num) {
    // 1. Row check
    for (int c = 0; c < SIZE; c++)
        if (grid[row][c] == num) return false;

    // 2. Column check
    for (int r = 0; r < SIZE; r++)
        if (grid[r][col] == num) return false;

    // 3. 3x3 subgrid check
    int startRow = (row / 3) * 3;
    int startCol = (col / 3) * 3;
    for (int r = startRow; r < startRow + 3; r++)
        for (int c = startCol; c < startCol + 3; c++)
            if (grid[r][c] == num) return false;

    return true;
}

// ─── Backtracking Solver ─────────────────────────────────────────────────────

int steps = 0;   // track how many attempts were made

bool solve(int grid[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {

            if (grid[row][col] == 0) {          // found an empty cell
                for (int num = 1; num <= 9; num++) {
                    steps++;

                    if (isSafe(grid, row, col, num)) {
                        grid[row][col] = num;   // tentatively place num

                        if (solve(grid))        // recurse
                            return true;

                        grid[row][col] = 0;     // backtrack
                    }
                }
                return false;   // no valid number → trigger backtrack
            }
        }
    }
    return true;   // no empty cells left → solved!
}

// ─── User Input ──────────────────────────────────────────────────────────────

void getUserPuzzle(int grid[SIZE][SIZE]) {
    cout << GREEN << "  Enter your Sudoku puzzle row by row.\n" << RESET;
    cout << DIM   << "  Use 0 for empty cells. Separate with spaces.\n\n" << RESET;

    for (int r = 0; r < SIZE; r++) {
        bool valid = false;
        while (!valid) {
            cout << BOLD << "  Row " << (r + 1) << ": " << RESET;
            valid = true;
            for (int c = 0; c < SIZE; c++) {
                cin >> grid[r][c];
                if (grid[r][c] < 0 || grid[r][c] > 9) {
                    cout << RED << "  ! Only digits 0-9 allowed. Re-enter row "
                         << (r + 1) << ".\n" << RESET;
                    valid = false;
                    // flush line
                    cin.clear();
                    cin.ignore(1000, '\n');
                    break;
                }
            }
        }
    }
}

// ─── Main ─────────────────────────────────────────────────────────────────────

int main() {
    printBanner();

    // Ask user: use built-in puzzle or enter their own
    cout << "  " << BOLD << "1." << RESET << " Solve the built-in sample puzzle\n";
    cout << "  " << BOLD << "2." << RESET << " Enter my own puzzle\n\n";
    cout << "  " << BOLD << "Choice: " << RESET;

    int choice;
    cin >> choice;
    cout << "\n";

    if (choice == 2) {
        getUserPuzzle(puzzle);
    }

    // Save original state (to color given vs solved cells)
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            original[r][c] = puzzle[r][c];

    // Show unsolved grid
    cout << BOLD << CYAN << "  ── Puzzle (to solve) ──\n" << RESET;
    printGrid(puzzle, false);

    // Solve with timing
    cout << BOLD << "  Solving... please wait.\n\n" << RESET;

    auto start = chrono::high_resolution_clock::now();
    bool solved = solve(puzzle);
    auto end   = chrono::high_resolution_clock::now();

    double ms = chrono::duration<double, milli>(end - start).count();

    if (solved) {
        cout << GREEN << BOLD << "  ✓ Puzzle Solved!\n" << RESET;
        cout << DIM   << "  Steps taken : " << steps << "\n";
        cout << "  Time taken  : " << ms << " ms\n\n" << RESET;

        cout << BOLD << GREEN << "  ── Solution ──\n" << RESET;
        printGrid(puzzle, true);
    } else {
        cout << RED << BOLD << "  ✗ No solution exists for this puzzle.\n" << RESET;
        cout << DIM << "  Double-check the numbers you entered.\n\n" << RESET;
    }

    return 0;
}