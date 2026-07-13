#include "headers.h"
#include "server_functions.h"
#include "server_messages.h"

int create_server_socket(const char *port)
{
    int sockfd;

    int status;
    struct addrinfo hints;
    struct addrinfo *serverinfo;
    struct addrinfo *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, port, &hints, &serverinfo)) != 0)
    {
        return -1;
    }

    for (p = serverinfo; p != NULL; p = p->ai_next)
    {

        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            continue;
        }

        break;
    }

    freeaddrinfo(serverinfo);

    if (p == NULL)
    {
        return -1;
    }

    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);

    getsockname(sockfd, (struct sockaddr *)&addr, &len);

    char ip[INET6_ADDRSTRLEN];

    if (addr.ss_family == AF_INET)
    {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)&addr;

        inet_ntop(AF_INET, &ipv4->sin_addr, ip, sizeof(ip));
    }

    fprintf(stdout, "TCP server listening on %s:%s\n", ip, PORT);

    if (listen(sockfd, BACKLOG) == -1)
    {
        return -1;
    }

    return sockfd;
}

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
        if (strcmp(tokens[0], "REGISTER") == 0)
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

bool authenticate_user(char *db_path, char *login, client_session_t *session)
{
    FILE *db = fopen(db_path, "rb");

    if (db == NULL)
    {
        perror("fopen");
        return false;
    }

    rewind(db);

    user_t user;

    char *tokens[10];
    int ntokens = 0;

    char *token = strtok(login, " ");

    while (token != NULL && ntokens < 10)
    {
        tokens[ntokens++] = token;
        token = strtok(NULL, " ");
    }

    if (ntokens != 3)
    {
        fclose(db);
        return false;
    }

    while (read_user(db, &user))
    {
        if (strcmp(user.username, tokens[1]) == 0 && strcmp(user.password, tokens[2]) == 0)
        {
            strcpy(session->username, tokens[1]);
            free_user_t(&user);
            fclose(db);
            return true;
        }

        free_user_t(&user);
    }

    fclose(db);

    return false;
}

command_state_t parse_command(char *command)
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
        if (strcmp("LOGIN", tokens[0]) == 0)
        {
            return LOGIN_OK; // comando LOGIN com sucesso, pronto pra mandar pra autenticação
        }

        else if (strcmp("REGISTER", tokens[0]) == 0)
        {
            return REGISTER_OK; // comando REGISTER com sucesso, pronto pra mandar pro banco de dados
        }

        else
        {
            return UNKNOWN_3_ARGUMENTS_COMMAND; // comando não conhecido, lançe fatal error
        }
    }

    else if (ntokens == 1)
    {
        if (strcmp("LOGOUT", tokens[0]) == 0)
        {
            return LOGOUT_OK; // comando LOGOUT com sucesso
        }

        else
        {
            return UNKNOWN_1_ARGUMENTS_COMMAND; // comando não conhecido, lançe fatal error
        }
    }

    else 
    {
        return UNKNOWN_COMMAND; // comando não conhecido, lançe fatal error
    }
}

int handle_client(int client_sockfd)
{
    char recv_buffer[1024];
    char line_buffer[4096];
    size_t line_len = 0;

    client_session_t session;
    session.sockfd = client_sockfd;
    session.s_state = SESSION_WAITING_LOGIN;

   while (1)
    {
        ssize_t n = recv(client_sockfd, recv_buffer, sizeof(recv_buffer), 0);

        if (n <= 0)
            break;

        for (ssize_t i = 0; i < n; i++)
        {
            char c = recv_buffer[i];

            line_buffer[line_len++] = c;

            if (c == '\n')
            {
                line_buffer[line_len - 1] = '\0';

                fprintf(stdout, "client message: %s\n", line_buffer);

                char buffer[32];

                strcpy(buffer, line_buffer);

                command_state_t parse_status = parse_command(buffer);

                if (parse_status == LOGIN_OK && session.s_state == SESSION_WAITING_LOGIN) // comando LOGIN com estado de conexão correto
                {
                    bool auth_status = authenticate_user(DB_PATH, line_buffer, &session);

                    if (auth_status == true)
                    {
                        fprintf(stdout, "authentication succedeed\n");
                        send(client_sockfd, MESSAGE_AUTHENTICATION_SUCCESSFULL, strlen(MESSAGE_AUTHENTICATION_SUCCESSFULL), 0);
                        session.s_state = SESSION_AUTHENTICATED;
                    }

                    else if (auth_status == false)
                    {
                        fprintf(stdout, "authentication failed\n");
                        send(client_sockfd, MESSAGE_AUTHENTICATION_FAILED, strlen(MESSAGE_AUTHENTICATION_FAILED), 0);
                    }
                }

                else if (parse_status == LOGIN_OK && session.s_state == SESSION_AUTHENTICATED) // comando LOGIN com estado de conexão incorreto
                {
                    fprintf(stdout, "client already authenticated\n");
                    send(client_sockfd, CLIENT_ALREADY_AUTHENTICATED, strlen(CLIENT_ALREADY_AUTHENTICATED), 0);
                }

                else if (parse_status == REGISTER_OK && strcmp(session.username, "admin") == 0)
                {
                    char temp[32];

                    strcpy(temp, line_buffer);

                    add_user_into_table(temp);

                    fprintf(stdout, "user was successfully recorded\n");

                    send(client_sockfd, USER_INSERTED_DATABASE_SUCCESSFULLY, strlen(USER_INSERTED_DATABASE_SUCCESSFULLY), 0);
                }

                else if (parse_status == REGISTER_OK && strcmp(session.username, "admin") != 0)
                {
                    fprintf(stdout, "permission denied\n");
                    send(client_sockfd, USER_INSERTED_DATABASE_FAILED, strlen(USER_INSERTED_DATABASE_FAILED), 0);
                }

                else if (parse_status == LOGOUT_OK && session.s_state == SESSION_AUTHENTICATED) // comando LOGOUT com estado de conexão correto
                {
                    fprintf(stdout, "client logout session succeeded\n");
                    send(client_sockfd, CLIENT_LOGOUT_SUCCESSFULL, strlen(CLIENT_LOGOUT_SUCCESSFULL), 0);
                    session.s_state = SESSION_WAITING_LOGIN;
                }

                else if (parse_status == LOGOUT_OK && session.s_state == SESSION_WAITING_LOGIN) // comando LOGOUT com estado de conexão incorreto
                {
                    fprintf(stdout, "client logout failed, client not authenticated\n");
                    send(client_sockfd, CLIENT_LOGOUT_FAILED, strlen(CLIENT_LOGOUT_FAILED), 0);
                }

                else if (parse_status == UNKNOWN_3_ARGUMENTS_COMMAND) // comando com 3 argumentos não conhecido
                {
                    fprintf(stdout, "command not found\n");
                    send(client_sockfd, COMMAND_NOT_FOUND, strlen(COMMAND_NOT_FOUND), 0);
                }

                else if (parse_status == UNKNOWN_1_ARGUMENTS_COMMAND)
                {
                    fprintf(stdout, "command not found\n");
                    send(client_sockfd, COMMAND_NOT_FOUND, strlen(COMMAND_NOT_FOUND), 0);
                }

                else if (parse_status == UNKNOWN_COMMAND) // comando com argumentos não conhecido
                {
                    fprintf(stdout, "command not found\n");
                    send(client_sockfd, COMMAND_NOT_FOUND, strlen(COMMAND_NOT_FOUND), 0);
                }

                line_len = 0;
            }
        }
    }

}