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

bool authenticate_user(char *db_path, char *login)
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
            free_user_t(&user);
            fclose(db);
            return true;
        }

        free_user_t(&user);
    }

    fclose(db);

    return false;
}

int parse_command(char *command)
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
            return 1; // comando LOGIN com sucesso, pronto pra mandar pra autenticação
        }

        else
        {
            return -1; // comando não conhecido, lançe fatal error
        }
    }

    else if (ntokens == 1)
    {
        if (strcmp("LOGOUT", tokens[0]) == 0)
        {
            return 2; // comando LOGOUT com sucesso
        }

        else
        {
            return -2; // comando não conhecido, lançe fatal error
        }
    }

    else 
    {
        return 0; // comando não conhecido, lançe fatal error
    }
}

int handle_client(int client_sockfd)
{
    char recv_buffer[1024];
    char line_buffer[4096];
    size_t line_len = 0;

    client_session_t session;
    session.sockfd = client_sockfd;
    session.state = SESSION_WAITING_LOGIN;

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

                int parse_status = parse_command(buffer);

                if (parse_status == 1 && session.state == SESSION_WAITING_LOGIN) // comando LOGIN com estado de conexão correto
                {
                    bool auth_status = authenticate_user(DB_PATH, line_buffer);

                    if (auth_status == true)
                    {
                        fprintf(stdout, "authentication succedeed\n");
                        send(client_sockfd, MESSAGE_AUTHENTICATION_SUCCESSFULL, strlen(MESSAGE_AUTHENTICATION_SUCCESSFULL), 0);
                        session.state = SESSION_AUTHENTICATED;
                    }

                    else if (auth_status == false)
                    {
                        fprintf(stdout, "authentication failed\n");
                        send(client_sockfd, MESSAGE_AUTHENTICATION_FAILED, strlen(MESSAGE_AUTHENTICATION_FAILED), 0);
                    }
                }

                else if (parse_status == 1 && session.state == SESSION_AUTHENTICATED) // comando LOGIN com estado de conexão incorreto
                {
                    fprintf(stdout, "client already authenticated\n");
                    send(client_sockfd, CLIENT_ALREADY_AUTHENTICATED, strlen(CLIENT_ALREADY_AUTHENTICATED), 0);
                }

                else if (parse_status == 2 && session.state == SESSION_AUTHENTICATED) // comando LOGOUT com estado de conexão correto
                {
                    fprintf(stdout, "client logout session succeeded\n");
                    send(client_sockfd, CLIENT_LOGOUT_SUCCESSFULL, strlen(CLIENT_LOGOUT_SUCCESSFULL), 0);
                    session.state = SESSION_WAITING_LOGIN;
                }

                else if (parse_status == 2 && session.state == SESSION_WAITING_LOGIN) // comando LOGOUT com estado de conexão incorreto
                {
                    fprintf(stdout, "client logout failed, client not authenticated\n");
                    send(client_sockfd, CLIENT_LOGOUT_FAILED, strlen(CLIENT_LOGOUT_FAILED), 0);
                }

                else if (parse_status == -1) // comando com 3 argumentos não conhecido
                {
                    fprintf(stdout, "command not found\n");
                    send(client_sockfd, COMMAND_NOT_FOUND, strlen(COMMAND_NOT_FOUND), 0);
                }

                else if (parse_status == -2)
                {
                    fprintf(stdout, "command not found\n");
                    send(client_sockfd, COMMAND_NOT_FOUND, strlen(COMMAND_NOT_FOUND), 0);
                }

                else if (parse_status == 0) // comando com argumentos não conhecido
                {
                    fprintf(stdout, "command not found\n");
                    send(client_sockfd, COMMAND_NOT_FOUND, strlen(COMMAND_NOT_FOUND), 0);
                }

                line_len = 0;
            }
        }
    }

}