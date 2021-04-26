#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define BUFSIZE 1024 // Größe des Buffers
#define TRUE 1
#define ENDLOSSCHLEIFE 1
#define PORT 5678

typedef struct {
    char key[BUFSIZE];
    char value[BUFSIZE];
} storage, *storage_p;

int main(void) {
    int rfd; // Rendevouz-Descriptor
    int cfd; // Verbindungs-Descriptor

    struct sockaddr_in client; // Socketadresse eines Clients
    socklen_t client_len; // Länge der Client-Daten
    char in[BUFSIZE]; // Daten vom Client an den Server
    char buff[BUFSIZE]; // String-Buffer
    int bytes_read; // Anzahl der Bytes, die der Client geschickt hat


    // Socket erstellen
    rfd = socket(AF_INET, SOCK_STREAM, 0);
    if (rfd < 0 ){
        fprintf(stderr, "socket konnte nicht erstellt werden\n");
        exit(-1);
    }


    // Socket Optionen setzen für schnelles wiederholtes Binden der Adresse
    int option = 1;
    if(setsockopt(rfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &option, sizeof(int)) < 0) {
        fprintf(stderr, "reuse option kann auf socket nicht gesetzt werden.\n");
    }


    // Socket binden
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    int brt = bind(rfd, (struct sockaddr *) &server, sizeof(server));
    if (brt < 0 ){
        fprintf(stderr, "socket konnte nicht gebunden werden\n");
        exit(-1);
    }


    // Socket lauschen lassen
    int lrt = listen(rfd, 5);
    if (lrt < 0 ){
        fprintf(stderr, "socket konnte nicht listen gesetzt werden\n");
        exit(-1);
    }

    while (ENDLOSSCHLEIFE) {

        // Verbindung eines Clients wird entgegengenommen
        cfd = accept(rfd, (struct sockaddr *) &client, &client_len);
        if(cfd < 0){
            fprintf(stderr, "verbindung zum client konnte nicht hergestellt werden\n");
        } else {
            printf("verbindung von %s, port %d\n", inet_ntop(AF_INET, &client.sin_addr, buff, sizeof(buff)), ntohl(client.sin_port));
        }

        // Lesen von Daten, die der Client schickt
        bytes_read = read(cfd, in, BUFSIZE);

        // TODO: Telnet-Nachricht vergleichen mit GET, PUT, DEL, QUIT und entsprechende Programmlogik schreiben

        // Zurückschicken der Daten, solange der Client welche schickt (und kein Fehler passiert)
        while (bytes_read > 0) {
            printf("sending back the %d bytes I received...\n", bytes_read);
            write(cfd, in, bytes_read);
            bytes_read = read(cfd, in, BUFSIZE);
        }
        close(cfd);
    }

    // Rendevouz Descriptor schließen
    close(rfd);
    
    return EXIT_SUCCESS;
}
