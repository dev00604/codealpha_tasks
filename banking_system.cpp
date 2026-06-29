#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include <limits>

using namespace std;

// ─── ANSI Colors ─────────────────────────────────────────────────────────────

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"
#define DIM     "\033[2m"
#define MAGENTA "\033[35m"

// ─── Helpers ─────────────────────────────────────────────────────────────────

void clearInput() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void printLine() {
    cout << DIM << "  ────────────────────────────────────────────\n" << RESET;
}

void printHeader() {
    cout << "\n";
    cout << CYAN << BOLD;
    cout << "  ╔══════════════════════════════════════════╗\n";
    cout << "  ║        BANK MANAGEMENT SYSTEM  (C++)     ║\n";
    cout << "  ╚══════════════════════════════════════════╝\n";
    cout << RESET << "\n";
}

string currentTimestamp() {
    time_t t = time(nullptr);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
    return string(buf);
}

// ─── Transaction Class ───────────────────────────────────────────────────────

class Transaction {
public:
    string type;       // DEPOSIT / WITHDRAW / TRANSFER_IN / TRANSFER_OUT
    double amount;
    double balanceAfter;
    string timestamp;
    string note;

    Transaction(string t, double amt, double bal, string n = "")
        : type(t), amount(amt), balanceAfter(bal),
          timestamp(currentTimestamp()), note(n) {}

    // Serialize to string for file storage
    string serialize() const {
        return type + "|" + to_string(amount) + "|" +
               to_string(balanceAfter) + "|" + timestamp + "|" + note;
    }

    // Deserialize from string
    static Transaction deserialize(const string &line) {
        istringstream ss(line);
        string type, amtStr, balStr, ts, note;
        getline(ss, type,   '|');
        getline(ss, amtStr, '|');
        getline(ss, balStr, '|');
        getline(ss, ts,     '|');
        getline(ss, note);
        Transaction tx(type, stod(amtStr), stod(balStr), note);
        tx.timestamp = ts;
        return tx;
    }

    void print() const {
        string color = RESET;
        if (type == "DEPOSIT"      || type == "TRANSFER_IN")  color = GREEN;
        if (type == "WITHDRAW"     || type == "TRANSFER_OUT") color = RED;

        cout << "  " << DIM << timestamp << RESET << "  "
             << color << BOLD << left << setw(14) << type << RESET
             << "  " << YELLOW << setw(10) << fixed << setprecision(2) << amount << RESET
             << "  Bal: " << setw(10) << balanceAfter;
        if (!note.empty()) cout << "  " << DIM << note << RESET;
        cout << "\n";
    }
};

// ─── Account Class ───────────────────────────────────────────────────────────

class Account {
public:
    int    accNumber;
    string ownerID;       // links to Customer
    double balance;
    vector<Transaction> history;

    Account() : accNumber(0), balance(0.0) {}
    Account(int num, string owner, double opening)
        : accNumber(num), ownerID(owner), balance(opening) {
        history.emplace_back("DEPOSIT", opening, opening, "Opening deposit");
    }

    void deposit(double amount, string note = "") {
        balance += amount;
        history.emplace_back("DEPOSIT", amount, balance, note);
    }

    bool withdraw(double amount, string note = "") {
        if (amount > balance) return false;
        balance -= amount;
        history.emplace_back("WITHDRAW", amount, balance, note);
        return true;
    }

    void printStatement() const {
        cout << "\n" << BOLD << CYAN
             << "  Account #" << accNumber << "  —  Balance: "
             << fixed << setprecision(2) << balance << RESET << "\n";
        printLine();
        cout << "  " << DIM
             << left << setw(20) << "Timestamp"
             << setw(16) << "Type"
             << setw(12) << "Amount"
             << "Balance After\n" << RESET;
        printLine();
        for (const auto &tx : history)
            tx.print();
        printLine();
    }

    // ── File I/O ──
    void saveToFile() const {
        string filename = "acc_" + to_string(accNumber) + ".dat";
        ofstream f(filename);
        f << accNumber << "\n" << ownerID << "\n"
          << fixed << setprecision(2) << balance << "\n";
        for (const auto &tx : history)
            f << tx.serialize() << "\n";
    }

    static Account loadFromFile(int accNum) {
        string filename = "acc_" + to_string(accNum) + ".dat";
        ifstream f(filename);
        Account a;
        if (!f.is_open()) return a;
        string line;
        getline(f, line); a.accNumber = stoi(line);
        getline(f, a.ownerID);
        getline(f, line); a.balance = stod(line);
        while (getline(f, line))
            if (!line.empty())
                a.history.push_back(Transaction::deserialize(line));
        return a;
    }
};

// ─── Customer Class ──────────────────────────────────────────────────────────

