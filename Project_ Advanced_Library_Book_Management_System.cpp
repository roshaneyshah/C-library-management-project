#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <limits>
#include <ctime>
using namespace std;

struct Book {
    int id;
    char title[100];
    char author[100];
    char genre[50];
    bool isIssued;
    time_t issueDate;
    time_t returnDate;
};

void addBook();
void displayBooks();
void searchBook();
void issueBook();
void returnBook();
void deleteBook();
void generateReport();
void saveToFile(const Book& book);
int findBookPosition(int id);
void inputValidation(int& var);
void inputValidation(char* str, int size);
int generateID();
void clearScreen();

const char FILENAME[] = "library.dat";
const int MAX_BOOKS = 1000;

int main() {
    int choice;
    do {
        clearScreen();
        cout << "\n===== Advanced Library Management System =====";
        cout << "\n1. Add Book";
        cout << "\n2. Display All Books";
        cout << "\n3. Search Book";
        cout << "\n4. Issue Book";
        cout << "\n5. Return Book";
        cout << "\n6. Delete Book";
        cout << "\n7. Generate Report";
        cout << "\n8. Exit";
        cout << "\n\nEnter your choice: ";
        
        inputValidation(choice);

        switch(choice) {
            case 1: addBook(); break;
            case 2: displayBooks(); break;
            case 3: searchBook(); break;
            case 4: issueBook(); break;
            case 5: returnBook(); break;
            case 6: deleteBook(); break;
            case 7: generateReport(); break;
            case 8: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice! Please try again.\n";
        }
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    } while (choice != 8);
    
    return 0;
}

void inputValidation(int& var) {
    while (!(cin >> var)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input! Please enter a number: ";
    }
}

void inputValidation(char* str, int size) {
    cin.ignore();
    cin.getline(str, size);
    while (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Input too long! Please enter within " << size-1 << " characters: ";
        cin.getline(str, size);
    }
}

int generateID() {
    static int counter = 1000;
    ifstream fin(FILENAME, ios::binary);
    Book book;
    int maxID = counter;
    
    while (fin.read(reinterpret_cast<char*>(&book), sizeof(book))) {
        if (book.id > maxID) maxID = book.id;
    }
    fin.close();
    
    return maxID + 1;
}

void addBook() {
    Book book;
    ofstream fout(FILENAME, ios::binary | ios::app);
    
    book.id = generateID();
    cout << "Auto-generated Book ID: " << book.id << endl;
    
    cout << "Enter Book Title: ";
    inputValidation(book.title, 100);
    
    cout << "Enter Author Name: ";
    inputValidation(book.author, 100);
    
    cout << "Enter Genre: ";
    inputValidation(book.genre, 50);
    
    book.isIssued = false;
    book.issueDate = 0;
    book.returnDate = 0;
    
    fout.write(reinterpret_cast<const char*>(&book), sizeof(book));
    fout.close();
    
    cout << "\nBook added successfully!\n";
}

int findBookPosition(int id) {
    fstream file(FILENAME, ios::in | ios::binary);
    Book book;
    int position = 0;
    
    while (file.read(reinterpret_cast<char*>(&book), sizeof(book))) {
        if (book.id == id) {
            file.close();
            return position;
        }
        position++;
    }
    
    file.close();
    return -1;
}

void issueBook() {
    int bookID;
    cout << "Enter Book ID to issue: ";
    inputValidation(bookID);
    
    int position = findBookPosition(bookID);
    if (position == -1) {
        cout << "Book not found!\n";
        return;
    }
    
    fstream file(FILENAME, ios::in | ios::out | ios::binary);
    file.seekg(position * sizeof(Book));
    
    Book book;
    file.read(reinterpret_cast<char*>(&book), sizeof(book));
    
    if (book.isIssued) {
        cout << "Book is already issued!\n";
    } else {
        book.isIssued = true;
        book.issueDate = time(nullptr);
        book.returnDate = book.issueDate + (14 * 24 * 60 * 60);
        
        file.seekp(position * sizeof(Book));
        file.write(reinterpret_cast<const char*>(&book), sizeof(book));
        cout << "Book issued successfully!\n";
        cout << "Due Date: " << ctime(&book.returnDate);
    }
    file.close();
}

void returnBook() {
    int bookID;
    cout << "Enter Book ID to return: ";
    inputValidation(bookID);
    
    int position = findBookPosition(bookID);
    if (position == -1) {
        cout << "Book not found!\n";
        return;
    }
    
    fstream file(FILENAME, ios::in | ios::out | ios::binary);
    file.seekg(position * sizeof(Book));
    
    Book book;
    file.read(reinterpret_cast<char*>(&book), sizeof(book));
    
    if (!book.isIssued) {
        cout << "Book was not issued!\n";
    } else {
        time_t currentTime = time(nullptr);
        double daysLate = difftime(currentTime, book.returnDate) / (24 * 60 * 60);
        
        if (daysLate > 0) {
            double fine = daysLate * 1.0;
            cout << "Book returned late by " << daysLate << " days. Fine: $" << fine << endl;
        }
        
        book.isIssued = false;
        file.seekp(position * sizeof(Book));
        file.write(reinterpret_cast<const char*>(&book), sizeof(book));
        cout << "Book returned successfully!\n";
    }
    file.close();
}

