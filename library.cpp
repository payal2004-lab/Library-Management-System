#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
using namespace std;

// ========== BOOK CLASS ==========
class Book {
private:
    int bookId;
    char title[50];
    char author[50];
    int quantity;

public:
    Book() { bookId = 0; quantity = 0; }

    void addBook() {
        cout << "\n--- Add New Book ---\n";
        cout << "Enter Book ID: ";
        cin >> bookId;
        cout << "Enter Title: ";
        cin.ignore();
        cin.getline(title, 50);
        cout << "Enter Author: ";
        cin.getline(author, 50);
        cout << "Enter Quantity: ";
        cin >> quantity;
        cout << "✓ Book Added!\n";
    }

    void display() const {
        cout << left << setw(6)  << bookId
             << setw(25) << title
             << setw(20) << author
             << setw(5)  << quantity << "\n";
    }

    int getId()           const { return bookId; }
    int getQuantity()     const { return quantity; }
    string getTitle()     const { return string(title); }
    string getAuthor()    const { return string(author); }

    void setQuantity(int q) { quantity = q; }
};

// ========== MEMBER CLASS ==========
class Member {
private:
    int memberId;
    char name[50];
    int borrowedBookId;

public:
    Member() { memberId = 0; borrowedBookId = 0; }

    void addMember() {
        cout << "\n--- Add New Member ---\n";
        cout << "Enter Member ID: ";
        cin >> memberId;
        cout << "Enter Name: ";
        cin.ignore();
        cin.getline(name, 50);
        borrowedBookId = 0;
        cout << "✓ Member Added!\n";
    }

    void display() const {
        cout << left << setw(8)  << memberId
             << setw(25) << name;
        if (borrowedBookId == 0)
            cout << setw(10) << "None";
        else
            cout << setw(10) << borrowedBookId;
        cout << "\n";
    }

    int getId()             const { return memberId; }
    int getBorrowedBookId() const { return borrowedBookId; }
    void setBorrowedBook(int id)  { borrowedBookId = id; }
};

// ========== FILE NAMES ==========
const string BOOK_FILE   = "books.dat";
const string MEMBER_FILE = "members.dat";

// ========== ADD BOOK ==========
void addBook() {
    Book b;
    b.addBook();
    ofstream file(BOOK_FILE, ios::binary | ios::app);
    file.write((char*)&b, sizeof(b));
    file.close();
}

// ========== ADD MEMBER ==========
void addMember() {
    Member m;
    m.addMember();
    ofstream file(MEMBER_FILE, ios::binary | ios::app);
    file.write((char*)&m, sizeof(m));
    file.close();
}

// ========== DISPLAY ALL BOOKS ==========
void displayBooks() {
    ifstream file(BOOK_FILE, ios::binary);
    if (!file) { cout << "No books found!\n"; return; }

    cout << "\n===== ALL BOOKS =====\n";
    cout << left << setw(6)  << "ID"
         << setw(25) << "Title"
         << setw(20) << "Author"
         << setw(5)  << "Qty" << "\n";
    cout << string(56, '-') << "\n";

    Book b;
    bool found = false;
    while (file.read((char*)&b, sizeof(b))) {
        b.display();
        found = true;
    }
    if (!found) cout << "No books in library!\n";
    file.close();
}

// ========== DISPLAY ALL MEMBERS ==========
void displayMembers() {
    ifstream file(MEMBER_FILE, ios::binary);
    if (!file) { cout << "No members found!\n"; return; }

    cout << "\n===== ALL MEMBERS =====\n";
    cout << left << setw(8)  << "ID"
         << setw(25) << "Name"
         << setw(10) << "Borrowed" << "\n";
    cout << string(43, '-') << "\n";

    Member m;
    bool found = false;
    while (file.read((char*)&m, sizeof(m))) {
        m.display();
        found = true;
    }
    if (!found) cout << "No members!\n";
    file.close();
}

// ========== ISSUE BOOK ==========
void issueBook() {
    int bookId, memberId;
    cout << "\n--- Issue Book ---\n";
    cout << "Enter Member ID: ";
    cin >> memberId;
    cout << "Enter Book ID: ";
    cin >> bookId;

    // Check member
    fstream mFile(MEMBER_FILE, ios::binary | ios::in | ios::out);
    if (!mFile) { cout << "No members!\n"; return; }

    Member m;
    bool mFound = false;
    while (mFile.read((char*)&m, sizeof(m))) {
        if (m.getId() == memberId) {
            if (m.getBorrowedBookId() != 0) {
                cout << "Member already has a book!\n";
                mFile.close();
                return;
            }
            mFound = true;
            break;
        }
    }
    if (!mFound) { cout << "Member not found!\n"; mFile.close(); return; }

    // Check book
    fstream bFile(BOOK_FILE, ios::binary | ios::in | ios::out);
    if (!bFile) { cout << "No books!\n"; mFile.close(); return; }

    Book b;
    bool bFound = false;
    while (bFile.read((char*)&b, sizeof(b))) {
        if (b.getId() == bookId) {
            if (b.getQuantity() <= 0) {
                cout << "Book not available!\n";
                bFile.close(); mFile.close();
                return;
            }
            b.setQuantity(b.getQuantity() - 1);
            bFile.seekp(-(int)sizeof(b), ios::cur);
            bFile.write((char*)&b, sizeof(b));
            bFound = true;
            break;
        }
    }
    bFile.close();

    if (!bFound) { cout << "Book not found!\n"; mFile.close(); return; }

    // Update member
    mFile.seekg(0);
    while (mFile.read((char*)&m, sizeof(m))) {
        if (m.getId() == memberId) {
            m.setBorrowedBook(bookId);
            mFile.seekp(-(int)sizeof(m), ios::cur);
            mFile.write((char*)&m, sizeof(m));
            break;
        }
    }
    mFile.close();

    cout << "✓ Book Issued Successfully!\n";
}

