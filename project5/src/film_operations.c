#include "film_operations.h"
#include "db_utils.h" // Для simple_exec

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int select_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    NotUsed = NotUsed;
    for (int i = 0; i < argc; i++) {
        printf("%s: %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void read_string_safely(char *buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        } else {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }
    } else {
        buffer[0] = '\0';
    }
}

void view_all_films(sqlite3 *db) {
    printf("\n--- Все фильмы ---\n");
    const char *sql = "SELECT f.id AS ID_Фильма, f.название, f.год_выхода AS Год, "
                      "f.режиссер_фамилия || ' ' || f.режиссер_имя AS Режиссер, "
                      "f.жанр_кино AS Жанр, f.цена_билета AS Цена_билета, "
                      "c.ct_name AS Кинотеатр, c.ct_city AS Город_кинотеатра "
                      "FROM Фильмы f "
                      "LEFT JOIN cinematheatres c ON f.cinematheatre_id = c.id;";
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, select_callback, 0, &err_msg);

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        printf("Список фильмов выведен.\n");
    }
}

void find_film_by_id_interactive(sqlite3 *db) {
    printf("\n--- Поиск фильма по ID ---\n");
    int id;
    printf("Введите ID фильма: ");
    if (scanf("%d", &id) != 1) {
        printf("Ошибка ввода ID.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    const char *sql = "SELECT f.id, f.название, f.год_выхода, "
                      "f.режиссер_фамилия || ' ' || f.режиссер_имя AS Режиссер, "
                      "f.жанр_кино, f.цена_билета, f.стоимость, f.доход, f.прибыль, "
                      "c.ct_name AS Кинотеатр, c.ct_city AS Город_Кинотеатра "
                      "FROM Фильмы f "
                      "LEFT JOIN cinematheatres c ON f.cinematheatre_id = c.id "
                      "WHERE f.id = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt, 1, id);

    printf("\nРезультат поиска:\n");
    int found = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        found = 1;
        printf("---------------------------------\n");
        printf("ID: %d\n", sqlite3_column_int(stmt, 0));
        printf("Название: %s\n", sqlite3_column_text(stmt, 1));
        printf("Год выхода: %d\n", sqlite3_column_int(stmt, 2));
        printf("Режиссер: %s\n", sqlite3_column_text(stmt, 3));
        printf("Жанр: %s\n", sqlite3_column_text(stmt, 4));
        printf("Цена билета: %.2f\n", sqlite3_column_double(stmt, 5));
        printf("Стоимость (производство): %.2f\n", sqlite3_column_double(stmt, 6));
        printf("Доход: %.2f\n", sqlite3_column_double(stmt, 7));
        printf("Прибыль: %.2f\n", sqlite3_column_double(stmt, 8));
        const unsigned char* cinema_name = sqlite3_column_text(stmt, 9);
        const unsigned char* cinema_city = sqlite3_column_text(stmt, 10);
        printf("Кинотеатр: %s (%s)\n",
               cinema_name ? (const char*)cinema_name : "N/A",
               cinema_city ? (const char*)cinema_city : "N/A");
        printf("---------------------------------\n\n");
    }
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute step: %s\n", sqlite3_errmsg(db));
    }
    if (!found) {
        printf("Фильм с ID %d не найден.\n", id);
    }
    sqlite3_finalize(stmt);
}

