#include "headers.h"
#include "handle_client_functions.h"
#include "db.h"

char* get_timestamp() {
    static char buffer[30];
    time_t now;
    struct tm *local;

    time(&now);
    local = localtime(&now);

    strftime(buffer, sizeof(buffer), "%d/%m/%Y - %H:%M:%S", local);

    return buffer;
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

void execute_help_command(int sockfd)
{
    send(sockfd, HELP_MESSAGE, strlen(HELP_MESSAGE), 0);
}

void execute_about_command(int sockfd)
{
    send(sockfd, ABOUT_MESSAGE, strlen(ABOUT_MESSAGE), 0);
}

void execute_quit_command(int sockfd)
{
    send(sockfd, QUIT_MESSAGE, strlen(QUIT_MESSAGE), 0);
}