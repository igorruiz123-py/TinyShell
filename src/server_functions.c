#include "headers.h"
#include "server_functions.h"
#include "handle_client_functions.h"

int create_server_socket(const char *port, FILE *server_log)
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

    fprintf(server_log, "[%s] [INFO] Server initialized.\n", get_timestamp());
    fprintf(server_log, "[%s] [INFO] Server listening on %s:%s\n", get_timestamp(), ip, PORT);

    if (listen(sockfd, BACKLOG) == -1)
    {
        return -1;
    }

    return sockfd;
}
