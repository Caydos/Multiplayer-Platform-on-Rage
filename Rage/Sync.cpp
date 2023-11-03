#include "pch.h"
#include "Sync.h"
#include "Fiber.h"
#include "Events.h"
#include "Natives.h"
#include "../Shared/Encryption.h"
#include "../Shared/Reading.h"

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

		std::string callBString;
		const char separator = '/';
		//for (size_t entityId = 0; entityId < nodes[nodeId]->entityCount; entityId++)
		{
			//if (entityId)
			//{
			//	callBString += '|';
			//}
			Entity::Entity currentEnt;
			callBString += Encryption::Encode(separator,
				currentEnt.ownerServerId,
				currentEnt.serverId,
				currentEnt.nodeCount,
				currentEnt.remove,
				currentEnt.type,
				currentEnt.task,
				currentEnt.position.x,
				currentEnt.position.y,
				currentEnt.position.z,
				currentEnt.rotations.x,
				currentEnt.rotations.y,
				currentEnt.rotations.z
			);
		}

		TriggerServerEvent("Synchronization::MainEvent", pCoords.x, pCoords.y, pCoords.z, callBString.c_str());

		Fibers::Suspend(SYNC_TIME);
	}
}

void Synchronization::NodeDataEvent(char** _args)
{
	char* entArray = _args[0];

	char** entArrays = nullptr;
	unsigned int entCount = 0;
	Encryption::GetAsArguments(entArrays, entCount, entArray, '|');

	for (size_t i = 0; i < entCount; i++)
	{
		char** entArray = nullptr;
		unsigned int attributesCount = 0;
		Encryption::GetAsArguments(entArray, attributesCount, entArrays[i], '/');
		std::cout << "Entity buffer : " << entArrays[i] << std::endl;
	}


	std::cout << "Just recieved node data : " << std::endl;
	std::cout << entArray << std::endl;
}





void Synchronization::PlayerLanding(char** _args)
{
	int playerId = ToInt(_args[0]);

	std::cout << "Player id is : " << playerId << std::endl;
}

void Synchronization::PedSyncEvents(void)
{
	Events::Register("Sync::PlayerLanding", &PlayerLanding);
	Events::Register("Sync::NodeDataEvent", &Synchronization::NodeDataEvent);
}
