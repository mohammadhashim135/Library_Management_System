#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <ctime> // For date and time operations

using namespace std;

// ANSI color codes
const string ANSI_COLOR_RED = "\033[1;31m";
const string ANSI_COLOR_RESET = "\033[0m";

// Class definitions
class Student {
private:
    string name;
    int id;

public:
    Student(const string& n, int i) : name(n), id(i) {}

    string getName() const { return name; }
    int getId() const { return id; }
};

class Book {
private:
    string name;
    int studentID;

public:
    Book(const string& n, int sID) : name(n), studentID(sID) {}

    string getName() const { return name; }
    int getStudentID() const { return studentID; }
};

class IssuedBook {
private:
    string bookName;
    int studentID;
    string issueDate;
    string returnDate;

public:
    IssuedBook(const string& bName, int sID, const string& iDate, const string& rDate)
        : bookName(bName), studentID(sID), issueDate(iDate), returnDate(rDate) {}

    string getBookName() const { return bookName; }
    int getStudentID() const { return studentID; }
    string getIssueDate() const { return issueDate; }
    string getReturnDate() const { return returnDate; }
};

class Library {
private:
    static const int MAX_BOOKS = 100;
    vector<Student> students;
    vector<Book> books;
    vector<IssuedBook> issuedBooks;

public:
    // Constructor to load existing books and students from files
    Library() {
        loadBooksFromFile();
        loadStudentsFromFile();
    }

    // Function to load existing books from books.txt
    void loadBooksFromFile() {
        ifstream file("books.txt");
        if (!file.is_open()) {
            cout << "Error: Unable to open file 'books.txt'. No books loaded." << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            string name;
            int studentID;
            if (getline(iss, name, ',') && (iss >> studentID)) {
                books.push_back(Book(name, studentID));
            }
        }
        file.close();
    }

    // Function to load existing students from students.txt
    void loadStudentsFromFile() {
        ifstream file("students.txt");
        if (!file.is_open()) {
            cout << "Error: Unable to open file 'students.txt'. No students loaded." << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            string name;
            int id;
            if (getline(iss, name, ',') && (iss >> id)) {
                students.push_back(Student(name, id));
            }
        }
        file.close();
    }

    void addStudent(const string& name, int id) {
        if (find_if(students.begin(), students.end(), [&](const Student& student) { return student.getId() == id; }) != students.end()) {
            cout << "Student with ID " << id << " already exists." << endl;
            return;
        }

        students.push_back(Student(name, id));
        updateStudentsFile(name, id); // Update students.txt file
        cout << "Student '" << name << "' with ID " << id << " added successfully." << endl;
    }

    void addBook(const string& name) {
        // Check if the book already exists in the program's memory
        auto it = find_if(books.begin(), books.end(), [&](const Book& book) { return book.getName() == name; });
        if (it != books.end()) {
            cout << "Book '" << name << "' already exists in the library." << endl;
            return;
        }

        if (books.size() >= MAX_BOOKS) {
            cout << "Sorry, the library is currently full and cannot accept more books." << endl;
            return;
        }
        int newID = generateUniqueID();
        books.push_back(Book(name, newID));
        updateBooksFile(name, newID); // Update books.txt file
        cout << "Book '" << name << "' added successfully with ID " << newID << "." << endl;
    }

    void removeBook(const string& name) {
        auto it = find_if(books.begin(), books.end(), [&](const Book& book) { return book.getName() == name; });
        if (it != books.end()) {
            books.erase(it);
            cout << "Book '" << name << "' removed successfully." << endl;
        } else {
            cout << "Book '" << name << "' not found." << endl;
        }
    }

    void issueBook(const string& bookName, int studentID) {
        auto bookIt = find_if(books.begin(), books.end(), [&](const Book& book) { return book.getName() == bookName; });
        if (bookIt == books.end()) {
            cout << "Book '" << bookName << "' not found in the library." << endl;
            return;
        }

        auto studentIt = find_if(students.begin(), students.end(), [&](const Student& student) { return student.getId() == studentID; });
        if (studentIt == students.end()) {
            cout << "Student with ID " << studentID << " not found." << endl;
            return;
        }

        string currentDate = getCurrentDate();
        string returnDate = getReturnDate();

        issuedBooks.push_back(IssuedBook(bookName, studentID, currentDate, returnDate));
        cout << "Book '" << bookName << "' issued to student with ID " << studentID << "." << endl;
    }

    void searchBook(const string& name) const {
        cout << "\nSearch Results for '" << name << "':\n" << endl;
        bool found = false;
        for (const auto& book : books) {
            if (book.getName().find(name) != string::npos) {
                cout << "Book Name: " << setw(20) << book.getName() << " Book ID: " << setw(10) << book.getStudentID() << endl;
                found = true;
            }
        }
        if (!found) {
            cout << "No books found with the name '" << name << "'." << endl;
        }
    }

    void displayAvailableBooks() const {
        cout << "\n\t\t\t\t\t\tList of Available Books:\n" << endl;
        cout << setw(85) << left << "Book Name" << setw(70) << left << "Book ID" << endl;
        cout << setfill('-') << setw(95) << "-" << setfill(' ') << endl;

        // Display books from program's memory
        for (const auto& book : books) {
            cout << setw(90) << left << book.getName() << setw(15) << left << book.getStudentID() << endl;
        }
    }

