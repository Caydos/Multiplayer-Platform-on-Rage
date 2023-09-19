#include "Header.h"
#include "Connections.h"
#include "Events.h"

typedef struct
{
	int id;
	int socket;

	std::thread eventListener;

	bool alive;
	std::thread verifications;
} Client;

Client List[MAX_CONNECTIONS];
int ConnectionsCount = 0;
int historyCount = 0;

void Connections::Detect(int socketfd)
{
	while (true)
	{
		socklen_t clilen;
		struct sockaddr_in cli_addr;

		listen(socketfd, 5);
		clilen = sizeof(cli_addr);
		int connectionId = accept(socketfd, (struct sockaddr*)&cli_addr, &clilen);
		if (connectionId < 0)
		{
			printf("Failed to accept Connection\n");
		}
		else
		{
			Accept(connectionId);
		}
	}
}

void Connections::Accept(int socketfd)
{
	List[ConnectionsCount].id = historyCount;
	List[ConnectionsCount].alive = true;
	List[ConnectionsCount].socket = socketfd;

	List[ConnectionsCount].eventListener = std::thread(Events::Listener, socketfd);
	List[ConnectionsCount].eventListener.detach();

	printf("New connection with id : %d\n", ConnectionsCount);
	ConnectionsCount++;
	historyCount++;

}

void Connections::Reject(int socketfd)
{

}

void Connections::Remove(int connectionId)
{
	// kill the event thread
	for (unsigned int i = 0; i < ConnectionsCount; i++)
	{

	}
}

void Connections::Lost(int socketfd)
{
	bool foundSocket = false;
	for (int i = 0; i < ConnectionsCount; i++)
	{
		if (List[i].socket == socketfd)
		{
			std::cout << "Connection lost with : " << i << std::endl;
			foundSocket = true;
		}
		if (foundSocket)
		{
			std::memcpy(&List[i], &List[i + 1], sizeof(Client));
		}
	}
	ConnectionsCount--;
}

void Connections::Kick(int socketfd, const char* _reason)
{
	bool foundSocket = false;
	for (int i = 0; i < ConnectionsCount; i++)
	{
		if (List[i].socket == socketfd)
		{
			std::cout << "Kick : " << i << " , reason :" << _reason << std::endl;
			foundSocket = true;
		}
		if (foundSocket)
		{
			std::memcpy(&List[i], &List[i + 1], sizeof(Client));
		}
	}
	ConnectionsCount--;
}

void Connections::SendData(int connectionId, char* _buffer)
{
	for (unsigned int i = 0; i < ConnectionsCount; i++)
	{
		if (connectionId == -1 || List[i].id == connectionId)
		{
			//printf("Triggering to %d : %s\n", List[i].id, _buffer);
			write(List[i].socket, _buffer, strlen(_buffer));
		}
	}
}