#ifndef EVENT_H
#define EVENT_H

//
//// "-1�_eventName�arg01�"
//namespace ServerEvents
//{
//	class Event {
//	public:
//		void** argStack = nullptr;
//		int argCount = 0;
//		const char* name;
//		void (*function)(void** _args);
//		void AddArg(void* value)
//		{
//			int newSize = argCount + 1;
//
//			void** newArray = (void**)realloc(argStack, newSize * sizeof(void*));
//
//			if (newArray != nullptr)
//			{
//				argStack = newArray;
//				argCount = newSize;
//
//				argStack[argCount - 1] = value;
//			}
//			else
//			{
//				std::cout << "Memory reallocation failed!" << std::endl;
//			}
//		}
//	};
//
//	Event* events = nullptr;
//	unsigned int count = 0;
//
//	template<class... Type>
//	void Call(const char* _name, Type... _args)
//	{
//		for (unsigned int i = 0; i < count; i++)
//		{
//			if (std::strcmp(events[i].name, _name) == 0)
//			{
//				((events[i].AddArg(&_args)), ...);
//				events[i].function(events[i].argStack);
//				events[i].argCount = 0;
//				free(events[i].argStack);
//				events[i].argStack = (void**)malloc(sizeof(void*));
//				return;
//			}
//		}
//		std::cout << "Event not found: " << _name << std::endl;
//	}
//
//	void Register(const char* _name, void (*function)(void** _args)) {
//		Event* newEvents = new Event[count + 1];
//		std::memcpy(newEvents, events, count * sizeof(Event));
//		delete[] events;
//		events = newEvents;
//
//		events[count].name = _name;
//		events[count].function = function;
//		count++;
//	}
//}

#endif // !EVENT_H
