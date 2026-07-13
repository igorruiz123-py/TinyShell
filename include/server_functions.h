typedef struct
{
    char *username;
    char *password;
    
} user_t;

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

} client_session_t;

typedef enum
{
    LOGIN_OK,
    REGISTER_OK,
    LOGOUT_OK,
    HELP_OK,

    UNKNOWN_COMMAND,
    UNKNOWN_3_ARGUMENTS_COMMAND,
    UNKNOWN_2_ARGUMENTS_COMMAND,
    UNKNOWN_1_ARGUMENTS_COMMAND

} command_state_t;


#define DB_PATH "db/db.bin"

int create_server_socket(const char *port);

int handle_client(int sockfd);

command_state_t parse_command(char *command);

void free_user_t(user_t *user);

bool read_user(FILE *db, user_t *user);

bool authenticate_user(char *db_path, char *message, client_session_t *session);

void add_user_file(FILE *db, user_t *user);

void add_user_into_table(char *command);

void send_prompt(client_session_t *session);