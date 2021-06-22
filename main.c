#include <stdio.h>
#include <stdlib.h>
#include "connection.h"
#include "keyValueStore.h"
#define TRUE 1
#define ENDLOSSCHLEIFE 1
#define PORT 5678


//kv_storage storage[STORAGESIZE];

int main(void) {
    createValueStore();
    /*char res[50];
    GET("KEY1", res);
    printf("Der Value für den Key KEY1: %s\n", res);*/
    initServer();

    /*PUT("KEY4", "VALUE4");
    printf("Der Value für den Key KEY4: %s\n", GET("KEY4"));

    PUT("KEY5", "VALUE5");
    DEL("KEY4");

    printf("Der Value für den Key KEY4: %s\n", GET("KEY4"));

    printf("Der Value für den Key KEY1: %s\n", GET("KEY1"));

    DEL("KEY1");
    printf("Der Value für den Key KEY1: %s\n", GET("KEY1"));*/
}