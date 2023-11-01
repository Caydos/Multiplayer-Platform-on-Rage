#ifndef EVENTS_H
#define EVENTS_H
#include <typeinfo>
#include <string.h>
#include <sstream>
const char END_CHARACTER = '§';
const char SEPARATOR_CHARACTER = 'µ';
namespace Events
{
	void Listener(int socketfd);
	void Unserialize(char* buffer);
	void Register(const char* _name, void (*function)(char** _args));

	void* AddWaiting(const char* array);
	void RemoveWaiting(unsigned int index);
	void Fiber(void);
}

void CreateEvents(void);

#pragma region Triggers

class EventSv
{
public:
	char* buffer = nullptr;
	unsigned int bufferSize = 0;

	template<typename Arg>
	void SerializeArg(Arg _arg)
	{
		std::stringstream valueString;
		valueString /*<< typeid(_arg).raw_name()*/ << _arg; // Potential issue 1
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
		buffer[bufferSize - 2] = SEPARATOR_CHARACTER; // Null-terminate the string
		buffer[bufferSize - 1] = '\0'; // Null-terminate the string
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
	evtSv.buffer[evtSv.bufferSize - 2] = SEPARATOR_CHARACTER;

	((evtSv.SerializeArg(_args)), ...);
	evtSv.buffer[evtSv.bufferSize - 2] = END_CHARACTER;
	evtSv.buffer[evtSv.bufferSize - 1] = '\0';
	//std::cout << evtSv.buffer << std::endl;
	send(clientSocket, evtSv.buffer, strlen(evtSv.buffer) + 1, 0);
	delete[] targetString;
	delete[] evtSv.buffer;
	evtSv.buffer = nullptr;
	evtSv.bufferSize = 0;
};
#pragma endregion

#endif // !EVENTS_H

