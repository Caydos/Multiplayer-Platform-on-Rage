#include "pch.h"
#include "Events.h"
#include "EventsListing.h"
#include "Fiber.h"
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)  



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
	char* eventBuffer = nullptr;
	int size = 0;
	while (true)
	{
		int bytesRead = recv(socketfd, msgBuffer, ARRAY_SIZE, 0);
		if (bytesRead <= 0)
		{
			std::cout << "Error reading data." << std::endl;
			delete[] eventBuffer;
			//Connections::Lost(socketfd);
			break;
		}
		//std::cout << "recv : " << msgBuffer << std::endl;
		void* result = std::memchr(msgBuffer, END_CHARACTER, ARRAY_SIZE);
		if (result != nullptr)
		{//Found
			char* position = (char*)result;
			int index = static_cast<int>(position - msgBuffer);

			int length = (eventBuffer == nullptr) ? 0 : strlen(eventBuffer);
			int nSize = (eventBuffer == nullptr) ? index : length + index;
			char* temp = new char[nSize + 1];
			if (eventBuffer != nullptr)
			{
				std::memcpy(temp, eventBuffer, length);
				delete[] eventBuffer;
			}
			std::memcpy(temp + length, msgBuffer, index);
			eventBuffer = temp;
			// Trigger;
			std::cout << "Event is : " << eventBuffer << std::endl;
			Events::AddWaiting(eventBuffer);
			delete[] eventBuffer;
			eventBuffer = nullptr;
			size = 0;
			// Copy for the next event
			if (index < ARRAY_SIZE)
			{
				char* nextEvt = msgBuffer + index + 1;
				int nextSize = strlen(nextEvt);
				if (nextSize > 1)
				{
					eventBuffer = new char[nextSize + 1];
					std::memcpy(eventBuffer, nextEvt, nextSize);
					eventBuffer[nextSize] = '\0';
					size = nextSize;
				}
			}
		}
		else
		{
			if (eventBuffer == nullptr)
			{
				eventBuffer = new char[ARRAY_SIZE + 1];
				size = ARRAY_SIZE;
				std::memcpy(eventBuffer, msgBuffer, ARRAY_SIZE);
				eventBuffer[ARRAY_SIZE] = '\0';
			}
			else
			{
				char* temp = new char[size + ARRAY_SIZE + 1];
				std::memcpy(temp, eventBuffer, size);
				delete[] eventBuffer;
				std::memcpy(temp + size, msgBuffer, ARRAY_SIZE);
				temp[size + ARRAY_SIZE] = '\0';
				eventBuffer = temp;
				size += ARRAY_SIZE;
				if (strlen(eventBuffer) > BUFFER_OVERSIZE)
				{
					delete[] eventBuffer;
					std::cout << "Server sent too much informations" << std::endl;
					return;
				}
			}
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
			//std::this_thread::sleep_for(std::chrono::milliseconds(10));
			Fibers::Suspend(10);
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


bool eventInitialization = false;
void CreateEvents(void)
{
	if (eventInitialization)
	{
		std::cout << "Event has already been initialized" << std::endl;
		return;
	}
	eventInitialization = true;
	RegisterEvents();
}



char** waitingEvents = nullptr;
unsigned int waitingEventsCount = 0;
bool editingEvents = false;

void* Events::AddWaiting(const char* array)
{
	char** temp = new char* [waitingEventsCount + 1];

	for (unsigned int i = 0; i < waitingEventsCount; i++)
	{
		temp[i] = waitingEvents[i];
	}

	temp[waitingEventsCount] = new char[strlen(array) + 1];
	strcpy(temp[waitingEventsCount], array);

	delete[] waitingEvents;

	waitingEvents = temp;
	waitingEventsCount++;
	return &waitingEvents[waitingEventsCount - 1];
}

void Events::RemoveWaiting(unsigned int index)
{
	if (index >= waitingEventsCount)
		return;

	delete[] waitingEvents[index];

	for (unsigned int i = index; i < waitingEventsCount - 1; i++)
	{
		waitingEvents[i] = waitingEvents[i + 1];
	}

	waitingEventsCount--;
}

#include "Natives.h"
void Events::Fiber(void)
{
	while (1)
	{
		while (editingEvents)
		{
			Fibers::Suspend(0);
		}
		if (waitingEventsCount > 0)
		{
			Events::Unserialize(waitingEvents[0]);
			RemoveWaiting(0);
		}

		Fibers::Suspend(0);
	}
}