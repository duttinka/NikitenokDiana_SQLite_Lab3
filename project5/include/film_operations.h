#ifndef FILM_OPERATIONS_H
#define FILM_OPERATIONS_H

#include <sqlite3.h> 

//Объявления функций для операций с фильмами

void read_string_safely(char *buffer, int size);

void view_all_films(sqlite3 *db);

void find_film_by_id_interactive(sqlite3 *db);

void add_film_interactive(sqlite3 *db);

void find_films_by_title_pattern_interactive(sqlite3 *db);

void find_films_by_genre_interactive(sqlite3 *db);

void delete_film_by_id_interactive(sqlite3 *db);

void add_photo_to_film_interactive(sqlite3 *db);

void extract_film_photo_interactive(sqlite3 *db);

void demonstrate_autocommit_inserts(sqlite3 *db);

void demonstrate_transaction_inserts(sqlite3 *db);

#endif // FILM_OPERATIONS_H