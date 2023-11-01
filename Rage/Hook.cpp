#include "pch.h"
#include "Hook.h"
#include "Process.h"
#include "Scripting.h"
#include "Fiber.h"

GtaThreadVtbl* thread_vftable;

GtaThread* get_current_script_thread()
{
	char* tls;

	tls = *(char**)__readgsqword(0x58);

	return *(GtaThread**)(tls + 0x848);
}

GtaThread* set_current_script_thread(GtaThread* thread)
{
	char* tls;
	GtaThread* original_thread;

	tls = *(char**)__readgsqword(0x58);

	original_thread = *(GtaThread**)(tls + 0x848);

	*(GtaThread**)(tls + 0x848) = thread;

	*(char*)(tls + 0x850) = thread != NULL;

	return original_thread;
}

int (*og_tick)(GtaThread* that, uint32_t ops_to_execute);
int hk_tick(GtaThread* that, uint32_t ops_to_execute)
{
	GtaThread* original_thread;

	switch (that->script_hash)
	{
	case 0x5700179C: /* main_persistent from base address */

		original_thread = set_current_script_thread(that);

		Fibers::TickAll();

		set_current_script_thread(original_thread);

		break;
	}

	return og_tick(that, ops_to_execute);
}



void Hook::Initialize()
{
	char* ptr;
	DWORD og_protect;


	ptr = ScanPattern(scan_base, scan_length, "\x48\x89\x03\xE8\x00\x00\x00\x00\x48\x8B\xC3\x48\x83\xC4\x20\x5B\xC3\xCC\xED", "xxxx????xxxxxxxxxxx");

	if (!ptr)
	{
		printf("Error finding Gta Thread VFTable");
	}

	ptr -= 7;
	ptr += *(int32_t*)(ptr)+sizeof(int32_t);
	thread_vftable = (GtaThreadVtbl*)ptr;

	VirtualProtect(thread_vftable, sizeof(GtaThreadVtbl), PAGE_READWRITE, &og_protect);
	{
		og_tick = thread_vftable->Tick;
		thread_vftable->Tick = &hk_tick;
	}
	VirtualProtect(thread_vftable, sizeof(GtaThreadVtbl), og_protect, NULL);
}

void Hook::Uninitialize()
{
	DWORD og_protect;

	VirtualProtect(thread_vftable, sizeof(GtaThreadVtbl), PAGE_READWRITE, &og_protect);
	{
		thread_vftable->Tick = og_tick;
	}
	VirtualProtect(thread_vftable, sizeof(GtaThreadVtbl), og_protect, NULL);
}
