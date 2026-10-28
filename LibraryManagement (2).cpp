#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
using namespace std;

// Class representing one book
class Book {
private:
    int id;
    string title;
    string author;
    bool available;

public:
    Book() {
        id = 0;
        title = "";
        author = "";
        available = true;
    }

    Book(int bookId, string bookTitle, string bookAuthor, bool isAvailable = true) {
        id = bookId;
        title = bookTitle;
        author = bookAuthor;
        available = isAvailable;
    }

    int getId() const { return id; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    bool isAvailable() const { return available; }

    void setTitle(string newTitle) { title = newTitle; }
    void setAuthor(string newAuthor) { author = newAuthor; }
    void setAvailability(bool status) { available = status; }

    void display() const {
        cout << "\nBook ID: " << id
             << "\nTitle: " << title
             << "\nAuthor: " << author
             << "\nStatus: " << (available ? "Available" : "Not available")
             << "\n-----------------------------\n";
    }

    // Store one book as a line in the data file.
    // The fields are separated by '|'.
    string toFileLine() const {
        return to_string(id) + "|" + title + "|" + author + "|" +
               (available ? "1" : "0");
    }

    static bool fromFileLine(const string& line, Book& result) {
        string idText, titleText, authorText, statusText;
        stringstream ss(line);

        if (!getline(ss, idText, '|') ||
            !getline(ss, titleText, '|') ||
            !getline(ss, authorText, '|') ||
            !getline(ss, statusText, '|')) {
            return false;
        }

        try {
            size_t used = 0;
            int bookId = stoi(idText, &used);
            if (used != idText.size() || titleText.empty() || authorText.empty() ||
                (statusText != "0" && statusText != "1")) {
                return false;
            }

            result = Book(bookId, titleText, authorText, statusText == "1");
            return true;
        } catch (...) {
            return false;
        }
    }
};

// Abstract base class: common menu behaviour for different roles
class Person {
public:
    virtual void showMenu() = 0;  // Pure virtual function
    virtual ~Person() {}
};

// Library manages books and file storage
class Library {
private:
    vector<Book> books;
    const string fileName = "books.txt";

    int findBookIndex(int id) const {
        for (int i = 0; i < static_cast<int>(books.size()); i++) {
            if (books[i].getId() == id) {
                return i;
            }
        }
        return -1;
    }

    void saveToFile() const {
        ofstream file(fileName);
        if (!file) {
            cout << "Error: Could not save book records.\n";
            return;
        }

        for (const Book& book : books) {
            file << book.toFileLine() << '\n';
        }
    }

    void loadFromFile() {
        ifstream file(fileName);
        if (!file) {
            // First run: the file will be created when the first book is saved.
            return;
        }

        string line;
        while (getline(file, line)) {
            Book book;
            if (Book::fromFileLine(line, book) && findBookIndex(book.getId()) == -1) {
                books.push_back(book);
            }
        }
    }

public:
    Library() {
        loadFromFile();
    }

    void addBook() {
        int id;
        string title, author;

        cout << "Enter book ID: ";
        if (!(cin >> id)) {
            cout << "Invalid ID.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (findBookIndex(id) != -1) {
            cout << "A book with this ID already exists.\n";
            return;
        }

        cout << "Enter book title: ";
        getline(cin, title);
        cout << "Enter author name: ";
        getline(cin, author);

        if (title.empty() || author.empty() ||
            title.find('|') != string::npos || author.find('|') != string::npos) {
            cout << "Title and author must be non-empty and cannot contain '|'.\n";
            return;
        }

        books.push_back(Book(id, title, author, true));
        saveToFile();
        cout << "Book added successfully.\n";
    }

    void displayBooks() const {
        if (books.empty()) {
            cout << "No books found in the library.\n";
            return;
        }

        cout << "\n========== ALL BOOKS ==========\n";
        for (const Book& book : books) {
            book.display();
        }
    }

    void searchBook() const {
        cout << "\nSearch by:\n1. Book ID\n2. Title\n3. Author\nChoose: ";
        int choice;
        if (!(cin >> choice)) {
            cout << "Invalid choice.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        if (choice == 1) {
            int id;
            cout << "Enter book ID: ";
            if (!(cin >> id)) {
                cout << "Invalid ID.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return;
            }
            int index = findBookIndex(id);
            if (index == -1) cout << "Book not found.\n";
            else books[index].display();
        } else if (choice == 2 || choice == 3) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string key;
            cout << (choice == 2 ? "Enter title keyword: " : "Enter author keyword: ");
            getline(cin, key);

            bool found = false;
            for (const Book& book : books) {
                string value = (choice == 2) ? book.getTitle() : book.getAuthor();
                if (value.find(key) != string::npos) {
                    book.display();
                    found = true;
                }
            }
            if (!found) cout << "No matching book found.\n";
        } else {
            cout << "Invalid choice.\n";
        }
    }

