#include "pch.h"
#include "Events.h"
#include "EventsListing.h"
#include "Fiber.h"

bool eventInitialization = false;



class DataT
{
public:
	int* integers = nullptr;
	unsigned int intCount = 0;

	unsigned int* unsigned_integers = nullptr;
	unsigned int unsigned_intCount = 0;

	char* characters = nullptr;
	unsigned int charactersCount = 0;

	float* flotants = nullptr;
	unsigned int floatCount = 0;

	double* doubles = nullptr;
	unsigned int doubleCount = 0;

	bool* booleans = nullptr;
	unsigned int boolCount = 0;

	char** strings = nullptr;
	unsigned int stringCount = 0;


	void* AddString(const char* array)
	{
		char** temp = new char* [stringCount + 1];

		for (int i = 0; i < stringCount; i++)
		{
			temp[i] = strings[i];
		}

		temp[stringCount] = new char[strlen(array) + 1];
		strcpy(temp[stringCount], array);

		delete[] strings;

		strings = temp;
		stringCount++;
		return &strings[stringCount - 1];
	}

	void Reset(void)
	{

		free(integers);
		integers = nullptr;
		intCount = 0;

		free(unsigned_integers);
		unsigned_integers = nullptr;
		unsigned_intCount = 0;

		free(characters);
		characters = nullptr;
		charactersCount = 0;

		free(flotants);
		flotants = nullptr;
		floatCount = 0;

		free(doubles);
		doubles = nullptr;
		doubleCount = 0;

		free(booleans);
		booleans = nullptr;
		boolCount = 0;

		free(strings);
		strings = nullptr;
		stringCount = 0;
	}

};

namespace ClientEvents
{
	class Event {
	public:
		void** argStack = nullptr;
		int argCount = 0;
		const char* name;
		void (*function)(void** _args);
		char* buffer;
		unsigned int bufferSize;
		DataT DataHolder;

		void AddArg(void* value, size_t size)
		{
			int newSize = argCount + 1;

			void** newArray = (void**)realloc(argStack, newSize * sizeof(void*));

			if (newArray != nullptr)
			{
				argStack = newArray;
				argCount = newSize;

				// Allocate memory for the value and copy it
				void* newValue = malloc(size);
				memcpy(newValue, value, size);

				// Store the copied value in the array
				argStack[argCount - 1] = newValue;
			}
			else
			{
				std::cout << "Memory reallocation failed!" << std::endl;
			}
		}

		template<typename Arg>
		void SerializeArg(Arg _arg)
		{
			std::stringstream valueString;
			valueString << typeid(_arg).name() /*<< '_'*/ << _arg;
			std::string argStr = valueString.str();


			int oldBufferSize = bufferSize;
			bufferSize += argStr.length() + 1;
			buffer = (char*)realloc(buffer, bufferSize * sizeof(char));

			buffer[oldBufferSize] = 'µ';
			for (size_t i = 0; i < argStr.length(); ++i)
			{
				buffer[oldBufferSize + i + 1] = argStr[i];
			}
		}
	};

	Event* events = nullptr;
	unsigned int count = 0;

	template<class... Type>
	void Call(const char* _name, Type... _args)
	{
		for (unsigned int i = 0; i < count; i++)
		{
			if (std::strcmp(events[i].name, _name) == 0)
			{
				((events[i].AddArg(&_args, sizeof(Type))), ...);
				events[i].function(events[i].argStack);
				events[i].argCount = 0;
				free(events[i].argStack);
				events[i].argStack = (void**)malloc(sizeof(void*));
				return;
			}
		}
		std::cout << "Event not found: " << _name << std::endl;
	}


