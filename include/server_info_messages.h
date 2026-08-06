#define INFO_AUTHENTICATION_SUCCEEDED "TinyShell server [INFO]: authentication succeeded!\n"

#define INFO_USER_LOGOUT_SUCCESSFULL "TinyShell server [INFO]: logout succeeded!\n"

#define INFO_USER_INSERTED_DATABASE_SUCCESSFULLY "TinyShell server [INFO]: user was successfully recorded!\n"

#define INTRODUCE_TINYSHELL "TinyShell - Virtual Shell - 'help' for more details!\n\n"

#define HELP_MESSAGE \
    "Available commands for users not authenticated:\n\n" \
    "   help                            Display this help message\n" \
    "   login <username> <password>     Authenticate user\n" \
    "   about                           Display information about TinyShell\n" \
    "   version                         Display the current version\n" \
    "   date                            Display the server time\n" \
    "   echo <text>                     Display the inserted text\n" \
    "   clear                           Clear the current session screen\n" \
    "   quit                            logout the current session\n\n" \
    "Available commands for authenticated users:\n\n" \
    "   asciiart <option>                Display an ascii formatted image of an animal\n\n" \
    "       options: [cow, frog, elephant, cat, dog, bat, spider, bear, owl and whale]\n\n" \
    "   reverse <text>                   Display the reversed order of a inserted text\n" \
    "   turnupper <text>                 Display in upper case the inserted text\n" \
    "   turnlower <text>                 Display in lower case the inserted text\n" \
    "   lenght <text>                    Display the inserted text number of characters\n" \
    "   sum <number> <number>            Display the addition operation result of 2 numbers\n" \
    "   sub <number> <number>            Display the subtraction operation result of 2 numbers\n" \
    "   mult <number> <number>           Display the multiplication operation result of 2 numbers\n" \
    "   div <number> <number>            Display the division operation result of 2 numbers\n\n"

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

#define TINYSHELL_VERSION "TinyShell - © copyright - v1.0.0\n"