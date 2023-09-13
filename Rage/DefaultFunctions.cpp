#include "pch.h"
#include "Scripts.h"
#include "ScriptNames.h"
#include "Fiber.h"

bool hasStoppedScripts = false;



void textDisplay()
{
	HUD::SET_TEXT_COLOUR(255, 255, 255, 255);
	HUD::SET_TEXT_SCALE(1.f, 1.f);
	HUD::SET_TEXT_FONT(4);
	HUD::SET_TEXT_DROP_SHADOW();
	HUD::SET_TEXT_OUTLINE();

	HUD::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
	HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Alpha 0.1");
	HUD::END_TEXT_COMMAND_DISPLAY_TEXT({ 0.01f, 0.01f }, 1);
}


void DefaultFunctions(void)
{
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

	textDisplay();
	PAD::DISABLE_CONTROL_ACTION(2, 199, 1);
	PAD::DISABLE_CONTROL_ACTION(2, 200, 1);

	if (PAD::IS_DISABLED_CONTROL_JUST_PRESSED(2, 199) || PAD::IS_DISABLED_CONTROL_JUST_PRESSED(2, 200))
	{
		if (!HUD::IS_PAUSE_MENU_ACTIVE())
		{
			Hash menuHash = MISC::GET_HASH_KEY("fe_menu_version_sp_pause");
			HUD::ACTIVATE_FRONTEND_MENU(menuHash, 0, 0);
		}
	}
}

const char* TextBox(const char* _title, const char* _exampleText, unsigned int _maxLength)
{
	MISC::DISPLAY_ONSCREEN_KEYBOARD(false, "FMMC_KEY_TIP", "", _exampleText, "", "", "", _maxLength);
	while (MISC::UPDATE_ONSCREEN_KEYBOARD() != 1 && MISC::UPDATE_ONSCREEN_KEYBOARD() != 2)
	{
		Fibers::Suspend(0);
	}
	if (MISC::UPDATE_ONSCREEN_KEYBOARD() != 2)
	{
		const char* result = MISC::GET_ONSCREEN_KEYBOARD_RESULT();
		printf("Result is : %s\n", result);
		Fibers::Suspend(500);
		return result;
	}
	else
	{
		Fibers::Suspend(500);
	}
	return NULL;
}
