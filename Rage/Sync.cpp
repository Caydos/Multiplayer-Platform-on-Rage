#include "pch.h"
#include "Sync.h"
#include "Fiber.h"
#include "Events.h"
#include "Natives.h"

void Synchronization::Init(void)
{
	Fibers::Create("SyncLoop", &Synchronization::Loop);
	// Entity detection Fiber
}

void Synchronization::Loop(void)
{
	while (true)
	{/*always send player data and each X time send entity data as XML tags*/
		NativeVector3 pCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), false);
		TriggerServerEvent("Synchronization::MainEvent", pCoords.x, pCoords.y, pCoords.z);

		Fibers::Suspend(SYNC_TIME);
	}
}
