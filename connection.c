//
// Created by Mert Mutlu on 21.06.2021.
//

#include "connection.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include "stdlib.h"
#include "keyValueStore.h"

#define BUFFSIZE 256 // Größe des Buffers

int rfd; // Rendevouz-Descriptor
int cfd; // Verbindungs-Descriptor
int pid;

struct sockaddr_in client; // Socketadresse eines Clients
socklen_t client_len; // Länge der Client-Daten
char in[BUFFSIZE]; // Daten vom Client an den Server
char buff[BUFFSIZE]; // String-Buffer
int bytes_read; // Anzahl der Bytes, die der Client geschickt hat

void initServer() {
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
    clientConnection();
}


int clientConnection () {

    while (1) {

        // Verbindung eines Clients wird entgegengenommen
        cfd = accept(rfd, (struct sockaddr *) &client, &client_len);
        if (cfd < 0) {
            fprintf(stderr, "accept: %s\n", strerror(errno));
        } else {
            printf("verbindung zu %s hergestellt\n", inet_ntop(AF_INET, &client.sin_addr, buff, sizeof(buff)));
            write(cfd, "Verbindung zum KVSTORE hergestellt\n", 35);
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

            if(strncmp("GET", in, 3) == 0) {
                printf("GET aufgerufen\n");
                char res[50];
                char* key = strtok(in+3, " ");
                //value = strtok(NULL, "\0");
                GET(key, res);
                printf("Hello world\n");
                printf("Der Value für den Key KEY1: %s\n", res);
                char str[80];
                strcpy(str, "GET:");
                strcat(str, key);
                strcat(str, ":");
                strcat(str, res);
                write(cfd, str, strlen(str));
            }

            if(strncmp("PUT", in, 3) == 0) {
                PUT();
                write(cfd, "PUT VALUE\n", 10);
            }


            // Zurückschicken der Daten, solange der Client welche schickt (und kein Fehler passiert)
            /*while (bytes_read > 0) {
                if (strncmp("GET", in, 3) == 0) {
                    printf("GET key wird ausgeführt...\n");
                    GET();
                }
            }*/
        }

        printf("sending back the %d bytes I received...\n", bytes_read);

        //write(cfd, in, bytes_read);
        bytes_read = read(cfd, in, BUFFSIZE);
        printf("CLIENT ANFRAGE: %s\n", in);
    }
    close(cfd);

    return EXIT_SUCCESS;

}