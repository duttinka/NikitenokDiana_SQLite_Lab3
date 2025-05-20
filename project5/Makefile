CC = gcc
CFLAGS = -Wall -std=c99 -Iinclude # -Iinclude говорит компилятору искать .h файлы в папке 
include
LDFLAGS = -lsqlite3
RM = rm -f

# Исходные .c файлы из папки src
SRCS_DIR = src
SRCS = $(wildcard $(SRCS_DIR)/*.c) 

# Объектные .o файлы (будут создаваться в той же папке src)
OBJS = $(SRCS:.c=.o)

# Имя исполняемого файла в папке bin (папка bin будет создана)
TARGET_DIR = bin
TARGET = $(TARGET_DIR)/film_app

# Правило по умолчанию
all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(TARGET_DIR) # Создать папку bin, если ее нет
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Правило для компиляции .c файлов из src в .o файлы в src
$(SRCS_DIR)/%.o: $(SRCS_DIR)/%.c include/*.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(SRCS_DIR)/*.o $(TARGET)
	@echo "Clean complete."
