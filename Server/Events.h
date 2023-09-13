#ifndef EVENT_H
#define EVENT_H
#define EVENT_BUFFER_SIZE 500

void CreateEvents(void);
void UnserializeEvent(char* _buffer);

void RegisterClientEvent(const char* _name, void (*function)(void** _args));

class EventCl 
{
public:
	char* buffer = nullptr;
	unsigned int bufferSize = 0;

	template<typename Arg>
	void SerializeArg(Arg _arg)
	{
		std::stringstream valueString;
		valueString << typeid(_arg).name() /*<< '_'*/ << _arg;
		std::string argStr = valueString.str();


		int argLength = argStr.length();

		int oldBufferSize = bufferSize - 1;
		bufferSize += (argLength + 1); // +1 for the null-terminator

		char* newBuffer = new char[bufferSize + 1];
		if (newBuffer == nullptr)
		{
			printf("Allocation error here\n");
		}

		if (buffer != nullptr)
		{
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
		buffer[bufferSize - 1] = '\0'; // Null-terminate the string
	}
};


template<class...Arg>
void TriggerClientEvent(int _target, const char* _name, Arg ..._args)
{
	extern int playerCount;
	extern Client playerList[MAX_CLIENTS];
	EventCl evtcl;
	int nameLength = strlen(_name);

	char* targetString = new char[nameLength + 1];
	strcpy(targetString, _name);
	
	evtcl.bufferSize = strlen(targetString) + 2;
	evtcl.buffer = new char[evtcl.bufferSize];
	strcpy(evtcl.buffer, targetString);
	evtcl.buffer[evtcl.bufferSize - 2] = 'µ';

	((evtcl.SerializeArg(_args)), ...);
	evtcl.buffer[evtcl.bufferSize - 2] = '§';
	printf("%s\n", evtcl.buffer);
	delete[] targetString;
	for (unsigned int i = 0; i < playerCount; i++)
	{
		if (_target == -1 || playerList[i].id == _target)
		{
			printf("Triggering to %d : %s\n", playerList[i].id, evtcl.buffer);
			//write(playerList[i].socketId, evtcl.buffer, evtcl.bufferSize + 1);
		}
	}
	return;
}

#endif // !EVENT_H
