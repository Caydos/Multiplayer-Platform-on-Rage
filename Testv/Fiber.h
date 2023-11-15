#pragma once
#ifndef FIBER_H
#define FIBER_H
#include <stdint.h>

#include <functional>
struct fiber
{
	const char* name;
	void* script_fiber;
	void* main_fiber;
	void (*function)();
	std::function<void()> function2;
	uint32_t wake_at;
};


namespace Fibers
{
	void Create(const char* _name, void(*function)());
	void Init(const char* _name, void(*function)());
	void Delete();
	void Tick();
	void TickAll(void);
	void Suspend(uint32_t time);
}

#endif // !FIBER_H
