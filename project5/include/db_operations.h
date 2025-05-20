#ifndef DB_UTILS_H
#define DB_UTILS_H

#include <sqlite3.h>

// Функция для открытия соединения с БД
int open_database(const char *db_path, sqlite3 **db);

// Функция для закрытия соединения с БД
void close_database(sqlite3 *db);

// Функция для выполнения SQL-запроса без возврата результата (например, INSERT, UPDATE, DELETE)
// Возвращает 0 при успехе, 1 при ошибке.
// Использует параметризованные запросы, если sql_params не NULL.
int execute_sql(sqlite3 *db, const char *sql);

// Обертка для sqlite3_exec с простой обработкой ошибок и без callback
int simple_exec(sqlite3 *db, const char *sql_stmt);

#endif // DB_UTILS_H