    void displayIssuedBooks() const {
        cout << "\n\n\t\t\t\tList of Issued Books:\n" << endl;
        cout << setw(20) << left << "Student Name" << setw(10) << left << "ID" << setw(30) << left
             << "Book" << setw(25) << left << "Date of Issue" << setw(20) << left << "Date of Return" << endl;
        cout << setfill('-') << setw(100) << "-" << setfill(' ') << endl;

        for (const auto& issuedBook : issuedBooks) {
            string studentName = getStudentNameByID(issuedBook.getStudentID());
            cout << setw(20) << left << studentName << setw(10) << left << issuedBook.getStudentID() << setw(30) << left
                 << issuedBook.getBookName() << setw(30) << left << issuedBook.getIssueDate() << setw(20) << left << issuedBook.getReturnDate() << endl;
        }
    }

    void displayStudents() const {
        cout << "\n\tList of Students:\n" << endl;
        cout << setw(20) << left << "Student Name" << setw(10) << left << "ID" << endl;
        cout << setfill('-') << setw(30) << "-" << setfill(' ') << endl;

        for (const auto& student : students) {
            cout << setw(20) << left << student.getName() << setw(10) << left << student.getId() << endl;
        }
    }

    string getStudentNameByID(int id) const {
        auto it = find_if(students.begin(), students.end(), [&](const Student& student) { return student.getId() == id; });
        if (it != students.end()) {
            return it->getName();
        }
        return "Unknown";
    }

    // Function to generate a unique ID for a new book
    int generateUniqueID() {
        // Find the maximum existing ID and increment it to generate a unique ID
        int maxID = 0;
        for (const auto& book : books) {
            if (book.getStudentID() > maxID) {
                maxID = book.getStudentID();
            }
        }
        return maxID + 1;
    }

    // Function to update the books.txt file with a new book
    void updateBooksFile(const string& name, int studentID) const {
        ofstream file("books.txt", ios::app);
        if (!file.is_open()) {
            cout << "Error: Unable to open file 'books.txt' for updating." << endl;
            return;
        }
        file << name << "," << studentID << endl;
        file.close();
    }

    // Function to update the students.txt file with a new student
    void updateStudentsFile(const string& name, int id) const {
        ofstream file("students.txt", ios::app);
        if (!file.is_open()) {
            cout << "Error: Unable to open file 'students.txt' for updating." << endl;
            return;
        }
        file << name << "," << id << endl;
        file.close();
    }

    // Function to get the current date in YYYY-MM-DD format
    string getCurrentDate() const {
        time_t now = time(0);
        tm* timeinfo = localtime(&now);
        stringstream ss;
        ss << (timeinfo->tm_year + 1900) << '-' << setw(2) << setfill('0') << (timeinfo->tm_mon + 1) << '-' << setw(2) << setfill('0') << timeinfo->tm_mday;
        return ss.str();
    }

    // Function to get the return date (1 week from the issue date)
    string getReturnDate() const {
        time_t now = time(0) + 7 * 24 * 3600; // Add 7 days (1 week) to the current time
        tm* timeinfo = localtime(&now);
        stringstream ss;
        ss << (timeinfo->tm_year + 1900) << '-' << setw(2) << setfill('0') << (timeinfo->tm_mon + 1) << '-' << setw(2) << setfill('0') << timeinfo->tm_mday;
        return ss.str();
    }
};

// Function to check if a given string contains only alphabetic characters
bool isAlpha(const string& str) {
    for (char c : str) {
        if (!isalpha(c) && c != ' ') {
            return false;
        }
    }
    return true;
}

int main() {
    cout << ANSI_COLOR_RED << "\t\t\t\t*LIBRARY MANAGEMENT SYSTEM*" << ANSI_COLOR_RESET << endl;

    Library library;

    char choice;

    do {
        cout << "1. Add Student\n";
        cout << "2. Add Book\n";
        cout << "3. Remove Book\n";
        cout << "4. Issue Book\n";
        cout << "5. Search Book\n";
        cout << "6. Display Available Books\n";
        cout << "7. Display Issued Books\n";
        cout << "8. Display Students\n";
        cout << "9. Exit\n\n";
        cout << "Enter your choice (1-9):";
        cin >> choice;
        cin.ignore(); // Clear the input buffer

        switch (choice) {
            case '1': {
                string name;
                int id;
                cout << "Enter student name: ";
                getline(cin, name);
                if (!isAlpha(name)) {
                    cout << "Invalid name. Please enter an alphabetic name." << endl;
                    continue;
                }
                cout << "Enter student ID: ";
                cin >> id;
                cin.ignore(); // Ignore the newline character in the input buffer
                if (id < 0) {
                    cout << "Invalid ID. Please enter a positive integer." << endl;
                    continue;
                }
                library.addStudent(name, id);
                break;
            }
            case '2': {
                string bookName;
                cout << "Enter book name: ";
                getline(cin, bookName);
                library.addBook(bookName);
                break;
            }
            case '3': {
                string bookName;
                cout << "Enter the name of the book to remove: ";
                getline(cin, bookName);
                library.removeBook(bookName);
                break;
            }
            case '4': {
                string bookName;
                int studentID;
                cout << "Enter the name of the book to issue: ";
                getline(cin, bookName);
                cout << "Enter the student ID: ";
                cin >> studentID;
                cin.ignore(); // Ignore the newline character in the input buffer
                library.issueBook(bookName, studentID);
                break;
            }
            case '5': {
                string bookName;
                cout << "Enter the name of the book to search: ";
                getline(cin, bookName);
                library.searchBook(bookName);
                break;
            }
            case '6': {
                library.displayAvailableBooks();
                break;
            }
            case '7': {
                library.displayIssuedBooks();
                break;
            }
            case '8': {
                library.displayStudents();
                break;
            }
            case '9': {
                cout << "Exiting the program. Thank you for using the Library Management System." << endl;
                break;
            }
            default: {
                cout << "Invalid choice. Please enter a number between 1 and 9." << endl;
                break;
            }
        }
    } while (choice != '9');

    return 0;
}