	void ArgType(const char* _arg, unsigned int _eventId)
	{
		if (_arg[0] == 'P')
		{//pointers
			if (_arg[1] == 'K' && _arg[2] == 'c')
			{//const char*
				const char* string = _arg + 3;
				void* addr = events[_eventId].DataHolder.AddString(string);
				events[_eventId].AddArg(addr, sizeof(addr));
			}
			else if (_arg[1] == 'c')
			{//char*
				const char* string = _arg + 2;
				void* addr = events[_eventId].DataHolder.AddString(string);
				events[_eventId].AddArg(addr, sizeof(addr));
			}
			else if (_arg[1] == 'v')
			{//void*
				const char* string = _arg + 2;
				void* addr = events[_eventId].DataHolder.AddString(string);
				events[_eventId].AddArg(addr, sizeof(addr));
			}
		}
		else if (_arg[0] == 'c')
		{//char
			char char_integer = std::atoi(_arg + 1);
			events[_eventId].DataHolder.charactersCount++;
			char* temp = new char[events[_eventId].DataHolder.charactersCount];
			std::memcpy(temp, events[_eventId].DataHolder.characters, (events[_eventId].DataHolder.charactersCount - 1) * sizeof(char));
			delete[] events[_eventId].DataHolder.characters;
			events[_eventId].DataHolder.characters = temp;
			events[_eventId].DataHolder.characters[events[_eventId].DataHolder.charactersCount - 1] = char_integer;
			events[_eventId].AddArg(&events[_eventId].DataHolder.characters[events[_eventId].DataHolder.charactersCount - 1], sizeof(char));
		}
		else if (_arg[0] == 'i')
		{//int
			int integer = std::atoi(_arg + 1);
			events[_eventId].DataHolder.intCount++;
			int* temp = new int[events[_eventId].DataHolder.intCount];
			std::memcpy(temp, events[_eventId].DataHolder.integers, (events[_eventId].DataHolder.intCount - 1) * sizeof(int));
			delete[] events[_eventId].DataHolder.integers;
			events[_eventId].DataHolder.integers = temp;
			events[_eventId].DataHolder.integers[events[_eventId].DataHolder.intCount - 1] = integer;
			events[_eventId].AddArg(&events[_eventId].DataHolder.integers[events[_eventId].DataHolder.intCount - 1], sizeof(int));
		}
		else if (_arg[0] == 'j')
		{//unsigned int
			unsigned int unsigned_integer = std::atoi(_arg + 1);
			events[_eventId].DataHolder.unsigned_intCount++;
			unsigned int* temp = new unsigned int[events[_eventId].DataHolder.unsigned_intCount];
			std::memcpy(temp, events[_eventId].DataHolder.unsigned_integers, (events[_eventId].DataHolder.unsigned_intCount - 1) * sizeof(unsigned int));
			delete[] events[_eventId].DataHolder.unsigned_integers;
			events[_eventId].DataHolder.unsigned_integers = temp;
			events[_eventId].DataHolder.unsigned_integers[events[_eventId].DataHolder.unsigned_intCount - 1] = unsigned_integer;
			events[_eventId].AddArg(&events[_eventId].DataHolder.unsigned_integers[events[_eventId].DataHolder.unsigned_intCount - 1], sizeof(unsigned int));
		}
		else if (_arg[0] == 'f')
		{//float
			float flotant = std::atof(_arg + 1);
			events[_eventId].DataHolder.floatCount++;
			float* temp = new float[events[_eventId].DataHolder.floatCount];
			std::memcpy(temp, events[_eventId].DataHolder.flotants, (events[_eventId].DataHolder.floatCount - 1) * sizeof(float));
			delete[] events[_eventId].DataHolder.flotants;
			events[_eventId].DataHolder.flotants = temp;
			events[_eventId].DataHolder.flotants[events[_eventId].DataHolder.floatCount - 1] = flotant;
			events[_eventId].AddArg(&events[_eventId].DataHolder.flotants[events[_eventId].DataHolder.floatCount - 1], sizeof(float));
		}
		else if (_arg[0] == 'd')
		{//double
			double db = std::atof(_arg + 1);
			events[_eventId].DataHolder.doubleCount++;
			double* temp = new double[events[_eventId].DataHolder.doubleCount];
			std::memcpy(temp, events[_eventId].DataHolder.doubles, (events[_eventId].DataHolder.doubleCount - 1) * sizeof(double));
			delete[] events[_eventId].DataHolder.doubles;
			events[_eventId].DataHolder.doubles = temp;
			events[_eventId].DataHolder.doubles[events[_eventId].DataHolder.doubleCount - 1] = db;
			events[_eventId].AddArg(&events[_eventId].DataHolder.doubles[events[_eventId].DataHolder.doubleCount - 1], sizeof(double));
		}
		else if (_arg[0] == 'b')
		{//bool
			bool boolean = std::atoi(_arg + 1);
			events[_eventId].DataHolder.boolCount++;
			bool* temp = new bool[events[_eventId].DataHolder.boolCount];
			std::memcpy(temp, events[_eventId].DataHolder.booleans, (events[_eventId].DataHolder.boolCount - 1) * sizeof(bool));
			delete[] events[_eventId].DataHolder.booleans;
			events[_eventId].DataHolder.booleans = temp;
			events[_eventId].DataHolder.booleans[events[_eventId].DataHolder.boolCount - 1] = boolean;
			events[_eventId].AddArg(&events[_eventId].DataHolder.booleans[events[_eventId].DataHolder.boolCount - 1], sizeof(bool));
		}
		else
		{
			printf("Invalid arg %s\n", _arg);
			const char* string = _arg;
			void* addr = events[_eventId].DataHolder.AddString(string);
			events[_eventId].AddArg(addr, sizeof(addr));
		}
	}

