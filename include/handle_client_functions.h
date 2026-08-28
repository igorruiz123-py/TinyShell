typedef enum 
{
    SESSION_WAITING_LOGIN,
    SESSION_AUTHENTICATED,
    SESSION_CLOSED

} session_state_t;

typedef struct
{
    int sockfd;
    session_state_t s_state;
    char username[16];
    int id;

} client_session_t;

typedef enum
{
    LOGIN_OK,
    REGISTER_OK,
    LOGOUT_OK,
    HELP_OK,
    VERSION_OK,
    DATE_OK,
    ECHO_OK,
    CLEAR_OK,
    ABOUT_OK,
    QUIT_OK,
    ASCIIART_OK,
    REVERSE_OK,
    TURNUPPER_OK,
    TURNLOWER_OK,
    LENGHT_OK,
    SUM_OK,
    SUB_OK,
    MULT_OK,
    DIV_OK,
    FETCH_OK,
    EXPORT_OK,
    DELETE_OK,
    UNKNOWN_COMMAND,
    UNKNOWN_3_ARGUMENTS_COMMAND,
    UNKNOWN_2_ARGUMENTS_COMMAND,
    UNKNOWN_1_ARGUMENTS_COMMAND

} command_state_t;

#define REPORT_PATH "report/report.txt"

command_state_t parse_command(char *command);

char *parse_subcommand(char *command);

bool parse_float(const char *str, float *num);

bool authenticate_user(char *db_path, char *message, client_session_t *session);

void send_prompt(client_session_t *session);

void display_tinyshell_version(int sockfd);

void execute_echo_command(int sockfd, char *command);

char* get_timestamp();

void execute_date_command(int sockfd);

void execute_clear_command(int sockfd);

void execute_help_command(int sockfd);

void execute_about_command(int sockfd);

void execute_quit_command(int sockfd);

void execute_asciiart_command(int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session);

void execute_reverse_command(int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session);

void execute_turnupper_command(int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session);

void execute_turnlower_command(int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session);

void execute_lenght_command(int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session);

void execute_sum_command(int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session);

void execute_sub_command(int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session);

void execute_mult_command(int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session);

void execute_div_command(int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session);

void execute_fetch_command(const char *db_path, int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session);

void execute_export_command(const char *report_path, const char *db_path, int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session);

void execute_delete_command(const char *db_temp_path, const char *db_path, int sockfd, char *command, FILE *server_log, char *client_ip, client_session_t *session);