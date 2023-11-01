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
	std::unique_lock<std::shared_mutex> lock(shConnexionMtx);
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
	// TriggerClientEvent(?, "Connection rejected", "Too many players");
}

void Connections::Remove(int _socketfd)
{
	std::unique_lock<std::shared_mutex> lock(shConnexionMtx);
	bool foundSocket = false;
	for (int i = 0; i < connectionsCount; i++)
	{
		if (clients[i].socket == _socketfd)
		{
			std::cout << "Connection removed with : " << i << std::endl;
			foundSocket = true;
		}
		if (foundSocket)
		{
			//std::memcpy(&clients[i], &clients[i + 1], sizeof(Client));
		}
	}
	connectionsCount--;
}

void Connections::Lost(int _socketfd)
{
	Remove(_socketfd);
}

void Connections::Kick(int _socketfd, const char* _reason)
{
	Remove(_socketfd);
	std::cout << "User kicked for : " << _reason << std::endl;
}

void Connections::SendData(int _connectionId, char* _buffer)
{
	std::shared_lock<std::shared_mutex> lock(shConnexionMtx);
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
	std::shared_lock<std::shared_mutex> lock(shConnexionMtx);
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
	std::shared_lock<std::shared_mutex> lock(shConnexionMtx);
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
	std::shared_lock<std::shared_mutex> lock(shConnexionMtx);
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
