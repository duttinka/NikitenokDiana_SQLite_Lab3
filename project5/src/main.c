#include <stdio.h>
#include "db_utils.h"       
#include "film_operations.h" 

void print_menu() {
    printf("\n--- Меню управления базой данных фильмов ---\n");
    printf("1. Показать все фильмы\n");
    printf("2. Найти фильм по ID\n");
    printf("3. Добавить новый фильм\n");
    printf("4. Найти фильмы по названию (шаблон)\n"); 
    printf("5. Найти фильмы по жанру\n");         
    printf("6. Удалить фильм по ID\n");              
    printf("7. Добавить фото к фильму\n");
    printf("8. Извлечь фото фильма\n");
    printf("9. Демонстрация Autocommit INSERT\n");
    printf("10. Демонстрация Transaction INSERT\n");
    printf("0. Выход\n");
    printf("Выберите опцию: ");
}

int main() {
    sqlite3 *db = NULL; 
    const char *db_path = "data/kinematica.db"; 

    if (open_database(db_path, &db) != 0) {
        return 1; 
    }

    int choice;
    do {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            printf("Некорректный ввод. Пожалуйста, введите число.\n");
            while (getchar() != '\n'); 
            choice = -1; 
            continue;
        }
        while (getchar() != '\n'); 

        switch (choice) {
            case 1: view_all_films(db); break;
            case 2: find_film_by_id_interactive(db); break;
            case 3: add_film_interactive(db); break;
            case 4: find_films_by_title_pattern_interactive(db); break; 
            case 5: find_films_by_genre_interactive(db); break;      
            case 6: delete_film_by_id_interactive(db); break;        
            case 7: add_photo_to_film_interactive(db); break;
            case 8: extract_film_photo_interactive(db); break;
            case 9: demonstrate_autocommit_inserts(db); break;
            case 10: demonstrate_transaction_inserts(db); break;
            case 0: printf("Выход из программы.\n"); break;
            default: printf("Неверный выбор. Пожалуйста, попробуйте снова.\n");
        }
    } while (choice != 0);

    close_database(db);
    return 0;
}