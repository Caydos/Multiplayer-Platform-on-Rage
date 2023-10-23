#include "pch.h"
#include "Sync.h"
#include "Fiber.h"
#include "Events.h"
#include "Natives.h"
#include "API_Discord.h"

void SyncMe(void)
{
	while (true)
	{
		const char* name = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
		NativeVector3 pCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), false);
		TriggerServerEvent("SyncMe", /*API_Discord::GetUser()._id*/, pCoords.x, pCoords.y, pCoords.z);

		
		Fibers::Suspend(3000);
	}
}


void SyncFunctions(void)
{
	std::cout << "creating both" << std::endl;
	Fibers::Create("SyncMe", &SyncMe);
}