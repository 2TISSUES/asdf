#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/wait.h>
#include "keyValueStore.h"
#include "shMem.h"
#define BUFFSIZE 256 // Größe des Buffers
#define STORAGESIZE 5 // Größe des Storage
#define TRUE 1
#define ENDLOSSCHLEIFE 1
#define PORT 5678

kv_storage storage[STORAGESIZE];

int main(void) {
    int rfd; // Rendevouz-Descriptor
    int cfd; // Verbindungs-Descriptor

    int pid;


    // BEGIN testdaten für storage
    shm_seg[0].index = 0;
    strcpy(shm_seg[0].key, "KEY1");
    strcpy(shm_seg[0].value, "VALUE1");

    shm_seg[1].index = 1;
    strcpy(shm_seg[1].key, "KEY2");
    strcpy(shm_seg[1].value, "VALUE2");

    shm_seg[2].index = 2;
    strcpy(shm_seg[2].key, "KEY3");
    strcpy(shm_seg[2].value, "VALUE3");

    // ENDE testdaten für storage

    struct sockaddr_in client; // Socketadresse eines Clients
    socklen_t client_len; // Länge der Client-Daten
    char in[BUFFSIZE]; // Daten vom Client an den Server
    char buff[BUFFSIZE]; // String-Buffer
    int bytes_read; // Anzahl der Bytes, die der Client geschickt hat


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
    int lrt = listen(rfd, 5);
    if (lrt < 0) {
        fprintf(stderr, "listen: %s\n", strerror(errno));
        exit(-1);
    }

    while (ENDLOSSCHLEIFE) {

        // Verbindung eines Clients wird entgegengenommen
        cfd = accept(rfd, (struct sockaddr *) &client, &client_len);
        if (cfd < 0) {
            fprintf(stderr, "accept: %s\n", strerror(errno));
        } else {
            printf("verbindung zu %s hergestellt\n", inet_ntop(AF_INET, &client.sin_addr, buff, sizeof(buff)));
        }

        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "fork: %s\n", strerror(errno));
            exit(1);
        }

        if (pid == 0) { // Kindprozess
            printf("Im Kindprozess\n");
            // Lesen von Daten, die der Client schickt
            if ((bytes_read = read(cfd, in, BUFFSIZE)) < 0) {
                fprintf(stderr, "read: %s\n", strerror(errno));
            }

            in[strcspn(in, "\r\n")] = 0;

            char * befehl;
            char * key;
            char * value;

            befehl = strtok(in, " \0");
            key = strtok(NULL, " ");
            value = strtok(NULL, "\0");


            printf("befehl %s\n", befehl);
            printf("key %s\n", key);
            printf("value %s\n", value);



            // Zurückschicken der Daten, solange der Client welche schickt (und kein Fehler passiert)
            while (bytes_read > 0) {
                if (strncmp("QUIT", in, 4) == 0) {
                    printf("QUIT wird ausgeführt\n");
                    if (close(cfd) < 0) {
                        fprintf(stderr, "close: %s\n", strerror(errno));
                    }
                } else if (strncmp("GET", in, 3) == 0) {
                    printf("GET key wird ausgeführt...\n");
                    printf("DATA: Index: %d, Key: %s, Value: %s\n", shm_seg[3].index, shm_seg[3].key, shm_seg[3].value);
                } else if (strncmp("PUT", in, 3) == 0) {
                    printf("PUT key value wird ausgeführt...\n");
                    printf("DATA: Index: %d, Key: %s, Value: %s\n", shm_seg[1].index, shm_seg[1].key, shm_seg[1].value);
                } else if (strncmp("DEL", in, 3) == 0) {
                    printf("DELETE key wird ausgeführt...\n");
                    printf("DATA: Index: %d, Key: %s, Value: %s\n", shm_seg[2].index, shm_seg[2].key, shm_seg[2].value);
                } else if (strncmp("CHANGE", in, 6) == 0) {
                    printf("Ändere Daten bei Index 0....\n");
                    strcpy(shm_seg[0].key, "LMAO");
                    strcpy(shm_seg[0].value, "IT WORKS");
                }
                bytes_read = read(cfd, in, BUFFSIZE);
            }
            close(cfd);
        }
    }

    if (pid > 0) {
        // Vaterprozess
        printf("Im Vaterprozess\n");
    }

    if (shmdt(shm_seg) == -1) {
        perror("shmdt");
        exit(1);
    }

    if (shmctl(shm_id, IPC_RMID, 0) == -1) {
        fprintf(stderr, "shmctl %s", strerror(errno));
        exit(1);
    }

    // Rendevouz Descriptor schließen
    close(rfd);

    return EXIT_SUCCESS;
}