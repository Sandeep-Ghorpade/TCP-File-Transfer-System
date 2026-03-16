# TCP File Transfer System (C)

This project implements a simple TCP-based file transfer system using C and Linux socket programming.

## Features
- Client-server architecture
- File transfer over TCP
- Concurrent client handling using fork()
- Buffer-based file transmission

## Technologies Used
- C Programming
- Linux System Programming
- TCP Socket Programming

## Project Structure
server.c  -> Server program  
client.c  -> Client program  

## Compile

gcc server.c -o server  
gcc client.c -o client  

## Run

Start server:
./server 9000

Run client:
./client 127.0.0.1 9000 filename.txt newfile.txt

## Author
Sandeep Ghorpade
