#include "Header.h"
#include "Connections.h"
#include "Events.h"

std::shared_mutex shConnexionMtx;
Client clients[MAX_CONNECTIONS];
int connectionsCount = 0;

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
			std::cout << "Failed to accept Connection" << std::endl;
		}
		else
		{
			Accept(connectionId);
		}
	}
}

void Connections::Accept(int _socketfd)
{
	clients[connectionsCount].id = connectionsCount;
	clients[connectionsCount].alive = true;
	clients[connectionsCount].socket = _socketfd;

	clients[connectionsCount].eventListener = std::thread(Events::Listener, _socketfd);
	clients[connectionsCount].threadId = clients[connectionsCount].eventListener.get_id();
	clients[connectionsCount].eventListener.detach();

	std::cout << "New connection with id : " << connectionsCount << std::endl;
	connectionsCount++;
}

void Connections::Reject(int _socketfd)
{

}

void Connections::Remove(int _socketfd)
{
	for (unsigned int i = 0; i < connectionsCount; i++)
	{

	}
}

void Connections::Lost(int _socketfd)
{
	bool foundSocket = false;
	for (int i = 0; i < connectionsCount; i++)
	{
		if (clients[i].socket == _socketfd)
		{
			std::cout << "Connection lost with : " << i << std::endl;
			foundSocket = true;
		}
		if (foundSocket)
		{
			std::memcpy(&clients[i], &clients[i + 1], sizeof(Client));
		}
	}
	connectionsCount--;
}

void Connections::Kick(int _socketfd, const char* _reason)
{
	bool foundSocket = false;
	for (int i = 0; i < connectionsCount; i++)
	{
		if (clients[i].socket == _socketfd)
		{
			std::cout << "Kick : " << i << " , reason :" << _reason << std::endl;
			foundSocket = true;
		}
		if (foundSocket)
		{
			std::memcpy(&clients[i], &clients[i + 1], sizeof(Client));
		}
	}
	connectionsCount--;
}

void Connections::SendData(int _connectionId, char* _buffer)
{
	if (_connectionId != -2)
	{
		for (int i = 0; i < connectionsCount; i++)
		{
			if (_connectionId == -1 || clients[i].id == _connectionId)
			{
				write(clients[i].socket, _buffer, strlen(_buffer));
			}
		}
	}
	else
	{
		std::thread::id currentThreadId = std::this_thread::get_id();
		bool found = false;
		for (int i = 0; i < connectionsCount; i++)
		{
			std::cout << i << " " << connectionsCount << std::endl;
			if (!found)
			{
				if (clients[i].threadId == currentThreadId)
				{
					found = true;
				}
				else
				{
					write(clients[i].socket, _buffer, strlen(_buffer));
				}
			}
			else
			{
				write(clients[i].socket, _buffer, strlen(_buffer));
			}
		}
	}
}

std::int64_t Connections::GetDiscordId(void)
{
	std::thread::id currentThreadId = std::this_thread::get_id();

	for (int i = 0; i < connectionsCount; i++)
	{
		if (clients[i].threadId == currentThreadId)
		{
			return clients[i].discordId;
		}
	}
}

void Connections::SetDiscordId(std::int64_t _id)
{
	std::thread::id currentThreadId = std::this_thread::get_id();

	for (int i = 0; i < connectionsCount; i++)
	{
		if (clients[i].threadId == currentThreadId)
		{
			clients[i].discordId = _id;
		}
	}
}

int Connections::GetLocalThreadId(void)
{
	std::thread::id currentThreadId = std::this_thread::get_id();

	for (int i = 0; i < connectionsCount; i++)
	{
		if (clients[i].threadId == currentThreadId)
		{
			return clients[i].id;
		}
	}
	return 0;
}
