#include "pch.h"
#include "PCreation.h"
#include "Vectors.h"
#include "Natives.h"
#include "Fiber.h"
#include "Pools.h"
#include "Sync.h"


void PedSync::Creation(void** _args)
{
	unsigned int hash = *(unsigned int*)_args[0];
	printf("Hashes : %d\n", hash);
	Vector3 position;
	position.x = *(float*)_args[1];
	position.y = *(float*)_args[2];
	position.z = *(float*)_args[3];

	STREAMING::REQUEST_MODEL(hash);

	while (!STREAMING::HAS_MODEL_LOADED(hash))
	{
		Fibers::Suspend(1);
	}

	PLAYER::SET_PLAYER_MODEL(PLAYER::PLAYER_ID(), hash);
	PED::SET_PED_DEFAULT_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID());
	PED::RESET_PED_MOVEMENT_CLIPSET(PLAYER::PLAYER_PED_ID(), 0);
	Ped ped = PED::CREATE_PED(1, hash,
		position, ENTITY::GET_ENTITY_HEADING(PLAYER::PLAYER_PED_ID()), true, false);
	printf("Model finished loading at position : %f %f %f\n", ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true).x, ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true).y, ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true).z);
	STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
	TASK::TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);
	printf("\n\nCreating new ped\n\n");

	//SyncFunctions();
}

void PedSync::Main(void** _args)
{
	const char* name = *(const char**)_args[0];
	float x = *(float*)_args[1];
	float y = *(float*)_args[2];
	float z = *(float*)_args[3];

	//printf("%s - New vector : %f %f %f\n", name, x, y, z);
}