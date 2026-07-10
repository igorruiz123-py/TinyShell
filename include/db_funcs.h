typedef struct
{
    char *username;
    char *password;
    
} user_t;

#define DB_PATH "../db/db.bin"

#include <stdbool.h>

void free_user_t(user_t *user);

void add_user_db(FILE *db, user_t *user);