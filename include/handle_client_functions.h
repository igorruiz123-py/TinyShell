#define HELP_MESSAGE \
    "Available commands for users not authenticated:\n\n" \
    "   help                            Display this help message\n" \
    "   login <username> <password>     Authenticate user\n" \
    "   about                           Display information about TinyShell\n" \
    "   version                         Display the current version\n" \
    "   date                            Display the server time\n" \
    "   echo <text>                     Display the inserted text\n" \
    "   clear                           Clear the current session screen\n" \
    "   quit                            logout the current session\n\n"

#define ABOUT_MESSAGE \
    "TinyShell is an open-source software project that simulates a virtual shell environment, allowing Linux Netcat clients\n" \
    "to establish TCP connections with a server and execute built-in CLI commands. Each command received is\n" \
    "interpreted by the server and processed according to its implementation, with the corresponding response\n" \
    "returned to the client.\n\n" \
    "When a client connects, TinyShell provides three permission levels: Public, Authenticated, and\n" \
    "Administrator. The set of available commands depends on the client's current permission level.\n" \
    "To view the commands available to your session, simply execute the 'help' command.\n\n" \
    "For additional information about the project, its architecture, and the communication protocol,\n" \
    "visit the official GitHub repository:\n" \
    "https://github.com/igorruiz123-py/TinyShell\n\n"

#define QUIT_MESSAGE "TinyShell server [INFO]: session closed!\n"

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
    VERSION_OK,
    DATE_OK,
    ECHO_OK,
    CLEAR_OK,
    ABOUT_OK,
    QUIT_OK,

    UNKNOWN_COMMAND,
    UNKNOWN_3_ARGUMENTS_COMMAND,
    UNKNOWN_2_ARGUMENTS_COMMAND,
    UNKNOWN_1_ARGUMENTS_COMMAND

} command_state_t;

command_state_t parse_command(char *command);

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