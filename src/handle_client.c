#include "headers.h"
#include "server_functions.h"
#include "server_messages.h"

int handle_client_interaction(int client_sockfd)
{
    char recv_buffer[1024];
    char line_buffer[4096];
    size_t line_len = 0;

    client_session_t session;
    memset(&session, 0, sizeof(session));
    session.sockfd = client_sockfd;
    session.s_state = SESSION_WAITING_LOGIN;

    send_prompt(&session);

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

                char buffer[4096];

                strcpy(buffer, line_buffer);

                command_state_t parse_status = parse_command(buffer);

                if (parse_status == HELP_OK)
                {
                    fprintf(stdout, "help message sent\n");
                    send(client_sockfd, HELP_MESSAGE, strlen(HELP_MESSAGE), 0);
                    send_prompt(&session);
                }

                else if (parse_status == VERSION_OK)
                {
                    fprintf(stdout, "TinyShell version sent\n");
                    display_tinyshell_version(client_sockfd);
                    send_prompt(&session);
                }

                else if (parse_status == ECHO_OK)
                {
                    fprintf(stdout, "echo command sent\n");
                    execute_echo_command(client_sockfd, line_buffer);
                    send_prompt(&session);
                }

                else if (parse_status == DATE_OK)
                {
                    fprintf(stdout, "date command sent\n");
                    execute_date_command(client_sockfd);
                    send_prompt(&session);
                }

                else if (parse_status == ABOUT_OK)
                {
                    fprintf(stdout, "about command sent\n");
                    send(client_sockfd, ABOUT_MESSAGE, strlen(ABOUT_MESSAGE), 0);
                    send_prompt(&session);
                }

                else if (parse_status == QUIT_OK)
                {
                    fprintf(stdout, "quit command sent\n");
                    send(client_sockfd, QUIT_MESSAGE, strlen(QUIT_MESSAGE), 0);
                    close(client_sockfd);
                    return 0;
                }

                else if (parse_status == CLEAR_OK)
                {
                    fprintf(stdout, "clear command sent\n");
                    execute_clear_command(client_sockfd);
                    send_prompt(&session);
                }    

                else if (parse_status == LOGIN_OK && session.s_state == SESSION_WAITING_LOGIN) // comando LOGIN com estado de conexão correto
                {
                    bool auth_status = authenticate_user(DB_PATH, line_buffer, &session);

                    if (auth_status == true)
                    {
                        fprintf(stdout, "authentication succedeed\n");
                        send(client_sockfd, INFO_AUTHENTICATION_SUCCEEDED, strlen(INFO_AUTHENTICATION_SUCCEEDED), 0);
                        session.s_state = SESSION_AUTHENTICATED;
                        send_prompt(&session);
                    }

                    else if (auth_status == false)
                    {
                        fprintf(stdout, "authentication failed\n");
                        send(client_sockfd, FATAL_ERROR_AUTHENTICATION_FAILED, strlen(FATAL_ERROR_AUTHENTICATION_FAILED), 0);
                        send_prompt(&session);
                    }
                }

                else if (parse_status == LOGIN_OK && session.s_state == SESSION_AUTHENTICATED) // comando LOGIN com estado de conexão incorreto
                {
                    fprintf(stdout, "client already authenticated\n");
                    send(client_sockfd, FATAL_ERROR_USER_ALREADY_AUTHENTICATED, strlen(FATAL_ERROR_USER_ALREADY_AUTHENTICATED), 0);
                    send_prompt(&session);
                }

                else if (parse_status == REGISTER_OK && strcmp(session.username, "admin") == 0 && session.s_state == SESSION_AUTHENTICATED) // comando REGISTER com estado de conexão correto
                {
                    char temp[32];

                    strcpy(temp, line_buffer);

                    add_user_into_table(temp);

                    fprintf(stdout, "user was successfully recorded\n");

                    send(client_sockfd, INFO_USER_INSERTED_DATABASE_SUCCESSFULLY, strlen(INFO_USER_INSERTED_DATABASE_SUCCESSFULLY), 0);
                    send_prompt(&session);
                }

                else if (parse_status == REGISTER_OK && strcmp(session.username, "admin") != 0) // comando REGISTER com estado de conexão incorreto
                {
                    fprintf(stdout, "permission denied\n");
                    send(client_sockfd, FATAL_ERROR_ADMINS_ONLY, strlen(FATAL_ERROR_ADMINS_ONLY), 0);
                    send_prompt(&session);
                }

                else if (parse_status == LOGOUT_OK && session.s_state == SESSION_AUTHENTICATED) // comando LOGOUT com estado de conexão correto
                {
                    fprintf(stdout, "client logout session succeeded\n");
                    send(client_sockfd, INFO_USER_LOGOUT_SUCCESSFULL, strlen(INFO_USER_LOGOUT_SUCCESSFULL), 0);
                    session.s_state = SESSION_WAITING_LOGIN;
                    session.username[0] = '\0';
                    send_prompt(&session);
                }

                else if (parse_status == LOGOUT_OK && session.s_state == SESSION_WAITING_LOGIN) // comando LOGOUT com estado de conexão incorreto
                {
                    fprintf(stdout, "client logout failed, client not authenticated\n");
                    send(client_sockfd, FATAL_ERROR_AUTHENTICATED_USERS_ONLY, strlen(FATAL_ERROR_AUTHENTICATED_USERS_ONLY), 0);
                    send_prompt(&session);
                }

                else if (parse_status == UNKNOWN_3_ARGUMENTS_COMMAND) // comando com 3 argumentos não conhecido
                {
                    fprintf(stdout, "command not found\n");
                    send(client_sockfd, FATAL_ERROR_COMMAND_NOT_FOUND, strlen(FATAL_ERROR_COMMAND_NOT_FOUND), 0);
                    send_prompt(&session);
                }

                else if (parse_status == UNKNOWN_1_ARGUMENTS_COMMAND)
                {
                    fprintf(stdout, "command not found\n");
                    send(client_sockfd, FATAL_ERROR_COMMAND_NOT_FOUND, strlen(FATAL_ERROR_COMMAND_NOT_FOUND), 0);
                    send_prompt(&session);
                }

                else if (parse_status == UNKNOWN_2_ARGUMENTS_COMMAND)
                {
                    fprintf(stdout, "command not found\n");
                    send(client_sockfd, FATAL_ERROR_COMMAND_NOT_FOUND, strlen(FATAL_ERROR_COMMAND_NOT_FOUND), 0);
                    send_prompt(&session);
                }

                else if (parse_status == UNKNOWN_COMMAND) // comando com argumentos não conhecido
                {
                    fprintf(stdout, "command not found\n");
                    send(client_sockfd, FATAL_ERROR_COMMAND_NOT_FOUND, strlen(FATAL_ERROR_COMMAND_NOT_FOUND), 0);
                    send_prompt(&session);
                }

                line_len = 0;
            }
        }
    }
}