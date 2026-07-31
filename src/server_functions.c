#include "headers.h"
#include "server_functions.h"

char* get_timestamp() {
    static char buffer[30];
    time_t now;
    struct tm *local;

    time(&now);
    local = localtime(&now);

    strftime(buffer, sizeof(buffer), "%d/%m/%Y - %H:%M:%S", local);

    return buffer;
}

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

    fprintf(stdout, "TinyShell server listening on %s:%s\n", ip, PORT);

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

    if (ntokens == 3 || ntokens >= 2)
    {
        if (strcmp("login", tokens[0]) == 0)
        {
            return LOGIN_OK; // comando LOGIN com sucesso, pronto pra mandar pra autenticação
        }

        else if (strcmp("register", tokens[0]) == 0)
        {
            return REGISTER_OK; // comando REGISTER com sucesso, pronto pra mandar pro banco de dados
        }

        else if (strcmp("echo", tokens[0]) == 0)
        {
            return ECHO_OK; // comando ECHO com sucesso
        }

        else
        {
            return UNKNOWN_3_ARGUMENTS_COMMAND; // comando não conhecido, lançe fatal error
        }
    }

    else if (ntokens == 1)
    {
        if (strcmp("logout", tokens[0]) == 0)
        {
            return LOGOUT_OK; // comando LOGOUT com sucesso
        }

        else if (strcmp("help", tokens[0]) == 0)
        {
            return HELP_OK; // comando HELP com sucesso
        }

        else if (strcmp("version", tokens[0]) == 0)
        {
            return VERSION_OK; // comando VERSION com sucesso
        }

        else if (strcmp("date", tokens[0]) == 0)
        {
            return DATE_OK; // comando DATE com sucesso
        }

        else if (strcmp("about", tokens[0]) == 0)
        {
            return ABOUT_OK; // comando ABOUT com sucesso
        }

        else if (strcmp("quit", tokens[0]) == 0)
        {
            return QUIT_OK; // comando QUIT com sucesso 
        }

        else if (strcmp("clear", tokens[0]) == 0)
        {
            return CLEAR_OK; //comando CLEAR com sucesso
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

void send_prompt(client_session_t *session)
{
    char prompt[64];

    if (session->s_state == SESSION_AUTHENTICATED)
    {
        snprintf(prompt, sizeof(prompt), "tinyshell@%s $ ", session->username);
    }

    else
    {
        snprintf(prompt, sizeof(prompt), "tinyshell@guest $ ");
    }

    send(session->sockfd, prompt, strlen(prompt), 0);
}

void display_tinyshell_version(int sockfd)
{
    static char version[] = "TinyShell - © copyright - v1.0.0\n";

    send(sockfd, version, strlen(version), 0);
}

void execute_echo_command(int sockfd, char *command)
{
    
    char *tokens[32];
    int ntokens = 0;

    char *token = strtok(command, " ");

    while (token != NULL && ntokens < 32)
    {
        tokens[ntokens++] = token;
        token = strtok(NULL, " ");
    }

    for (int i = 1; i < ntokens; i++)
    {
        send(sockfd, tokens[i], strlen(tokens[i]), 0);

        if (i < ntokens - 1)
        {
            send(sockfd, " ", 1, 0);
        }
    }

    send(sockfd, "\n", 1, 0);
}

void execute_date_command(int sockfd)
{
    char date[30];

    snprintf(date, sizeof(date), "%s\n", get_timestamp());

    send(sockfd, date, strlen(date), 0);
}

void execute_clear_command(int sockfd)
{
    static char clear[] = "\033[2J\033[H";

    send(sockfd, clear, strlen(clear), 0);
}