#include "Header.h"
#include "SyncEvents.h"
#include "Sync.h"
#include "Reading.h"
#include "Connections.h"

using namespace Synchronization;

void PlayerConnection(char** _args)
{
	std::int64_t id = ToInt64(_args[0]);
	unsigned int hash = ToUInt(_args[1]);
	float x = ToFloat(_args[2]);
	float y = ToFloat(_args[3]);
	float z = ToFloat(_args[4]);

	Connections::SetDiscordId(id);

	Entity::Entity player;
	player.position = Vector3(x, y, z);
	player.type = 1;
	Entity::Entity* playerPtr = Entity::Add(player);

	Node::Add(id, Vector3(x, y, z), playerPtr);

	std::cout << "Id : " << id << std::endl;
	TriggerClientEvent(-2, "PedSync::Creation", hash, x, y, z);
}

void SyncMe(char** _args)
{
	std::int64_t id = ToInt64(_args[0]);
	float x = ToFloat(_args[1]);
	float y = ToFloat(_args[2]);
	float z = ToFloat(_args[3]);

	Node::SendUpdate(id, Vector3(x, y, z));

	//TriggerClientEvent(-2, "Way_01", id, x, y, z);
}

