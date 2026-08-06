#include "headers.h"
#include "server_info_messages.h"
#include "server_error_messages.h"
#include "handle_client_functions.h"
#include "db.h"
#include "asciiart.h"

int handle_client_interaction(int client_sockfd, FILE *server_log, char *client_ip)
{
    char recv_buffer[1024];
    char line_buffer[4096];
    size_t line_len = 0;

    client_session_t session;
    memset(&session, 0, sizeof(session));
    session.sockfd = client_sockfd;
    session.s_state = SESSION_WAITING_LOGIN;

    char *admin_name = getenv("ADMINISTRATOR_USERNAME");

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

                fprintf(server_log, "[%s] [INFO] (RECV) IP='%s' DATA='%s'\n", get_timestamp(), client_ip, line_buffer);

                char buffer[4096];

                strcpy(buffer, line_buffer);

                command_state_t parse_status = parse_command(buffer);

                if (parse_status == HELP_OK)
                {
                    fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' BYTES=%zu HELP OUTPUT SENT\n", get_timestamp(), client_ip, sizeof(HELP_MESSAGE));
                    execute_help_command(client_sockfd);
                    send_prompt(&session);
                }

                else if (parse_status == VERSION_OK)
                {
                    fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' BYTES=%zu VERSION OUTPUT SENT\n", get_timestamp(), client_ip, sizeof(TINYSHELL_VERSION));
                    display_tinyshell_version(client_sockfd);
                    send_prompt(&session);
                }

                else if (parse_status == ECHO_OK)
                {
                    fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' BYTES=%zu ECHO OUTPUT SENT\n", get_timestamp(), client_ip, sizeof(line_buffer));
                    execute_echo_command(client_sockfd, line_buffer);
                    send_prompt(&session);
                }

                else if (parse_status == DATE_OK)
                {
                    char date[30];
                    snprintf(date, sizeof(date), "%s", get_timestamp());
                    fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' BYTES=%zu DATE OUTPUT SENT\n", get_timestamp(), client_ip, sizeof(date));
                    execute_date_command(client_sockfd);
                    send_prompt(&session);
                }

                else if (parse_status == ABOUT_OK)
                {
                    fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' BYTES=%zu ABOUT OUTPUT SENT\n", get_timestamp(), client_ip, sizeof(ABOUT_MESSAGE));
                    execute_about_command(client_sockfd);
                    send_prompt(&session);
                }

                else if (parse_status == QUIT_OK)
                {
                    fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' BYTES=%zu USER EXITED SUCESSFULLY\n", get_timestamp(), client_ip, sizeof(QUIT_MESSAGE));
                    execute_quit_command(client_sockfd);
                    close(client_sockfd);
                    return 0;
                }

                else if (parse_status == CLEAR_OK)
                {
                    fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' BYTES=%zu CLEAR COMMAND SENT\n", get_timestamp(), client_ip, sizeof("\033[2J\033[H"));
                    execute_clear_command(client_sockfd);
                    send_prompt(&session);
                }

                else if (parse_status == ASCIIART_OK && session.s_state == SESSION_AUTHENTICATED)
                {
                    execute_asciiart_command(client_sockfd, line_buffer, server_log, client_ip, &session);
                    send_prompt(&session);
                }

                else if (parse_status == ASCIIART_OK && session.s_state == SESSION_WAITING_LOGIN)
                {
                    fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' BYTES=%zu PERMISSION DENIED, FOR AUTHENTICATED USERS ONLY\n", get_timestamp(), client_ip, sizeof(FATAL_ERROR_AUTHENTICATED_USERS_ONLY));
                    send(client_sockfd, FATAL_ERROR_AUTHENTICATED_USERS_ONLY, strlen(FATAL_ERROR_AUTHENTICATED_USERS_ONLY), 0);
                    send_prompt(&session);
                }

                else if (parse_status == REVERSE_OK && session.s_state == SESSION_AUTHENTICATED)
                {
                    execute_reverse_command(client_sockfd, line_buffer, server_log, client_ip, &session);
                    send_prompt(&session);
                }

                else if (parse_status == REVERSE_OK && session.s_state == SESSION_WAITING_LOGIN)
                {
                    fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' BYTES=%zu PERMISSION DENIED, FOR AUTHENTICATED USERS ONLY\n", get_timestamp(), client_ip, sizeof(FATAL_ERROR_AUTHENTICATED_USERS_ONLY));
                    send(client_sockfd, FATAL_ERROR_AUTHENTICATED_USERS_ONLY, strlen(FATAL_ERROR_AUTHENTICATED_USERS_ONLY), 0);
                    send_prompt(&session);
                }

                else if (parse_status == TURNUPPER_OK && session.s_state == SESSION_AUTHENTICATED)
                {
                    execute_turnupper_command(client_sockfd, line_buffer, server_log, client_ip, &session);
                    send_prompt(&session);
                }

                else if (parse_status == TURNUPPER_OK && session.s_state == SESSION_WAITING_LOGIN)
                {
                    fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' BYTES=%zu PERMISSION DENIED, FOR AUTHENTICATED USERS ONLY\n", get_timestamp(), client_ip, sizeof(FATAL_ERROR_AUTHENTICATED_USERS_ONLY));
                    send(client_sockfd, FATAL_ERROR_AUTHENTICATED_USERS_ONLY, strlen(FATAL_ERROR_AUTHENTICATED_USERS_ONLY), 0);
                    send_prompt(&session);
                }

                else if (parse_status == TURNLOWER_OK && session.s_state == SESSION_AUTHENTICATED)
                {
                    execute_turnlower_command(client_sockfd, line_buffer, server_log, client_ip, &session);
                    send_prompt(&session);
                }

                else if (parse_status == TURNLOWER_OK && session.s_state == SESSION_WAITING_LOGIN)
                {
                    fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' BYTES=%zu PERMISSION DENIED, FOR AUTHENTICATED USERS ONLY\n", get_timestamp(), client_ip, sizeof(FATAL_ERROR_AUTHENTICATED_USERS_ONLY));
                    send(client_sockfd, FATAL_ERROR_AUTHENTICATED_USERS_ONLY, strlen(FATAL_ERROR_AUTHENTICATED_USERS_ONLY), 0);
                    send_prompt(&session);
                }

                else if (parse_status == LENGHT_OK && session.s_state == SESSION_AUTHENTICATED)
                {
                    execute_lenght_command(client_sockfd, line_buffer, server_log, client_ip, &session);
                    send_prompt(&session);
                }

                else if (parse_status == LENGHT_OK && session.s_state == SESSION_WAITING_LOGIN)
                {
                    fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' BYTES=%zu PERMISSION DENIED, FOR AUTHENTICATED USERS ONLY\n", get_timestamp(), client_ip, sizeof(FATAL_ERROR_AUTHENTICATED_USERS_ONLY));
                    send(client_sockfd, FATAL_ERROR_AUTHENTICATED_USERS_ONLY, strlen(FATAL_ERROR_AUTHENTICATED_USERS_ONLY), 0);
                    send_prompt(&session);
                }

                else if (parse_status == SUM_OK && session.s_state == SESSION_AUTHENTICATED)
                {
                    execute_sum_command(client_sockfd, line_buffer, server_log, client_ip, &session);
                    send_prompt(&session);
                }

                else if (parse_status == SUM_OK && session.s_state == SESSION_WAITING_LOGIN)
                {
                    fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' BYTES=%zu PERMISSION DENIED, FOR AUTHENTICATED USERS ONLY\n", get_timestamp(), client_ip, sizeof(FATAL_ERROR_AUTHENTICATED_USERS_ONLY));
                    send(client_sockfd, FATAL_ERROR_AUTHENTICATED_USERS_ONLY, strlen(FATAL_ERROR_AUTHENTICATED_USERS_ONLY), 0);
                    send_prompt(&session);
                }

                else if (parse_status == SUB_OK && session.s_state == SESSION_AUTHENTICATED)
                {
                    execute_sub_command(client_sockfd, line_buffer, server_log, client_ip, &session);
                    send_prompt(&session);
                }

                else if (parse_status == SUB_OK && session.s_state == SESSION_WAITING_LOGIN)
                {
                    fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' BYTES=%zu PERMISSION DENIED, FOR AUTHENTICATED USERS ONLY\n", get_timestamp(), client_ip, sizeof(FATAL_ERROR_AUTHENTICATED_USERS_ONLY));
                    send(client_sockfd, FATAL_ERROR_AUTHENTICATED_USERS_ONLY, strlen(FATAL_ERROR_AUTHENTICATED_USERS_ONLY), 0);
                    send_prompt(&session);
                }

                else if (parse_status == MULT_OK && session.s_state == SESSION_AUTHENTICATED)
                {
                    execute_mult_command(client_sockfd, line_buffer, server_log, client_ip, &session);
                    send_prompt(&session);
                }

                else if (parse_status == MULT_OK && session.s_state == SESSION_WAITING_LOGIN)
                {
                    fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' BYTES=%zu PERMISSION DENIED, FOR AUTHENTICATED USERS ONLY\n", get_timestamp(), client_ip, sizeof(FATAL_ERROR_AUTHENTICATED_USERS_ONLY));
                    send(client_sockfd, FATAL_ERROR_AUTHENTICATED_USERS_ONLY, strlen(FATAL_ERROR_AUTHENTICATED_USERS_ONLY), 0);
                    send_prompt(&session);
                }

                else if (parse_status == DIV_OK && session.s_state == SESSION_AUTHENTICATED)
                {
                    execute_div_command(client_sockfd, line_buffer, server_log, client_ip, &session);
                    send_prompt(&session);
                }

                else if (parse_status == DIV_OK && session.s_state == SESSION_WAITING_LOGIN)
                {
                    fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' BYTES=%zu PERMISSION DENIED, FOR AUTHENTICATED USERS ONLY\n", get_timestamp(), client_ip, sizeof(FATAL_ERROR_AUTHENTICATED_USERS_ONLY));
                    send(client_sockfd, FATAL_ERROR_AUTHENTICATED_USERS_ONLY, strlen(FATAL_ERROR_AUTHENTICATED_USERS_ONLY), 0);
                    send_prompt(&session);
                }

                else if (parse_status == LOGIN_OK && session.s_state == SESSION_WAITING_LOGIN) // comando LOGIN com estado de conexão correto
                {
                    bool auth_status = authenticate_user(DB_PATH, line_buffer, &session);

                    if (auth_status == true)
                    {
                        fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' USER='%s' BYTES=%zu AUTHENTICATION SUCCESSFULLY\n", get_timestamp(), client_ip, session.username, sizeof(INFO_AUTHENTICATION_SUCCEEDED));
                        send(client_sockfd, INFO_AUTHENTICATION_SUCCEEDED, strlen(INFO_AUTHENTICATION_SUCCEEDED), 0);
                        session.s_state = SESSION_AUTHENTICATED;
                        send_prompt(&session);
                    }

                    else if (auth_status == false)
                    {
                        fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' BYTES=%zu AUTHENTICATION FAILED\n", get_timestamp(), client_ip, sizeof(FATAL_ERROR_AUTHENTICATION_FAILED));
                        send(client_sockfd, FATAL_ERROR_AUTHENTICATION_FAILED, strlen(FATAL_ERROR_AUTHENTICATION_FAILED), 0);
                        send_prompt(&session);
                    }
                }

                else if (parse_status == LOGIN_OK && session.s_state == SESSION_AUTHENTICATED) // comando LOGIN com estado de conexão incorreto
                {
                    fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' USER='%s' BYTES=%zu AUTHENTICATED ALREADY\n", get_timestamp(), client_ip, session.username, sizeof(FATAL_ERROR_USER_ALREADY_AUTHENTICATED));
                    send(client_sockfd, FATAL_ERROR_USER_ALREADY_AUTHENTICATED, strlen(FATAL_ERROR_USER_ALREADY_AUTHENTICATED), 0);
                    send_prompt(&session);
                }

                else if (parse_status == REGISTER_OK && strcmp(session.username, admin_name) == 0 && session.s_state == SESSION_AUTHENTICATED) // comando REGISTER com estado de conexão correto
                {
                    char temp[32];

                    strcpy(temp, line_buffer);

                    add_user_into_table(temp);

                    fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' BYTES=%zu USER REGISTERED SUCCESSFULLY\n", get_timestamp(), client_ip, sizeof(INFO_USER_INSERTED_DATABASE_SUCCESSFULLY));
                    send(client_sockfd, INFO_USER_INSERTED_DATABASE_SUCCESSFULLY, strlen(INFO_USER_INSERTED_DATABASE_SUCCESSFULLY), 0);
                    send_prompt(&session);
                }

                else if (parse_status == REGISTER_OK && strcmp(session.username, admin_name) != 0) // comando REGISTER com estado de conexão incorreto
                {
                    fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' BYTES=%zu PERMISSION DENIED, FOR ADMINISTRADOR ONLY\n", get_timestamp(), client_ip, sizeof(FATAL_ERROR_ADMINS_ONLY));
                    send(client_sockfd, FATAL_ERROR_ADMINS_ONLY, strlen(FATAL_ERROR_ADMINS_ONLY), 0);
                    send_prompt(&session);
                }

                else if (parse_status == LOGOUT_OK && session.s_state == SESSION_AUTHENTICATED) // comando LOGOUT com estado de conexão correto
                {
                    fprintf(server_log, "[%s] [INFO] (SEND) IP='%s' BYTES=%zu USER LOGOUT SUCCESSFULLY\n", get_timestamp(), client_ip, sizeof(INFO_USER_LOGOUT_SUCCESSFULL));
                    send(client_sockfd, INFO_USER_LOGOUT_SUCCESSFULL, strlen(INFO_USER_LOGOUT_SUCCESSFULL), 0);
                    session.s_state = SESSION_WAITING_LOGIN;
                    session.username[0] = '\0';
                    send_prompt(&session);
                }

                else if (parse_status == LOGOUT_OK && session.s_state == SESSION_WAITING_LOGIN) // comando LOGOUT com estado de conexão incorreto
                {
                    fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' BYTES=%zu USER LOGOUT FAILED, NOT AUTHENTICATED\n", get_timestamp(), client_ip, sizeof(FATAL_ERROR_AUTHENTICATED_USERS_ONLY));
                    send(client_sockfd, FATAL_ERROR_AUTHENTICATED_USERS_ONLY, strlen(FATAL_ERROR_AUTHENTICATED_USERS_ONLY), 0);
                    send_prompt(&session);
                }

                else if (parse_status == UNKNOWN_3_ARGUMENTS_COMMAND) // comando com 3 argumentos não conhecido
                {
                    fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' BYTES=%zu COMMAND NOT FOUND\n", get_timestamp(), client_ip, sizeof(FATAL_ERROR_COMMAND_NOT_FOUND));
                    send(client_sockfd, FATAL_ERROR_COMMAND_NOT_FOUND, strlen(FATAL_ERROR_COMMAND_NOT_FOUND), 0);
                    send_prompt(&session);
                }

                else if (parse_status == UNKNOWN_1_ARGUMENTS_COMMAND)
                {
                    fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' BYTES=%zu COMMAND NOT FOUND\n", get_timestamp(), client_ip, sizeof(FATAL_ERROR_COMMAND_NOT_FOUND));
                    send(client_sockfd, FATAL_ERROR_COMMAND_NOT_FOUND, strlen(FATAL_ERROR_COMMAND_NOT_FOUND), 0);
                    send_prompt(&session);
                }

                else if (parse_status == UNKNOWN_2_ARGUMENTS_COMMAND)
                {
                    fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' BYTES=%zu COMMAND NOT FOUND\n", get_timestamp(), client_ip, sizeof(FATAL_ERROR_COMMAND_NOT_FOUND));
                    send(client_sockfd, FATAL_ERROR_COMMAND_NOT_FOUND, strlen(FATAL_ERROR_COMMAND_NOT_FOUND), 0);
                    send_prompt(&session);
                }

                else if (parse_status == UNKNOWN_COMMAND) // comando com argumentos não conhecido
                {
                    fprintf(server_log, "[%s] [WARN] (SEND) IP='%s' BYTES=%zu COMMAND NOT FOUND\n", get_timestamp(), client_ip, sizeof(FATAL_ERROR_COMMAND_NOT_FOUND));
                    send(client_sockfd, FATAL_ERROR_COMMAND_NOT_FOUND, strlen(FATAL_ERROR_COMMAND_NOT_FOUND), 0);
                    send_prompt(&session);
                }

                line_len = 0;
            }
        }
    }
}