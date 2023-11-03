#include "Hud.h"
#include "Natives.h"
#include "Fiber.h"
#include <cstring>
#include <string>

void Hud::TextDisplay(int _red = 255, int _green = 255, int _blue = 255, int _alpha = 255, Vector2 _scale = { .0f,.0f }, int _font = 4, const char* _text = "Null")
{
	HUD::SET_TEXT_COLOUR(_red, _green, _blue, _alpha);
	HUD::SET_TEXT_SCALE(_scale.x, _scale.y);
	HUD::SET_TEXT_FONT(_font);
	HUD::SET_TEXT_DROP_SHADOW();
	HUD::SET_TEXT_OUTLINE();

	HUD::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
	HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(_text);
	HUD::END_TEXT_COMMAND_DISPLAY_TEXT({ 0.01f, 0.01f }, 1);
}

std::string Hud::TextBox(const char* _title, const char* _exampleText, unsigned int _maxLength)
{
	std::string entry;
	MISC::DISPLAY_ONSCREEN_KEYBOARD(false, "FMMC_KEY_TIP8", "", _exampleText, "", "", "", _maxLength);
	while (MISC::UPDATE_ONSCREEN_KEYBOARD() != 1 && MISC::UPDATE_ONSCREEN_KEYBOARD() != 2)
	{
		Fibers::Suspend(0);
	}
	if (MISC::UPDATE_ONSCREEN_KEYBOARD() != 2)
	{
		entry = MISC::GET_ONSCREEN_KEYBOARD_RESULT();
	}
	else
	{
		Fibers::Suspend(500);
	}
	return entry;
}


void Hud::AddLongString(const char* _txt)
{
	size_t len = std::strlen(_txt); // Get the length of the C-style string
	char sub[100]; // Buffer to hold the substrings (+1 for null-terminator)

	for (size_t i = 0; i < len; i += 99) {
		// Copy up to 99 characters from txt starting at position i into sub
		//std::strncpy(sub, _txt + i, 99);
		std::memcpy(sub, _txt + i, 99);
		sub[99] = '\0'; // Ensure null-termination

		// Call the function that uses the substring
		HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(sub);
	}
}

void Hud::Notify(const char* _text = "NULL", int _color = 0)
{
	HUD::THEFEED_SET_BACKGROUND_COLOR_FOR_NEXT_POST(_color);
	HUD::BEGIN_TEXT_COMMAND_THEFEED_POST("jamyfafi");
	HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(_text);

	if (strlen(_text) > 99)
	{
		AddLongString(_text);
	}

	HUD::END_TEXT_COMMAND_THEFEED_POST_TICKER(0,1);
}