    void updateBook() {
        int id;
        cout << "Enter the ID of the book to update: ";
        if (!(cin >> id)) {
            cout << "Invalid ID.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        int index = findBookIndex(id);
        if (index == -1) {
            cout << "Book not found.\n";
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string title, author;
        cout << "Enter new title: ";
        getline(cin, title);
        cout << "Enter new author: ";
        getline(cin, author);

        if (title.empty() || author.empty() ||
            title.find('|') != string::npos || author.find('|') != string::npos) {
            cout << "Title and author must be non-empty and cannot contain '|'.\n";
            return;
        }

        books[index].setTitle(title);
        books[index].setAuthor(author);
        saveToFile();
        cout << "Book updated successfully.\n";
    }

    void deleteBook() {
        int id;
        cout << "Enter the ID of the book to delete: ";
        if (!(cin >> id)) {
            cout << "Invalid ID.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        int index = findBookIndex(id);
        if (index == -1) {
            cout << "Book not found.\n";
            return;
        }

        books.erase(books.begin() + index);
        saveToFile();
        cout << "Book deleted successfully.\n";
    }

    void changeAvailability() {
        int id;
        cout << "Enter book ID: ";
        if (!(cin >> id)) {
            cout << "Invalid ID.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        int index = findBookIndex(id);
        if (index == -1) {
            cout << "Book not found.\n";
            return;
        }

        cout << "1. Mark as available\n2. Mark as not available\nChoose: ";
        int choice;
        if (!(cin >> choice) || (choice != 1 && choice != 2)) {
            cout << "Invalid choice.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        books[index].setAvailability(choice == 1);
        saveToFile();
        cout << "Book availability updated.\n";
    }

    void displayAvailableBooks() const {
        bool found = false;
        cout << "\n====== AVAILABLE BOOKS ======\n";
        for (const Book& book : books) {
            if (book.isAvailable()) {
                book.display();
                found = true;
            }
        }
        if (!found) cout << "No available books at the moment.\n";
    }
};

// Inheritance: Admin and User both inherit from Person
class Admin : public Person {
private:
    Library& library;

public:
    Admin(Library& lib) : library(lib) {}

    void showMenu() override {
        int choice;
        do {
            cout << "\n========== ADMIN MENU ==========\n"
                 << "1. Add book\n"
                 << "2. Display all books\n"
                 << "3. Search book\n"
                 << "4. Update book\n"
                 << "5. Delete book\n"
                 << "6. Change book availability\n"
                 << "7. Back to main menu\n"
                 << "Enter choice: ";

            if (!(cin >> choice)) {
                cout << "Invalid input.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            switch (choice) {
                case 1: library.addBook(); break;
                case 2: library.displayBooks(); break;
                case 3: library.searchBook(); break;
                case 4: library.updateBook(); break;
                case 5: library.deleteBook(); break;
                case 6: library.changeAvailability(); break;
                case 7: cout << "Returning to main menu...\n"; break;
                default: cout << "Invalid choice. Try again.\n";
            }
        } while (choice != 7);
    }
};

class User : public Person {
private:
    Library& library;

public:
    User(Library& lib) : library(lib) {}

    void showMenu() override {
        int choice;
        do {
            cout << "\n========== USER MENU ==========\n"
                 << "1. Browse all books\n"
                 << "2. Search book\n"
                 << "3. View available books\n"
                 << "4. Back to main menu\n"
                 << "Enter choice: ";

            if (!(cin >> choice)) {
                cout << "Invalid input.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            switch (choice) {
                case 1: library.displayBooks(); break;
                case 2: library.searchBook(); break;
                case 3: library.displayAvailableBooks(); break;
                case 4: cout << "Returning to main menu...\n"; break;
                default: cout << "Invalid choice. Try again.\n";
            }
        } while (choice != 4);
    }
};

int main() {
    Library library;
    Admin admin(library);
    User user(library);

    int choice;
    do {
        cout << "\n=================================\n"
             << "     LIBRARY MANAGEMENT SYSTEM\n"
             << "=================================\n"
             << "1. Admin login\n"
             << "2. User menu\n"
             << "3. Exit\n"
             << "Enter choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        // Polymorphism: base-class pointer calls the appropriate overridden menu
        Person* currentPerson = nullptr;

        if (choice == 1) {
            string password;
            cout << "Enter admin password (demo password: admin123): ";
            cin >> password;

            if (password == "admin123") {
                currentPerson = &admin;
            } else {
                cout << "Incorrect password.\n";
            }
        } else if (choice == 2) {
            currentPerson = &user;
        } else if (choice == 3) {
            cout << "Thank you for using the Library Management System!\n";
        } else {
            cout << "Invalid choice. Try again.\n";
        }

        if (currentPerson != nullptr) {
            currentPerson->showMenu();
        }

    } while (choice != 3);

    return 0;
}
