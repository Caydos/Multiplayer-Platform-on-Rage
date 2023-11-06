#include "Header.h"
#include "Events.h"
#include <algorithm>
#include "Connections.h"
#include "DataHolder.h"

class Event
{
public:
	char** argStack = nullptr;
	int argCount = 0;
	const char* name;
	void (*function)(char** _args);
	bool processing = false;

	void AddArg(const char* array)
	{
		char** temp = new char* [argCount + 1];

		for (int i = 0; i < argCount; i++)
		{
			temp[i] = argStack[i];
		}

		temp[argCount] = new char[strlen(array) + 1];
		strcpy(temp[argCount], array);

		delete[] argStack;

		argStack = temp;
		argCount++;
	}
	void Call(void)
	{
		function(argStack);
		// Reset
		if (argStack != nullptr)
		{
			for (unsigned int i = 0; i < argCount; i++)
			{
				if (argStack[i] != nullptr)
				{
					delete[] argStack[i];
				}
			}
			if (argStack != nullptr)
			{
				delete[] argStack;
				argStack = nullptr;
			}
		}
		argCount = 0;
		processing = false;
	}
};

Event* events = nullptr;
unsigned int count = 0;

const unsigned int ARRAY_SIZE = 1024;
const unsigned int BUFFER_OVERSIZE = 30000;

void Events::Listener(int socketfd)
{
	char msgBuffer[ARRAY_SIZE];
	std::string eventBuffer;
	while (true)
	{
		int bytesRead = recv(socketfd, msgBuffer, ARRAY_SIZE, 0);
		if (bytesRead <= 0)
		{
			std::cout << "Error reading data." << std::endl;
			eventBuffer.clear();
			Connections::Lost(socketfd);
			break;
		}
		eventBuffer += msgBuffer;

		//std::cout << "recv : " << msgBuffer << std::endl;
		const char* result = static_cast<const char*>(std::memchr(msgBuffer, END_CHARACTER, ARRAY_SIZE));
		if (result != nullptr)
		{
			const char* position = result;
			int index = position - msgBuffer;
			eventBuffer.erase(index);

			/* Do somthing with the event */

			//std::cout << "Event is : " << eventBuffer << std::endl;
			Events::Unserialize((char*) eventBuffer.c_str());


			eventBuffer.clear();
			char* leftData = &msgBuffer[index + 1];
			eventBuffer += leftData;
			std::cout << eventBuffer << std::endl;
		}
		if (eventBuffer.size() > BUFFER_OVERSIZE)
		{
			eventBuffer.clear();
			Connections::Kick(socketfd, "Buffer Oversize happened");
			return;
		}
		bzero(msgBuffer, sizeof(msgBuffer));
	}
}


void Events::Unserialize(char* _buffer)
{
	void* result = std::memchr(_buffer, SEPARATOR_CHARACTER, strlen(_buffer));
	if (result != nullptr)
	{//Found
		char* position = (char*)result;
		int index = static_cast<int>(position - _buffer);
		char* name = new char[index + 1];
		std::memcpy(name, _buffer, index);
		name[index] = '\0';

		//printf("Event name : %s\n", name);
		int eventId = -1;
		for (int i = 0; i < count; i++)
		{
			if (strcmp(events[i].name, name) == 0)
			{
				//std::cout << "Found event : " << events[i].name << std::endl;
				eventId = i;
				break;
			}
		}
		if (eventId < 0)
		{
			std::cout << "Event not found : " << name << std::endl;
			return;
		}

		// Lock
		while (events[eventId].processing == true)
		{
			//std::cout << "Waitin room" << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		events[eventId].processing = true;

		int lastIndex = index + 1;
		while (true)
		{
			char* nBuffer = _buffer + lastIndex;
			void* marker = std::memchr(nBuffer, SEPARATOR_CHARACTER, strlen(nBuffer));
			if (marker != nullptr)
			{
				char* markerPosition = (char*)marker;
				int markerIndex = static_cast<int>(markerPosition - nBuffer);
				char* newArg = new char[markerIndex + 1];
				std::memcpy(newArg, nBuffer, markerIndex);
				newArg[markerIndex] = '\0';
				//std::cout << "Arg : " << newArg << std::endl;
				events[eventId].AddArg(newArg);
				delete[] newArg;
				lastIndex += (markerIndex + 1);
			}
			else
			{
				events[eventId].AddArg(nBuffer);
				break;
			}
		}
		events[eventId].Call();
		delete[] name;
	}
	else
	{
		std::cout << "Unserializing Error : " << _buffer << std::endl;
	}

}

void Events::Register(const char* _name, void(*function)(char** _args))
{
	Event* newEvents = new Event[count + 1];
	if (count > 0)
	{
		std::memcpy(newEvents, events, count * sizeof(Event));
		delete[] events;
	}
	newEvents[count].name = _name;
	newEvents[count].function = function;
	newEvents[count].processing = false;
	newEvents[count].argStack = nullptr;
	newEvents[count].argCount = 0;
	events = newEvents;

	count++;
}


