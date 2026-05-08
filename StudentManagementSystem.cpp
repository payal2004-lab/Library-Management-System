/*
 ============================================================
  STUDENT MANAGEMENT SYSTEM
  Language  : C++
  Concepts  : OOP, File Handling, Vectors, Menu-Driven UI
  File      : StudentManagementSystem.cpp
 ============================================================
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits>

using namespace std;

// ─────────────────────────────────────────────
//  STUDENT CLASS  (OOP — Encapsulation)
// ─────────────────────────────────────────────
class Student {
private:
    int    id;
    string name;
    int    age;
    string gender;
    string course;
    float  marks;
    string contact;

public:
    // ── Constructors ──
    Student() : id(0), age(0), marks(0.0f) {}

    Student(int id, string name, int age, string gender,
            string course, float marks, string contact)
        : id(id), name(name), age(age), gender(gender),
          course(course), marks(marks), contact(contact) {}

    // ── Getters ──
    int    getId()      const { return id; }
    string getName()    const { return name; }
    int    getAge()     const { return age; }
    string getGender()  const { return gender; }
    string getCourse()  const { return course; }
    float  getMarks()   const { return marks; }
    string getContact() const { return contact; }

    // ── Setters ──
    void setName(string n)    { name    = n; }
    void setAge(int a)        { age     = a; }
    void setGender(string g)  { gender  = g; }
    void setCourse(string c)  { course  = c; }
    void setMarks(float m)    { marks   = m; }
    void setContact(string ct){ contact = ct; }

    // ── Auto Grade Calculator ──
    string getGrade() const {
        if (marks >= 90) return "A+";
        if (marks >= 80) return "A";
        if (marks >= 70) return "B";
        if (marks >= 60) return "C";
        if (marks >= 50) return "D";
        return "F";
    }

    // ── Display in a single row ──
    void displayRow() const {
        cout << left
             << setw(6)  << id
             << setw(20) << name
             << setw(5)  << age
             << setw(8)  << gender
             << setw(15) << course
             << setw(8)  << fixed << setprecision(1) << marks
             << setw(6)  << getGrade()
             << setw(14) << contact
             << "\n";
    }

    // ── Display full detail ──
    void displayDetail() const {
        cout << "\n";
        cout << "  Student ID   : " << id      << "\n";
        cout << "  Name         : " << name    << "\n";
        cout << "  Age          : " << age     << "\n";
        cout << "  Gender       : " << gender  << "\n";
        cout << "  Course       : " << course  << "\n";
        cout << "  Marks        : " << fixed << setprecision(1) << marks << "\n";
        cout << "  Grade        : " << getGrade() << "\n";
        cout << "  Contact      : " << contact << "\n";
    }
};


// ─────────────────────────────────────────────
//  STUDENT MANAGER CLASS
// ─────────────────────────────────────────────
class StudentManager {
private:
    vector<Student> students;
    const string    FILE_NAME = "students.dat";
    int             nextId;

    // ── Save all records to file ──
    void saveToFile() {
        ofstream file(FILE_NAME, ios::binary | ios::trunc);
        if (!file) {
            cerr << "  [ERROR] Cannot open file for writing!\n";
            return;
        }
        // Save nextId first
        file.write(reinterpret_cast<char*>(&nextId), sizeof(nextId));
        int count = (int)students.size();
        file.write(reinterpret_cast<char*>(&count), sizeof(count));

        for (auto& s : students) {
            int id = s.getId(), age = s.getAge();
            float marks = s.getMarks();
            file.write(reinterpret_cast<char*>(&id),    sizeof(id));
            file.write(reinterpret_cast<char*>(&age),   sizeof(age));
            file.write(reinterpret_cast<char*>(&marks), sizeof(marks));

            auto writeStr = [&](const string& str) {
                int len = (int)str.size();
                file.write(reinterpret_cast<char*>(&len), sizeof(len));
                file.write(str.c_str(), len);
            };
            writeStr(s.getName());
            writeStr(s.getGender());
            writeStr(s.getCourse());
            writeStr(s.getContact());
        }
        file.close();
    }

    // ── Load records from file ──
    void loadFromFile() {
        ifstream file(FILE_NAME, ios::binary);
        if (!file) {
            nextId = 1001;   // default starting ID
            return;
        }
        students.clear();
        file.read(reinterpret_cast<char*>(&nextId), sizeof(nextId));
        int count = 0;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));

        for (int i = 0; i < count; i++) {
            int id, age; float marks;
            file.read(reinterpret_cast<char*>(&id),    sizeof(id));
            file.read(reinterpret_cast<char*>(&age),   sizeof(age));
            file.read(reinterpret_cast<char*>(&marks), sizeof(marks));

            auto readStr = [&]() -> string {
                int len = 0;
                file.read(reinterpret_cast<char*>(&len), sizeof(len));
                string str(len, '\0');
                file.read(&str[0], len);
                return str;
            };
            string name    = readStr();
            string gender  = readStr();
            string course  = readStr();
            string contact = readStr();
            students.emplace_back(id, name, age, gender, course, marks, contact);
        }
        file.close();
    }

    // ── Input validation helpers ──
    static int getValidInt(const string& prompt, int minVal, int maxVal) {
        int val;
        while (true) {
            cout << prompt;
            if (cin >> val && val >= minVal && val <= maxVal) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return val;
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  [!] Enter a number between " << minVal << " and " << maxVal << "\n";
        }
    }

    static float getValidFloat(const string& prompt, float minVal, float maxVal) {
        float val;
        while (true) {
            cout << prompt;
            if (cin >> val && val >= minVal && val <= maxVal) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return val;
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  [!] Enter a value between " << minVal << " and " << maxVal << "\n";
        }
    }

    static string getNonEmptyString(const string& prompt) {
        string val;
        while (true) {
            cout << prompt;
            getline(cin, val);
            if (!val.empty()) return val;
            cout << "  [!] This field cannot be empty.\n";
        }
    }

    // ── Table header ──
    static void printTableHeader() {
        cout << "\n";
        cout << string(82, '-') << "\n";
        cout << left
             << setw(6)  << "ID"
             << setw(20) << "Name"
             << setw(5)  << "Age"
             << setw(8)  << "Gender"
             << setw(15) << "Course"
             << setw(8)  << "Marks"
             << setw(6)  << "Grade"
             << setw(14) << "Contact"
             << "\n";
        cout << string(82, '-') << "\n";
    }

public:
    // ── Constructor: load data on startup ──
    StudentManager() {
        loadFromFile();
        cout << "  [+] " << students.size() << " record(s) loaded from file.\n";
    }

    // ─────────────────────────────────────────
    //  1. ADD STUDENT
    // ─────────────────────────────────────────
    void addStudent() {
        cout << "\n  ╔══════════════════════════╗\n";
        cout <<   "  ║    ADD NEW STUDENT       ║\n";
        cout <<   "  ╚══════════════════════════╝\n\n";

        string name    = getNonEmptyString("  Full Name     : ");
        int    age     = getValidInt      ("  Age (5-100)   : ", 5, 100);
        string gender  = getNonEmptyString("  Gender        : ");
        string course  = getNonEmptyString("  Course/Class  : ");
        float  marks   = getValidFloat    ("  Marks (0-100) : ", 0.0f, 100.0f);
        string contact = getNonEmptyString("  Contact       : ");

        Student s(nextId, name, age, gender, course, marks, contact);
        students.push_back(s);
        nextId++;
        saveToFile();

        cout << "\n  [SUCCESS] Student added! ID = " << s.getId()
             << "  |  Grade = " << s.getGrade() << "\n";
    }

    // ─────────────────────────────────────────
    //  2. DISPLAY ALL STUDENTS
    // ─────────────────────────────────────────
    void displayAll() {
        if (students.empty()) {
            cout << "\n  [INFO] No records found.\n";
            return;
        }
        cout << "\n  ╔══════════════════════════════╗\n";
        cout <<   "  ║    ALL STUDENT RECORDS       ║\n";
        cout <<   "  ╚══════════════════════════════╝\n";
        printTableHeader();
        for (auto& s : students) s.displayRow();
        cout << string(82, '-') << "\n";
        cout << "  Total records: " << students.size() << "\n";

        // Average marks
        float total = 0;
        for (auto& s : students) total += s.getMarks();
        cout << "  Class average: " << fixed << setprecision(1)
             << (total / students.size()) << " marks\n";
    }

    // ─────────────────────────────────────────
    //  3. SEARCH STUDENT
    // ─────────────────────────────────────────
    void searchStudent() {
        cout << "\n  Search by: 1. Student ID   2. Name   3. Course\n";
        int choice = getValidInt("  Your choice: ", 1, 3);
        bool found = false;

        if (choice == 1) {
            int sid = getValidInt("  Enter Student ID: ", 1000, 9999999);
            for (auto& s : students) {
                if (s.getId() == sid) {
                    s.displayDetail();
                    found = true; break;
                }
            }
        } else if (choice == 2) {
            string keyword = getNonEmptyString("  Enter name (or part of name): ");
            transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
            printTableHeader();
            for (auto& s : students) {
                string nm = s.getName();
                transform(nm.begin(), nm.end(), nm.begin(), ::tolower);
                if (nm.find(keyword) != string::npos) {
                    s.displayRow(); found = true;
                }
            }
        } else {
            string keyword = getNonEmptyString("  Enter course name: ");
            transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
            printTableHeader();
            for (auto& s : students) {
                string cr = s.getCourse();
                transform(cr.begin(), cr.end(), cr.begin(), ::tolower);
                if (cr.find(keyword) != string::npos) {
                    s.displayRow(); found = true;
                }
            }
        }

        if (!found) cout << "\n  [INFO] No matching record found.\n";
    }

    // ─────────────────────────────────────────
    //  4. UPDATE STUDENT
    // ─────────────────────────────────────────
    void updateStudent() {
        int sid = getValidInt("\n  Enter Student ID to update: ", 1000, 9999999);
        for (auto& s : students) {
            if (s.getId() == sid) {
                cout << "\n  Current record:";
                s.displayDetail();

                cout << "\n  What to update?\n";
                cout << "  1. Name    2. Age    3. Gender\n";
                cout << "  4. Course  5. Marks  6. Contact\n";
                int choice = getValidInt("  Choice: ", 1, 6);

                switch (choice) {
                    case 1: s.setName   (getNonEmptyString("  New Name    : ")); break;
                    case 2: s.setAge    (getValidInt("  New Age (5-100): ", 5, 100)); break;
                    case 3: s.setGender (getNonEmptyString("  New Gender  : ")); break;
                    case 4: s.setCourse (getNonEmptyString("  New Course  : ")); break;
                    case 5: s.setMarks  (getValidFloat("  New Marks (0-100): ", 0.0f, 100.0f)); break;
                    case 6: s.setContact(getNonEmptyString("  New Contact : ")); break;
                }
                saveToFile();
                cout << "\n  [SUCCESS] Record updated! New grade: " << s.getGrade() << "\n";
                return;
            }
        }
        cout << "\n  [ERROR] Student ID not found.\n";
    }

    // ─────────────────────────────────────────
    //  5. DELETE STUDENT
    // ─────────────────────────────────────────
    void deleteStudent() {
        int sid = getValidInt("\n  Enter Student ID to delete: ", 1000, 9999999);
        for (int i = 0; i < (int)students.size(); i++) {
            if (students[i].getId() == sid) {
                cout << "\n  Record found:";
                students[i].displayDetail();
                cout << "\n  Are you sure? (y/n): ";
                char confirm;
                cin >> confirm;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (confirm == 'y' || confirm == 'Y') {
                    students.erase(students.begin() + i);
                    saveToFile();
                    cout << "  [SUCCESS] Record deleted.\n";
                } else {
                    cout << "  [CANCELLED] No changes made.\n";
                }
                return;
            }
        }
        cout << "\n  [ERROR] Student ID not found.\n";
    }

    // ─────────────────────────────────────────
    //  6. SORT & DISPLAY
    // ─────────────────────────────────────────
    void sortAndDisplay() {
        if (students.empty()) { cout << "\n  [INFO] No records.\n"; return; }
        cout << "\n  Sort by: 1. Marks (High to Low)   2. Name (A-Z)\n";
        int choice = getValidInt("  Choice: ", 1, 2);
        vector<Student> sorted = students;
        if (choice == 1)
            sort(sorted.begin(), sorted.end(),
                 [](const Student& a, const Student& b){ return a.getMarks() > b.getMarks(); });
        else
            sort(sorted.begin(), sorted.end(),
                 [](const Student& a, const Student& b){ return a.getName() < b.getName(); });

        printTableHeader();
        for (auto& s : sorted) s.displayRow();
        cout << string(82, '-') << "\n";
    }

    // ─────────────────────────────────────────
    //  7. EXPORT REPORT
    // ─────────────────────────────────────────
    void exportReport() {
        if (students.empty()) { cout << "\n  [INFO] No records to export.\n"; return; }
        ofstream report("student_report.txt");
        if (!report) { cerr << "  [ERROR] Cannot create report file.\n"; return; }

        report << "================================================\n";
        report << "         STUDENT PERFORMANCE REPORT\n";
        report << "================================================\n";
        report << left
               << setw(6)  << "ID"
               << setw(20) << "Name"
               << setw(15) << "Course"
               << setw(8)  << "Marks"
               << setw(6)  << "Grade"
               << "\n";
        report << string(55, '-') << "\n";

        float total = 0;
        for (auto& s : students) {
            report << left
                   << setw(6)  << s.getId()
                   << setw(20) << s.getName()
                   << setw(15) << s.getCourse()
                   << setw(8)  << fixed << setprecision(1) << s.getMarks()
                   << setw(6)  << s.getGrade()
                   << "\n";
            total += s.getMarks();
        }
        report << string(55, '-') << "\n";
        report << "Total students : " << students.size() << "\n";
        report << "Class average  : " << fixed << setprecision(1)
               << (total / students.size()) << "\n";
        report.close();
        cout << "\n  [SUCCESS] Report exported to 'student_report.txt'\n";
    }
};


// ─────────────────────────────────────────────
//  SIMPLE LOGIN SYSTEM
// ─────────────────────────────────────────────
bool login() {
    const string ADMIN_USER = "admin";
    const string ADMIN_PASS = "1234";
    int tries = 3;
    cout << "\n  ╔══════════════════════════════════╗\n";
    cout <<   "  ║   STUDENT MANAGEMENT SYSTEM      ║\n";
    cout <<   "  ║         LOGIN REQUIRED           ║\n";
    cout <<   "  ╚══════════════════════════════════╝\n\n";
    while (tries-- > 0) {
        string user, pass;
        cout << "  Username : "; cin >> user;
        cout << "  Password : "; cin >> pass;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (user == ADMIN_USER && pass == ADMIN_PASS) return true;
        cout << "  [!] Wrong credentials. " << tries << " attempt(s) left.\n\n";
    }
    return false;
}


// ─────────────────────────────────────────────
//  MAIN MENU
// ─────────────────────────────────────────────
int main() {
    if (!login()) {
        cout << "\n  Access denied. Exiting.\n";
        return 1;
    }

    StudentManager manager;

    while (true) {
        cout << "\n  ╔══════════════════════════════════════╗\n";
        cout <<   "  ║        MAIN MENU                     ║\n";
        cout <<   "  ╠══════════════════════════════════════╣\n";
        cout <<   "  ║  1. Add Student                      ║\n";
        cout <<   "  ║  2. Display All Students             ║\n";
        cout <<   "  ║  3. Search Student                   ║\n";
        cout <<   "  ║  4. Update Student                   ║\n";
        cout <<   "  ║  5. Delete Student                   ║\n";
        cout <<   "  ║  6. Sort & Display Records           ║\n";
        cout <<   "  ║  7. Export Performance Report        ║\n";
        cout <<   "  ║  0. Exit                             ║\n";
        cout <<   "  ╚══════════════════════════════════════╝\n";
        cout <<   "  Enter your choice: ";

        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  [!] Invalid input. Enter 0-7.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: manager.addStudent();     break;
            case 2: manager.displayAll();     break;
            case 3: manager.searchStudent();  break;
            case 4: manager.updateStudent();  break;
            case 5: manager.deleteStudent();  break;
            case 6: manager.sortAndDisplay(); break;
            case 7: manager.exportReport();   break;
            case 0:
                cout << "\n  Goodbye! Data saved.\n\n";
                return 0;
            default:
                cout << "\n  [!] Invalid choice. Enter 0-7.\n";
        }
    }
}