void add_film_interactive(sqlite3 *db) {
    printf("\n--- Добавление нового фильма ---\n");
    char название[100], режиссер_фамилия[50], режиссер_имя[50], страна[50], жанр_кино[50];
    int год_выхода;
    double стоимость = 0.0, доход = 0.0, прибыль = 0.0, цена_билета = 0.0;

    printf("Название фильма: "); read_string_safely(название, sizeof(название));
    printf("Фамилия режиссера: "); read_string_safely(режиссер_фамилия, sizeof(режиссер_фамилия));
    printf("Имя режиссера: "); read_string_safely(режиссер_имя, sizeof(режиссер_имя));
    printf("Год выхода: "); 
    if(scanf("%d", &год_выхода) != 1) { printf("Ошибка ввода года.\n"); while(getchar()!='\n'); return; } 
    while(getchar()!='\n');
    printf("Страна: "); read_string_safely(страна, sizeof(страна));
    printf("Жанр: "); read_string_safely(жанр_кино, sizeof(жанр_кино));
    printf("Стоимость фильма (производство): "); 
    if(scanf("%lf", &стоимость) != 1) { printf("Ошибка ввода стоимости.\n"); while(getchar()!='\n'); return; }
    while(getchar()!='\n');
    printf("Доход фильма: "); 
    if(scanf("%lf", &доход) != 1) { printf("Ошибка ввода дохода.\n"); while(getchar()!='\n'); return; }
    while(getchar()!='\n');
    printf("Прибыль фильма: "); 
    if(scanf("%lf", &прибыль) != 1) { printf("Ошибка ввода прибыли.\n"); while(getchar()!='\n'); return; }
    while(getchar()!='\n');
    printf("Цена билета: "); 
    if(scanf("%lf", &цена_билета) != 1) { printf("Ошибка ввода цены билета.\n"); while(getchar()!='\n'); return; }
    while(getchar()!='\n');

    const char *sql = "INSERT INTO Фильмы (название, режиссер_фамилия, режиссер_имя, год_выхода, страна, стоимость, доход, прибыль, цена_билета, жанр_кино) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, название, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, режиссер_фамилия, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, режиссер_имя, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, год_выхода);
    sqlite3_bind_text(stmt, 5, страна, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 6, стоимость);
    sqlite3_bind_double(stmt, 7, доход);
    sqlite3_bind_double(stmt, 8, прибыль);
    sqlite3_bind_double(stmt, 9, цена_билета);
    sqlite3_bind_text(stmt, 10, жанр_кино, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Фильм '%s' успешно добавлен! ID фильма: %lld\n", название, sqlite3_last_insert_rowid(db));
    }
    sqlite3_finalize(stmt);
}

void find_films_by_title_pattern_interactive(sqlite3 *db) {
    printf("\n--- Поиск фильмов по шаблону названия ---\n");
    char pattern_input[100];
    char pattern_sql[105];

    printf("Введите шаблон для поиска в названии фильма: ");
    read_string_safely(pattern_input, sizeof(pattern_input));
    if (strlen(pattern_input) == 0) { printf("Шаблон не введен.\n"); return; }

    sprintf(pattern_sql, "%%%s%%", pattern_input);
    const char *sql = "SELECT f.id, f.название, f.год_выхода, f.режиссер_фамилия || ' ' || f.режиссер_имя AS Режиссер, f.жанр_кино, c.ct_name AS Кинотеатр FROM Фильмы f LEFT JOIN cinematheatres c ON f.cinematheatre_id = c.id WHERE f.название LIKE ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) { fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db)); return; }

    sqlite3_bind_text(stmt, 1, pattern_sql, -1, SQLITE_STATIC);

    printf("\nРезультаты поиска по названию '%s':\n", pattern_input);
    int found_count = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        found_count++;
        printf("ID: %d, Название: %s, Год: %d, Режиссер: %s, Жанр: %s, Кинотеатр: %s\n",
               sqlite3_column_int(stmt, 0), sqlite3_column_text(stmt, 1),
               sqlite3_column_int(stmt, 2), sqlite3_column_text(stmt, 3),
               sqlite3_column_text(stmt, 4), sqlite3_column_text(stmt, 5) ? (const char*)sqlite3_column_text(stmt, 5) : "N/A");
    }
    if (rc != SQLITE_DONE) { fprintf(stderr, "Failed to execute step: %s\n", sqlite3_errmsg(db)); }
    if (found_count == 0) { printf("Фильмы, соответствующие шаблону '%s', не найдены.\n", pattern_input);
    } else { printf("\nНайдено фильмов: %d\n", found_count); }
    sqlite3_finalize(stmt);
}

