#include "PedFactory.h"
#include "Memory.h"
#include <ped/CPedFactory.hpp>
#include "Fiber.h"
#include <iostream>
#include <basetsd.h>
#include <wtypes.h>

Memory::handle ptrPF;
CPedFactory** m_ped_factory;

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
	ptrPF = Memory::handle(Memory::ScanPattern(scan_base, scan_length,
		"\x48\x8B\x05\x00\x00\x00\x00\x48\x8B\x48\x08\x48\x85\xC9\x74\x52",
		"xxx????xxxxxxxxx"));
	m_ped_factory = ptrPF.add(3).rip().as<CPedFactory**>();
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