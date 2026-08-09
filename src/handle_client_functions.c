#include "headers.h"
#include "handle_client_functions.h"
#include "db.h"
#include "asciiart.h"
#include "server_error_messages.h"
#include "server_info_messages.h"

char *parse_subcommand(char *command)
{
    char *tokens[10];
    int ntokens = 0;

    char *token = strtok(command, " ");

    while (token != NULL && ntokens < 10)
    {
        tokens[ntokens++] = token;
        token = strtok(NULL, " ");
    }

    return tokens[1];
}

bool parse_float(const char *str, float *num)
{
    char *end;
    errno = 0;

    float value = strtof(str, &end);

    if (end == str){
        return false;
    }

    if (*end != '\0'){
        return false;
    }

    if (errno == ERANGE ){
        return false;
    }

    *num = value;

    return true;
}

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

        if (ntokens == 2)
        {
            if (strcmp("asciiart", tokens[0]) == 0)
            {
                return ASCIIART_OK; // comando ASCIIART com sucesso
            }

            else if (strcmp("reverse", tokens[0]) == 0)
            {
                return REVERSE_OK; // comando REVERSE com sucesso
            }

            else if (strcmp("turnupper", tokens[0]) == 0)
            {
                return TURNUPPER_OK;
            }

            else if (strcmp("turnlower", tokens[0]) == 0)
            {
                return TURNLOWER_OK;
            }

            else if (strcmp("lenght", tokens[0]) == 0)
            {
                return LENGHT_OK;
            }

            else if (strcmp("echo", tokens[0]) == 0)
            {
                return ECHO_OK;
            }

            else if (strcmp("fetch", tokens[0]) == 0)
            {
                return FETCH_OK;
            }

            else if (strcmp("delete", tokens[0]) == 0)
            {
                return DELETE_OK;
            }

            else
            {
                return UNKNOWN_2_ARGUMENTS_COMMAND;
            }
        }

        else if (ntokens == 3)
        {
            if (strcmp("sum", tokens[0]) == 0)
            {
                return SUM_OK;
            }

            else if (strcmp("sub", tokens[0]) == 0)
            {
                return SUB_OK;
            }

            else if (strcmp("mult", tokens[0]) == 0)
            {
                return MULT_OK;
            }

            else if (strcmp("div", tokens[0]) == 0)
            {
                return DIV_OK;
            }

            else if (strcmp("login", tokens[0]) == 0)
            {
                return LOGIN_OK; // comando LOGIN com sucesso, pronto pra mandar pra autenticação
            }

            else if (strcmp("register", tokens[0]) == 0)
            {
                return REGISTER_OK; // comando REGISTER com sucesso, pronto pra mandar pro banco de dados
            }

            else if (strcmp("echo", tokens[0]) == 0)
            {
                return ECHO_OK;
            }

            else
            {
                return UNKNOWN_3_ARGUMENTS_COMMAND;
            }
        }

        if (strcmp("echo", tokens[0]) == 0)
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

        else if (strcmp("export", tokens[0]) == 0)
        {
            return EXPORT_OK;
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
    send(sockfd, TINYSHELL_VERSION, strlen(TINYSHELL_VERSION), 0);
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

void execute_asciiart_command(int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session)
{
    char *subcommand = parse_subcommand(command);

    if (strcmp("cow", subcommand) == 0)
    {
        fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu COW ASCIIART OUTPUT SENT\n", get_timestamp(), client_ip, session->username, sizeof(COW_ASCII));
        send(sockfd, "\n", 1, 0);
        send(sockfd, COW_ASCII, strlen(COW_ASCII), 0);
        send(sockfd, "\n", 1, 0);
    }

    else if (strcmp("frog", subcommand) == 0)
    {
        fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu FROG ASCIIART OUTPUT SENT\n", get_timestamp(), client_ip, session->username, sizeof(FROG_ASCII));
        send(sockfd, "\n", 1, 0);
        send(sockfd, FROG_ASCII, strlen(FROG_ASCII), 0);
        send(sockfd, "\n", 1, 0);
    }

    else if (strcmp("elephant", subcommand) == 0)
    {
        fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu ELEPHANT ASCIIART OUTPUT SENT\n", get_timestamp(), client_ip, session->username, sizeof(ELEPHANT_ASCII));
        send(sockfd, "\n", 1, 0);
        send(sockfd, ELEPHANT_ASCII, strlen(ELEPHANT_ASCII), 0);
        send(sockfd, "\n", 1, 0);
    }

    else if (strcmp("cat", subcommand) == 0)
    {
        fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu CAT ASCIIART OUTPUT SENT\n", get_timestamp(), client_ip, session->username, sizeof(CAT_ASCII));
        send(sockfd, "\n", 1, 0);
        send(sockfd, CAT_ASCII, strlen(CAT_ASCII), 0);
        send(sockfd, "\n", 1, 0);
    }

    else if (strcmp("dog", subcommand) == 0)
    {
        fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu DOG ASCIIART OUTPUT SENT\n", get_timestamp(), client_ip, session->username, sizeof(DOG_ASCII));
        send(sockfd, "\n", 1, 0);
        send(sockfd, DOG_ASCII, strlen(DOG_ASCII), 0);
        send(sockfd, "\n", 1, 0);
    }

    else if (strcmp("bat", subcommand) == 0)
    {
        fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu BAT ASCIIART OUTPUT SENT\n", get_timestamp(), client_ip, session->username, sizeof(BAT_ASCII));
        send(sockfd, "\n", 1, 0);
        send(sockfd, BAT_ASCII, strlen(BAT_ASCII), 0);
        send(sockfd, "\n", 1, 0);
    }

    else if (strcmp("spider", subcommand) == 0)
    {
        fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu SPIDER ASCIIART OUTPUT SENT\n", get_timestamp(), client_ip, session->username, sizeof(SPIDER_ASCII));
        send(sockfd, "\n", 1, 0);
        send(sockfd, SPIDER_ASCII, strlen(SPIDER_ASCII), 0);
        send(sockfd, "\n", 1, 0);
    }

    else if (strcmp("bear", subcommand) == 0)
    {
        fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu BEAR ASCIIART OUTPUT SENT\n", get_timestamp(), client_ip, session->username, sizeof(BEAR_ASCII));
        send(sockfd, "\n", 1, 0);
        send(sockfd, BEAR_ASCII, strlen(BEAR_ASCII), 0);
        send(sockfd, "\n", 1, 0);
    }

    else if (strcmp("owl", subcommand) == 0)
    {
        fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu OWL ASCIIART OUTPUT SENT\n", get_timestamp(), client_ip, session->username, sizeof(OWL_ASCII));
        send(sockfd, "\n", 1, 0);
        send(sockfd, OWL_ASCII, strlen(OWL_ASCII), 0);
        send(sockfd, "\n", 1, 0);
    }

    else if (strcmp("whale", subcommand) == 0)
    {
        fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu WHALE ASCIIART OUTPUT SENT\n", get_timestamp(), client_ip, session->username, sizeof(WHALE_ASCII));
        send(sockfd, "\n", 1, 0);
        send(sockfd, WHALE_ASCII, strlen(WHALE_ASCII), 0);
        send(sockfd, "\n", 1, 0);
    }

    else
    {
        fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' USER='%s' BYTES=%zu UNKNOWN OPTION FOR ASCIIART\n", get_timestamp(), client_ip, session->username, sizeof(FATAL_ERROR_UNKNOWN_OPTION));
        send(sockfd, FATAL_ERROR_UNKNOWN_OPTION, strlen(FATAL_ERROR_UNKNOWN_OPTION), 0);
    }
}

void execute_reverse_command(int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session)
{
    char *subcommand = parse_subcommand(command);

    int len = strlen(subcommand);

    int start = 0;
    int end = len - 1;

    while (start < end){

        char temp = subcommand[start];
        subcommand[start] = subcommand[end];
        subcommand[end] = temp;

        end--;
        start++;
    }

    fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu REVERSE OUTPUT SENT\n", get_timestamp(), client_ip, session->username, strlen(subcommand));
    send(sockfd, subcommand, strlen(subcommand), 0);
    send(sockfd, "\n", 1, 0);

}

void execute_turnupper_command(int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session)
{
    char *subcommand = parse_subcommand(command);

    for (int i = 0; subcommand[i] != '\0'; i++)
    {
        subcommand[i] = toupper(subcommand[i]);
    }

    fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu TURNUPPER OUTPUT SENT\n", get_timestamp(), client_ip, session->username, sizeof(subcommand));
    send(sockfd, subcommand, strlen(subcommand), 0);
    send(sockfd, "\n", 1, 0);
}

void execute_turnlower_command(int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session)
{
    char *subcommand = parse_subcommand(command);

    for (int i = 0; subcommand[i] != '\0'; i++)
    {
        subcommand[i] = tolower(subcommand[i]);
    }

    fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu TURNLOWER OUTPUT SENT\n", get_timestamp(), client_ip, session->username, sizeof(subcommand));
    send(sockfd, subcommand, strlen(subcommand), 0);
    send(sockfd, "\n", 1, 0);
}

void execute_lenght_command(int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session)
{
    char *subcommand = parse_subcommand(command);

    int len = strlen(subcommand);
    char temp[16];
    snprintf(temp, sizeof(temp), "%d", len);

    fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu LENGHT OUTPUT SENT\n", get_timestamp(), client_ip, session->username, sizeof(temp));
    send(sockfd, temp, strlen(temp), 0);
    send(sockfd, "\n", 1, 0);
}

void execute_sum_command(int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session)
{
    char *tokens[10];
    int ntokens = 0;

    char *token = strtok(command, " ");

    while (token != NULL && ntokens < 10)
    {
        tokens[ntokens++] = token;
        token = strtok(NULL, " ");
    }

    float x, y;

    if (!parse_float(tokens[1], &x) || !parse_float(tokens[2], &y))
    {
        fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' USER='%s' BYTES=%zu ONLY NUMBERS ALLOWED\n", get_timestamp(), client_ip, session->username, sizeof(FATAL_ERROR_NOT_NUMBER));
        send(sockfd, FATAL_ERROR_NOT_NUMBER, strlen(FATAL_ERROR_NOT_NUMBER), 0);
        return;
    }

    float sum = x + y;

    char temp[16];

    snprintf(temp, sizeof(temp), "%.2f", sum);

    fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu SUM OUTPUT SENT\n", get_timestamp(), client_ip, session->username, strlen(temp));
    send(sockfd, temp, strlen(temp), 0);
    send(sockfd, "\n", 1, 0);
}

void execute_sub_command(int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session)
{
    char *tokens[10];
    int ntokens = 0;

    char *token = strtok(command, " ");

    while (token != NULL && ntokens < 10)
    {
        tokens[ntokens++] = token;
        token = strtok(NULL, " ");
    }

    float x, y;

    if (!parse_float(tokens[1], &x) || !parse_float(tokens[2], &y))
    {
        fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' USER='%s' BYTES=%zu ONLY NUMBERS ALLOWED\n", get_timestamp(), client_ip, session->username, sizeof(FATAL_ERROR_NOT_NUMBER));
        send(sockfd, FATAL_ERROR_NOT_NUMBER, strlen(FATAL_ERROR_NOT_NUMBER), 0);
        return;
    }

    float sub = x - y;

    char temp[16];

    snprintf(temp, sizeof(temp), "%.2f", sub);

    fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu SUB OUTPUT SENT\n", get_timestamp(), client_ip, session->username, strlen(temp));
    send(sockfd, temp, strlen(temp), 0);
    send(sockfd, "\n", 1, 0);
}

void execute_mult_command(int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session)
{
    char *tokens[10];
    int ntokens = 0;

    char *token = strtok(command, " ");

    while (token != NULL && ntokens < 10)
    {
        tokens[ntokens++] = token;
        token = strtok(NULL, " ");
    }

    float x, y;

    if (!parse_float(tokens[1], &x) || !parse_float(tokens[2], &y))
    {
        fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' USER='%s' BYTES=%zu ONLY NUMBERS ALLOWED\n", get_timestamp(), client_ip, session->username, sizeof(FATAL_ERROR_NOT_NUMBER));
        send(sockfd, FATAL_ERROR_NOT_NUMBER, strlen(FATAL_ERROR_NOT_NUMBER), 0);
        return;
    }

    float mult = x * y;

    char temp[16];

    snprintf(temp, sizeof(temp), "%.2f", mult);

    fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu MULT OUTPUT SENT\n", get_timestamp(), client_ip, session->username, strlen(temp));
    send(sockfd, temp, strlen(temp), 0);
    send(sockfd, "\n", 1, 0);
}

void execute_div_command(int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session)
{
    char *tokens[10];
    int ntokens = 0;

    char *token = strtok(command, " ");

    while (token != NULL && ntokens < 10)
    {
        tokens[ntokens++] = token;
        token = strtok(NULL, " ");
    }

    float x, y;

    if (!parse_float(tokens[1], &x) || !parse_float(tokens[2], &y))
    {
        fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' USER='%s' BYTES=%zu ONLY NUMBERS ALLOWED\n", get_timestamp(), client_ip, session->username, sizeof(FATAL_ERROR_NOT_NUMBER));
        send(sockfd, FATAL_ERROR_NOT_NUMBER, strlen(FATAL_ERROR_NOT_NUMBER), 0);
        return;
    }

    if (y == 0)
    {
        fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' USER='%s' BYTES=%zu NOT POSSIBLE TO DIVIDE BY ZERO\n", get_timestamp(), client_ip, session->username, sizeof(FATAL_ERROR_DIVIDE_BY_ZERO));
        send(sockfd, FATAL_ERROR_DIVIDE_BY_ZERO, strlen(FATAL_ERROR_DIVIDE_BY_ZERO), 0);
        return;
    }

    float div = x / y;

    char temp[16];

    snprintf(temp, sizeof(temp), "%.2f", div);

    fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu DIV OUTPUT SENT\n", get_timestamp(), client_ip, session->username, strlen(temp));
    send(sockfd, temp, strlen(temp), 0);
    send(sockfd, "\n", 1, 0);
}

void execute_fetch_command(const char *db_path, int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session)
{
    FILE *db = fopen(db_path, "rb");

    if (db == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    rewind(db);

    user_t user;

    bool check = false;

    char *subcommand = parse_subcommand(command);

    while (read_user(db, &user))
    {
        if (strcmp(user.username, subcommand) == 0)
        {
            check = true;
            free_user_t(&user);
            break;
        }

        free_user_t(&user);
    }

    fclose(db);

    if (check == true)
    {
        fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu USER FOUND SUCCESSFULLY\n", get_timestamp(), client_ip, session->username, sizeof(INFO_USER_FOUND));
        send(sockfd, INFO_USER_FOUND, strlen(INFO_USER_FOUND), 0);
    }

    else
    {
        fprintf(server_log, "[%s] [WARN] [SEND] IP='%s' USER='%s' BYTES=%zu USER NOT FOUND\n", get_timestamp(), client_ip, session->username, sizeof(FATAL_ERROR_USER_NOT_FOUND));
        send(sockfd, FATAL_ERROR_USER_NOT_FOUND, strlen(FATAL_ERROR_USER_NOT_FOUND), 0);
    }
}

void execute_export_command(const char *report_path, const char *db_path, int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session)
{
    FILE *db = fopen(db_path, "rb");

    FILE *report = fopen(report_path, "w");

    if (db == NULL || report == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    rewind(db);
    rewind(report);

    user_t user;

    fprintf(report, "REPORT GENERATED AT %s\n\n", get_timestamp());

    while (read_user(db, &user))
    {
        fprintf(report, "Username: %s\n", user.username);
        fprintf(report, "Password: %s\n", user.password);
        fprintf(report, "---------------\n");

        free_user_t(&user);
    }

    fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu REPORT GENERATED SUCCESSFULLY\n", get_timestamp(), client_ip, session->username, sizeof(INFO_REPORTED_GENERATED));
    send(sockfd, INFO_REPORTED_GENERATED, strlen(INFO_REPORTED_GENERATED), 0);

    fclose(db);
    fclose(report);
}

void execute_delete_command(const char *db_temp_path, const char *db_path, int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session)
{
    char *username = parse_subcommand(command);

    FILE *db = fopen(db_path, "rb");

    FILE *temp = fopen(db_temp_path, "wb");

    if (db == NULL || temp == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    user_t user;
    bool found = false;

    while(read_user(db, &user))
    {
        if (strcmp(user.username, username) == 0)
        {
            found = true;
        }

        else
        {
            add_user_file(temp, &user);
        }

        free_user_t(&user);
    }

    fclose(db);
    fclose(temp);

    if (found == false)
    {
        fprintf(server_log, "[%s] [WARN] [SEND] IP='%s' USER='%s' BYTES=%zu USER NOT FOUND\n", get_timestamp(), client_ip, session->username, sizeof(FATAL_ERROR_USER_NOT_FOUND));
        send(sockfd, FATAL_ERROR_USER_NOT_FOUND, strlen(FATAL_ERROR_USER_NOT_FOUND), 0);
        remove(db_temp_path);
        return;
    }

    remove(db_path);
    rename(db_temp_path, db_path);

    fprintf(server_log, "[%s] [INFO] [SEND] IP='%s' USER='%s' BYTES=%zu USER DELETED FROM THE DATA BASE SUCCESSFULLY\n", get_timestamp(), client_ip, session->username, sizeof(INFO_USER_DELETED_SUCCESSFULLY));
    send(sockfd, INFO_USER_DELETED_SUCCESSFULLY, strlen(INFO_USER_DELETED_SUCCESSFULLY), 0);

}