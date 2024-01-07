#ifndef NETWORK_H
#define NETWORK_H
#include "Framework.h"

namespace Network
{
	namespace Connection
	{
		bool Create(const char* _address, unsigned int _port);
		void Close(const char* _address, unsigned int _port);
		void Send(const char* _buffer, unsigned int _connectionId = 0);
		void Shutdown(void);
	}
	namespace Events
	{
		const char END_CHARACTER = '§';
		const char SEPARATOR_CHARACTER = 'µ';
		const unsigned int ARRAY_SIZE = 1024;
		const unsigned int BUFFER_OVERSIZE = 30000;

		void Listener(int _socketfd);
		void Unserialize(char* _buffer);
		void Register(const char* _name, void (*_function)(char** _args));
		void CreateEvents(void);

		void* AddWaiting(const char* _array);
		void RemoveWaiting(unsigned int _index);
		void Fiber(void);

		class EventSv
		{
		public:
			char* buffer = nullptr;
			unsigned int bufferSize = 0;

			template<typename Arg>
			void SerializeArg(Arg _arg)
			{
				std::stringstream valueString;
				valueString /*<< typeid(_arg).raw_name()*/ << _arg;
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

				for (int i = 0; i < argLength; i++)
				{
					buffer[oldBufferSize + i] = argStr[i];
				}
				buffer[bufferSize - 2] = SEPARATOR_CHARACTER;
				buffer[bufferSize - 1] = '\0';
			}
		};


		template<typename Name, class...Arg>
		void TriggerServer(Name _name, Arg..._args)
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
			std::cout << evtSv.buffer << std::endl;
			Network::Connection::Send(evtSv.buffer, 0);
			delete[] targetString;
			delete[] evtSv.buffer;
			evtSv.buffer = nullptr;
			evtSv.bufferSize = 0;
		};
		template<typename Name, class...Arg>
		void Trigger(Name _name, Arg..._args)
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
			Events::AddWaiting(evtSv.buffer);
			delete[] targetString;
			delete[] evtSv.buffer;
			evtSv.buffer = nullptr;
			evtSv.bufferSize = 0;
		}
	}
}

#endif // !NETWORK_H