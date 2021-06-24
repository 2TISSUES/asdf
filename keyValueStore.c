#include "stdio.h"
#include "keyValueStore.h"
#include <sys/shm.h>
#define SEGSIZE 10000
#include <stdlib.h>
#include "string.h"
#include <errno.h>

int GET(char* key, char* res) {
    printf("KVSTORE: GET wird von KeyValueStore ausgefuehrt\n");
    printf("KEY: %s\n", key);
    for(int i=0; i < STORAGESIZE; i++) {
        if (strncmp(shm_seg[i].key, key, strlen(key)) == 0) {
            strcpy(res, shm_seg[i].value);
            printf("Der Gesuchte Key wurde gefunden: %s\n", res);
            return 1;
        }
    }
    return 0;
}

int PUT(char* key, char* value) {
    printf("KVSTORE: PUT wird von KeyValueStore ausgefuehrt\n");
    for(int i=0; i < STORAGESIZE; i++) {
        if (strncmp(shm_seg[i].key, key, strlen(key)) == 0) {
            strcpy(shm_seg[i].value, value);
            return 1;
        }
    }

    for(int i=0; i < STORAGESIZE; i++) {
        if (strncmp(shm_seg[i].key, " ", 1) == 0) {
            strcpy(shm_seg[i].key, key);
            strcpy(shm_seg[i].value, value);
            return 1;
        }
    }
    return 0;
}

int DEL(char* key) {
    printf("KVSTORE: DEL wird von KeyValueStore ausgefuehrt\n");

    for(int i=0; i < STORAGESIZE; i++) {
        if (strncmp(shm_seg[i].key, key, strlen(key)) == 0) {
            strcpy(shm_seg[i].key, " ");
            strcpy(shm_seg[i].value, "*");
            return 1;
        }
    }
    return 0;
}

void createValueStore() {
    int shm_id;

    shm_id = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT | 0600);
    if (shm_id < 0) {
        fprintf(stderr, "shmget: %s", strerror(errno));
    }

    printf("Hallo");
    shm_seg = (kv_storage *) shmat(shm_id, NULL, 0);
    if (shm_seg == (void *) -1) {
        fprintf(stderr, "shmat: %s", strerror(errno));
        exit(1);
    }

    for (int i = 0; i < STORAGESIZE; i++) {
        //shm_seg[i].index = i;
        strcpy(shm_seg[i].key, " ");
        strcpy(shm_seg[i].value, "*");
    }

// BEGIN testdaten für storage
    //shm_seg[0].index = 0;
    strcpy(shm_seg[0].key, "KEY1");
    strcpy(shm_seg[0].value, "VALUE1");

    //shm_seg[1].index = 1;
    strcpy(shm_seg[1].key, "KEY2");
    strcpy(shm_seg[1].value, "VALUE2");

    //shm_seg[2].index = 2;
    strcpy(shm_seg[2].key, "KEY3");
    strcpy(shm_seg[2].value, "VALUE3");

    strcpy(shm_seg[3].key, "hallo");
    strcpy(shm_seg[3].value, "arrivederci");

// ENDE testdaten für storage
}