class Customer {
public:
    string id;         // e.g. "C001"
    string name;
    string phone;
    vector<int> accountNumbers;

    Customer() {}
    Customer(string i, string n, string p)
        : id(i), name(n), phone(p) {}

    void printInfo() const {
        cout << "\n" << BOLD << YELLOW << "  Customer: " << name << RESET
             << "  (ID: " << id << "  Phone: " << phone << ")\n";
        cout << "  Accounts: ";
        if (accountNumbers.empty()) cout << "none";
        for (int a : accountNumbers) cout << "#" << a << "  ";
        cout << "\n";
    }

    // ── File I/O ──
    void saveToFile() const {
        string filename = "cust_" + id + ".dat";
        ofstream f(filename);
        f << id << "\n" << name << "\n" << phone << "\n";
        for (int a : accountNumbers) f << a << "\n";
    }

    static Customer loadFromFile(const string &custID) {
        string filename = "cust_" + custID + ".dat";
        ifstream f(filename);
        Customer c;
        if (!f.is_open()) return c;
        getline(f, c.id);
        getline(f, c.name);
        getline(f, c.phone);
        string line;
        while (getline(f, line))
            if (!line.empty()) c.accountNumbers.push_back(stoi(line));
        return c;
    }

    static bool exists(const string &custID) {
        ifstream f("cust_" + custID + ".dat");
        return f.good();
    }
};

// ─── Bank Operations ─────────────────────────────────────────────────────────

int nextAccNumber() {
    ifstream f("acc_counter.dat");
    int n = 1001;
    if (f.is_open()) f >> n;
    f.close();
    ofstream o("acc_counter.dat");
    o << (n + 1);
    return n;
}

string nextCustID() {
    ifstream f("cust_counter.dat");
    int n = 1;
    if (f.is_open()) f >> n;
    f.close();
    ofstream o("cust_counter.dat");
    o << (n + 1);
    ostringstream oss;
    oss << "C" << setw(3) << setfill('0') << n;
    return oss.str();
}

// ─── Menus ───────────────────────────────────────────────────────────────────

void registerCustomer() {
    string name, phone;
    cout << "\n" << BOLD << "[ NEW CUSTOMER ]\n" << RESET;
    printLine();
    cout << "  Name  : "; clearInput(); getline(cin, name);
    cout << "  Phone : "; getline(cin, phone);

    if (name.empty()) { cout << RED << "  ! Name cannot be empty.\n" << RESET; return; }

    string id = nextCustID();
    Customer c(id, name, phone);
    c.saveToFile();
    cout << GREEN << BOLD << "\n  ✓ Customer registered! Your ID is: " << id << "\n" << RESET;
}

void createAccount() {
    string custID;
    double opening;
    cout << "\n" << BOLD << "[ CREATE ACCOUNT ]\n" << RESET;
    printLine();
    cout << "  Customer ID : "; cin >> custID;

    if (!Customer::exists(custID)) {
        cout << RED << "  ! Customer " << custID << " not found.\n" << RESET;
        return;
    }

    cout << "  Opening Deposit : "; cin >> opening;
    if (opening < 0) { cout << RED << "  ! Cannot be negative.\n" << RESET; return; }

    int accNum = nextAccNumber();
    Account a(accNum, custID, opening);
    a.saveToFile();

    Customer c = Customer::loadFromFile(custID);
    c.accountNumbers.push_back(accNum);
    c.saveToFile();

    cout << GREEN << BOLD << "\n  ✓ Account #" << accNum
         << " created for " << c.name << ".\n" << RESET;
}

void depositMenu() {
    int accNum; double amount;
    cout << "\n" << BOLD << "[ DEPOSIT ]\n" << RESET; printLine();
    cout << "  Account # : "; cin >> accNum;

    Account a = Account::loadFromFile(accNum);
    if (a.accNumber == 0) { cout << RED << "  ! Account not found.\n" << RESET; return; }

    cout << "  Current Balance : " << YELLOW << fixed << setprecision(2)
         << a.balance << RESET << "\n";
    cout << "  Amount to deposit : "; cin >> amount;
    if (amount <= 0) { cout << RED << "  ! Must be positive.\n" << RESET; return; }

    a.deposit(amount);
    a.saveToFile();
    cout << GREEN << BOLD << "  ✓ Deposited " << amount
         << "  |  New Balance: " << a.balance << "\n" << RESET;
}

