#include "Header.h"
#include "Events.h"
#include "Reading.h"
#include "Sync.h"
#include "Connections.h"

void PlayerConnection(char** _args)
{
	std::int64_t id = ToInt64(_args[0]);
	unsigned int hash = ToUInt(_args[1]);
	float x = ToFloat(_args[2]);
	float y = ToFloat(_args[3]);
	float z = ToFloat(_args[4]);

	Connections::SetDiscordId(id);

	std::cout << "Id : " << id << std::endl;
	TriggerClientEvent(-2, "PedSync::Creation", hash, x, y, z);
}

void Way_01(char** _args)
{
	std::int64_t id = ToInt64(_args[0]);
	float x = ToFloat(_args[1]);
	float y = ToFloat(_args[2]);
	float z = ToFloat(_args[3]);

	TriggerClientEvent(-2, "Way_01", id, x, y, z);

}