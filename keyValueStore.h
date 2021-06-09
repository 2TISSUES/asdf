#ifndef PRAKBS21TEAM13_KEYVALUESTORE_H
#define PRAKBS21TEAM13_KEYVALUESTORE_H

#endif //PRAKBS21TEAM13_KEYVALUESTORE_H

#define BUFFSIZE 256 // Größe des Buffers
#define STORAGESIZE 5 // Größe des Storage


typedef struct kv_storage {
    char key[BUFFSIZE];
    char value[BUFFSIZE];
    int index;
} kv_storage, *kv_storage_p;

void get(char * key);
void put(char * key, char * value, int index);
void del(char * key);
void quit();