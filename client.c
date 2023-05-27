#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#define MAX 80
#define PORT 12349
#define SA struct sockaddr


 // oublic key y public adress de server
int main(int argc, char* args[])
{
    if(argc != 3){
        return 0;
    }
    char * server_endpoint = args[1];
    char * allowed_ips = args[2];


    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;


    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
 
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(server_endpoint);
    servaddr.sin_port = htons(PORT);

    char * public_key, private_key;
    // generate_public_and_private_keys(public_key, private_key);
    system("./client_setup");
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))
        != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");
 
    // function for chat
    send(sockfd, public_key, strlen(public_key), 0);
    char * server_key = malloc(strlen(public_key));
    recv(sockfd, server_key, strlen(server_key), 0);

    // connect_to_vpn(server_key, private_key, allowed_ips);
    char* command = malloc(2000);
    sprintf(command, "./client_interface %s %s %s", server_key, allowed_ips, server_endpoint);
    system(command);
    free(command);
    // close the socket
    close(sockfd);
}