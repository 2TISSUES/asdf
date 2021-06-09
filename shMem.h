#ifndef PRAKBS21TEAM13_SHMEM_H
#define PRAKBS21TEAM13_SHMEM_H

#endif //PRAKBS21TEAM13_SHMEM_H

#include <sys/ipc.h>
#include "keyValueStore.h"
#include <sys/shm.h>

int shm_id;
kv_storage *shm_seg; /*  id für das Shared Memory Segment        */
/*  mit *shar_mem kann der im Shared Memory */
/*  gespeicherte Wert verändert werden      */