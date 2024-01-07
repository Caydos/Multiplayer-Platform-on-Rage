#include "Pools.h"
#include <ped/CPedFactory.hpp>
#include "PoolsClasses.h"
#include "Memory.h"
#include <basetsd.h>
#include <wtypes.h>
#include <iostream>


Memory::handle pedFactoryHandle;
CPedFactory** m_ped_factory;
Memory::handle replayInterfaceHandle;
CReplayInterface* replay_interface;

inline CPlayerInfo* GetLocalPlayerInfo()
{
	if (auto ped_factory = *m_ped_factory)
	{
		if (auto ped = ped_factory->m_local_ped)
		{
			return ped->m_player_info;
		}
	}
	return nullptr;
}

CPed* Pools::GetLocalPed()
{
	if (auto ped_factory = *m_ped_factory)
	{
		return ped_factory->m_local_ped;
	}

	return nullptr;
}

GenericPool** m_ped_pool;
GenericPool** m_prop_pool;
GenericPool** m_pickup_pool;
//VehiclePool*** m_vehicle_pool;

inline auto& get_all_peds()
{
	return **m_ped_pool;
}

void Pools::Hook(void)
{
	pedFactoryHandle = Memory::handle(Memory::ScanPattern(scan_base, scan_length,
		"\x48\x8B\x05\x00\x00\x00\x00\x48\x8B\x48\x08\x48\x85\xC9\x74\x52",
		"xxx????xxxxxxxxx"));
	m_ped_factory = pedFactoryHandle.add(3).rip().as<CPedFactory**>();

	//Memory::handle pedPoolHandle = Memory::handle(Memory::ScanPattern(scan_base, scan_length,
	//	"\x4C\x8B\x35\x00\x00\x00\x00\xB8",
	//	"xxx????x"));

	//m_ped_pool = pedPoolHandle.add(3).rip().as<GenericPool**>();


	//	// Prop Pool
	//	{
	//		"PRP",
	//		"48 8B 05 ? ? ? ? 0F B7 50 10 48 8B 05",
	//		[](memory::handle ptr)
	//		{
	//			g_pointers->m_gta.m_prop_pool = ptr.add(3).rip().as<GenericPool**>();
	//			g_pointers->m_gta.m_pickup_pool = ptr.add(0xE).rip().as<GenericPool**>();
	//		}
	//	},
	//	// Vehicle Pool
	//	{
	//		"VEP",
	//		"4C 8B 25 ? ? ? ? 8B 29 33 F6 49 8B 04 24 33 DB 4C 8D 71 08 44 8B 78 08 45 85 FF 0F 8E ? ? ? ? 4D 8B 0C 24 41 3B 59 08 7D 29 49 8B 51 30 44 8B C3 8B CB 49 C1 E8 05 83 E1 1F 44 8B D3 42 8B 04 82",
	//		[](memory::handle ptr)
	//		{
	//			g_pointers->m_gta.m_vehicle_pool = ptr.add(3).rip().as<VehiclePool***>();
	//		}
	//	},
}

void Pools::Test(void)
{
	for (auto ped : get_all_peds())
	{
		auto pos = ped->get_position();
		//std::cout << pos->x << " " << pos->y << pos->z << std::endl;
	}
}
