#include "Header.h"
#include "Events.h"
#include "SyncTree.h"

extern int sockfd;
extern int playerCount;
extern Client playerList[MAX_CLIENTS];
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
		if (integers != nullptr)
		{
			free(integers);
			integers = nullptr;
		}
		intCount = 0;
		if (unsigned_integers != nullptr)
		{
			free(unsigned_integers);
			unsigned_integers = nullptr;
		}
		unsigned_intCount = 0;
		if (characters != nullptr)
		{
			free(characters);
			characters = nullptr;
		}
		charactersCount = 0;
		if (integers != nullptr)
		{
			free(flotants);
			flotants = nullptr;
		}
		floatCount = 0;
		if (doubles != nullptr)
		{
			free(doubles);
			doubles = nullptr;
		}
		doubleCount = 0;
		if (booleans != nullptr)
		{
			free(booleans);
			booleans = nullptr;
		}
		boolCount = 0;
		if (strings != nullptr)
		{
			for (unsigned int i = 0; i < stringCount; i++)
			{
				if (strings[i] != nullptr)
				{
					delete[] strings[i];
				}
			}
			if (strings != nullptr)
			{
				delete[] strings;
				strings = nullptr;
			}
		}
		stringCount = 0;
	}

};

namespace ServerEvents
{
	class Event {
	public:
		void** argStack = nullptr;
		int argCount = 0;
		const char* name;
		void (*function)(void** _args);
		char* buffer = nullptr;
		unsigned int bufferSize = 0;
		DataT DataHolder;

		void AddArg(void* value, size_t size)
		{
			void** newArray = new void* [argCount + 1];
			if (newArray != nullptr)
			{
				if (argStack != nullptr)
				{
					for (int i = 0; i < argCount; i++)
					{
						newArray[i] = argStack[i];
					}

					delete[] argStack;
				}

				argStack = newArray;
				argCount++;
				argStack[argCount - 1] = value;
			}
			else
			{
				std::cout << "Memory allocation failed!" << std::endl;
			}
		}


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
				if (events[i].argStack != nullptr)
				{
					delete[] events[i].argStack;
				}
				events[i].argStack = nullptr;
				return;
			}
		}
		std::cout << "Event not found: " << _name << std::endl;
	}


	void ArgType(const char* _arg, unsigned int _eventId)
	{
		if (_arg[0] == '.')
		{
			if (_arg[1] == 'H')
			{
				int integer = std::atoi(_arg + 2);
				int* temp = new int[events[_eventId].DataHolder.intCount + 1];
				std::memcpy(temp, events[_eventId].DataHolder.integers, events[_eventId].DataHolder.intCount * sizeof(int));
				temp[events[_eventId].DataHolder.intCount] = integer;
				delete[] events[_eventId].DataHolder.integers;
				events[_eventId].DataHolder.integers = temp;
				events[_eventId].DataHolder.intCount++;

				events[_eventId].AddArg(&events[_eventId].DataHolder.integers[events[_eventId].DataHolder.intCount - 1], sizeof(int));
			}
			else if (_arg[1] == 'I')
			{
				printf("%s\n", _arg+2);
				unsigned int unsigned_integer = std::atoi(_arg + 2);
				unsigned int* temp = new unsigned int[events[_eventId].DataHolder.unsigned_intCount + 1];
				std::memcpy(temp, events[_eventId].DataHolder.unsigned_integers, events[_eventId].DataHolder.unsigned_intCount * sizeof(unsigned int));
				temp[events[_eventId].DataHolder.unsigned_intCount] = unsigned_integer;
				delete[] events[_eventId].DataHolder.unsigned_integers;
				events[_eventId].DataHolder.unsigned_integers = temp;
				events[_eventId].DataHolder.unsigned_intCount++;

				events[_eventId].AddArg(&events[_eventId].DataHolder.unsigned_integers[events[_eventId].DataHolder.unsigned_intCount - 1], sizeof(unsigned int));
			}
			else if (_arg[1] == 'D')
			{
				char char_integer = std::atoi(_arg + 2);
				char* temp = new char[events[_eventId].DataHolder.charactersCount + 1];
				std::memcpy(temp, events[_eventId].DataHolder.characters, events[_eventId].DataHolder.charactersCount * sizeof(char));
				temp[events[_eventId].DataHolder.charactersCount] = char_integer;
				delete[] events[_eventId].DataHolder.characters;
				events[_eventId].DataHolder.characters = temp;
				events[_eventId].DataHolder.charactersCount++;

				events[_eventId].AddArg(&events[_eventId].DataHolder.characters[events[_eventId].DataHolder.charactersCount - 1], sizeof(char));
			}
			else if (_arg[1] == 'M')
			{
				float flotant = std::atof(_arg + 2);
				float* temp = new float[events[_eventId].DataHolder.floatCount + 1];
				std::memcpy(temp, events[_eventId].DataHolder.flotants, events[_eventId].DataHolder.floatCount * sizeof(float));
				temp[events[_eventId].DataHolder.floatCount] = flotant;
				delete[] events[_eventId].DataHolder.flotants;
				events[_eventId].DataHolder.flotants = temp;
				events[_eventId].DataHolder.floatCount++;

				events[_eventId].AddArg(&events[_eventId].DataHolder.flotants[events[_eventId].DataHolder.floatCount - 1], sizeof(float));
			}
			else if (_arg[1] == 'N')
			{
				double db = std::atof(_arg + 2);
				double* temp = new double[events[_eventId].DataHolder.doubleCount + 1];
				std::memcpy(temp, events[_eventId].DataHolder.doubles, events[_eventId].DataHolder.doubleCount * sizeof(double));
				temp[events[_eventId].DataHolder.doubleCount] = db;
				delete[] events[_eventId].DataHolder.doubles;
				events[_eventId].DataHolder.doubles = temp;
				events[_eventId].DataHolder.doubleCount++;

				events[_eventId].AddArg(&events[_eventId].DataHolder.doubles[events[_eventId].DataHolder.doubleCount - 1], sizeof(double));
			}
			else if (_arg[1] == '_' && _arg[2] == 'N')
			{
				bool boolean = std::atoi(_arg + 3);
				bool* temp = new bool[events[_eventId].DataHolder.boolCount + 1];
				std::memcpy(temp, events[_eventId].DataHolder.booleans, events[_eventId].DataHolder.boolCount * sizeof(bool));
				temp[events[_eventId].DataHolder.boolCount] = boolean;
				delete[] events[_eventId].DataHolder.booleans;
				events[_eventId].DataHolder.booleans = temp;
				events[_eventId].DataHolder.boolCount++;

				events[_eventId].AddArg(&events[_eventId].DataHolder.booleans[events[_eventId].DataHolder.boolCount - 1], sizeof(bool));
			}
			else if (_arg[1] == 'P')
			{
				if (_arg[2] == 'E')
				{
					if (_arg[3] == 'B' || _arg[3] == 'D')
					{
						//printf("Zoni\n");;
						events[_eventId].AddArg(events[_eventId].DataHolder.AddString(_arg + 4), sizeof(_arg + 4));
					}
					else if (_arg[3] == 'A')
					{
						if (_arg[4] == 'D')
						{
							printf("Daaa\n");
							char* string = (char*)_arg + 4;
							void* addr = events[_eventId].DataHolder.AddString(string);
							events[_eventId].AddArg(addr, sizeof(addr));
							free(string);
						}
						else if (_arg[4] == 'X')
						{// Impossible with void type
							printf("impossible\n");
							char* string = (char*)_arg + 4;
							void* addr = events[_eventId].DataHolder.AddString(string);
							events[_eventId].AddArg(addr, sizeof(addr));
							free(string);
						}
					}
				}
			}
			else
			{
				printf("Unknown arg %s\n", _arg);
				char* string = (char*)_arg;
				void* addr = events[_eventId].DataHolder.AddString(string);
				events[_eventId].AddArg(addr, sizeof(addr));
				free(string);
			}
		}
		else
		{
			printf("Invalid arg %s\n", _arg);
			char* string = (char*)_arg;
			void* addr = events[_eventId].DataHolder.AddString(string);
			events[_eventId].AddArg(addr, sizeof(addr));
			free(string);
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
					printf("%c\n", *t);
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
				if (bff != nullptr)
				{
					delete[] bff;
				}
#pragma endregion
				printf("Got to there\n");
				//events[i].function(events[i].argStack);
				events[i].argCount = 0;
				if (events[i].argStack != nullptr)
				{
					delete[] events[i].argStack;
					events[i].argStack = nullptr;
				}
				events[i].DataHolder.Reset();//
				printf("Got to there2\n");
				return;
			}
		}
		std::cout << "Event not found: " << _name << std::endl;
	}


	void Register(const char* _name, void (*function)(void** _args))
	{
		Event* newEvents = new Event[count + 1];
		if (events != nullptr)
		{
			std::memcpy(newEvents, events, count * sizeof(Event));
			delete[] events;
		}
		events = newEvents;

		events[count].name = _name;
		events[count].function = function;
		count++;
	}

}

