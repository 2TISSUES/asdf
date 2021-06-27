#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <sys/msg.h>
#include "keyValueStore.h"
#include "sub.h"

#define BUFF 256 // Größe des Buffers
#define MAX_NUM_OF_CLIENTS 25

union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
};

unsigned short signals[1];
bool myTrans = false;

struct sockaddr_in client; // Socketadresse eines Clients
socklen_t client_len; // Länge der Client-Daten
char in[BUFF]; // Daten vom Client an den Server
char buff[BUFF]; // String-Buffer
int bytes_read; // Anzahl der Bytes, die der Client geschickt hat

void exitHandler (int s)
{
    printf("test: %i", s);
    clearAll();
    exit(1);
}

void setupExitHandler() {
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = exitHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
}

void initServer() {

    setupExitHandler();
    // Socket erstellen
    rfd = socket(AF_INET, SOCK_STREAM, 0);
    if (rfd < 0) {
        fprintf(stderr, "socket: %s\n", strerror(errno));
        exit(-1);
    }

    // Socket Optionen setzen für schnelles wiederholtes Binden der Adresse
    int option = 1;
    if (setsockopt(rfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &option, sizeof(int)) < 0) {
        fprintf(stderr, "setsockopt: %s\n", strerror(errno));
    }

    // Socket binden
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    int brt = bind(rfd, (struct sockaddr *) &server, sizeof(server));
    if (brt < 0) {
        fprintf(stderr, "bind: %s\n", strerror(errno));
        exit(-1);
    }

    // Socket lauschen lassen
    int lrt = listen(rfd, MAX_NUM_OF_CLIENTS);
    if (lrt < 0) {
        fprintf(stderr, "listen: %s\n", strerror(errno));
        exit(-1);
    }
    clientConnection();
}

void clearAll() {
    if (semctl(semID, 0, IPC_RMID) == -1) {
        perror("semctl");
        exit(1);
    }
    if (semctl(transID, 0, IPC_RMID) == -1) {
        perror("semctl");
        exit(1);
    }
    if (shmdt(shm_seg) == -1) {
        perror("shmdt");
        exit(1);
    }
}

void initMQ() {
    mqID= msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    if(mqID== -1) {
        printf("cannot get message queue\n");
        exit(1);
    }
}

int initSemaphore() {
    union semun arg;
    semID = semget(IPC_PRIVATE, 1, IPC_CREAT | 0644);
    if (semID == -1) {
        printf("Semaphorengruppe konnte nicht erzeugt werden!\n");
        return 2;
    }

    transID = semget(IPC_PRIVATE, 1, IPC_CREAT | 0644);
    if(transID == -1) {
        printf("Semaphorengruppe konnte nicht erzeugt werden!\n");
        return 2;
    }

    subID = semget(IPC_PRIVATE, 1, IPC_CREAT | 0644);
    if(subID == -1) {
        printf("Semaphorengruppe konnte nicht erzeugt werden!\n");
        return 2;
    }

    signals[0] = 1;
    arg.val = 1;
    semctl(semID, 0, SETALL, signals);
    semctl(transID, 0, SETALL, signals);
    semctl(subID, 0, SETALL, signals);
    return 1;
}

void semOP( int op ) {
    struct sembuf buf [1];
    buf[0].sem_num = 0;
    buf[0].sem_flg = SEM_UNDO;
    buf[0].sem_op = 1;

    if(op) {
        buf[0].sem_op = -1;
    }

    if (semop(semID, buf, 1) == -1) {
        perror("semop");
        exit(1);
    }
}

void transOP (int op) {
    struct sembuf trans;
    trans.sem_num = 0;
    trans.sem_flg = SEM_UNDO;
    trans.sem_op = 1;

    if(op) {
        trans.sem_op = -1;
    }

    if (semop(transID, &trans, 1) == -1) {
        perror("semop");
        exit(1);
    }
}

void subOP (int op) {
    struct sembuf sub;
    sub.sem_num = 0;
    sub.sem_flg = SEM_UNDO;
    sub.sem_op = 1;

    if(op) {
        sub.sem_op = -1;
    }

    if (semop(subID, &sub, 1) == -1) {
        perror("semop");
        exit(1);
    }
}

void getSemVal( char* msg) {
    printf("semVal %s: %d\n", msg, semctl(semID, 0, GETVAL));
}

void getSubVal( char* msg) {
    printf("subVal %s: %d\n", msg, semctl(subID, 0, GETVAL));
}

void getTransVal( char* msg) {
    printf("transVal %s: %d\n", msg, semctl(transID, 0, GETVAL));
}

