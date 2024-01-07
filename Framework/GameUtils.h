#ifndef GAME_UTILS_H
#define GAME_UTILS_H
#include "Framework.h"
#include "Types.h"

namespace GameUtils
{
	using ptr_to_handleType = Entity (*)(void*);
	struct GtaPointers
	{
		ptr_to_handleType ptr_to_handle;
	};
	GtaPointers* GetGtaPointers();
	void Hook(void);
}

#endif // !GAME_UTILS_H