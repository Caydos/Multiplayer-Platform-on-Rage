#pragma once
#ifndef FIBER_H
#define FIBER_H

typedef struct fiber
{
	const char* name;
	void* script_fiber;
	void* main_fiber;
	void (*function)();
	uint32_t wake_at;
} fiber;


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
