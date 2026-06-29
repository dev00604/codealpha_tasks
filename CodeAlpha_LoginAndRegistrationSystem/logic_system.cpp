#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>

using namespace std;

// ─── Constants ───────────────────────────────────────────────────────────────

const string DB_FILE     = "users.dat";
const int    MIN_PASS_LEN = 6;

// ─── Simple Hash (djb2) ──────────────────────────────────────────────────────
// Note: For production use a proper library like bcrypt.
// This keeps the project self-contained with no external dependencies.

string hashPassword(const string &password) {
    unsigned long hash = 5381;
    for (char c : password)
        hash = ((hash << 5) + hash) + (unsigned char)c;

    ostringstream oss;
    oss << hex << setw(16) << setfill('0') << hash;
    return oss.str();
}

// ─── Helpers ─────────────────────────────────────────────────────────────────

void printLine() {
    cout << "------------------------------------------------------------\n";
}

void printHeader() {
    cout << "\n============================================================\n";
    cout << "           LOGIN & REGISTRATION SYSTEM  (C++)\n";
    cout << "============================================================\n";
}

void clearInput() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool isValidUsername(const string &u) {
    if (u.length() < 3 || u.length() > 20) return false;
    for (char c : u)
        if (!isalnum(c) && c != '_') return false;
    return true;
}

bool isValidPassword(const string &p) {
    if (p.length() < MIN_PASS_LEN) return false;
    bool hasUpper = false, hasDigit = false;
    for (char c : p) {
        if (isupper(c)) hasUpper = true;
        if (isdigit(c)) hasDigit = true;
    }
    return hasUpper && hasDigit;
}

// ─── File Operations ─────────────────────────────────────────────────────────

// Returns true if username already exists in DB
bool userExists(const string &username) {
    ifstream file(DB_FILE);
    if (!file.is_open()) return false;

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string storedUser;
        getline(iss, storedUser, ':');
        if (storedUser == username) return true;
    }
    return false;
}

// Saves a new user record  (username:hashedPassword)
void saveUser(const string &username, const string &hashedPass) {
    ofstream file(DB_FILE, ios::app);
    file << username << ":" << hashedPass << "\n";
}

// Returns true if credentials match a stored record
bool verifyUser(const string &username, const string &hashedPass) {
    ifstream file(DB_FILE);
    if (!file.is_open()) return false;

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string storedUser, storedPass;
        getline(iss, storedUser, ':');
        getline(iss, storedPass);
        if (storedUser == username && storedPass == hashedPass) return true;
    }
    return false;
}

// ─── Registration ────────────────────────────────────────────────────────────

void registerUser() {
    string username, password, confirm;

    cout << "\n[ REGISTER ]\n";
    printLine();

    // ── Username ──
    cout << "  Enter username (3-20 chars, letters/digits/_): ";
    cin >> username;

    if (!isValidUsername(username)) {
        cout << "  ✗ Invalid username. Use 3–20 alphanumeric characters or '_'.\n";
        clearInput();
        return;
    }
    if (userExists(username)) {
        cout << "  ✗ Username '" << username << "' is already taken.\n";
        clearInput();
        return;
    }

    // ── Password ──
    cout << "  Enter password (min " << MIN_PASS_LEN
         << " chars, 1 uppercase, 1 digit): ";
    cin >> password;

    if (!isValidPassword(password)) {
        cout << "  ✗ Password too weak. Need >= " << MIN_PASS_LEN
             << " chars, at least 1 uppercase letter and 1 digit.\n";
        clearInput();
        return;
    }

    cout << "  Confirm password : ";
    cin >> confirm;

    if (password != confirm) {
        cout << "  ✗ Passwords do not match.\n";
        clearInput();
        return;
    }

    // ── Save ──
    saveUser(username, hashPassword(password));
    clearInput();
    cout << "  ✓ Registration successful! You can now log in.\n";
}

// ─── Login ───────────────────────────────────────────────────────────────────

void loginUser() {
    string username, password;
    const int MAX_ATTEMPTS = 3;

    cout << "\n[ LOGIN ]\n";
    printLine();

    cout << "  Enter username : ";
    cin >> username;
    clearInput();

    if (!userExists(username)) {
        cout << "  ✗ Username '" << username << "' not found.\n";
        return;
    }

    for (int attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
        cout << "  Enter password (attempt " << attempt << "/"
             << MAX_ATTEMPTS << "): ";
        cin >> password;
        clearInput();

        if (verifyUser(username, hashPassword(password))) {
            cout << "  ✓ Login successful! Welcome, " << username << "!\n";
            return;
        }
        cout << "  ✗ Incorrect password.\n";
    }

    cout << "  ✗ Too many failed attempts. Access denied.\n";
}

// ─── List Users (debug / admin) ──────────────────────────────────────────────

void listUsers() {
    ifstream file(DB_FILE);
    if (!file.is_open()) {
        cout << "\n  ! No users registered yet.\n";
        return;
    }

    cout << "\n[ REGISTERED USERS ]\n";
    printLine();
    cout << left << setw(22) << "Username" << "Password Hash\n";
    printLine();

    string line;
    int count = 0;
    while (getline(file, line)) {
        istringstream iss(line);
        string u, h;
        getline(iss, u, ':');
        getline(iss, h);
        cout << left << setw(22) << u << h << "\n";
        count++;
    }

    if (count == 0) cout << "  (No users found)\n";
    else { printLine(); cout << "  Total users: " << count << "\n"; }
}

// ─── Main Menu ────────────────────────────────────────────────────────────────

int main() {
    int choice;

    do {
        printHeader();
        cout << "  1. Register\n";
        cout << "  2. Login\n";
        cout << "  3. List Registered Users\n";
        cout << "  0. Exit\n";
        printLine();
        cout << "  Enter your choice: ";
        cin >> choice;
        clearInput();

        switch (choice) {
            case 1: registerUser(); break;
            case 2: loginUser();    break;
            case 3: listUsers();    break;
            case 0: cout << "\n  Goodbye!\n\n"; break;
            default: cout << "\n  ! Invalid choice.\n"; break;
        }

        if (choice != 0) {
            cout << "\n  Press Enter to continue...";
            cin.get();
        }

    } while (choice != 0);

    return 0;
}