void find_films_by_genre_interactive(sqlite3 *db) {
    printf("\n--- Поиск фильмов по жанру ---\n");
    char genre_input[100];
    printf("Введите жанр для поиска: ");
    read_string_safely(genre_input, sizeof(genre_input));
    if (strlen(genre_input) == 0) { printf("Жанр не введен.\n"); return; }

    const char *sql = "SELECT f.id, f.название, f.год_выхода, f.режиссер_фамилия || ' ' || f.режиссер_имя AS Режиссер, f.жанр_кино, c.ct_name AS Кинотеатр FROM Фильмы f LEFT JOIN cinematheatres c ON f.cinematheatre_id = c.id WHERE f.жанр_кино LIKE ?;"; // Используем LIKE для гибкости
    
    char genre_pattern_sql[105];
    sprintf(genre_pattern_sql, "%%%s%%", genre_input); // Поиск по частичному совпадению жанра

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) { fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db)); return; }

    sqlite3_bind_text(stmt, 1, genre_pattern_sql, -1, SQLITE_STATIC);

    printf("\nРезультаты поиска по жанру '%s':\n", genre_input);
    int found_count = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        found_count++;
        printf("ID: %d, Название: %s, Год: %d, Режиссер: %s, Жанр: %s, Кинотеатр: %s\n",
               sqlite3_column_int(stmt, 0), sqlite3_column_text(stmt, 1),
               sqlite3_column_int(stmt, 2), sqlite3_column_text(stmt, 3),
               sqlite3_column_text(stmt, 4), sqlite3_column_text(stmt, 5) ? (const char*)sqlite3_column_text(stmt, 5) : "N/A");
    }
    if (rc != SQLITE_DONE) { fprintf(stderr, "Failed to execute step: %s\n", sqlite3_errmsg(db)); }
    if (found_count == 0) { printf("Фильмы с жанром, содержащим '%s', не найдены.\n", genre_input);
    } else { printf("\nНайдено фильмов: %d\n", found_count); }
    sqlite3_finalize(stmt);
}

