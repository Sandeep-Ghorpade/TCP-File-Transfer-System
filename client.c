/*
    Project : TCP File Transfer System
    File    : client.c
    Language: C
    Platform: Linux

    Description:
    This program acts as the client for the TCP File Transfer System.
    It connects to the server, sends the requested filename, receives
    the file data, and stores it locally.

    Concepts Used:
    - TCP Socket Programming
    - Client-Server Communication
    - File Handling
    - Linux System Calls

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


int ReadLine(int Sock, char *line, int max)
{
    int i = 0;
    char ch = {'\0'};
    int n = 0;

    char Header[64] = {'\0'};

    while(i < max - 1 )
    {
        n = read(Sock,&ch,1);

        if(n <= 0)
        {
            break;
        }
        line[i++] = ch;
        if(ch == '\0')
        {
            break;
        }
    } // End of while

    line[i] = '\0';

    return i;
} // end of readline
///////////////////////////////////////////////////
//
// CommandLine Argument : Application
// 1st Argument : IP Address
// 2nd Argument : Port number
// 3rd Argument : Target file name
// 4th Argument : New file Name
//
// ./client     127.0.0.1   9000      Demo.txt   A.txt
//  argv[0]     argv[1]     argv[2]   argv[3]    argv[4]
//
// argc = 5
// 
///////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    int Sock = 0;
    int Port = 0; 
    int iRet = 0;
    char Header[64] = {'\0'};

    struct sockaddr_in ServerAddr;

    char *ip = NULL;        
    char *Filename = NULL;     
    char *OutFilename = NULL; 

    if(argc < 5 || argc > 5)
    {
        printf("unable to proceed as invalid number of arguments\n");

        printf("Please provise below arguments\n");
        printf("1st Argument : IP Address\n");
        printf("2nd Argument : Port number\n");
        printf("3rd Argument : Target file name\n");
        printf("4th Argument : New file Name\n");

        return -1;

    }

    ip = argv[1];
    Port = atoi(argv[2]);
    Filename = argv[3];
    OutFilename = argv[4];

    ////////////////////////////////////////////////////////
    // Step 1 : Create TCP Socket
    ////////////////////////////////////////////////////////

    Sock = socket(AF_INET, SOCK_STREAM, 0);

    if(Sock < 0)
    {
        printf("unable to create the client socket\n");
        return -1;
    }

    ////////////////////////////////////////////////////////
    // Step 2 : Connect with server
    ////////////////////////////////////////////////////////

    memset(&ServerAddr, 0, sizeof(ServerAddr));

    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(Port);

    //Convert the IP address into binary format
    inet_pton(AF_INET, ip, &ServerAddr.sin_addr);

    iRet = connect(Sock, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr));

    if(iRet == -1)
    {
        printf("Unable to connect with server\n");

        close(Sock);

        return -1;
    }

    ////////////////////////////////////////////////////////
    // Step 3 : Send file name
    ////////////////////////////////////////////////////////

    write(Sock,Filename,strlen(Filename));
    write(Sock,"\n",1);

    ////////////////////////////////////////////////////////
    // Step 4 : read the header
    ////////////////////////////////////////////////////////

    iRet = ReadLine(Sock, Header, sizeof(Header));

    if(iRet <= 0)
    {
        printf("Server gets disconnected abnormally\n");
        close(Sock);
        return -1;

    }

    long FileSize = 0;

    sscanf(Header,"OK %ld",&FileSize);
    printf("Filesize is : %ld\n",FileSize);

    ////////////////////////////////////////////////////////
    // Step 5 : Create new file
    ////////////////////////////////////////////////////////

    int outfd = 0;

    outfd = open(OutFilename, O_CREAT | O_WRONLY | O_TRUNC, 0777);

    if(outfd < 0)
    {
        printf("Unable to create downloaded file\n");
        return -1;
    }

    char Buffer[1024] = {'\0'};
    long received = 0;
    long remaining = 0;
    int n = 0;
    int toread = 0;

    while(received < FileSize)
    {
        remaining = FileSize - received;

        if(remaining > 1024)
        {
            toread = 1024;
        }
        else
        {
            toread = remaining;
        }

        n = read(Sock, Buffer, toread);

        write(outfd,Buffer,n);

        received = received + n;

    }// End of while

    close(outfd);

    close(Sock);

    if(received == FileSize)
    {
        printf("Download Complete...\n");

        return 0;
    }
    else
    {
        printf("Download failed...\n");
        return -1;
    }

    return 0;

}// End of main

