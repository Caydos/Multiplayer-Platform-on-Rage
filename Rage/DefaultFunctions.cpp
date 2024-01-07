#include "Scripts.h"
#include <iostream>
#include "Fiber.h"
#include "Natives.h"
#include "Controls.h"
#include "Types.h"
#include "Hud.h"
#include "ScriptNames.h"
#include "PedFactory.h"
#include "Pools.h"

bool hasStoppedScripts = false;


Vehicle CreateVehicle(const char* _model, Vector3 _coords, float _heading, bool _networked)
{
	Hash modelHash = MISC::GET_HASH_KEY(_model);
	if (!STREAMING::IS_MODEL_IN_CDIMAGE(modelHash))
	{
		Hud::Notify("Invalid vehicle model", 6);
		return 0;
	}
	STREAMING::REQUEST_MODEL(modelHash);
	while (!STREAMING::HAS_MODEL_LOADED(modelHash))
	{
		Fibers::Suspend(10);
	}

	Vehicle vehicle = VEHICLE::CREATE_VEHICLE(modelHash, _coords, _heading, _networked, true, 0);
	VEHICLE::SET_VEHICLE_HAS_BEEN_OWNED_BY_PLAYER(vehicle, true);
	VEHICLE::SET_VEHICLE_NEEDS_TO_BE_HOTWIRED(vehicle, false);

	/* if (_networked) */


	AUDIO::SET_VEH_RADIO_STATION(vehicle, "OFF");
	STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);
	return vehicle;
}

void DefaultFunctions(void)
{
	Pools::Display();
	if (!hasStoppedScripts)
	{
		hasStoppedScripts = true;
		for (int i = 0; i < 804; i++)
		{
			SCRIPT::SET_SCRIPT_AS_NO_LONGER_NEEDED(scriptNames[i]);
			PLAYER::FORCE_CLEANUP_FOR_ALL_THREADS_WITH_THIS_NAME(scriptNames[i], 8); //Cleanup first
			MISC::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME(scriptNames[i]); //Terminate
		}
	}

	//Hud::TextDisplay({ 255, 255, 255, 255 }, Vector2(1.f, 1.f), {.0f,.0f}, 4, "Alpha 0.1");
	PAD::DISABLE_CONTROL_ACTION(0, VehicleRadioWheel, 1);/*Radio*/
	PAD::DISABLE_CONTROL_ACTION(0, EnterCheatCode, 1);/*CheatInput*/

	if (PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, EnterCheatCode))
	{
		std::string model = Hud::TextBox("Model : ", "", 15);
		if (strcmp(model.c_str(), "tp") == 0)
		{
			ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), Vector3(-127.91f, 1269.07f, 307.43f), 1, 0, 0, 1);
			Fibers::Suspend(125);
		}
		else if (strcmp(model.c_str(), "test") == 0)
		{
			PedFactory::TestThing();
		}
		else if (strcmp(model.c_str(), "coords") == 0)
		{
			NativeVector3 vec = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
			std::cout << vec.x << " " << vec.y << " " << vec.z << std::endl;
		}
		else if (strcmp(model.c_str(), "test01") == 0)
		{
			PedFactory::Tp();
			Fibers::Suspend(125);
		}
		else
		{
			std::cout << model.c_str() << std::endl;
			if (model.size() > 1)
			{
				NativeVector3 playerCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
				float heading = ENTITY::GET_ENTITY_HEADING(PLAYER::PLAYER_PED_ID());
				CreateVehicle(model.c_str(), Vector3(playerCoords.x, playerCoords.y, playerCoords.z), heading, true);
			}
		}
	}

	PAD::DISABLE_CONTROL_ACTION(2, FrontendPause, 1);
	PAD::DISABLE_CONTROL_ACTION(2, FrontendPauseAlternate, 1);

	if (PAD::IS_DISABLED_CONTROL_JUST_PRESSED(2, 199) || PAD::IS_DISABLED_CONTROL_JUST_PRESSED(2, 200))
	{
		if (!HUD::IS_PAUSE_MENU_ACTIVE())
		{
			Hash menuHash = MISC::GET_HASH_KEY("fe_menu_version_sp_pause");
			HUD::ACTIVATE_FRONTEND_MENU(menuHash, 0, 0);
		}
	}
	VEHICLE::SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0f);
	VEHICLE::SET_RANDOM_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0f);
	VEHICLE::SET_PARKED_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0f);
	VEHICLE::SET_DISABLE_RANDOM_TRAINS_THIS_FRAME(0.0f);
	VEHICLE::SET_AMBIENT_VEHICLE_RANGE_MULTIPLIER_THIS_FRAME(0.0f);
}
