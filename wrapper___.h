#ifndef SQL_CPP_WRAPPER_H
#define SQL_CPP_WRAPPER_H

#include <string>
#include <vector>
#include "sqlite3.h"

using namespace std;

class SQLite {
    string dbname;
    sqlite3* connection;

    sqlite3_stmt* query;
    vector<string> queryColumnsNames;
    const char ** _row =  nullptr;

public:
    SQLite(const string&);
    ~SQLite();

    // sql methods
    int select(const char * sql, ...);
    const char** fetch_row();

    // int execute(const char * sql, ...);
    // const char * value(const char * sql, ...);

    int getQueryColumnsCount() { return queryColumnsNames.size(); }
    string getDbname() { return dbname; }
    void message() const;
    void message(const char *) const;

private:
    void disconnect();
    void reset();
    int prepare(const char * sql, va_list ap);
};

#endif
