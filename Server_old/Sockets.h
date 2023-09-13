#ifndef SOCKETS_H
#define SOCKETS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>/*Close, usleep*/

void Wait(unsigned int _miliseconds);

#pragma region Connection / Disconnection
void ConnectPlayer(int _clientId);
void DisconnectPlayer(int _clientIndex);
void ConnectionListener(void);
#pragma endregion


#pragma region Player
void* EventListener(int _clientIndex);
void* EventSender(int _clientIndex);
#pragma endregion

#endif // !SOCKETS_H
