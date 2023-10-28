#include "Header.h"
#include "Connections.h"
#include "Events.h"

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
			std::cout << "Failed to accept Connection" << std::endl;
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
	List[ConnectionsCount].threadId = List[ConnectionsCount].eventListener.get_id();
	List[ConnectionsCount].eventListener.detach();

	std::cout << "New connection with id : " << ConnectionsCount << std::endl;
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
	if (connectionId != -2)
	{
		for (int i = 0; i < ConnectionsCount; i++)
		{
			if (connectionId == -1 || List[i].id == connectionId)
			{
				//printf("Triggering to %d : %s\n", List[i].id, _buffer);
				write(List[i].socket, _buffer, strlen(_buffer));
			}
		}
	}
	else
	{
		std::thread::id currentThreadId = std::this_thread::get_id();
		bool found = false;
		for (int i = 0; i < ConnectionsCount; i++)
		{
			std::cout << i << " " << ConnectionsCount << std::endl;
			if (!found)
			{
				if (List[i].threadId == currentThreadId)
				{
					found = true;
					std::cout << "found him " << i << std::endl;
				}
				else
				{
					std::cout << "Trigger to " << i << " " << List[i].threadId << " " << currentThreadId << std::endl;
					write(List[i].socket, _buffer, strlen(_buffer));
				}
			}
			else
			{
				std::cout << "Trigger to " << i << " " << List[i].threadId << " " << currentThreadId << std::endl;
				write(List[i].socket, _buffer, strlen(_buffer));
			}
		}
	}
}

std::int64_t Connections::GetDiscordId(void)
{
	std::thread::id currentThreadId = std::this_thread::get_id();

	for (int i = 0; i < ConnectionsCount; i++)
	{
		if (List[i].threadId == currentThreadId)
		{
			return List[i].discordId;
		}
	}
}

void Connections::SetDiscordId(std::int64_t _id)
{
	std::thread::id currentThreadId = std::this_thread::get_id();

	for (int i = 0; i < ConnectionsCount; i++)
	{
		if (List[i].threadId == currentThreadId)
		{
			List[i].discordId = _id;
		}
	}
}