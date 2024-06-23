#include <cstdio>
#include <iostream>
#include <cstdarg>
#include <string>
#include "wrapper.h"

using namespace std;

SQLite::SQLite(const string& dbname) : dbname(dbname) {
    disconnect();
    if (sqlite3_open(dbname.data(), &connection)) {
        message();
        disconnect();
        exit(0);
    }
}

SQLite::~SQLite() {
    disconnect();
}

void SQLite::disconnect() {
    reset();
    if (connection) {
        sqlite3_close(connection);
        connection = nullptr;
    }
}

void SQLite::reset() {
    if (query) {
        sqlite3_finalize(query);
        query = nullptr;
    }
    if (_row) {
        delete[] _row;
        _row = nullptr;
    }
    queryColumnsNames.clear();
}

int SQLite::prepare(const char * sql, va_list ap) {
    reset();
    if (sqlite3_prepare_v2(connection, sql, -1, &query, nullptr)) {
        message();
        reset();
        return 0;
    }

    int count = sqlite3_column_count(query);
    for (int i = 0; i < count; ++i) {
        string name(sqlite3_column_name(query, i));
        queryColumnsNames.push_back(name);
    }

    count = sqlite3_bind_parameter_count(query);
    if (count) {
        for(int i = 1; i <= count; i++) {
            const char * parameter = va_arg(ap, const char *);
            sqlite3_bind_text(query, i, parameter, -1, SQLITE_STATIC);
        }
    }
    return count;
}

int SQLite::select(const char * sql, ...) {
    va_list ap;
    va_start(ap, sql);
    prepare(sql, ap);
    va_end(ap);
    
    return getQueryColumnsCount();
}

int SQLite::execute(const char * sql, ...) {
    va_list ap;
    va_start(ap, sql);
    prepare(sql, ap);
    va_end(ap);
    
    sqlite3_step(query);
    reset();
    return sqlite3_changes(connection);
}

const char * SQLite::value(const char * sql, ...) {
    va_list ap;
    va_start(ap, sql);
    prepare(sql, ap);
    va_end(ap);
    
    const char ** row = fetch_row();
    if (row) {
        return row[0];
    } else {
        return nullptr;
    }
}

const char ** SQLite::fetch_row() {
    // do we have a statement?
    if (!query) {
        reset();
        return nullptr;
    }
    // get the next row, if avail
    if (sqlite3_step(query) != SQLITE_ROW) {
        reset();
        return nullptr;
    }
    // make sure we have allocated space
    int count = getQueryColumnsCount();
    if (count && !_row) {
        _row = new const char * [count];
    }
    for (int i = 0; i < count; i++) {
        _row[i] = (const char *) sqlite3_column_text(query, i);
    }
    return _row;
}


void SQLite::message() const {
    if (connection) {
        cout << sqlite3_errmsg(connection) << endl;
    } else {
        cout << "Unknown error" << endl;
    }
}

void SQLite::message(const char * text) const {
    cout << text << endl;
    message();
}
