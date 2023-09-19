#include "pch.h"
#include "PCreation.h"
#include "Vectors.h"
#include "Natives.h"
#include "Fiber.h"
#include "Pools.h"
#include "Sync.h"
#include "Reading.h"

void PedSync::Creation(char** _args)
{
	unsigned int hash = ToUInt(_args[0]);
	Vector3 position;
	position.x = ToFloat(_args[1]);
	position.y = ToFloat(_args[2]);
	position.z = ToFloat(_args[3]);

	Ped ped = PED::CREATE_PED(1, hash, position, ENTITY::GET_ENTITY_HEADING(PLAYER::PLAYER_PED_ID()), true, false);
	STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
	TASK::TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);

	SyncFunctions();
}

void PedSync::Main(char** _args)
{
	const char* name = ToString(_args[0]);
	float x = ToFloat(_args[1]);
	float y = ToFloat(_args[2]);
	float z = ToFloat(_args[3]);

	//printf("%s - New vector : %f %f %f\n", name, x, y, z);
}