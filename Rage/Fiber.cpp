#include "pch.h"
#include "Fiber.h"

fiber* fibers = new fiber[1];
unsigned int fiberCount = 0;

void Fibers::Create(const char* _name, void(*function)())
{
	fiberCount++;
	fiber* temp = new fiber[fiberCount];
	std::memcpy(temp, fibers, (fiberCount - 1) * sizeof(fiber));
	delete[] fibers;
	fibers = temp;
	fibers[fiberCount - 1].name = _name;
	fibers[fiberCount - 1].script_fiber = CreateFiber(0, (LPFIBER_START_ROUTINE)function, &fibers[fiberCount - 1]);
	fibers[fiberCount - 1].function = function;
	fibers[fiberCount - 1].wake_at = GetTickCount();
}

void Fibers::Init(const char* _name, void(*function)())
{
	for (unsigned int i = 0; i < fiberCount; i++)
	{
		if (strcmp(_name, fibers[i].name) == 0)
		{
			fibers[i].script_fiber = CreateFiber(0, (LPFIBER_START_ROUTINE)function, &fibers[i]);
			fibers[i].function = function;
			fibers[i].wake_at = GetTickCount();
		}
	}
}


void Fibers::Delete()
{
	void* actualFiber = GetCurrentFiber();
	bool foundId = false;
	for (unsigned int i = 0; i < fiberCount; i++)
	{
		if (!foundId)
		{
			if (actualFiber == fibers[i].script_fiber)
			{
				foundId = true;
				DeleteFiber(fibers[i].script_fiber);
			}
		}
		else if (i < fiberCount + 1)
		{
			fibers[i] = fibers[i + 1];
		}
	}
	fiberCount--;
}

void Fibers::Tick()
{
	void* actualFiber = GetCurrentFiber();
	for (unsigned int i = 0; i < fiberCount; i++)
	{
		if (actualFiber == fibers[i].script_fiber)
		{
			if (fibers[i].wake_at <= GetTickCount())
			{
				fibers[i].main_fiber = IsThreadAFiber() ? GetCurrentFiber() : ConvertThreadToFiber(NULL);

				SwitchToFiber(fibers[i].script_fiber);
			}
		}
	}
}

void Fibers::TickAll(void)
{
	for (unsigned int i = 0; i < fiberCount; i++)
	{
		if (fibers[i].wake_at <= GetTickCount())
		{
			fibers[i].main_fiber = IsThreadAFiber() ? GetCurrentFiber() : ConvertThreadToFiber(NULL);

			SwitchToFiber(fibers[i].script_fiber);
		}
	}
}

void Fibers::Suspend(uint32_t time)
{
	void* actualFiber = GetCurrentFiber();
	for (unsigned int i = 0; i < fiberCount; i++)
	{
		if (actualFiber == fibers[i].script_fiber)
		{
			fibers[i].wake_at = GetTickCount() + time;

			SwitchToFiber(fibers[i].main_fiber);
		}
	}
}