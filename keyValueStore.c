#include <sys/shm.h>
#include <stdlib.h>
#include <errno.h>
#include "string.h"
#include "stdio.h"
#include "keyValueStore.h"
#include "sub.h"
#include <unistd.h>

#define SEGSIZE 10000

int GET(char* key, char* res) {
    printf("GET\n");
    printf("KEY: %s\n", key);
    getSemVal( "vor ENTER" );
    semOP (ENTER);
    getSemVal("nach ENTER");
    for(int i=0; i < STORAGESIZE; i++) {
        if (strncmp(shm_seg[i].key, key, strlen(key)) == 0) {
            strcpy(res, shm_seg[i].value);
            semOP( LEAVE);
            return 1;
        }
    }
    semOP(LEAVE);
    return 0;
}

int PUT(char* key, char* value, int subs[SUBSIZE]) {
    printf("PUT\n");
    semOP (ENTER);
    for(int i=0; i < STORAGESIZE; i++) {
        if (strncmp(shm_seg[i].key, key, strlen(key)) == 0) {
            strcpy(shm_seg[i].value, value);
            for (int j = 0; j < SUBSIZE; ++j) {
                subs[j] = shm_seg[i].subs[j];
            }
            semOP (LEAVE);
            return 1;
        }
    }

    for(int i=0; i < STORAGESIZE; i++) {
        if (strncmp(shm_seg[i].key, " ", 1) == 0) {
            strcpy(shm_seg[i].key, key);
            strcpy(shm_seg[i].value, value);
            for (int j = 0; j < SUBSIZE; ++j) {
                subs[j] = shm_seg[i].subs[j];
            }
            return 1;
        }
    }
    semOP( LEAVE);
    return 0;
}

int DEL(char* key,int subs[SUBSIZE]) {
    printf("DEL\n");
    getSemVal( "vor ENTER" );
    semOP (ENTER);
    getSemVal("nach ENTER");
    for(int i=0; i < STORAGESIZE; i++) {
        if (strncmp(shm_seg[i].key, key, strlen(key)) == 0) {
            strcpy(shm_seg[i].key, " ");
            strcpy(shm_seg[i].value, "*");

            for (int j = 0; j < SUBSIZE; ++j) {
                subs[j] = shm_seg[i].subs[j];
                shm_seg[i].subs[j] = -1;
            }

            semOP( LEAVE);
            return 1;
        }
    }
    return 0;
}

int SUB(char* key, char* res, int cfd){
    printf("KVSTORE: SUB wird von KeyValueStore ausgefuehrt\n");
    printf("KEY: %s\n", key);
    for(int i=0; i < STORAGESIZE; i++) {
        if (strncmp(shm_seg[i].key, key, strlen(key)) == 0){
            strcpy(res, shm_seg[i].value);
            printf("Der Gesuchte Key wurde gefunden: %s\n", res);
            for(int j = 0; j < SUBSIZE; j++) {
                if(shm_seg[i].subs[j] < 0){
                    shm_seg[i].subs[j] = cfd;
                    return 1;
                }
            }
            return 0;
        }
    }
    return 0;
}

void QUIT(int cfd) { // unsubscribe
    for(int i=0; i < STORAGESIZE; i++) {
        for(int j = 0; j < SUBSIZE; j++) {
            if(shm_seg[i].subs[j] == cfd) {
                shm_seg[i].subs[j] = -1;
            }
        }
    }
}

int BEG() {
    transOP(ENTER);

    return 1;
}

void createValueStore() {
    int shm_id;

    shm_id = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT | 0600);
    if (shm_id < 0) {
        fprintf(stderr, "shmget: %s", strerror(errno));
    }

    shm_seg = (kv_storage *) shmat(shm_id, NULL, 0);
    if (shm_seg == (void *) -1) {
        fprintf(stderr, "shmat: %s", strerror(errno));
        exit(1);
    }

    for (int i = 0; i < STORAGESIZE; i++) {
        strcpy(shm_seg[i].key, " ");
        strcpy(shm_seg[i].value, "*");
    }

    printf("Test");
    shm_seg = (kv_storage *) shmat(shm_id, NULL, 0);
    if (shm_seg == (void *) -1) {
        fprintf(stderr, "shmat: %s", strerror(errno));
        exit(1);
    }

    for (int i = 0; i < STORAGESIZE; i++) {
        //shm_seg[i].index = i;
        strcpy(shm_seg[i].key, " ");
        strcpy(shm_seg[i].value, "*");
        for(int j = 0; j < SUBSIZE; j++){
            shm_seg[i].subs[j] = -1;
        }
    }

/*    shmid_ctr = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT | 0666);
    if (shmid_ctr == -1)
    {
        perror("shmget");
        exit(1);
    }
    counter = (int *)shmat(shmid_ctr, NULL, 0);
    if (counter == (void *)-1)
    {
        perror("shmat:");
        exit(1);
    }*/

// BEGIN testdaten für storage
    strcpy(shm_seg[0].key, "k1");
    strcpy(shm_seg[0].value, "v1");

    strcpy(shm_seg[1].key, "k2");
    strcpy(shm_seg[1].value, "v2");

    strcpy(shm_seg[2].key, "k3");
    strcpy(shm_seg[2].value, "v2");

    strcpy(shm_seg[3].key, "k4");
    strcpy(shm_seg[3].value, "v4");
// ENDE testdaten für storage

}

