int create_server_socket(const char *port);

int handle_client(int sockfd);

typedef struct
{
    char *username;
    char *password;
    
} user_t;

#define DB_PATH "db/db.bin"

#include <stdbool.h>

void free_user_t(user_t *user);

bool read_user(FILE *db, user_t *user);

bool authenticate_user(char *db_path, char *message);