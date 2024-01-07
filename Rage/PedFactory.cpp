#include "PedFactory.h"
#include "Memory.h"
#include <ped/CPedFactory.hpp>
#include "Fiber.h"
#include <iostream>
#include <basetsd.h>
#include <wtypes.h>

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

inline CPed* GetLocalPed()
{
	if (auto ped_factory = *m_ped_factory)
	{
		return ped_factory->m_local_ped;
	}

	return nullptr;
}

void PedFactory::Hook(void)
{
	pedFactoryHandle = Memory::handle(Memory::ScanPattern(scan_base, scan_length,
		"\x48\x8B\x05\x00\x00\x00\x00\x48\x8B\x48\x08\x48\x85\xC9\x74\x52",
		"xxx????xxxxxxxxx"));
	m_ped_factory = pedFactoryHandle.add(3).rip().as<CPedFactory**>();


	//48 8D 0D ? ? ? ? 48 8B D7 E8 ? ? ? ? 48 8D 0D ? ? ? ? 8A D8 E8 ? ? ? ? 84 DB 75 13 48 8D 0D

	replayInterfaceHandle = Memory::handle(Memory::ScanPattern(scan_base, scan_length,
		"\x48\x8D\x0D\x00\x00\x00\x00\x48\x8B\xD7\xE8\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\x8A\xD8\xE8\x00\x00\x00\x00\x84\xDB\x75\x13\x48\x8D\x0D",
		"xxx????xxxx????xxx????xxx????xxxxxxx"));

	uint8_t* replay_interface_pattern = replayInterfaceHandle.as<uint8_t*>();
	uint8_t* replay_interface_address = replay_interface_pattern + *(uint32_t*)(replay_interface_pattern + 0x3) + 0x7;
	replay_interface = *(CReplayInterface**)replay_interface_address;

	//replay_interface = pedFactoryHandle.add(3).rip().as<CReplayInterface**>();

}


void PedFactory::Tp(void)
{
	CPed* lcPed = GetLocalPed();
	if (lcPed)
	{
		rage::fvector4 vec(-127.91f, 1269.07f, 307.43, 90.0f);
		lcPed->m_velocity = rage::fvector3(50.0f, 50.0f,50.0f);
		lcPed->m_transformation_matrix.rows[3].x = vec.x;
		lcPed->m_transformation_matrix.rows[3].y = vec.y;
		lcPed->m_transformation_matrix.rows[3].z = vec.z;
		lcPed->m_navigation->m_transformation_matrix.rows[3].x = vec.x;
		lcPed->m_navigation->m_transformation_matrix.rows[3].y = vec.y;
		lcPed->m_navigation->m_transformation_matrix.rows[3].z = vec.z;
		lcPed->SetPosition(&vec, true);
	}
}

void PedFactory::TestThing(void)
{
	int32_t count = replay_interface->m_ped->m_max;
	int32_t count2 = replay_interface->m_ped->m_cur;
	std::cout << count << std::endl;
	std::cout << count2 << std::endl;
	std::cout << GetLocalPed() << std::endl;


	for (size_t i = 0; i < replay_interface->m_ped->m_cur; i++)
	{ 
		CPed* curPed = replay_interface->m_ped->m_list->m_peds[i].m_ped;
		if (curPed != nullptr)
		{
			std::cout << curPed << std::endl;
		}
	}
}
