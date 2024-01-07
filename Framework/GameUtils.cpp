#include "GameUtils.h"
#include "Memory.h"
#include "Natives.h"
#include "Pools.h"

GameUtils::GtaPointers gta_pointers;

GameUtils::GtaPointers* GameUtils::GetGtaPointers()
{
	return &gta_pointers;
}

void GameUtils::Hook(void)
{
	Memory::handle ptrToHandleHandle = Memory::handle(Memory::ScanPattern(scan_base, scan_length,
		"\x48\x8B\xF9\x48\x83\xC1\x10\x33\xDB",
		"xxxxxxxxx"));

	gta_pointers.ptr_to_handle = ptrToHandleHandle.sub(0x15).as<GameUtils::ptr_to_handleType>();
}
