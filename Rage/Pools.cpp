#include "Pools.h"
#include "Memory.h"
#include "Fiber.h"
#include <iostream>
#include "Hud.h"

using namespace Pools;

GenericPool** m_ped_pool;


void Pools::Hook(void)
{
	Memory::handle ptr;
	ptr = Memory::handle(Memory::ScanPattern(scan_base, scan_length,
		"\x4C\x8B\x35\x00\x00\x00\x00\xB8",
		"xxx????x"));
	m_ped_pool = ptr.add(3).rip().as<GenericPool**>();
}

void Pools::Display(void)
{
	std::string pedPoolDp = "Ped Pool : ";
	pedPoolDp += std::to_string((*m_ped_pool)->get_item_count());
	Hud::TextDisplay({ 255, 255, 255, 255 }, Vector2(1.f, 1.f), { .0f,.0f }, 4, pedPoolDp.c_str());
}

