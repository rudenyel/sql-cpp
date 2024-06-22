
#include <string>
#include <sstream>
#include <iostream>
#include <functional>
#include "main.h"
#include "wrapper.h"

using namespace std;

constexpr auto queryCheckBookTable =
    "SELECT name FROM sqlite_master WHERE type = 'table' AND name = 'books'";

constexpr auto queryCreateTable =
    "CREATE TABLE IF NOT EXISTS books ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "title VARCHAR(127) CHECK(title != ''),"
    "first_name VARCHAR(127) CHECK(first_name != ''),"
    "last_name VARCHAR(127) CHECK(last_name != '')"
    ")"
;

constexpr auto queryBooks = "SELECT * FROM books";
constexpr auto queryBooksSortByAuthor = "SELECT * FROM books ORDER BY last_name, title";
constexpr auto queryBooksSortByTitle = "SELECT * FROM books ORDER BY title, last_name";
constexpr auto queryBooksFindByAuthor = "SELECT * FROM books WHERE last_name LIKE ?";
constexpr auto queryBooksFindByTitle = "SELECT * FROM books WHERE title LIKE ?";
constexpr auto queryBookAdd = "INSERT INTO books (title, first_name, last_name) VALUES (?, ?, ?)";
constexpr auto queryBookDelete = "DELETE FROM books WHERE id = ?";
constexpr auto queryBooksDropTable = "DROP TABLE IF EXISTS books";

constexpr const char* menu[] = {
    "List books",
    "List books (sort by author)",
    "List books (sort by title)",
    "Find books by author",
    "Find books by title",
    "Add book",
    "Delete book",
    "Drop books table and exit",
};

function<void(SQLite& db)> execute[] = {
    Books,
    BooksSortByAuthor,
    BooksSortByTitle,
    BooksFindByAuthor,
    BooksFindByTitle,
    BookAdd,
    BookDelete,
    BooksDropTable,
};

void TextMenu(SQLite& db) {
    while(true) {
        cout << endl;
        cout << "Current database " << db.filename() << ":" << endl;
        int count = 0;
        for (const char* item : menu)
            cout << count++ << ": " << item << endl;

        string input;
        int item;
        cout << "Select an action or 'q' to quit > ";
        while (true) {
            getline(cin, input);
            if (stringstream(input) >> item) {
                if ((item >=0) && (item <=count)) break;
            }
            else
                if (input == "q") exit(0);
            cout << "Incorrect input, try again > ";
        }

        execute[item](db);
    }    
}

void DisplayData(SQLite& db) {
    int rows = 0;
    const int columns = db.column_count();
    for (const char** row = db.fetch_row(); row; row = db.fetch_row()) {
        for (int column = 0; column < columns; ++column)
            cout << row[column] << ((column < columns - 1) ? "; " : "\n");
        rows++;
    }
    if (rows == 0) cout << "Queryset is empty" << endl;
}

void Books(SQLite& db) {
    cout << "List books:" << endl;
    db.select(queryBooks);
    DisplayData(db);
}

void BooksSortByAuthor(SQLite& db) {
    cout << "List books (sort by author):" << endl;
    db.select(queryBooksSortByAuthor);
    DisplayData(db);
}

void BooksSortByTitle(SQLite& db) {
    cout << "List books (sort by title):" << endl;
    db.select(queryBooksSortByTitle);
    DisplayData(db);
}

void BooksFindByAuthor(SQLite& db) {
    string last_name;
    cout << "Find books by author last name > ";
    getline(cin, last_name);

    db.select(queryBooksFindByAuthor, last_name.data());
    DisplayData(db);
}

void BooksFindByTitle(SQLite& db) {
    string title;
    cout << "Find books by title > ";
    getline(cin, title);

    db.select(queryBooksFindByTitle, title.data());
    DisplayData(db);
}

void BookAdd(SQLite& db) {
    string title;
    string first_name;
    string last_name;
    cout << "Add book:" << endl;
    cout << "Title > ";
    getline(cin, title);
    cout << "Author first name > ";
    getline(cin, first_name);
    cout << "Author last name > ";
    getline(cin, last_name);

    if (!db.execute(queryBookAdd, title.data(), first_name.data(), last_name.data())) {
        db.error_message("Could not add row");
    }
}

void BookDelete(SQLite& db) {
    string id;
    cout << "Delete book with id > ";
    getline(cin, id);
    
    if (!db.execute(queryBookDelete, id.data()))
        db.error_message("Could not delete row");
}

void BooksDropTable(SQLite& db) {
    db.execute(queryBooksDropTable);
    exit(0);
}

int main() {
    string filename;
    cout << "Create (or open if exists) database:" << endl;
    cout << "Database filename (default books.db) > ";
    getline(cin, filename);
    if (filename.empty()) {
        filename = "books.db";
    }

    SQLite db(filename.data());
    if (!db.value(queryCheckBookTable)) {
        db.execute(queryCreateTable);
    }

    TextMenu(db);

    return 0;
}
