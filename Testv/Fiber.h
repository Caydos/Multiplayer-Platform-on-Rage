#ifndef FIBER_H
#define FIBER_H
#include <stdint.h>
#include <string>

struct fiber
{
	std::string name;
	void* script_fiber;
	void* main_fiber;
	void (*function)();
	void** content;
	uint32_t wake_at;
};


namespace Fibers
{
	void Create(const char* _name, void(*function)(), void** _content = nullptr);
	void Init(const char* _name, void(*function)());
	void Delete();
	void DeleteFromName(const char* _name);
	void Tick();
	void TickAll(void);
	void Suspend(uint32_t time);
	void** GetContent(void);
	void SetContentValue(const char* _name, int _valueId, void* _value);
}

#endif // !FIBER_H
