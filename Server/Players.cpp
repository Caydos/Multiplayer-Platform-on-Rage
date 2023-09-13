#include "Header.h"
#include "Events.h"

extern int sockfd;
extern int playerCount;
extern Client playerList[MAX_CLIENTS];

void Wait(unsigned int _miliseconds)
{
	usleep(_miliseconds * 1000);
}

void ConnectPlayer(int _clientId)
{
	playerCount++;
	printf("\nNew client Connection : %d\n\n", playerCount);
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (playerList[i].isConnected != 1)
		{
			playerList[i].id = i;
			playerList[i].socketId = _clientId;
			playerList[i].isConnected = 1;

			playerList[i].eventListener = std::thread(EventListenerFunk, i);
			playerList[i].eventListener.detach();

			playerList[i].eventSender = std::thread(EventSenderFunk, i);
			playerList[i].eventSender.detach();
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
	//printf("Connection Listner Ready\n");
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


void EventListenerFunk(int _clientIndex)
{
	//printf("Event Listing for %d\n", playerList[_clientIndex].id);
	while (playerList[_clientIndex].isConnected == 1)
	{
		char buffer[EVENT_BUFFER_SIZE];
		bzero(buffer, EVENT_BUFFER_SIZE);
		int bytesRead = read(playerList[_clientIndex].socketId, buffer, EVENT_BUFFER_SIZE);
		if (bytesRead > 0)
		{
			char* newBuffer = new char[EVENT_BUFFER_SIZE];
			std::memcpy(newBuffer, buffer, bytesRead);
			newBuffer[bytesRead] = '\0'; // Null-terminate the buffer

			int newSize = EVENT_BUFFER_SIZE;
			while (bytesRead >= EVENT_BUFFER_SIZE && newBuffer[newSize - 1] != '§')
			{
				int oldSize = newSize;
				bzero(buffer, EVENT_BUFFER_SIZE);
				bytesRead = read(playerList[_clientIndex].socketId, buffer, EVENT_BUFFER_SIZE);
				newSize += bytesRead;
				char* tempBuff = new char[newSize];
				// Including old buffer
				for (unsigned int i = 0; i < oldSize; i++)
				{
					tempBuff[i] = newBuffer[i];
				}

				// Inserting new buffer
				for (unsigned int i = 0; i < bytesRead; i++)
				{//			 last index
					tempBuff[oldSize + i] = buffer[i];
					if (buffer[i] == '§')
					{
						break;
					}
				}

				delete[] newBuffer;
				newBuffer = tempBuff;
			}
			newBuffer[newSize] = '\0';

			UnserializeEvent(newBuffer);
			delete[] newBuffer;
		}


		else if (bytesRead == 0)
		{
			DisconnectPlayer(_clientIndex);
			break;
		}
		else
		{
			DisconnectPlayer(_clientIndex);
			break;
		}
	}
}

void EventSenderFunk(int _clientIndex)
{
	//printf("Event Sending for %d\n", playerList[_clientIndex].id);
	while (playerList[_clientIndex].isConnected == 1)
	{

	}
}