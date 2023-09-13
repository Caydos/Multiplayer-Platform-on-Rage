#include "pch.h"
#include "Threads.h"
#include "Logger.h"


void CommandsLoop()
{
	while (true)
	{
		char msg[100];
		std::cin.getline(msg, 100);
		if (strcmp(msg, "connect") == 0)
		{
			if (!Connect())
			{
				Logger::Fatal(L"Server may be offline or you are already connected", L"Server connection");
				break;
			}

			printf("Connection confirmed\n\n");
		}
		else if (strcmp(msg, "disconnect") == 0)
		{
			//unload();
			Logger::Fatal(L"Disconnection completed", L"Server disconnection");
		}
	}
	return;
}

void Thread::ConsoleCommands()
{
	std::thread consoleCommandsThread(CommandsLoop);
	consoleCommandsThread.detach();
}