void deleteBook() {
    int bookID;
    cout << "Enter Book ID to delete: ";
    inputValidation(bookID);
    
    ifstream fin(FILENAME, ios::binary);
    ofstream fout("temp.dat", ios::binary);
    Book book;
    bool found = false;
    
    while (fin.read(reinterpret_cast<char*>(&book), sizeof(book))) {
        if (book.id == bookID) {
            found = true;
            continue;
        }
        fout.write(reinterpret_cast<const char*>(&book), sizeof(book));
    }
    
    fin.close();
    fout.close();
    
    remove(FILENAME);
    rename("temp.dat", FILENAME);
    
    if (found) cout << "Book deleted successfully!\n";
    else cout << "Book not found!\n";
}

void generateReport() {
    ifstream fin(FILENAME, ios::binary);
    Book book;
    vector<Book> books;
    int totalBooks = 0, issuedBooks = 0;
    
    while (fin.read(reinterpret_cast<char*>(&book), sizeof(book))) {
        books.push_back(book);
        totalBooks++;
        if (book.isIssued) issuedBooks++;
    }
    fin.close();
    
    sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
        return strcmp(a.title, b.title) < 0;
    });
    
    cout << "\n===== Library Report =====\n";
    cout << "Total Books: " << totalBooks << endl;
    cout << "Issued Books: " << issuedBooks << endl;
    cout << "Available Books: " << totalBooks - issuedBooks << endl;
    
    cout << "\n===== Overdue Books =====\n";
    time_t currentTime = time(nullptr);
    bool foundOverdue = false;
    
    for (const auto& b : books) {
        if (b.isIssued && difftime(currentTime, b.returnDate) > 0) {
            cout << "ID: " << b.id << " | Title: " << b.title 
                 << " | Days Overdue: " 
                 << difftime(currentTime, b.returnDate) / (24*60*60)
                 << endl;
            foundOverdue = true;
        }
    }
    
    if (!foundOverdue) cout << "No overdue books found.\n";
}

void displayBooks() {
    int filter;
    cout << "\nDisplay Options:\n";
    cout << "1. All Books\n2. Available Books\n3. Issued Books\n";
    cout << "Enter choice: ";
    inputValidation(filter);
    
    ifstream fin(FILENAME, ios::binary);
    Book book;
    bool found = false;
    
    while (fin.read(reinterpret_cast<char*>(&book), sizeof(book))) {
        if ((filter == 1) || 
            (filter == 2 && !book.isIssued) || 
            (filter == 3 && book.isIssued)) {
                
            cout << "\nBook ID: " << book.id;
            cout << "\nTitle: " << book.title;
            cout << "\nAuthor: " << book.author;
            cout << "\nGenre: " << book.genre;
            cout << "\nStatus: " << (book.isIssued ? "Issued" : "Available");
            
            if (book.isIssued) {
                cout << "\nIssued Date: " << ctime(&book.issueDate);
                cout << "Due Date: " << ctime(&book.returnDate);
            }
            cout << endl;
            found = true;
        }
    }
    
    if (!found) cout << "No books found!\n";
    fin.close();
}

void searchBook() {
    int option;
    cout << "\nSearch by:\n1. ID\n2. Title\n3. Author\n4. Genre\n";
    cout << "Enter choice: ";
    inputValidation(option);
    
    char query[100];
    cout << "Enter search term: ";
    inputValidation(query, 100);
    
    ifstream fin(FILENAME, ios::binary);
    Book book;
    bool found = false;
    
    while (fin.read(reinterpret_cast<char*>(&book), sizeof(book))) {
        bool match = false;
        
        switch(option) {
            case 1: match = (book.id == atoi(query)); break;
            case 2: match = (strstr(book.title, query) != nullptr); break;
            case 3: match = (strstr(book.author, query) != nullptr); break;
            case 4: match = (strstr(book.genre, query) != nullptr); break;
        }
        
        if (match) {
            cout << "\nBook Found:";
            cout << "\nID: " << book.id;
            cout << "\nTitle: " << book.title;
            cout << "\nAuthor: " << book.author;
            cout << "\nGenre: " << book.genre;
            cout << "\nStatus: " << (book.isIssued ? "Issued" : "Available") << "\n\n";
            found = true;
        }
    }
    
    if (!found) cout << "No matching books found.\n";
    fin.close();
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

