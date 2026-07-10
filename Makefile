GREEN = \033[32m
YELLOW = \033[33m
RESET = \033[0m
CC = gcc
CFLAGS = -Iinclude
TARGET_SERVER = bin/server
OBJ_FILES_SERVER = obj/server.o obj/server_functions.o
TARGET_DB = bin/db
OBJ_FILES_DB = obj/db.o obj/db_funcs.o

$(TARGET_SERVER): $(OBJ_FILES_SERVER)
	$(CC) $(OBJ_FILES_SERVER) -o $(TARGET_SERVER)

$(TARGET_DB): $(OBJ_FILES_DB)
	$(CC) $(OBJ_FILES_DB) -o $(TARGET_DB)

obj/server.o: src/server.c
	$(CC) $(CFLAGS) -c src/server.c -o obj/server.o

obj/server_functions.o: src/server_functions.c
	$(CC) $(CFLAGS) -c src/server_functions.c -o obj/server_functions.o

obj/db_funcs.o: src/db_funcs.c
	$(CC) $(CFLAGS) -c src/db_funcs.c -o obj/db_funcs.o

obj/db.o: src/db.c
	$(CC) $(CFLAGS) -c src/db.c -o obj/db.o

.PHONY = compile

compile:
	@printf "$(YELLOW) [INFO] compiling source code... $(RESET) \n"
	$(MAKE) $(TARGET_SERVER)
	$(MAKE) $(TARGET_DB)
	@printf "$(GREEN) [OK] compilation done. $(RESET) \n"