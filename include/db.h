#include <stdio.h>
#include <stdbool.h>

typedef struct
{
    char *username;
    char *password;
    
} user_t;

#define DB_PATH "db/db.bin"
#define DB_TEMP_PATH "db/temp.bin"

void free_user_t(user_t *user);

bool read_user(FILE *db, user_t *user);

void add_user_file(FILE *db, user_t *user);

void add_user_into_table(char *command);

bool add_admin(void);