#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include "dhcp_utils.c"

#define PORT 51822
#define BUFFER_SIZE 1024



char * dhcp_lease_ip(char * publicKey) {
    printf("Getting IP address for public key: %s\n", publicKey);
    char * ip = getIPAddress(publicKey);
    if (ip == NULL) {
        ip = generateIPAddress();
        if(ip == NULL){
            exit(EXIT_FAILURE);
        }
        addEntry(publicKey, ip);
    }
    printf("IP address for client is %s\n", ip);
    return ip;
}


void generate_peer(char * interface, char* public_key, char* ip_address) {
    char command[BUFFER_SIZE];
    memset(command, 0, BUFFER_SIZE);

    sprintf(command, "sudo wg set %s peer %s allowed-ips %s/32",interface, public_key, ip_address);
    printf("Running command: %s\n", command);
    system(command);
}

void handle_client(int client_socket,char * interface, char* public_key, char * server_public_key) {
    printf("Handling client\n");
    printf("Generating Peer with public key: %s\n", public_key);
    generate_peer(interface, public_key, dhcp_lease_ip(public_key));
    printf("Going to send to client the server public key: %s\n", server_public_key);
    send(client_socket, server_public_key, strlen(server_public_key), 0);
}

int main(int argc, char * args[]) {
    if(argc != 3){
        return 0;
    }
    char * server_public_key = args[1];
    char * interface = args[2];
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    // Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0) {

        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the specified IP and port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        // Accept a new client connection
        if ((client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
        printf("Accepted new client connection\n");

        // Read the public key provided by the client
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_received = read(client_socket, buffer, BUFFER_SIZE);
        if (bytes_received < 0) {
            perror("read failed");
            exit(EXIT_FAILURE);
        }
        printf("Received %ld bytes\n", bytes_received);

        // Process the public key and send the file
        if (bytes_received > 0) {
            printf("Received public key: %s\n", buffer);
            handle_client(client_socket, interface, buffer, server_public_key);
        }

        // Close the client socket
        close(client_socket);
    }

    // Close the server socket
    close(server_fd);

    return 0;
}
