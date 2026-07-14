#define INFO_AUTHENTICATION_SUCCEEDED "TinyShell server [INFO]: authentication succeeded!\n"

#define FATAL_ERROR_AUTHENTICATION_FAILED "TinyShell server [ERROR]: authentication failed!\n"

#define FATAL_ERROR_COMMAND_NOT_FOUND "TinyShell server [ERROR]: command not found!\n"

#define FATAL_ERROR_USER_ALREADY_AUTHENTICATED "TinyShell server [ERROR]: user already authenticated!\n"

#define INFO_USER_LOGOUT_SUCCESSFULL "TinyShell server [INFO]: logout succeeded!\n"

#define INFO_USER_INSERTED_DATABASE_SUCCESSFULLY "TinyShell server [INFO]: user was successfully recorded!\n"

#define FATAL_ERROR_ADMINS_ONLY "TinyShell server [ERROR]: permission denied, for administrators only!\n"

#define FATAL_ERROR_AUTHENTICATED_USERS_ONLY "TinyShell server [ERROR]: permission denied, for authenticated users only!\n"

#define INTRODUCE_TINYSHELL "TinyShell - Virtual Shell - 'help' for more details!\n\n"

#define HELP_MESSAGE \
    "TinyShell is a Virtual Shell that responds to built-in CLI commands from Netcat clients.\n\n" \
    "Available commands:\n" \
    "   HELP                            Display this help message\n" \
    "   LOGIN <username> <password>     Authenticate user\n" \
    "   LOGOUT                          Logout from the current login (only logged in clients can logout)\n\n"