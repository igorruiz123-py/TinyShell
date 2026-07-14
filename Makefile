GREEN = \033[32m
YELLOW = \033[33m
RESET = \033[0m
CC = gcc
CFLAGS = -Iinclude
TARGET_SERVER = bin/server
OBJ_FILES_SERVER = obj/server.o obj/server_functions.o obj/handle_client.o

$(TARGET_SERVER): $(OBJ_FILES_SERVER)
	$(CC) $(OBJ_FILES_SERVER) -o $(TARGET_SERVER)


obj/server.o: src/server.c
	$(CC) $(CFLAGS) -c src/server.c -o obj/server.o

obj/server_functions.o: src/server_functions.c
	$(CC) $(CFLAGS) -c src/server_functions.c -o obj/server_functions.o

obj/handle_client.o: src/handle_client.c
	$(CC) $(CFLAGS) -c src/handle_client.c -o obj/handle_client.o


.PHONY: compile clean

clean:
	rm -f obj/*.o obj/*.d $(TARGET_SERVER)

compile:
	@printf "$(YELLOW) [INFO] compiling source code... $(RESET) \n"
	$(MAKE) $(TARGET_SERVER)
	@printf "$(GREEN) [OK] compilation done. $(RESET) \n"