void delete_film_by_id_interactive(sqlite3 *db) {
    printf("\n--- Удаление фильма по ID ---\n");
    int id;
    char confirmation[10];

    printf("Введите ID фильма для удаления: ");
    if (scanf("%d", &id) != 1) { printf("Ошибка ввода ID.\n"); while (getchar() != '\n'); return; }
    while (getchar() != '\n');

    const char *sql_select = "SELECT название FROM Фильмы WHERE id = ?;";
    sqlite3_stmt *check_stmt;
    if (sqlite3_prepare_v2(db, sql_select, -1, &check_stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare select for delete check: %s\n", sqlite3_errmsg(db)); return;
    }
    sqlite3_bind_int(check_stmt, 1, id);

    int rc_check = sqlite3_step(check_stmt);
    if (rc_check == SQLITE_ROW) {
        printf("Найден фильм для удаления: Название - %s (ID: %d)\n", sqlite3_column_text(check_stmt, 0), id);
        sqlite3_finalize(check_stmt);

        printf("Вы уверены, что хотите удалить этот фильм? (yes/no): ");
        read_string_safely(confirmation, sizeof(confirmation));

        if (strcmp(confirmation, "yes") == 0) {
            const char *sql_delete = "DELETE FROM Фильмы WHERE id = ?;";
            sqlite3_stmt *delete_stmt;
            if (sqlite3_prepare_v2(db, sql_delete, -1, &delete_stmt, 0) != SQLITE_OK) {
                fprintf(stderr, "Failed to prepare delete statement: %s\n", sqlite3_errmsg(db)); return;
            }
            sqlite3_bind_int(delete_stmt, 1, id);

            if (sqlite3_step(delete_stmt) == SQLITE_DONE) {
                if (sqlite3_changes(db) > 0) { printf("Фильм с ID %d успешно удален.\n", id); }
                else { printf("Фильм с ID %d не найден для удаления (возможно, уже удален).\n", id); }
            } else { fprintf(stderr, "Failed to execute delete statement: %s\n", sqlite3_errmsg(db)); }
            sqlite3_finalize(delete_stmt);
        } else { printf("Удаление отменено.\n"); }
    } else if (rc_check == SQLITE_DONE) {
        printf("Фильм с ID %d не найден.\n", id);
        sqlite3_finalize(check_stmt);
    } else {
        fprintf(stderr, "Error checking film for deletion: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(check_stmt);
    }
}

void add_photo_to_film_interactive(sqlite3 *db) {
    printf("\n--- Добавление фотографии к фильму ---\n");
    int film_id;
    char photo_path[256];

    printf("Введите ID фильма, к которому добавить фото: ");
    if (scanf("%d", &film_id) != 1) { printf("Ошибка ввода ID.\n"); while (getchar() != '\n'); return; }
    while (getchar() != '\n');

    printf("Введите полный путь к файлу фотографии (например, data/woman.jpg): ");
    read_string_safely(photo_path, sizeof(photo_path));
    if (strlen(photo_path) == 0) { printf("Путь к фото не указан.\n"); return; }

    FILE *fp = fopen(photo_path, "rb");
    if (fp == NULL) { fprintf(stderr, "Cannot open image file: %s\n", photo_path); return; }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (file_size <= 0) { fprintf(stderr, "Файл изображения пуст или ошибка: %s\n", photo_path); fclose(fp); return; }

    char *photo_data = malloc(file_size);
    if (photo_data == NULL) { fprintf(stderr, "Failed to allocate memory.\n"); fclose(fp); return; }

    if (fread(photo_data, 1, file_size, fp) != file_size) {
        fprintf(stderr, "Failed to read image file: %s\n", photo_path);
        fclose(fp); free(photo_data); return;
    }
    fclose(fp);

    const char *sql = "UPDATE Фильмы SET фотография = ? WHERE id = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement for photo update: %s\n", sqlite3_errmsg(db));
        free(photo_data); return;
    }
    sqlite3_bind_blob(stmt, 1, photo_data, file_size, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, film_id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute photo update: %s\n", sqlite3_errmsg(db));
    } else {
        if (sqlite3_changes(db) > 0) { printf("Фотография успешно добавлена к фильму ID %d.\n", film_id); }
        else { printf("Фильм с ID %d не найден, фото не добавлено.\n", film_id); }
    }
    sqlite3_finalize(stmt);
    free(photo_data);
}

void extract_film_photo_interactive(sqlite3 *db) {
    printf("\n--- Извлечение фотографии фильма ---\n");
    int film_id;
    char output_path[256];

    printf("Введите ID фильма, фото которого извлечь: ");
    if (scanf("%d", &film_id) != 1) { printf("Ошибка ввода ID.\n"); while (getchar() != '\n'); return; }
    while (getchar() != '\n');

    printf("Введите путь для сохранения файла (например, data/extracted_photo.jpg): ");
    read_string_safely(output_path, sizeof(output_path));
    if (strlen(output_path) == 0) { printf("Путь для сохранения не указан.\n"); return; }

    const char *sql = "SELECT фотография FROM Фильмы WHERE id = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement for photo extraction: %s\n", sqlite3_errmsg(db)); return;
    }
    sqlite3_bind_int(stmt, 1, film_id);

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const void *blob_data = sqlite3_column_blob(stmt, 0);
        int blob_size = sqlite3_column_bytes(stmt, 0);
        if (blob_data && blob_size > 0) {
            FILE *fp = fopen(output_path, "wb");
            if (fp == NULL) { fprintf(stderr, "Cannot open file for writing: %s\n", output_path); }
            else {
                if (fwrite(blob_data, 1, blob_size, fp) == blob_size) { printf("Фотография успешно сохранена в %s\n", output_path); }
                else { fprintf(stderr, "Failed to write photo data to file.\n"); }
                fclose(fp);
            }
        } else { printf("У фильма ID %d нет фотографии или она пуста.\n", film_id); }
    } else if (rc == SQLITE_DONE) { printf("Фильм с ID %d не найден.\n", film_id);
    } else { fprintf(stderr, "Failed to execute photo extraction step: %s\n", sqlite3_errmsg(db)); }
    sqlite3_finalize(stmt);
}

