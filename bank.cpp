#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector> 
using namespace std;

// ========== ACCOUNT CLASS ==========
class Account {
private:
    int accNumber;
    char name[50];
    char password[20];
    float balance;

public:
    // Constructor
    Account() {
        accNumber = 0;
        balance = 0;
    }

    // ===== CREATE NEW ACCOUNT =====
    void createAccount() {
        cout << "\n--- Create New Account ---\n";
        cout << "Enter Account Number: ";
        cin >> accNumber;
        cout << "Enter Name: ";
        cin.ignore();
        cin.getline(name, 50);
        cout << "Enter Password: ";
        cin >> password;
        cout << "Enter Initial Deposit: ";
        cin >> balance;
        if (balance < 500) {
            cout << "Minimum deposit is 500!\n";
            balance = 0;
            return;
        }
        cout << "✓ Account Created! Number: " << accNumber << "\n";
    }

    // ===== DEPOSIT =====
    void deposit() {
        float amount;
        cout << "\nEnter Amount to Deposit: ";
        cin >> amount;
        if (amount <= 0) {
            cout << "Invalid amount!\n";
            return;
        }
        balance += amount;
        cout << "✓ Deposited! New Balance: " << balance << "\n";
    }

    // ===== WITHDRAW =====
    void withdraw() {
        float amount;
        cout << "\nEnter Amount to Withdraw: ";
        cin >> amount;
        if (amount <= 0) {
            cout << "Invalid amount!\n";
            return;
        }
        if (amount > balance - 500) {
            cout << "Insufficient balance! (Min 500 required)\n";
            return;
        }
        balance -= amount;
        cout << "✓ Withdrawn! New Balance: " << balance << "\n";
    }

    // ===== DISPLAY =====
    void display() const {
        cout << "\n-----------------------------\n";
        cout << "Account No : " << accNumber << "\n";
        cout << "Name       : " << name << "\n";
        cout << "Balance    : Rs." << fixed << setprecision(2) << balance << "\n";
        cout << "-----------------------------\n";
    }

    // ===== GETTERS =====
    int getAccNumber() const { return accNumber; }
    float getBalance() const { return balance; }

    // ===== PASSWORD CHECK =====
    bool checkPassword(const char* pass) const {
        return (string(password) == string(pass));
    }
};

// ========== FILE NAME ==========
const string FILE_NAME = "bank.dat";

// ========== CREATE ACCOUNT ==========
void createNewAccount() {
    Account acc;
    acc.createAccount();

    ofstream file(FILE_NAME, ios::binary | ios::app);
    file.write((char*)&acc, sizeof(acc));
    file.close();
}

// ========== DEPOSIT ==========
void depositMoney() {
    int accNo;
    char pass[20];
    cout << "\nEnter Account Number: ";
    cin >> accNo;
    cout << "Enter Password: ";
    cin >> pass;

    fstream file(FILE_NAME, ios::binary | ios::in | ios::out);
    if (!file) { cout << "No records!\n"; return; }

    Account acc;
    bool found = false;

    while (file.read((char*)&acc, sizeof(acc))) {
        if (acc.getAccNumber() == accNo) {
            if (!acc.checkPassword(pass)) {
                cout << "Wrong Password!\n";
                return;
            }
            acc.deposit();
            file.seekp(-(int)sizeof(acc), ios::cur);
            file.write((char*)&acc, sizeof(acc));
            found = true;
            break;
        }
    }
    if (!found) cout << "Account not found!\n";
    file.close();
}

// ========== WITHDRAW ==========
void withdrawMoney() {
    int accNo;
    char pass[20];
    cout << "\nEnter Account Number: ";
    cin >> accNo;
    cout << "Enter Password: ";
    cin >> pass;

    fstream file(FILE_NAME, ios::binary | ios::in | ios::out);
    if (!file) { cout << "No records!\n"; return; }

    Account acc;
    bool found = false;

    while (file.read((char*)&acc, sizeof(acc))) {
        if (acc.getAccNumber() == accNo) {
            if (!acc.checkPassword(pass)) {
                cout << "Wrong Password!\n";
                return;
            }
            acc.withdraw();
            file.seekp(-(int)sizeof(acc), ios::cur);
            file.write((char*)&acc, sizeof(acc));
            found = true;
            break;
        }
    }
    if (!found) cout << "Account not found!\n";
    file.close();
}

// ========== BALANCE CHECK ==========
void checkBalance() {
    int accNo;
    char pass[20];
    cout << "\nEnter Account Number: ";
    cin >> accNo;
    cout << "Enter Password: ";
    cin >> pass;

    ifstream file(FILE_NAME, ios::binary);
    if (!file) { cout << "No records!\n"; return; }

    Account acc;
    while (file.read((char*)&acc, sizeof(acc))) {
        if (acc.getAccNumber() == accNo) {
            if (!acc.checkPassword(pass)) {
                cout << "Wrong Password!\n";
                return;
            }
            acc.display();
            file.close();
            return;
        }
    }
    cout << "Account not found!\n";
    file.close();
}

// ========== DISPLAY ALL ==========
void displayAll() {
    ifstream file(FILE_NAME, ios::binary);
    if (!file) { cout << "No records!\n"; return; }

    Account acc;
    bool found = false;
    cout << "\n===== ALL ACCOUNTS =====\n";
    while (file.read((char*)&acc, sizeof(acc))) {
        acc.display();
        found = true;
    }
    if (!found) cout << "No accounts found!\n";
    file.close();
}

// ========== CLOSE ACCOUNT ==========
void closeAccount() {
    int accNo;
    char pass[20];
    cout << "\nEnter Account Number to Close: ";
    cin >> accNo;
    cout << "Enter Password: ";
    cin >> pass;

    ifstream file(FILE_NAME, ios::binary);
    if (!file) { cout << "No records!\n"; return; }

    vector<Account> accounts;
    Account acc;
    bool found = false;

    while (file.read((char*)&acc, sizeof(acc))) {
        if (acc.getAccNumber() == accNo && acc.checkPassword(pass)) {
            found = true;
        } else {
            accounts.push_back(acc);
        }
    }
    file.close();

    if (!found) { cout << "Account not found or wrong password!\n"; return; }

    ofstream outFile(FILE_NAME, ios::binary | ios::trunc);
    for (auto& a : accounts)
        outFile.write((char*)&a, sizeof(a));
    outFile.close();
    cout << "✓ Account Closed!\n";
}

// ========== MAIN MENU ==========
int main() {
    int choice;

    cout << "================================\n";
    cout << "    BANK MANAGEMENT SYSTEM\n";
    cout << "================================\n";

    do {
        cout << "\n--- MENU ---\n";
        cout << "1. Create Account\n";
        cout << "2. Deposit Money\n";
        cout << "3. Withdraw Money\n";
        cout << "4. Check Balance\n";
        cout << "5. Display All Accounts\n";
        cout << "6. Close Account\n";
        cout << "0. Exit\n";
        cout << "Enter Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: createNewAccount(); break;
            case 2: depositMoney();     break;
            case 3: withdrawMoney();    break;
            case 4: checkBalance();     break;
            case 5: displayAll();       break;
            case 6: closeAccount();     break;
            case 0: cout << "Thank you!\n"; break;
            default: cout << "Invalid!\n";
        }

    } while (choice != 0);

    return 0;
}