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

typedef struct kv_storage {
    char key[BUFFSIZE];
    char value[BUFFSIZE];
    int index;
} kv_storage, *kv_storage_p;

kv_storage storage[STORAGESIZE];

int main(void) {
    int rfd; // Rendevouz-Descriptor
    int cfd; // Verbindungs-Descriptor

    int pid;

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
            // Lesen von Daten, die der Client schickt

            if ((bytes_read = read(cfd, in, BUFFSIZE)) < 0) {
                fprintf(stderr, "read: %s\n", strerror(errno));

            // Zurückschicken der Daten, solange der Client welche schickt (und kein Fehler passiert)
            while (bytes_read > 0) {

                printf("sending back the %d bytes I received...\n", bytes_read);
                write(cfd, in, bytes_read);
                bytes_read = read(cfd, in, BUFFSIZE);
            }
            close(cfd);
        }
    }

    // Rendevouz Descriptor schließen
    close(rfd);

    return EXIT_SUCCESS;
}