#include "keyValueStore.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "shMem.h"

in[strcspn(in, "\r\n")] = 0;

char * befehl;
char * key;
char * value;

befehl = strtok(in, " \0");
key = strtok(NULL, " ");
value = strtok(NULL, "\0");

void fillStore () {
    for (int i = 1; i <= STORAGESIZE; i++) {
        shm_seg[i].index = i;
        strcpy(shm_seg[i].key, " ");
        strcpy(shm_seg[i].value, "*");
    }
}

void get(char * key) {
    for (int i = 1; i <= STORAGESIZE; i++) {
        if ((strcmp(shm_seg[i].key, key) != 0) {
            printf("DATA: Index: %d, Key: %s, Value: %s\n", shm_seg[i].index, shm_seg[i].key,
                   shm_seg[i].value);
        } else {
            printf("%d Key nicht gefunden", shm_seg[i].key);
        }
    }
}

void put(char * key, char * value, int index) {
    for (int i = 1; i <= STORAGESIZE; i++) {
        if ((strcmp(shm_seg[i].key, str1) != 0) && (strcmp(shm_seg[i].value, str2) != 0)) {
            printf("Index bereits vergeben\n");
        } else {
            strncpy(shm_seg[i].key, key);
            strncpy(shm_seg[i].value, value);
            printf("DATA: Index: %d, Key: %s, Value: %s\n", shm_seg[i].index, shm_seg[i].key,
                   shm_seg[i].value);
            break;
        }
    }
}void change(char * key, char * value, int index) {
    for (int i = 1; i <= STORAGESIZE; i++) {
        if ((strcmp(shm_seg[i].key, str1) == 0) && (strcmp(shm_seg[i].value, str2) == 0)) {
            strncpy(shm_seg[i].key, key);
            strncpy(shm_seg[i].value, value);
            printf("DATA: Index: %d, Key: %s, Value: %s\n", shm_seg[i].index, shm_seg[i].key,
                   shm_seg[i].value);
            break;
        } else {
            printf("Index bereits leer");
        }
    }
}

void del(char * key) {
    for (int i = 1; i <= STORAGESIZE; i++) {
        if ((strcmp(shm_seg[i].key, key) != 0) {
            strcpy(shm_seg[i].key, " ");
            strcpy(shm_seg[i].value, "*");
            printf("Key deleted");
        } else if {
            printf("%d Key nicht gefunden", shm_seg[i].key);
        }
    }
}

void quit(int cfd) {
    if (close(cfd) < 0) {
        fprintf(stderr, "close: %s\n", strerror(errno));
    }
}


