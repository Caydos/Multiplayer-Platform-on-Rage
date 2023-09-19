#include "pch.h"
#include "Sync.h"
#include "Fiber.h"
#include "Events.h"
#include "Natives.h"


void Way_01(void)
{
	while (true)
	{
		const char* name = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
		NativeVector3 pCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), false);
		TriggerServerEvent("Way_01", name, pCoords.x, pCoords.y, pCoords.z);

		
		Fibers::Suspend(3000);
	}
}


void SyncFunctions(void)
{
	Fibers::Create("Way_01", &Way_01);
}