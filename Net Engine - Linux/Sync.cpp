#include "Header.h"
#include "Events.h"
#include "Reading.h"
#include "Sync.h"

void PlayerConnection(char** _args)
{
	std::cout << "Yeeheee send back" << std::endl;
	unsigned int hash = ToUInt(_args[0]);
	float x = ToFloat(_args[1]);
	float y = ToFloat(_args[2]);
	float z = ToFloat(_args[3]);

	TriggerClientEvent(-1, "PedSync::Creation", hash, x, y, z);
}

void Way_01(char** _args)
{
	std::cout << "Way_01 trgr" << std::endl;

	const char* name = ToString(_args[0]);
	float x = ToFloat(_args[1]);
	float y = ToFloat(_args[2]);
	float z = ToFloat(_args[3]);

	TriggerClientEvent(-1, "Way_01", name, x, y, z);

}