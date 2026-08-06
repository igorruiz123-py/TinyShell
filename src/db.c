#include "db.h"
#include "handle_client_functions.h"
#include "headers.h"

void free_user_t(user_t *user)
{
    free(user->username);
    free(user->password);

    user->username = NULL;
    user->password = NULL;
}

void add_user_file(FILE *db, user_t *user)
{
    unsigned int username_len = strlen(user->username) + 1;
    unsigned int password_len = strlen(user->password) + 1;

    fwrite(&username_len, sizeof(unsigned int), 1, db);
    fwrite(&password_len, sizeof(unsigned int), 1, db);

    fwrite(user->username, sizeof(char), username_len, db);
    fwrite(user->password, sizeof(char), password_len, db);
}

void add_user_into_table(char *command)
{
    char *tokens[10];
    int ntokens = 0;

    char *token = strtok(command, " ");

    while (token != NULL && ntokens < 10)
    {
        tokens[ntokens++] = token;
        token = strtok(NULL, " ");
    }

    if (ntokens == 3)
    {
        if (strcmp(tokens[0], "register") == 0)
        {
            FILE *db = fopen(DB_PATH, "a+b");

            if (db == NULL)
            {
                perror("fopen");
                exit(EXIT_FAILURE);
            }

            user_t *user = malloc(sizeof(user_t));

            if (user == NULL)
            {
                perror("malloc");
                fclose(db);
                exit(EXIT_FAILURE);
            }

            user->username = malloc(strlen(tokens[1]) + 1);
            user->password = malloc(strlen(tokens[2]) + 1);

            if (user->username == NULL || user->password == NULL)
            {
                perror("malloc");
                free_user_t(user);
                fclose(db);
                exit(EXIT_FAILURE);
            }

            strcpy(user->username, tokens[1]);
            strcpy(user->password, tokens[2]);

            fseek(db, 0, SEEK_END);

            add_user_file(db, user);
            free_user_t(user);
            fclose(db);
        }
    }

}

bool read_user(FILE *db, user_t *user)
{
    unsigned int username_len;
    unsigned int password_len;
    
    if (fread(&username_len, sizeof(username_len), 1, db) != 1)
        return false;

    if (fread(&password_len, sizeof(password_len), 1, db) != 1)
        return false;

    user->username = malloc(username_len);
    user->password = malloc(password_len);

    if (user->username == NULL || user->password == NULL)
    {
        free(user->username);
        free(user->password);
        return false;
    }

    if (fread(user->username, 1, username_len, db) != username_len)
    {
        free_user_t(user);
        return false;
    }

    if (fread(user->password, 1, password_len, db) != password_len)
    {
        free_user_t(user);
        return false;
    }

    return true;
}

bool add_admin(void)
{
    
    char *admin_name = getenv("ADMINISTRATOR_USERNAME");
    char *admin_pass = getenv("ADMINISTRATOR_PASSWORD");
    char *command = getenv("INSERT_ADMINISTRADOR_USER_COMMAND");

    if (admin_name == NULL || admin_pass == NULL || command == NULL)
    {
        fprintf(stderr, "bad\n");
        exit(EXIT_FAILURE);
    }

    FILE *db = fopen(DB_PATH, "rb");

    if (db == NULL)
        return false;

    user_t user;

    while (read_user(db, &user))
    {
        if (strcmp(user.username, admin_name) == 0 && strcmp(user.password, admin_pass) == 0)
        {
            free_user_t(&user);
            fclose(db);
            return true;
        }

        free_user_t(&user);
    }

    fclose(db);

    add_user_into_table(command);

    return true;
}