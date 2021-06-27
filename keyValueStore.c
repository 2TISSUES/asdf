#include <sys/shm.h>
#include <stdlib.h>
#include <errno.h>
#include "string.h"
#include "stdio.h"
#include "keyValueStore.h"
#include "sub.h"
#include <unistd.h>
#include <sys/msg.h>

#define SEGSIZE 10000

int GET(char* key, char* res) {
    getSemVal( "vor ENTER" );
    semOP (ENTER);
    getSemVal("nach ENTER");
    for(int i=0; i < STORAGESIZE; i++) {
        if (strncmp(shm_seg[i].key, key, strlen(key)) == 0) {
            strcpy(res, shm_seg[i].value);
            semOP( LEAVE);
            getSemVal("nach LEAVE");
            return 1;
        }
    }
    semOP(LEAVE);
    getSemVal("nach LEAVE");
    return 0;
}

int PUT(char* key, char* value) {
    printf("PUT\n");
    semOP (ENTER);
    for(int i=0; i < STORAGESIZE; i++) {
        if (strncmp(shm_seg[i].key, key, strlen(key)) == 0) {
            strcpy(shm_seg[i].value, value);
//            for (int j = 0; j < SUBSIZE; ++j) {
//                subs[j] = shm_seg[i].subs[j];
//            }
            semOP (LEAVE);
            getSemVal("nach LEAVE");
            return 1;
        }
    }

    for(int i=0; i < STORAGESIZE; i++) {
        if (strncmp(shm_seg[i].key, " ", 1) == 0) {
            strcpy(shm_seg[i].key, key);
            strcpy(shm_seg[i].value, value);
//            for (int j = 0; j < SUBSIZE; ++j) {
//                subs[j] = shm_seg[i].subs[j];
//            }
            semOP(LEAVE);
            getSemVal("nach LEAVE");
            return 1;
        }
    }
    semOP(LEAVE);
    getSemVal("nach LEAVE");
    return 0;
}

int DEL(char* key) {
    printf("DEL\n");
    getSemVal( "vor ENTER" );
    semOP (ENTER);
    getSemVal("nach ENTER");
    for(int i=0; i < STORAGESIZE; i++) {
        if (strncmp(shm_seg[i].key, key, strlen(key)) == 0) {
            strcpy(shm_seg[i].key, " ");
            strcpy(shm_seg[i].value, "*");

//            for (int j = 0; j < SUBSIZE; ++j) {
//                subs[j] = shm_seg[i].subs[j];
//                shm_seg[i].subs[j] = -1;
//            }

            semOP(LEAVE);
            getSemVal("nach LEAVE");
            return 1;
        }
    }
    semOP(LEAVE);
    getSemVal("nach LEAVE");
    return 0;
}

int SUB(char* key){
    getSubVal("vor ENTER");
    subOP(ENTER);
    getSubVal("nach ENTER");
    for(int i=0; i < STORAGESIZE; i++) {
        if (strncmp(shm_seg_subs->subs[i].subKey, " ", 1) == 0) {
            strcpy(shm_seg_subs->subs[i].subKey, key);
            shm_seg_subs->subs[i].subPid = getpid();
            subOP(LEAVE);
            getSubVal("nach LEAVE");
            return 1;
        }
    }
    subOP(LEAVE);
    getSubVal("nach LEAVE");
    return 0;
}

void createValueStore() {

    shm_id = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT | 0600);
    if (shm_id < 0) {
        fprintf(stderr, "shmget: %s", strerror(errno));
    }

    shm_seg = (kv_storage *) shmat(shm_id, NULL, 0);
    if (shm_seg == (void *) -1) {
        fprintf(stderr, "shmat: %s", strerror(errno));
        exit(1);
    }

    shm_id_subs = shmget(IPC_PRIVATE, sizeof(struct subscriptions), IPC_CREAT|0644);
    shm_seg_subs = shmat(shm_id_subs, NULL, 0);

    for (int i = 0; i < STORAGESIZE; i++) {
        strcpy(shm_seg[i].key, " ");
        strcpy(shm_seg[i].value, "*");
    }

    for (int i = 0; i < STORAGESIZE; i++) {
        strcpy(shm_seg_subs->subs[i].subKey, " ");
        shm_seg_subs->subs[i].subPid = 0;
    }


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

void notifySubs(char* key, char* msg){
    printf("checking subs\n");
    subOP(ENTER);
    for(int i=0; i < STORAGESIZE; i++) {
        if (strncmp(shm_seg_subs->subs[i].subKey, key, strlen(key)) == 0) {
            subMsg mySubMsg = {shm_seg_subs->subs[i].subPid};
            memset(mySubMsg.msg, 0, 80);
            strcpy(mySubMsg.msg, msg);
            printf("queued msg: %s\n", mySubMsg.msg);
            msgsnd(mqID, &mySubMsg, strlen(mySubMsg.msg), 0);
        }
    }
    subOP(LEAVE);
    return;
}