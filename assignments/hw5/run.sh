#!/bin/bash

# Compile and execute the client (foreground) and server (background)

gcc client.c -o client
gcc server.c -o server
./server 2000 &
./client 127.0.0.1 2000

# ctrl-c kills client
# use `ps` and `kill <pid>` to find, kill the server