#include "headers.h"
#include "db_funcs.h"

//./db insert user [username] [password]
//  0   1      2       3          4 

int main(int argc, char *argv[])
{
    if (argc == 5)
    {
        if (strcmp(argv[1], "insert") == 0 && strcmp(argv[2], "user") == 0)
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

            user->username = malloc(strlen(argv[3]) + 1);
            user->password = malloc(strlen(argv[4]) + 1);

            if (user->username == NULL || user->password == NULL)
            {
                perror("malloc");
                free_user_t(user);
                fclose(db);
                exit(EXIT_FAILURE);
            }

            strcpy(user->username, argv[3]);
            strcpy(user->password, argv[4]);

            fseek(db, 0, SEEK_END);

            add_user_db(db, user);
            free_user_t(user);
            fclose(db);
            fprintf(stdout, "user inserted successfully!\n");
            return 0;
        }
    }

    else
    {
        fprintf(stderr, "error\n");
        exit(EXIT_FAILURE);
    }
}