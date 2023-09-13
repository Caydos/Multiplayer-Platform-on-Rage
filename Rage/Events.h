#ifndef EVENTS_H
#define EVENTS_H
#include <typeinfo>
#include <string.h>
#include <sstream>

void CreateEvents(void);
void UnserializeEvent(char* _buffer);


class EventSv
{
public:
	char* buffer = nullptr;
	unsigned int bufferSize = 0;

	template<typename Arg>
	void SerializeArg(Arg _arg)
	{
		std::stringstream valueString;
		valueString << typeid(_arg).raw_name() << _arg; // Potential issue 1
		std::string argStr = valueString.str();

		int argLength = argStr.length();

		int oldBufferSize = bufferSize - 1;
		bufferSize += (argLength + 1); // +1 for the null-terminator

		char* newBuffer = new char[bufferSize + 1];

		if (buffer != nullptr) {
			std::memcpy(newBuffer, buffer, oldBufferSize);
			delete[] buffer;
		}

		buffer = newBuffer;
		//std::cout << argStr << ' ' << oldBufferSize << std::endl;
	
		for (int i = 0; i < argLength; i++)
		{
			buffer[oldBufferSize + i] = argStr[i];
		}
		buffer[bufferSize - 2] = 'µ'; // Null-terminate the string
		buffer[bufferSize-1] = '\0'; // Null-terminate the string
		//printf("%s\n", buffer);
	}
};
extern SOCKET clientSocket;

template<typename Name, class...Arg>
void TriggerServerEvent(Name _name, Arg..._args)
{
	EventSv evtSv;
	int nameLength = strlen(_name);

	char* targetString = new char[nameLength];
	sprintf(targetString, "%s", _name);
	evtSv.bufferSize = strlen(targetString) + 2;
	evtSv.buffer = new char[evtSv.bufferSize];
	strcpy(evtSv.buffer, targetString);
	evtSv.buffer[evtSv.bufferSize - 2] = 'µ';

	((evtSv.SerializeArg(_args)), ...);
	evtSv.buffer[evtSv.bufferSize-2] = '§';

	send(clientSocket, evtSv.buffer, evtSv.bufferSize, 0);
	delete[] targetString;
	delete[] evtSv.buffer;
	evtSv.buffer = nullptr;
	evtSv.bufferSize = 0;
};



void RegisterClientEvent(const char* _name, void (*function)(void** _args));


void* AddWaitingEvent(const char* array);
void EventFiber();
#endif // !EVENTS_H

