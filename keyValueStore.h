#ifndef SERVER_KEYVALUESTORE_H
#define SERVER_KEYVALUESTORE_H
#define BUFFSIZE 50 // Groeße des Buffers
#define STORAGESIZE 50 // Groeße des Storage

typedef struct kv_storage {
    char key[BUFFSIZE];
    char value[BUFFSIZE];
} kv_storage, *kv_storage_p;

kv_storage storage[STORAGESIZE];
kv_storage *shm_seg;
int *counter;
int shmid_ctr;

void createValueStore();
int GET(char* key, char* res);
int PUT();
int DEL();
void QUIT();



#endif //SERVER_KEYVALUESTORE_H