void RegisterServerEvent(const char* _name, void (*function)(void** _args))
{
	ServerEvents::Event* newEvents = new ServerEvents::Event[ServerEvents::count + 1];
	if (ServerEvents::events != nullptr)
	{
		std::memcpy(newEvents, ServerEvents::events, ServerEvents::count * sizeof(ServerEvents::Event));
		delete[] ServerEvents::events;
	}
	ServerEvents::events = newEvents;

	ServerEvents::events[ServerEvents::count].name = _name;
	ServerEvents::events[ServerEvents::count].function = function;
	ServerEvents::count++;
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

		char* tempName = new char[counter + 1];

		if (_evtName != nullptr)
		{
			std::memcpy(tempName, _evtName, counter);
			delete[] _evtName;
		}

		_evtName = tempName;
		_evtName[counter] = *t;

		counter++;
	}
	//_evtName[counter - 1] = '\0';
#pragma endregion
#pragma region Arguments stacking
	unsigned int bufferL = strlen(_buffer);
	unsigned int nameL = strlen(_evtName);
	int withoutNameL = bufferL - nameL;

	printf("%d\n", withoutNameL);
	if (withoutNameL < 0)
	{
		printf("Invalid buffer\n");
		if (_evtName != nullptr)
		{
			delete[] _evtName;
		}
		return;
	}
	char* argBuffer = new char[withoutNameL + 1];

	for (unsigned int i = 0; i < withoutNameL; i++)
	{
		argBuffer[i] = _buffer[nameL + i];
	}
	argBuffer[withoutNameL - 2] = '§';
	argBuffer[withoutNameL - 1] = '\0';
#pragma endregion
	ServerEvents::CallWithSerializedArgs(_evtName, argBuffer);
	if (_evtName != nullptr)
	{
		delete[] _evtName;
	}
	if (argBuffer != nullptr)
	{
		delete[] argBuffer;
	}
}



void CreateEvents(void)
{
	if (eventInitialization)
	{
		printf("Event has already been initialized\n");
		return;
	}
	eventInitialization = true;
#pragma region SyncTree
	ServerEvents::Register("PlayerConnection", &PlayerConnection);
	ServerEvents::Register("Way_01", &Way_01);
#pragma endregion
}



