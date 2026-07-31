#define INFO_AUTHENTICATION_SUCCEEDED "TinyShell server [INFO]: authentication succeeded!\n"

#define FATAL_ERROR_AUTHENTICATION_FAILED "TinyShell server [ERROR]: authentication failed!\n"

#define FATAL_ERROR_COMMAND_NOT_FOUND "TinyShell server [ERROR]: command not found!\n"

#define FATAL_ERROR_USER_ALREADY_AUTHENTICATED "TinyShell server [ERROR]: user already authenticated!\n"

#define INFO_USER_LOGOUT_SUCCESSFULL "TinyShell server [INFO]: logout succeeded!\n"

#define INFO_USER_INSERTED_DATABASE_SUCCESSFULLY "TinyShell server [INFO]: user was successfully recorded!\n"

#define FATAL_ERROR_ADMINS_ONLY "TinyShell server [ERROR]: permission denied, for administrators only!\n"

#define FATAL_ERROR_AUTHENTICATED_USERS_ONLY "TinyShell server [ERROR]: permission denied, for authenticated users only!\n"

#define INTRODUCE_TINYSHELL "TinyShell - Virtual Shell - 'help' for more details!\n\n"

#define QUIT_MESSAGE "TinyShell server [INFO]: session closed!\n"

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
