#ifndef PCH_H
#define PCH_H
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>/*bzero*/
#include <thread>

#define SERVER_PORT "55000"
int sockfd;
#define MAX_CLIENTS 15
int playerCount;

typedef struct
{
	unsigned int id;
	unsigned int socketId;
	unsigned int isConnected;
	unsigned int bufferSize;
	char* buffer;
} Client;
Client playerList[MAX_CLIENTS];

#endif //PCH_H