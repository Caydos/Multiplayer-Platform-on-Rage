#ifndef SCRIPTING_H
#define SCRIPTING_H
#include "Natives.h"
#include "Fiber.h"

extern int playerId;

namespace Scripting
{
	void Main();
	void Init();
	void Stop();
}

#endif // !SCRIPTING_H