GREEN = \033[32m
RED = \033[31m
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


.PHONY: build clean setup run

setup:

	@if [ -d ../db ]; then \
		echo "$(YELLOW) [INFO] db/ directory already exists. $(RESET)\n"; \
	else \
		echo "$(YELLOW) [INFO] making db/ directory and db.bin file... $(RESET)\n"; \
		mkdir ../db; \
		touch ../db/db.bin; \
		echo "$(GREEN) [OK] db/ directory and db.bin file made. $(RESET)\n"; \
	fi

	@if [ -d ../bin ]; then \
		echo "$(YELLOW) [INFO] bin/ directory already exists. $(RESET)\n"; \
	else \
		echo "$(YELLOW) [INFO] making bin/ directory... $(RESET)\n"; \
		mkdir ../bin; \
		echo "$(GREEN) [OK] bin/ directory made. $(RESET)\n"; \
	fi

	@if [ -d ../obj ]; then \
		echo "$(YELLOW) [INFO] obj/ directory already exists. $(RESET)\n"; \
	else \
		echo "$(YELLOW) [INFO] making obj/ directory... $(RESET)\n"; \
		mkdir ../obj; \
		echo "$(GREEN) [OK] obj/ directory made. $(RESET)\n"; \
	fi

	@if [ -d ../logs ]; then \
		echo "$(YELLOW) [INFO] logs/ directory already exists. $(RESET)\n"; \
	else \
		echo "$(YELLOW) [INFO] making logs/ directory and server_trail.log file... $(RESET)\n"; \
		mkdir ../logs; \
		touch ../logs/server_trail.log; \
		echo "$(GREEN) [OK] logs/ directory and server_trail.log made. $(RESET)\n"; \
	fi

	@if [ -d ../config ]; then \
		echo "$(YELLOW) [INFO] condig/ directory already exists. $(RESET)\n"; \
	else \
		echo "$(YELLOW) [INFO] making config/ directory and .env and .env.example files... $(RESET)\n"; \
		mkdir ../config; \
		touch ../config/.env; \
		touch ../config/.env.example \
		echo "$(GREEN) [OK] config/ directory and .env and .env.example files made. $(RESET)\n"; \
	fi

clean:
	@printf "$(YELLOW) [INFO] removing compiled program... $(RESET)\n"
	rm -f obj/*.o obj/*.d $(TARGET_SERVER)
	@printf "$(GREEN) [OK] removal done. $(RESET)\n"

build:
	@printf "$(YELLOW) [INFO] compiling source code... $(RESET)\n"
	$(MAKE) $(TARGET_SERVER)
	@printf "$(GREEN) [OK] compilation done. $(RESET)\n"

run:
	@if [ ! -f ../config/.env ]; then \
		printf "$(RED)[ERROR] missing config/.env$(RESET)\n"; \
		exit 1; \
	fi

	@printf "$(YELLOW)[INFO] loading environment variables...$(RESET)\n"

	@set -a && \
	. ../config/.env && \
	set +a && \
	printf "$(GREEN)[OK] environment loaded.$(RESET)\n" && \
	printf "$(GREEN)[OK] starting server...$(RESET)\n" && \
	cd .. && ./bin/server