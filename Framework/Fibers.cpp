#include "Fibers.h"

using namespace Fibers;
std::vector<Fiber*> fibers;

void Fibers::Create(const char* _name, void(*_function)())
{
	Fiber* fiber = new Fiber;
	fiber->name = _name;
	fiber->script_fiber = CreateFiber(0, (LPFIBER_START_ROUTINE)_function, fiber);
	fiber->function = _function;
	fiber->wake_at = GetTickCount();

	fibers.push_back(fiber);
}

void Fibers::DeleteCurrent()
{
	void* actualFiber = GetCurrentFiber();
	for (unsigned int i = 0; i < fibers.size(); i++)
	{
		if (actualFiber == fibers[i]->script_fiber)
		{
			DeleteFiber(fibers[i]->script_fiber);
			delete fibers[i];
			fibers.erase(fibers.begin() + i);
			return;
		}
	}
	Logger::GetFile() << "Failed to delete current fiber" << std::endl;
}

void Fibers::TickCurrent(void)
{
	void* actualFiber = GetCurrentFiber();
	for (unsigned int i = 0; i < fibers.size(); i++)
	{
		if (actualFiber == fibers[i]->script_fiber)
		{
			if (fibers[i]->wake_at <= GetTickCount())
			{
				fibers[i]->main_fiber = IsThreadAFiber() ? GetCurrentFiber() : ConvertThreadToFiber(NULL);

				SwitchToFiber(fibers[i]->script_fiber);
				return;
			}
		}
	}
	Logger::GetFile() << "Failed to tick current fiber" << std::endl;
}

void Fibers::TickAll(void)
{
	for (unsigned int i = 0; i < fibers.size(); i++)
	{
		if (fibers[i]->wake_at <= GetTickCount())
		{
			fibers[i]->main_fiber = IsThreadAFiber() ? GetCurrentFiber() : ConvertThreadToFiber(NULL);

			SwitchToFiber(fibers[i]->script_fiber);
		}
	}
}

void Fibers::Suspend(uint32_t _time)
{
	void* actualFiber = GetCurrentFiber();
	for (unsigned int i = 0; i < fibers.size(); i++)
	{
		if (actualFiber == fibers[i]->script_fiber)
		{
			fibers[i]->wake_at = GetTickCount() + _time;

			SwitchToFiber(fibers[i]->main_fiber);
		}
	}
	Logger::GetFile() << "Failed to suspend current fiber" << std::endl;
}
