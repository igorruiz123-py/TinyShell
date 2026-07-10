#include "headers.h"
#include "server_functions.h"

int create_server_socket(const char *port)
{
    int sockfd;

    int status;
    struct addrinfo hints;
    struct addrinfo *serverinfo;
    struct addrinfo *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, port, &hints, &serverinfo)) != 0)
    {
        return -1;
    }

    for (p = serverinfo; p != NULL; p = p->ai_next)
    {

        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            continue;
        }

        break;
    }

    freeaddrinfo(serverinfo);

    if (p == NULL)
    {
        return -1;
    }

    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);

    getsockname(sockfd, (struct sockaddr *)&addr, &len);

    char ip[INET6_ADDRSTRLEN];

    if (addr.ss_family == AF_INET)
    {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)&addr;

        inet_ntop(AF_INET, &ipv4->sin_addr, ip, sizeof(ip));
    }

    fprintf(stdout, "TCP server listening on %s:%s\n", ip, PORT);

    if (listen(sockfd, BACKLOG) == -1)
    {
        return -1;
    }

    return sockfd;
}

int handle_client(int sockfd)
{
    char recv_buffer[1024];
    char line_buffer[4096];
    size_t line_len = 0;
    const char *message = "TCP server: messaged received!\n";

   while (1)
    {
        ssize_t n = recv(sockfd, recv_buffer, sizeof(recv_buffer), 0);

        if (n <= 0)
            break;

        for (ssize_t i = 0; i < n; i++)
        {
            char c = recv_buffer[i];

            line_buffer[line_len++] = c;

            if (c == '\n')
            {
                line_buffer[line_len - 1] = '\0';

                printf("client message: %s\n", line_buffer);

                send(sockfd, message, strlen(message), 0);

                if (strcmp(line_buffer, "quit") == 0)
                {
                    printf("client disconnected\n");
                    return 0;
                }

                line_len = 0;
            }
        }
    }

}