// ========== RETURN BOOK ==========
void returnBook() {
    int memberId;
    cout << "\n--- Return Book ---\n";
    cout << "Enter Member ID: ";
    cin >> memberId;

    fstream mFile(MEMBER_FILE, ios::binary | ios::in | ios::out);
    if (!mFile) { cout << "No members!\n"; return; }

    Member m;
    bool found = false;
    int returnBookId = 0;

    while (mFile.read((char*)&m, sizeof(m))) {
        if (m.getId() == memberId) {
            if (m.getBorrowedBookId() == 0) {
                cout << "No book issued to this member!\n";
                mFile.close(); return;
            }
            returnBookId = m.getBorrowedBookId();
            m.setBorrowedBook(0);
            mFile.seekp(-(int)sizeof(m), ios::cur);
            mFile.write((char*)&m, sizeof(m));
            found = true;
            break;
        }
    }
    mFile.close();

    if (!found) { cout << "Member not found!\n"; return; }

    // Update book quantity
    fstream bFile(BOOK_FILE, ios::binary | ios::in | ios::out);
    Book b;
    while (bFile.read((char*)&b, sizeof(b))) {
        if (b.getId() == returnBookId) {
            b.setQuantity(b.getQuantity() + 1);
            bFile.seekp(-(int)sizeof(b), ios::cur);
            bFile.write((char*)&b, sizeof(b));
            break;
        }
    }
    bFile.close();
    cout << "✓ Book Returned Successfully!\n";
}

// ========== SEARCH BY TITLE ==========
void searchByTitle() {
    char searchTitle[50];
    cout << "\nEnter Title to Search: ";
    cin.ignore();
    cin.getline(searchTitle, 50);

    ifstream file(BOOK_FILE, ios::binary);
    if (!file) { cout << "No books!\n"; return; }

    Book b;
    bool found = false;
    string search = string(searchTitle);

    cout << "\n--- Search Results ---\n";
    while (file.read((char*)&b, sizeof(b))) {
        if (b.getTitle().find(search) != string::npos) {
            b.display();
            found = true;
        }
    }
    if (!found) cout << "No book found with this title!\n";
    file.close();
}

// ========== SEARCH BY AUTHOR ==========
void searchByAuthor() {
    char searchAuthor[50];
    cout << "\nEnter Author to Search: ";
    cin.ignore();
    cin.getline(searchAuthor, 50);

    ifstream file(BOOK_FILE, ios::binary);
    if (!file) { cout << "No books!\n"; return; }

    Book b;
    bool found = false;
    string search = string(searchAuthor);

    cout << "\n--- Search Results ---\n";
    while (file.read((char*)&b, sizeof(b))) {
        if (b.getAuthor().find(search) != string::npos) {
            b.display();
            found = true;
        }
    }
    if (!found) cout << "No book found by this author!\n";
    file.close();
}

// ========== MAIN MENU ==========
int main() {
    int choice;

    cout << "================================\n";
    cout << "   LIBRARY MANAGEMENT SYSTEM\n";
    cout << "================================\n";

    do {
        cout << "\n--- MENU ---\n";
        cout << "1. Add Book\n";
        cout << "2. Add Member\n";
        cout << "3. Display All Books\n";
        cout << "4. Display All Members\n";
        cout << "5. Issue Book\n";
        cout << "6. Return Book\n";
        cout << "7. Search by Title\n";
        cout << "8. Search by Author\n";
        cout << "0. Exit\n";
        cout << "Enter Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addBook();        break;
            case 2: addMember();      break;
            case 3: displayBooks();   break;
            case 4: displayMembers(); break;
            case 5: issueBook();      break;
            case 6: returnBook();     break;
            case 7: searchByTitle();  break;
            case 8: searchByAuthor(); break;
            case 0: cout << "Goodbye!\n"; break;
            default: cout << "Invalid!\n";
        }
    } while (choice != 0);

    return 0;
}