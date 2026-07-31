GREEN = \033[32m
YELLOW = \033[33m
RESET = \033[0m

CC = gcc
CFLAGS = -I../include
TARGET_SERVER = ../bin/server
OBJ_FILES = ../obj/server.o ../obj/server_functions.o ../obj/handle_client.o ../obj/handle_client_functions.o ../obj/db.o

$(TARGET_SERVER): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(TARGET_SERVER)


../obj/server.o: ../src/server.c
	$(CC) $(CFLAGS) -c ../src/server.c -o ../obj/server.o

../obj/server_functions.o: ../src/server_functions.c
	$(CC) $(CFLAGS) -c ../src/server_functions.c -o ../obj/server_functions.o

../obj/handle_client.o: ../src/handle_client.c
	$(CC) $(CFLAGS) -c ../src/handle_client.c -o ../obj/handle_client.o

../obj/handle_client_functions.o: ../src/handle_client_functions.c
	$(CC) $(CFLAGS) -c ../src/handle_client_functions.c -o ../obj/handle_client_functions.o

../obj/db.o: ../src/db.c
	$(CC) $(CFLAGS) -c ../src/db.c -o ../obj/db.o


.PHONY: compile clean

clean:
	@printf "$(YELLOW) [INFO] removing compiled program... $(RESET) \n"
	rm -f obj/*.o obj/*.d $(TARGET_SERVER)
	@printf "$(GREEN) [OK] removal done. $(RESET) \n"

compile:
	@printf "$(YELLOW) [INFO] compiling source code... $(RESET) \n"
	$(MAKE) $(TARGET_SERVER)
	@printf "$(GREEN) [OK] compilation done. $(RESET) \n"