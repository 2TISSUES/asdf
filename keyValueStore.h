#ifndef SERVER_KEYVALUESTORE_H
#define SERVER_KEYVALUESTORE_H
#define BUFFSIZE 50 // Groeße des Buffers
#define STORAGESIZE 50 // Groeße des Storage
#define SUBSIZE 10 // maximale Anzahl der Subs
#include "unistd.h"

typedef struct kv_storage {
    char key[BUFFSIZE];
    char value[BUFFSIZE];
} kv_storage, *kv_storage_p;

/*struct data {
    char key[30];
    char value[80];
};*/

typedef struct subMsg {
    long subPid;
    char msg[80];
}subMsg;

typedef struct subData {
    pid_t subPid;
    char subKey[BUFFSIZE];
}subData;

typedef struct subscriptions {
    subData subs[STORAGESIZE];
}subscriptions;

/*struct keyValStore {
    struct data storePos[20];
    struct subs subPos[10];
    int currentEntries;
    pid_t begPid;
};*/

kv_storage storage[STORAGESIZE];
kv_storage *shm_seg;
subscriptions *shm_seg_subs;

int shm_id;
int shm_id_store;
int shm_id_subs;
struct keyValStore *pStore;

void createValueStore();
int GET(char* key, char* res);
int PUT(char* key, char* value);
int DEL(char* key);
int SUB(char* key);
void QUIT(int cfd);
void notifySubs(char*, char*);

#endif //SERVER_KEYVALUESTORE_H
