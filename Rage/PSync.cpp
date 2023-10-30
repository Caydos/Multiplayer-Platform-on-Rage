#include "pch.h"
#include "PSync.h"
#include "Scripting.h"
#include "Reading.h"

void PlayerLanding(char** _args)
{
	int playerId = ToInt(_args[0]);

	std::cout << "Player id is : " << playerId << std::endl;
}

void PedSyncEvents(void)
{
	Events::Register("PlayerLanding", &PlayerLanding);
}







