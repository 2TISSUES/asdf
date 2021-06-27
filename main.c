#include "sub.h"
#include "keyValueStore.h"

int main(void) {
    createValueStore();
    initSemaphore();
    initMQ();
    initServer();
}
