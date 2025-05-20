#include "db_utils.h"
#include <stdio.h>

int open_database(const char *db_path, sqlite3 **db) {
    int rc = sqlite3_open(db_path, db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));
        sqlite3_close(*db);
        return 1;
    }
    printf("Database connection opened successfully: %s\n", db_path);
    return 0;
}

void close_database(sqlite3 *db) {
    if (db) {
        sqlite3_close(db);
        printf("Database connection closed.\n");
    }
}

int execute_sql(sqlite3 *db, const char *sql) {
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }
    return 0;
}

int simple_exec(sqlite3 *db, const char *sql_stmt) {
    char *errmsg;
    int rc = sqlite3_exec(db, sql_stmt, 0, 0, &errmsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error in simple_exec: %s\n", errmsg);
        sqlite3_free(errmsg);
        return 1;
    }
    return 0;
}