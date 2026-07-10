#include "headers.h"
#include "db_funcs.h"

//./db insert user [username] [password]
//  0   1      2       3          4 

void free_user_t(user_t *user)
{
    free(user->username);
    free(user->password);
    free(user);
}

void add_user_db(FILE *db, user_t *user)
{
    unsigned int username_len = strlen(user->username) + 1;
    unsigned int password_len = strlen(user->password) + 1;

    fwrite(&username_len, sizeof(unsigned int), 1, db);
    fwrite(&password_len, sizeof(unsigned int), 1, db);

    fwrite(user->username, sizeof(char), username_len, db);
    fwrite(user->password, sizeof(char), password_len, db);
}
