#ifndef HEADER_H
#define HEADER_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <thread>

#define SERVER_PORT "55000"
#define MAX_CLIENTS 15

typedef struct
{
	unsigned int id;
	unsigned int socketId;

	std::thread eventListener;
	std::thread eventSender;
	
	unsigned int isConnected;
	unsigned int bufferSize;
	char* buffer;
} Client;


void Wait(unsigned int _miliseconds);

#pragma region Connection / Disconnection
void ConnectPlayer(int _clientId);
void DisconnectPlayer(int _clientIndex);

void ConnectionListener(void);
#pragma endregion


#pragma region Player
void EventListenerFunk(int _clientIndex);
void EventSenderFunk(int _clientIndex);
#pragma endregion

#endif //HEADER_H