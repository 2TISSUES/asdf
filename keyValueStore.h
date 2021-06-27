#ifndef SERVER_KEYVALUESTORE_H
#define SERVER_KEYVALUESTORE_H
#define BUFFSIZE 50 // Groeße des Buffers
#define STORAGESIZE 50 // Groeße des Storage
#define SUBSIZE 5 // maximale Anzahl der Subs


typedef struct kv_storage {
    char key[BUFFSIZE];
    char value[BUFFSIZE];
    int subs[SUBSIZE];
} kv_storage, *kv_storage_p;

kv_storage storage[STORAGESIZE];
kv_storage *shm_seg;

int *counter;
int shmid_ctr;

void createValueStore();
int GET(char* key, char* res);
int PUT(char* key, char* value, int subs[SUBSIZE]);
int DEL(char* key, int subs[SUBSIZE]);
int SUB(char* key, char* res, int cfd);
void QUIT(int cfd);

#endif //SERVER_KEYVALUESTORE_H
