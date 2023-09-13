#include "Header.h"
#include "SyncTree.h"

void PlayerConnection(void** _args)
{
	unsigned int hash = *(unsigned int*)_args[0];
	float x = *(float*)_args[1];
	float y = *(float*)_args[2];
	float z = *(float*)_args[3];
	printf("Yeeheee send back\n");
	TriggerClientEvent(-1, "PedSync::Creation", hash, x, y, z);
}

void Way_01(void** _args)
{
	const char* name = *(const char**)_args[0];
	float x = *(float*)_args[1];
	float y = *(float*)_args[2];
	float z = *(float*)_args[3];

	//printf("Gotcha\n");
	//TriggerClientEvent(-1, "Way_01", name, x, y, z);
	
}