	void CallWithSerializedArgs(const char* _name, char* _args)
	{
		for (unsigned int i = 0; i < count; i++)
		{
			if (std::strcmp(events[i].name, _name) == 0)
			{
#pragma region Args treatment
				unsigned int counter = 0;
				char* bff = new char[1];
				for (char* t = _args + 1; *t != '\0'; t++)
				{
					if (*t == 'µ' || *t == '§')
					{
						bff[counter] = '\0';
						ArgType(bff, i);
						counter = 0;
						delete[] bff;
						bff = new char[1];
					}
					else
					{
						bff[counter] = *t;
						counter++;
						char* tempBuffer = new char[counter + 1];
						std::memcpy(tempBuffer, bff, counter);
						tempBuffer[counter] = '\0';
						delete[] bff;
						bff = tempBuffer;
					}
				}
				delete[] bff;
#pragma endregion
				events[i].function(events[i].argStack);
				events[i].argCount = 0;
				free(events[i].argStack);
				events[i].argStack = (void**)malloc(sizeof(void*));
				events[i].DataHolder.Reset();
				return;
			}
		}
		std::cout << "Event not found: " << _name << std::endl;
	}


	void Register(const char* _name, void (*function)(void** _args))
	{
		Event* newEvents = new Event[count + 1];
		std::memcpy(newEvents, events, count * sizeof(Event));
		delete[] events;
		events = newEvents;

		events[count].name = _name;
		events[count].function = function;
		count++;
	}

}

void RegisterClientEvent(const char* _name, void (*function)(void** _args))
{
	ClientEvents::Event* newEvents = new ClientEvents::Event[ClientEvents::count + 1];
	std::memcpy(newEvents, ClientEvents::events, ClientEvents::count * sizeof(ClientEvents::Event));
	delete[] ClientEvents::events;
	ClientEvents::events = newEvents;

	ClientEvents::events[ClientEvents::count].name = _name;
	ClientEvents::events[ClientEvents::count].function = function;
	ClientEvents::count++;
}


void UnserializeEvent(char* _buffer)
{
#pragma region Event Name Guess
	char* _evtName = nullptr;
	unsigned int counter = 0;
	for (char* t = _buffer; *t != '\0'; t++)
	{
		if (*t == 'µ' || *t == '§')
		{
			break;
		}
		counter++;
		_evtName = (char*)realloc(_evtName, counter * sizeof(char));
		_evtName[counter - 1] = *t;
	}
	_evtName[counter] = '\0';
#pragma endregion
#pragma region Arguments stacking
	unsigned int bufferL = strlen(_buffer);
	unsigned int nameL = strlen(_evtName);
	char* argBuffer = new char[bufferL - nameL];

	for (unsigned int i = 0; i < bufferL - nameL; i++)
	{
		argBuffer[i] = _buffer[nameL + i];
	}
	argBuffer[bufferL - nameL] = '\0';

#pragma endregion
	//printf("Event is : %s with buffer %s\n", _evtName, argBuffer);
	ClientEvents::CallWithSerializedArgs(_evtName, argBuffer);
	free(_evtName);
}



void CreateEvents(void)
{
	if (eventInitialization)
	{
		printf("Event has already been initialized\n");
		return;
	}
	eventInitialization = true;
	RegisterEvents();
}




char** waitingEvents = nullptr;
unsigned int waitingEventsCount = 0;
bool editingEvents = false;

void* AddWaitingEvent(const char* array)
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
void RemoveWaitingEvent(unsigned int index)
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

void EventFiber()
{
	while (1)
	{
		while (editingEvents)
		{
			Fibers::Suspend(0);
		}
		if (waitingEventsCount > 0)
		{
			UnserializeEvent(waitingEvents[0]);
			RemoveWaitingEvent(0);
		}
		Fibers::Suspend(0);
	}
}