void withdrawMenu() {
    int accNum; double amount;
    cout << "\n" << BOLD << "[ WITHDRAW ]\n" << RESET; printLine();
    cout << "  Account # : "; cin >> accNum;

    Account a = Account::loadFromFile(accNum);
    if (a.accNumber == 0) { cout << RED << "  ! Account not found.\n" << RESET; return; }

    cout << "  Current Balance : " << YELLOW << fixed << setprecision(2)
         << a.balance << RESET << "\n";
    cout << "  Amount to withdraw : "; cin >> amount;
    if (amount <= 0) { cout << RED << "  ! Must be positive.\n" << RESET; return; }

    if (!a.withdraw(amount)) {
        cout << RED << "  ! Insufficient funds.\n" << RESET; return;
    }
    a.saveToFile();
    cout << GREEN << BOLD << "  ✓ Withdrawn " << amount
         << "  |  New Balance: " << a.balance << "\n" << RESET;
}

void transferMenu() {
    int fromAcc, toAcc; double amount;
    cout << "\n" << BOLD << "[ FUND TRANSFER ]\n" << RESET; printLine();
    cout << "  From Account # : "; cin >> fromAcc;
    cout << "  To   Account # : "; cin >> toAcc;

    Account src = Account::loadFromFile(fromAcc);
    Account dst = Account::loadFromFile(toAcc);

    if (src.accNumber == 0) { cout << RED << "  ! Source account not found.\n" << RESET; return; }
    if (dst.accNumber == 0) { cout << RED << "  ! Destination account not found.\n" << RESET; return; }
    if (fromAcc == toAcc)   { cout << RED << "  ! Cannot transfer to same account.\n" << RESET; return; }

    cout << "  Your Balance : " << YELLOW << fixed << setprecision(2)
         << src.balance << RESET << "\n";
    cout << "  Amount to transfer : "; cin >> amount;
    if (amount <= 0) { cout << RED << "  ! Must be positive.\n" << RESET; return; }

    if (!src.withdraw(amount, "Transfer to #" + to_string(toAcc))) {
        cout << RED << "  ! Insufficient funds.\n" << RESET; return;
    }
    dst.deposit(amount, "Transfer from #" + to_string(fromAcc));

    src.saveToFile();
    dst.saveToFile();

    cout << GREEN << BOLD << "  ✓ Transferred " << amount
         << " from #" << fromAcc << " to #" << toAcc << "\n" << RESET;
    cout << DIM   << "  Your new balance : " << src.balance << "\n" << RESET;
}

void viewStatement() {
    int accNum;
    cout << "\n" << BOLD << "[ ACCOUNT STATEMENT ]\n" << RESET; printLine();
    cout << "  Account # : "; cin >> accNum;

    Account a = Account::loadFromFile(accNum);
    if (a.accNumber == 0) { cout << RED << "  ! Account not found.\n" << RESET; return; }

    a.printStatement();
}

void viewCustomer() {
    string custID;
    cout << "\n" << BOLD << "[ CUSTOMER INFO ]\n" << RESET; printLine();
    cout << "  Customer ID : "; cin >> custID;

    if (!Customer::exists(custID)) {
        cout << RED << "  ! Customer not found.\n" << RESET; return;
    }

    Customer c = Customer::loadFromFile(custID);
    c.printInfo();

    // Show balance of each account
    for (int accNum : c.accountNumbers) {
        Account a = Account::loadFromFile(accNum);
        if (a.accNumber != 0)
            cout << "    " << CYAN << "Account #" << accNum
                 << "  Balance: " << fixed << setprecision(2)
                 << a.balance << RESET << "\n";
    }
}

// ─── Main ─────────────────────────────────────────────────────────────────────

int main() {
    int choice;

    do {
        printHeader();
        cout << "  " << BOLD << "1." << RESET << " Register New Customer\n";
        cout << "  " << BOLD << "2." << RESET << " Create Account\n";
        cout << "  " << BOLD << "3." << RESET << " Deposit\n";
        cout << "  " << BOLD << "4." << RESET << " Withdraw\n";
        cout << "  " << BOLD << "5." << RESET << " Fund Transfer\n";
        cout << "  " << BOLD << "6." << RESET << " View Account Statement\n";
        cout << "  " << BOLD << "7." << RESET << " View Customer Info\n";
        cout << "  " << BOLD << "0." << RESET << " Exit\n";
        printLine();
        cout << "  Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: registerCustomer(); break;
            case 2: createAccount();    break;
            case 3: depositMenu();      break;
            case 4: withdrawMenu();     break;
            case 5: transferMenu();     break;
            case 6: viewStatement();    break;
            case 7: viewCustomer();     break;
            case 0: cout << "\n  " << GREEN << "Goodbye!\n\n" << RESET; break;
            default: cout << RED << "  ! Invalid choice.\n" << RESET; break;
        }

        if (choice != 0) {
            cout << "\n  " << DIM << "Press Enter to continue..." << RESET;
            clearInput();
            cin.get();
        }

    } while (choice != 0);

    return 0;
}