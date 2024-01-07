#include "GameComponents.h"
#include "Fibers.h"
#include "Network.h"
#include "Natives.h"
#include "ScriptsList.h"
#include "Controls.h"
#include "Hud.h"
#include "Network.h"
#include "Pools.h"
#include "Scenarios.h"
#include "GameUtils.h"

static bool loading = true;
static bool hasStoppedScripts = false;


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

void GameComponents::DefaultFunctions(void)
{
	if (!hasStoppedScripts)
	{
		hasStoppedScripts = true;
		for (int i = 0; i < SCRIPTS_COUNT; i++)
		{
			SCRIPT::SET_SCRIPT_AS_NO_LONGER_NEEDED(scriptNames[i]);
			PLAYER::FORCE_CLEANUP_FOR_ALL_THREADS_WITH_THIS_NAME(scriptNames[i], 8); //Cleanup first
			MISC::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME(scriptNames[i]); //Terminate
		}
	}
	PAD::DISABLE_CONTROL_ACTION(0, VehicleRadioWheel, 1);/*Radio*/
	PAD::DISABLE_CONTROL_ACTION(0, EnterCheatCode, 1);/*CheatInput*/

	if (PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, EnterCheatCode))
	{
		std::string model = Hud::TextBox("Model : ", "", 15);
		if (strcmp(model.c_str(), "revive") == 0)
		{
			NativeVector3 coords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), false);
			NETWORK::NETWORK_RESURRECT_LOCAL_PLAYER(Vector3(coords.x, coords.y, coords.z), .0f, true, true, true, 0, 0);
			PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_PED_ID(), false);
			PED::CLEAR_PED_BLOOD_DAMAGE(PLAYER::PLAYER_PED_ID());
		}
		if (strcmp(model.c_str(), "tp") == 0)
		{
			ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), Vector3(-127.91f, 1269.07f, 307.43f), 1, 0, 0, 1);
			Fibers::Suspend(125);
		}
		else if (strcmp(model.c_str(), "pools") == 0)
		{
			Pools::Test();
		}
		else if (strcmp(model.c_str(), "connect") == 0)
		{
			Network::Connection::Create("51.178.46.32", 55301);
		}
		else if (strcmp(model.c_str(), "coords") == 0)
		{
			NativeVector3 vec = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
			std::cout << vec.x << " " << vec.y << " " << vec.z << std::endl;
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
	//VEHICLE::SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0f);
	//VEHICLE::SET_RANDOM_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0f);
	//VEHICLE::SET_PARKED_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0f);
	//VEHICLE::SET_DISABLE_RANDOM_TRAINS_THIS_FRAME(0.0f);
	//VEHICLE::SET_AMBIENT_VEHICLE_RANGE_MULTIPLIER_THIS_FRAME(0.0f);

	//PED::SET_PED_DENSITY_MULTIPLIER_THIS_FRAME(.0f);
	//PED::SET_SCENARIO_PED_DENSITY_MULTIPLIER_THIS_FRAME(.0f, .0f);
}

void GameComponents::Main(void)
{
	while (1)
	{
		if (!loading)
		{
			DefaultFunctions();
		}
		else
		{
			loading = DLC::GET_IS_LOADING_SCREEN_ACTIVE();
			if (!loading)
			{
				MISC::SET_INSTANCE_PRIORITY_MODE(1);
				DLC::ON_ENTER_SP();
				//Disable Stunt Jumps
				for (int i = 0; i < 50; i++)
				{
					MISC::DISABLE_STUNT_JUMP_SET(i);
					MISC::DELETE_STUNT_JUMP(i);
				}
				//Disable Hospitals
				for (int i = 0; i <= 5; i++)
				{
					MISC::DISABLE_HOSPITAL_RESTART(i, true);
				}
				// Disable save option
				MISC::SET_MISSION_FLAG(true);
				GRAPHICS::DISABLE_VEHICLE_DISTANTLIGHTS(true);
				STREAMING::SET_PED_POPULATION_BUDGET(0);
				STREAMING::SET_VEHICLE_POPULATION_BUDGET(0);
				VEHICLE::SET_RANDOM_BOATS(false);
				VEHICLE::SET_RANDOM_BOATS_MP(false);
				VEHICLE::SET_GARBAGE_TRUCKS(false);

				PED::SET_CREATE_RANDOM_COPS(false);
				PED::SET_CREATE_RANDOM_COPS_ON_SCENARIOS(false);
				PED::SET_CREATE_RANDOM_COPS_NOT_ON_SCENARIOS(false);


				MISC::SET_RANDOM_EVENT_FLAG(false);
				for (size_t i = 0; i < SCENARIO_COUNT; i++)
				{
					TASK::SET_SCENARIO_TYPE_ENABLED(scenarios[i], false);
				}
			}
		}
		if (GetAsyncKeyState(VK_F9) & 0x8000)
		{
			//unload();
		}

		Fibers::Suspend(0);
	}
}
