#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#define BUFFSIZE 256 // Größe des Buffers
#define STORAGESIZE 5 // Größe des Storage
#define TRUE 1
#define ENDLOSSCHLEIFE 1
#define PORT 5678
#define SEGSIZE 10000
