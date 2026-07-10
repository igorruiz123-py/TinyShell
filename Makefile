GREEN = \033[32m
YELLOW = \033[33m
RESET = \033[0m
CC = gcc
CFLAGS = -Iinclude
TARGET = bin/server
OBJ_FILES = obj/server.o obj/server_functions.o

$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(TARGET)

obj/server.o: src/server.c
	$(CC) $(CFLAGS) -c src/server.c -o obj/server.o

obj/server_functions.o: src/server_functions.c
	$(CC) $(CFLAGS) -c src/server_functions.c -o obj/server_functions.o

.PHONY = compile

compile:
	@printf "$(YELLOW) [INFO] compiling source code... $(RESET) \n"
	$(MAKE) $(TARGET)
	@printf "$(GREEN) [OK] compilation done. $(RESET) \n"