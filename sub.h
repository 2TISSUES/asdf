#ifndef SERVER_SUB_H
#define SERVER_SUB_H
#define PORT 5678
#define ENTER 1
#define LEAVE 0
#include "stdio.h"


int rfd; // Rendevouz-Descriptor
int cfd; // Verbindungs-Descriptor
int pid;
int semID, transID, subID, mqID;

int clientConnection ();
void initServer();
int initSemaphore();
void semOP( int );
void getSemVal(char *);
void getSubVal(char *);
void getTransVal(char *);
void transOP(int op);
void exitHandler (int);
void clearAll();
void subOP (int);
void initMQ();

#endif //SERVER_SUB_H