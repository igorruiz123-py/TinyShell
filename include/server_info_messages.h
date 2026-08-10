#define INFO_AUTHENTICATION_SUCCEEDED "TinyShell server [INFO]: authentication succeeded!\n"

#define INFO_USER_LOGOUT_SUCCESSFULL "TinyShell server [INFO]: logout succeeded!\n"

#define INFO_USER_INSERTED_DATABASE_SUCCESSFULLY "TinyShell server [INFO]: user was successfully recorded!\n"

#define INTRODUCE_TINYSHELL "TinyShell - Virtual Shell - 'help' for more details!\n\n"

#define HELP_MESSAGE \
    "TinyShell Command Reference\n\n" \
    "Public Commands:\n\n" \
    "   help                              Show this command reference\n" \
    "   login <username> <password>       Authenticate into TinyShell\n" \
    "   about                             Display information about TinyShell\n" \
    "   version                           Display the current server version\n" \
    "   date                              Display the current server date and time\n" \
    "   echo <text>                       Print the provided text\n" \
    "   clear                             Clear the terminal screen\n" \
    "   quit                              Close the current session\n\n" \
    "Authenticated User Commands:\n\n" \
    "   asciiart <animal>                 Display an ASCII representation of an animal\n" \
    "       Available animals: cow, frog, elephant, cat, dog, bat, spider, bear, owl, whale\n\n" \
    "   reverse <text>                    Reverse the characters of the provided text\n" \
    "   uppercase <text>                  Convert text characters to uppercase\n" \
    "   lowercase <text>                  Convert text characters to lowercase\n" \
    "   length <text>                     Display the number of characters in the text\n" \
    "   sum <number> <number>             Calculate the sum of two numbers\n" \
    "   sub <number> <number>             Calculate the subtraction of two numbers\n" \
    "   mult <number> <number>            Calculate the multiplication of two numbers\n" \
    "   div <number> <number>             Calculate the division of two numbers\n\n" \
    "Administrator Commands:\n\n" \
    "   register <username> <password>    Create a new user account\n" \
    "   fetch <username>                  Check if a user exists in the database\n" \
    "   export                            Generate a report containing user information\n" \
    "   delete <username>                 Remove a user account from the database\n\n"

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

#define TINYSHELL_VERSION "TinyShell - © copyright - v1.0.1\n"

#define INFO_USER_FOUND "TinyShell server [INFO]: user active in the data base!\n"

#define INFO_REPORTED_GENERATED "TinyShell server [INFO]: report generated successfully at ../report/report.txt\n"

#define INFO_USER_DELETED_SUCCESSFULLY "TinyShell server [INFO]: user was successfully deleted from the data base!\n"