#include "Header.h"
#include "SyncEvents.h"
#include "Sync.h"
#include "Connections.h"
#include "../Shared/Reading.h"

using namespace Synchronization;

void PlayerConnection(char** _args)
{
	int serverId = Connections::GetLocalThreadId();
	std::int64_t discordId = ToInt64(_args[0]);
	unsigned int hash = ToUInt(_args[1]);
	float x = ToFloat(_args[2]);
	float y = ToFloat(_args[3]);
	float z = ToFloat(_args[4]);

	Connections::SetDiscordId(discordId);

	/*              */
	//Entity::Entity* testEnt = new Entity::Entity;
	//testEnt->hash = 1885233650;
	//testEnt->type = 1;
	//testEnt->position = Vector3(-127.90, 1268.96, 307.33);
	//testEnt->ownerServerId = 3;

	//Entity::Add(testEnt);
	/*              */

	Entity::Entity* playerEnt = new Entity::Entity;
	playerEnt->position = Vector3(x, y, z);
	playerEnt->ownerServerId = serverId;

	Node::Add(serverId, playerEnt->position, playerEnt);

	Entity::Add(playerEnt);

	playerEnt->Update();
	//testEnt->Update();
	TriggerClientEvent(serverId, "Sync::PlayerLanding", serverId, playerEnt->serverId);
}

void Synchronization::MainEvent(char** _args)
{
#pragma region Reception

	int serverId = Connections::GetLocalThreadId();
	float x = ToFloat(_args[0]);
	float y = ToFloat(_args[1]);
	float z = ToFloat(_args[2]);
	char* serializedEntities = _args[3];

	Node::Update(serverId, Vector3(x, y, z));

	if (serializedEntities != nullptr)
	{
		std::cout << std::endl << std::endl << "Serialized data : " << serializedEntities << std::endl;
		Entity::EntityDataUpdate(serializedEntities);
	}

#pragma endregion
#pragma region Callback

	Node::TriggerCallback(serverId);

	// Refresh node
	//Node::AskForRefresh(serverId);

#pragma endregion
}

