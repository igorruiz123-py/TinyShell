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
    session_state_t state;
    char username[16];

} client_session_t;

#define DB_PATH "db/db.bin"

int create_server_socket(const char *port);

void get_username(char *login);

int handle_client(int sockfd);

int parse_command(char *command);

void free_user_t(user_t *user);

bool read_user(FILE *db, user_t *user);

bool authenticate_user(char *db_path, char *message);