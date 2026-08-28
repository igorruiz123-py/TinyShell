#include "headers.h"
#include "handle_client_functions.h"
#include "handle_client.h"
#include "server_functions.h"
#include "server_info_messages.h"
#include "db.h"

int main(void)
{
    srand(time(NULL));
    FILE *server_log = fopen(SERVER_LOG_PATH, "a");

    if (server_log == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    setvbuf(server_log, NULL, _IONBF, 0);

    struct sockaddr_storage their_addr;
    socklen_t sin_size = sizeof(their_addr);
    char ip[INET_ADDRSTRLEN];

    int sockfd = create_server_socket(PORT, server_log);

    if (sockfd == -1)
    {
        exit(EXIT_FAILURE);
    }

    add_admin();    

    while (1)
    {
        sin_size = sizeof(their_addr);

        int client_sockfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

        client_session_t session;
        memset(&session, 0, sizeof(session));
        session.sockfd = client_sockfd;
        session.s_state = SESSION_WAITING_LOGIN;
        session.id = rand();

        if (their_addr.ss_family == AF_INET)
        {
            struct sockaddr_in *client;

            client = (struct sockaddr_in *)&their_addr;

            int client_port = ntohs(client->sin_port);

            inet_ntop(AF_INET, &client->sin_addr, ip, sizeof(ip));

            fprintf(server_log, "[%s] [INFO] Client ID='%d' connected from %s:%d\n", get_timestamp(), session.id, ip, client_port);

            send(client_sockfd, INTRODUCE_TINYSHELL, strlen(INTRODUCE_TINYSHELL), 0);

            pid_t child = fork();

            if (child < 0)
            {
                perror("fork");
                exit(EXIT_FAILURE);
            }

            if (child == 0)
            {
                int status = handle_client_interaction(client_sockfd, server_log, ip, &session);

                close(client_sockfd);

                exit(status);
            }

            close(client_sockfd);

        }
    }

    return 0;
}