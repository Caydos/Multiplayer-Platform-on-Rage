#include "Header.h"
#include "Sockets.h"

void Wait(unsigned int _miliseconds)
{
	usleep(_miliseconds * 1000);
}

#pragma region Connection / Disconnection
void ConnectPlayer(int _clientId)
{
	playerCount++;
	printf("\nNew client Connection : %d\n\n", playerCount);
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (playerList[i].id == 0)
		{
			playerList[i].id = i;
			playerList[i].socketId = _clientId;
			playerList[i].isConnected = 1;
			//std::thread eventListener (EventListener, i);
			//eventListener.detach();
			//std::thread eventSender (EventSender, i);
			//eventSender.detach();
			break;
		}
	}
}


void DisconnectPlayer(int _clientIndex)
{
	playerList[_clientIndex].isConnected = 0;
	close(playerList[_clientIndex].socketId);
	playerList[_clientIndex].socketId = 0;
	printf("Client with id %d was disconnected from server", playerList[_clientIndex].id);
	playerList[_clientIndex].id = 0;
	playerCount--;
}

void ConnectionListener(void)
{
	printf("Connection Listner Ready\n");
	while (1)
	{
		Wait(1000);
		if (playerCount < MAX_CLIENTS)
		{
			socklen_t clilen;
			struct sockaddr_in cli_addr;

			listen(sockfd, 5);
			clilen = sizeof(cli_addr);
			int clientId = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
			if (clientId < 0)
			{
				printf("Failed to accept Player Connection");
			}
			else
			{
				ConnectPlayer(clientId);
			}
		}
	}
}
#pragma endregion


#pragma region Player
void* EventListener(int _clientIndex)
{
	Client actualClient = playerList[_clientIndex];
	printf("Event Listing for %d\n", actualClient.socketId);
	while (actualClient.isConnected == 1)
	{
		char buffer[200];
		bzero(buffer, 200);
		int bytes = read(actualClient.socketId, buffer, 200 - 1);
		if (bytes > 0)
		{
			printf("Event Recieved : %s\n", buffer);
		}
		else if (bytes == 0)
		{
			DisconnectPlayer(_clientIndex);
			break;
		}
		else
		{
			DisconnectPlayer(_clientIndex);
			printf("Error recieving bytes\n");
			break;
		}
	}
}

void* EventSender(int _clientIndex)
{
	Client actualClient = playerList[_clientIndex];
	printf("Event Sending for %d\n", actualClient.socketId);
	while (actualClient.isConnected == 1)
	{

	}
}
#pragma endregion