#include "headers.h"
#include "server_functions.h"
#include "server_messages.h"

int handle_client_interaction(int client_sockfd)
{
    char recv_buffer[1024];
    char line_buffer[4096];
    size_t line_len = 0;

    client_session_t session;
    session.sockfd = client_sockfd;
    session.s_state = SESSION_WAITING_LOGIN;

   while (1)
    {
        ssize_t n = recv(client_sockfd, recv_buffer, sizeof(recv_buffer), 0);

        if (n <= 0)
            break;

        for (ssize_t i = 0; i < n; i++)
        {
            char c = recv_buffer[i];

            line_buffer[line_len++] = c;

            if (c == '\n')
            {
                line_buffer[line_len - 1] = '\0';

                fprintf(stdout, "client message: %s\n", line_buffer);

                char buffer[32];

                strcpy(buffer, line_buffer);

                command_state_t parse_status = parse_command(buffer);

                if (parse_status == LOGIN_OK && session.s_state == SESSION_WAITING_LOGIN) // comando LOGIN com estado de conexão correto
                {
                    bool auth_status = authenticate_user(DB_PATH, line_buffer, &session);

                    if (auth_status == true)
                    {
                        fprintf(stdout, "authentication succedeed\n");
                        send(client_sockfd, MESSAGE_AUTHENTICATION_SUCCESSFULL, strlen(MESSAGE_AUTHENTICATION_SUCCESSFULL), 0);
                        session.s_state = SESSION_AUTHENTICATED;
                    }

                    else if (auth_status == false)
                    {
                        fprintf(stdout, "authentication failed\n");
                        send(client_sockfd, MESSAGE_AUTHENTICATION_FAILED, strlen(MESSAGE_AUTHENTICATION_FAILED), 0);
                    }
                }

                else if (parse_status == LOGIN_OK && session.s_state == SESSION_AUTHENTICATED) // comando LOGIN com estado de conexão incorreto
                {
                    fprintf(stdout, "client already authenticated\n");
                    send(client_sockfd, CLIENT_ALREADY_AUTHENTICATED, strlen(CLIENT_ALREADY_AUTHENTICATED), 0);
                }

                else if (parse_status == REGISTER_OK && strcmp(session.username, "admin") == 0) // comando REGISTER com estado de conexão correto
                {
                    char temp[32];

                    strcpy(temp, line_buffer);

                    add_user_into_table(temp);

                    fprintf(stdout, "user was successfully recorded\n");

                    send(client_sockfd, USER_INSERTED_DATABASE_SUCCESSFULLY, strlen(USER_INSERTED_DATABASE_SUCCESSFULLY), 0);
                }

                else if (parse_status == REGISTER_OK && strcmp(session.username, "admin") != 0) // comando REGISTER com estado de conexão incorreto
                {
                    fprintf(stdout, "permission denied\n");
                    send(client_sockfd, USER_INSERTED_DATABASE_FAILED, strlen(USER_INSERTED_DATABASE_FAILED), 0);
                }

                else if (parse_status == LOGOUT_OK && session.s_state == SESSION_AUTHENTICATED) // comando LOGOUT com estado de conexão correto
                {
                    fprintf(stdout, "client logout session succeeded\n");
                    send(client_sockfd, CLIENT_LOGOUT_SUCCESSFULL, strlen(CLIENT_LOGOUT_SUCCESSFULL), 0);
                    session.s_state = SESSION_WAITING_LOGIN;
                }

                else if (parse_status == LOGOUT_OK && session.s_state == SESSION_WAITING_LOGIN) // comando LOGOUT com estado de conexão incorreto
                {
                    fprintf(stdout, "client logout failed, client not authenticated\n");
                    send(client_sockfd, CLIENT_LOGOUT_FAILED, strlen(CLIENT_LOGOUT_FAILED), 0);
                }

                else if (parse_status == UNKNOWN_3_ARGUMENTS_COMMAND) // comando com 3 argumentos não conhecido
                {
                    fprintf(stdout, "command not found\n");
                    send(client_sockfd, COMMAND_NOT_FOUND, strlen(COMMAND_NOT_FOUND), 0);
                }

                else if (parse_status == UNKNOWN_1_ARGUMENTS_COMMAND)
                {
                    fprintf(stdout, "command not found\n");
                    send(client_sockfd, COMMAND_NOT_FOUND, strlen(COMMAND_NOT_FOUND), 0);
                }

                else if (parse_status == UNKNOWN_COMMAND) // comando com argumentos não conhecido
                {
                    fprintf(stdout, "command not found\n");
                    send(client_sockfd, COMMAND_NOT_FOUND, strlen(COMMAND_NOT_FOUND), 0);
                }

                line_len = 0;
            }
        }
    }

}