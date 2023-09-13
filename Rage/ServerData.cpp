#include "pch.h"
#include "Threads.h"
#include "Events.h"
#include "Fiber.h"
extern SOCKET clientSocket;

void Thread::ServerPackages()
{
	while (true)
	{
		char buffer[EVENT_BUFFER_SIZE];
		ZeroMemory(buffer, EVENT_BUFFER_SIZE);
		int bytesRead = recv(clientSocket, buffer, EVENT_BUFFER_SIZE, 0);
		if (bytesRead > 0)
		{
			char* newBuffer = new char[EVENT_BUFFER_SIZE];
			std::memcpy(newBuffer, buffer, bytesRead);
			newBuffer[bytesRead] = '\0'; // Null-terminate the buffer

			int newSize = EVENT_BUFFER_SIZE;
			while (bytesRead >= EVENT_BUFFER_SIZE && newBuffer[newSize - 1] != '§')
			{

				int oldSize = newSize;
				ZeroMemory(buffer, EVENT_BUFFER_SIZE);
				bytesRead = recv(clientSocket, buffer, EVENT_BUFFER_SIZE, 0);
				newSize += bytesRead;
				char* tempBuff = new char[newSize];
				// Including old buffer
				for (unsigned int i = 0; i < oldSize; i++)
				{
					tempBuff[i] = newBuffer[i];
				}

				// Inserting new buffer
				for (unsigned int i = 0; i < bytesRead; i++)
				{// last index
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
			AddWaitingEvent(newBuffer);
			delete[] newBuffer;
		}
		else if (bytesRead == 0)
		{
			printf("Connection lost with server\n");
			break;
		}
		else
		{
			printf("Connection lost with server\n");
			break;
		}
	}
	printf("Server packages thread ended\n");
}