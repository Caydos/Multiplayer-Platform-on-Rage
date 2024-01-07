#ifndef FIBER_H
#define FIBER_H
#include "Framework.h"



namespace Fibers
{
	struct Fiber
	{
		std::string name;
		void* script_fiber;
		void* main_fiber;
		void (*function)();
		uint32_t wake_at;
	};
	void Create(const char* _name, void(*_function)());
	void DeleteCurrent(void);
	void TickCurrent(void);
	void TickAll(void);
	void Suspend(uint32_t _time);
}

#endif // !FIBER_H
