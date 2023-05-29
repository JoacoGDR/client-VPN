#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#define MAX 80
#define PORT 51822
#define SA struct sockaddr


 // oublic key y public adress de server
int main(int argc, char* args[])
{
    if(argc != 3){
        return 0;
    }
    char * server_endpoint = args[1];
    char * allowed_ips = args[2];

    printf("Server endpoint: %s\n", server_endpoint);
    printf("Allowed ips: %s\n", allowed_ips);
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    // socket create and verification

    printf("Creating socket...\n");
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    
    printf("Socket successfully created..\n");
    printf("Setting up server address...\n");
    bzero(&servaddr, sizeof(servaddr));
    
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(server_endpoint);
    servaddr.sin_port = htons(PORT);

    printf("Server address set\n");

    char * public_key;
    printf("Generating keys...\n");
    system("./client_setup.sh");


    //store the public key that is stored in public_key to the variable char * public_key
    FILE *fp;
    fp = fopen("publickey", "r");
    if(fp == NULL){
        printf("Error opening file\n");
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    public_key = malloc(fsize + 1);
    fread(public_key, fsize, 1, fp);
    fclose(fp);
    public_key[fsize] = 0;


    printf("Keys generated\n");
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))
        != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }

    printf("connected to the server..\n");
    
 
    // function for chat
    printf("Sending public key...\n");
    send(sockfd, public_key, strlen(public_key), 0);
    printf("Public key sent\n");

    char * server_key = malloc(strlen(public_key));
    printf("Receiving server key...\n");
    recv(sockfd, server_key, strlen(server_key), 0);
    printf("Server key received\n");

    // connect_to_vpn(server_key, private_key, allowed_ips);

    char* command = malloc(2000);
    printf("Running client interface...\n");
    sprintf(command, "./client_interface.sh %s %s %s", server_key, allowed_ips, server_endpoint);
    system(command);

    free(command);
    // close the socket
    close(sockfd);
}