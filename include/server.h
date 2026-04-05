#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define socket_t int

struct sock_data {
    u_int64_t server_port;
    char *target_host;
    socket_t new_socket;
    struct sockaddr_in sock_addr;
};

struct execute_data {
    socket_t victim_sock;
    char execute_command;
    char commad_response;
};


/** server function */
struct sock_data init_socket (const u_int64_t* server_port , char* server_host);
void start_server (struct sock_data* socket_data , struct execute_data* exec_d);
