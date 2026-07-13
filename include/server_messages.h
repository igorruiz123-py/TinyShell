#define MESSAGE_AUTHENTICATION_SUCCESSFULL "TinyShell server: authentication succeeded!\n"
#define MESSAGE_AUTHENTICATION_FAILED "TinyShell server: fatal error: authentication failed!\n"
#define COMMAND_NOT_FOUND "TinyShell server: fatal error: command not found!\n"
#define CLIENT_ALREADY_AUTHENTICATED "TinyShell server: fatal error: user authenticated already!\n"
#define CLIENT_LOGOUT_SUCCESSFULL "TinyShell server: logout succeeded!\n"
#define CLIENT_LOGOUT_FAILED "TinyShell server: fatal error: logout failed, client not authenticated!\n"
#define USER_INSERTED_DATABASE_SUCCESSFULLY "TinyShell server: user was successfully recorded!\n"
#define USER_INSERTED_DATABASE_FAILED "TinyShell server: fatal error: permission denied!\n"
#define INTRODUCE_TINYSHELL "TinyShell v1.0.0 - Virtual Shell - 'HELP' for more details!\n\n"
#define HELP_MESSAGE \
    "TinyShell is a Virtual Shell that responds to built-in CLI commands from Netcat clients.\n\n" \
    "Available commands:\n" \
    "   HELP                            Display this help message\n" \
    "   LOGIN <username> <password>     Authenticate user\n" \
    "   LOGOUT                          Logout from the current login (only logged in clients can logout)\n\n"