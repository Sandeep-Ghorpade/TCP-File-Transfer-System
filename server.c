/*
    Project : TCP File Transfer System
    Language : C
    Platform : Linux
    Concept : Socket Programming

    Description:
    This project implements a simple client-server file transfer system 
    using TCP sockets in C. The server listens for incoming client 
    connections and sends the requested file to the client over a 
    network connection.

    The client connects to the server, sends the filename, receives 
    the file data, and saves it locally.

    Key Features:
    - TCP Socket Programming
    - Client-Server Architecture
    - Concurrent client handling using fork()
    - File transfer using buffer-based transmission
    - Linux system calls (socket, bind, listen, accept, read, write)

    Components:
    1. server.c  -> Handles client connections and sends requested files
    2. client.c  -> Requests file from server and downloads it

    Author: Sandeep Ghorpade
*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>

void SendFileToClient(int ClientSocket, char * Filename)
{
    int fd = 0;
    struct stat sobj;
    char Buffer[1024];
    int BytesRead = 0;
    char Header[64] = {'\0'};

    printf("File name is : %s : %lu\n",Filename,strlen(Filename));

    fd = open(Filename, O_RDONLY);;

    //Unable to open file
    if(fd < 0)
    {
        write(ClientSocket, "ERR\n",4);

        return;
    }

    stat(Filename,&sobj);

    snprintf(Header,sizeof(Header),"OK %ld\n",(long)sobj.st_size);

    write(ClientSocket,Header,strlen(Header));

    while((BytesRead = read(fd, Buffer, sizeof(Buffer))) > 0)
    {
        write(ClientSocket,Buffer,BytesRead);
    }

    close(fd);

}

///////////////////////////////////////////////////
//
// CommandLine Argument : Application
// 1st Argument : Port Number
// ./server     9000
// argv[0]      argv[1]
// 
///////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    int ServerSocket = 0;
    int ClientSocket = 0;
    int Port = 0;
    int iRet = 0;

    char FileName[50] = {'\0'};
    pid_t pid = 0;

    socklen_t Addrlen = sizeof(ClientSocket);

    struct sockaddr_in ServerAddr;
    struct sockaddr_in ClientAddr;


    if(argc < 2 || argc > 2)
    {
        printf("Unable to processed as invalid number of arguments\n");
        printf("Please provide the port number\n");
        return -1;
    }

    Port = atoi(argv[1]);


    /////////////////////////////////////////////////
    // Step 1 : Create TCP Socket
    /////////////////////////////////////////////////

    ServerSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(ServerSocket < 0)
    {
        printf("unable to create server socket\n");
        return -1;
    }
    
    /////////////////////////////////////////////////
    // Step 2 : Bind socket to IP and Port
    /////////////////////////////////////////////////

    memset (&ServerAddr, 0, sizeof(ServerAddr));

    // Initialize the structure

    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(Port);
    ServerAddr.sin_addr.s_addr = INADDR_ANY;

    iRet = bind(ServerSocket, (struct sockaddr *)&ServerAddr, sizeof(ServerAddr));

    if(iRet == -1)
    {
        printf("Ubable to bind\n");

        close(ServerSocket);

        return -1;
    }

    /////////////////////////////////////////////////
    // Step 3 : Listen for client connection
    /////////////////////////////////////////////////

    iRet = listen(ServerSocket, 11);

    if(iRet == -1)
    {
        printf("Server unable to listen the request\n");

        close(ServerSocket);

        return -1;
    }

    printf("Server is running on port : %d\n",Port);

    /////////////////////////////////////////////////
    // Loop which accepts clients request continuously
    /////////////////////////////////////////////////

    while(1)
    {
        /////////////////////////////////////////////////
        // Step 4 : Accept the client request
        /////////////////////////////////////////////////

        memset (&ClientAddr, 0, sizeof(ClientAddr));

        printf("Server is waiting for client request\n");

        ClientSocket = accept(ServerSocket, (struct sockaddr *)&ClientAddr, &Addrlen);

        if(ClientSocket < 0)
        {
            printf("Unable to accept client request\n");

            continue;
        }

        printf("Clients get connected : %s\n",inet_ntoa(ClientAddr.sin_addr));

        /////////////////////////////////////////////////
        // Step 5 : Create new process to handle client request
        /////////////////////////////////////////////////

        pid = fork();

        if(pid < 0)
        {
            printf("Unable to create a new process for client request\n");

            close(ClientSocket);

            continue;
        }

        // New process gets created for client
        if(pid == 0)
        {
            printf("New process is created for client request\n");

            close(ServerSocket);


            iRet = read(ClientSocket, FileName, sizeof(FileName));

            printf("Requested File by client : %s\n",FileName);
            FileName[strspn(FileName, "\r\n")] = '\0';

            SendFileToClient(ClientSocket,FileName); 

            close(ClientSocket);

            printf("File transfer done and client disconnect\n");

            exit(0);
        }// End of if (fork)
        else // parent process(Server)
        {
            close(ClientSocket);
        } // End of else
        
    }// End of while

    close(ServerSocket);

    return 0;

}// End of main

