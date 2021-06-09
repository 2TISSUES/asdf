#include "shMem.h"

#define SEGSIZE 10000


/* Nun wird das Shared Memory Segment angefordert, an den Prozess   */
/* angehängt, und der dort gespreicherte Wert auf 0 gesetzt         */
shm_id = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT | 0600);
if (shm_id < 0) {
fprintf(stderr, "shmget: %s", strerror(errno));
}

shm_seg = (kv_storage *) shmat(shm_id, NULL, 0);
if (shm_seg == (void *) -1) {
fprintf(stderr, "shmat: %s", strerror(errno));
exit(1);
}