int clientConnection () {
    while (1) {
        // Verbindung eines Clients wird entgegengenommen
        cfd = accept(rfd, (struct sockaddr *) &client, &client_len);
        printf("cfd: %d\n", cfd);
        if (cfd < 0) {
            fprintf(stderr, "accept: %s\n", strerror(errno));
        } else {
            printf("verbindung zu %s hergestellt\n", inet_ntop(AF_INET, &client.sin_addr, buff, sizeof(buff)));
        }

        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "fork: %s\n", strerror(errno));
            exit(0);
        }

        if (pid == 0) { // Kindprozess
            // Lesen von Daten, die der Client schickt
            close(rfd);
            int clientPID = getpid();
            int subPid = fork();
            if (subPid == 0) {
                while(1) {
                    subMsg mySubMsg;
                    memset(mySubMsg.msg, 0, 80);
                    int v = msgrcv(mqID, &mySubMsg, 80, clientPID, 0);
                    if(v < 0) {
                        printf("errorreadingfromqueue\n");
                        continue;
                    }
                    printf("sending subbed msg: %s\n", mySubMsg.msg);
                    //mySubMsg.msg[strcspn(mySubMsg.msg, "\r\n")] = '\n';
                    write(cfd, mySubMsg.msg, strlen(mySubMsg.msg));
                }
            }

            if (subPid > 0) {


                if ((bytes_read = read(cfd, in, BUFF)) < 0) {
                    fprintf(stderr, "read: %s\n", strerror(errno));
                }

                while (bytes_read > 0) {
                    if (!myTrans) {
                        transOP(ENTER);
                        transOP(LEAVE);
                    }

                    in[strcspn(in, "\r\n")] = 0;

                    if (strncmp("GET", in, 3) == 0) {
                        printf("GET aufgerufen\n");
                        char res[50];
                        bzero(res, 50);
                        char *key = strtok(in + 3, " ");
//                        char *value = strtok(in + 3 + strlen(key) + 2, " ");

                        if (!GET(key, res)) {
                            strcpy(res, "key_nonexistent");
                        }
                        char str[80];
                        strcpy(str, "GET:");
                        strcat(str, key);
                        strcat(str, ":");
                        strcat(str, res);
                        strcat(str, "\n");
                        write(cfd, str, strlen(str));
                    }

                    if (strncmp("PUT", in, 3) == 0) {
                        printf("PUT aufgerufen\n");

                        char *key = strtok(in + 3, " ");

                        char *value = strtok(in + 3 + strlen(key) + 2, " ");
                        printf("%s - %s\n", key, value);
                        PUT(key, value);
                        char str[80];
                        strcpy(str, "PUT:");
                        strcat(str, key);
                        strcat(str, ":");
                        strcat(str, value);
                        strcat(str, "\n");
                        notifySubs(key, str);
                        write(cfd, str, strlen(str));
                    }

                    if (strncmp("DEL", in, 3) == 0) {
                        printf("DEL aufgerufen\n");
                        char res[50];
                        bzero(res, 50);
                        char *key = strtok(in + 3, " ");

                        if (!DEL(key)) {
                            strcpy(res, "key_nonexistent");
                        } else {
                            strcpy(res, "key_deleted");
                        }

                        char str[80];
                        strcpy(str, "DEL:");
                        strcat(str, key);
                        strcat(str, ":");
                        strcat(str, res);
                        strcat(str, "\n");
                        notifySubs(key, str);
                        write(cfd, str, strlen(str));
                    }

                    if (strncmp("QUIT", in, 4) == 0) {
                        close(cfd);
                        exit(EXIT_SUCCESS);
                    }

                    if (strncmp("BEG", in, 3) == 0) {
                        transOP(ENTER);
                        myTrans = true;
                    }

                    if (strncmp("END", in, 3) == 0) {
                        myTrans = false;
                        transOP(LEAVE);
                    }

                    if (strncmp("SUB", in, 3) == 0) {
                        printf("SUB aufgerufen\n");
                        char res[50];
                        bzero(res, 50);
                        char *key = strtok(in + 3, " "); // GET hallo'\0'\n\0
                        if (!GET(key, res)) {
                            strcpy(res, "key_nonexistent");
                        } else {
                            SUB(key);
                        }
                        char str[80];
                        strcpy(str, "SUB:");
                        strcat(str, key);
                        strcat(str, ":");
                        strcat(str, res);
                        strcat(str, "\n");
                        write(cfd, str, strlen(str));
                    }
                    bytes_read = read(cfd, in, BUFF);
                }
            }
            close(cfd);
        }
    }
}