void demonstrate_autocommit_inserts(sqlite3 *db) {
    printf("\n--- Демонстрация Autocommit INSERT ---\n");
    if (simple_exec(db, "INSERT INTO Фильмы (название, год_выхода, жанр_кино) VALUES ('Autocommit Film 1 (Успех)', 2025, 'Демо');") == 0) {
        printf("Autocommit Film 1 успешно вставлен.\n");
    }
    printf("Попытка ошибочной вставки (несуществующее поле 'bad_column'):\n");
    if (simple_exec(db, "INSERT INTO Фильмы (название, bad_column) VALUES ('Autocommit Film 2 (Ошибка)', 'Ошибка');") != 0) {
        printf("Ошибка при вставке Autocommit Film 2, как и ожидалось.\n");
    }
    if (simple_exec(db, "INSERT INTO Фильмы (название, год_выхода, жанр_кино) VALUES ('Autocommit Film 3 (Успех)', 2026, 'Демо');") == 0) {
        printf("Autocommit Film 3 успешно вставлен.\n");
    }
    printf("Проверьте БД: 'Autocommit Film 1' и 'Autocommit Film 3' должны быть, 'Autocommit Film 2' - нет.\n");
}

void demonstrate_transaction_inserts(sqlite3 *db) {
    printf("\n--- Демонстрация Transaction INSERT ---\n");
    printf("Начинаем транзакцию...\n");
    if (simple_exec(db, "BEGIN TRANSACTION;") != 0) return;

    int success = 1; // Флаг успеха всех операций в транзакции

    if (simple_exec(db, "INSERT INTO Фильмы (название, год_выхода, жанр_кино) VALUES ('Transaction Film 1 (В транз.)', 2027, 'Транзакция');") == 0) {
        printf("Transaction Film 1 подготовлен к вставке.\n");
    } else {
        printf("Ошибка при подготовке Transaction Film 1.\n"); success = 0;
    }

    printf("Попытка ошибочной вставки в транзакции (несуществующее поле 'bad_column'):\n");
    if (simple_exec(db, "INSERT INTO Фильмы (название, bad_column) VALUES ('Transaction Film 2 (Ошибка в транз.)', 'Ошибка');") != 0) {
        printf("Ошибка при подготовке Transaction Film 2, как и ожидалось.\n");
        // Ошибка произошла, но мы не прерываем транзакцию здесь, чтобы показать, что ROLLBACK отменит все
        success = 0; // Помечаем, что транзакция не полностью успешна
    } else {
         printf("Transaction Film 2 (ошибочный) подготовлен - этого не должно было быть.\n");
         success = 0; // Помечаем, что транзакция не полностью успешна
    }

    if (simple_exec(db, "INSERT INTO Фильмы (название, год_выхода, жанр_кино) VALUES ('Transaction Film 3 (В транз.)', 2028, 'Транзакция');") == 0) {
        printf("Transaction Film 3 подготовлен к вставке.\n");
    } else {
        printf("Ошибка при подготовке Transaction Film 3.\n"); success = 0;
    }

    if (success) {
        printf("Ошибок в командах транзакции не было. Коммитим изменения (COMMIT)...\n");
        if (simple_exec(db, "COMMIT;") == 0) {
            printf("Транзакция успешно закоммичена.\n");
        } else {
            printf("Ошибка при COMMIT. Откатываем изменения (ROLLBACK)...\n");
            simple_exec(db, "ROLLBACK;"); // Явный откат, если COMMIT не удался
        }
    } else {
        printf("Были ошибки в транзакции. Откатываем изменения (ROLLBACK)...\n");
        simple_exec(db, "ROLLBACK;");
    }
    printf("Проверьте БД: если были ошибки, фильмы 'Transaction Film ...' НЕ должны быть добавлены.\n");
    printf("Если ошибок не было (вручную исправлен ошибочный INSERT), то должны быть добавлены.\n");
}