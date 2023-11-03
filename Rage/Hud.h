#ifndef HUD_H
#define HUD_H

#include "../Shared/Vector.h"
#include <string>

namespace Hud
{
	void TextDisplay(int, int, int, int, Vector2, int, const char*);
	std::string TextBox(const char* _title, const char* _exampleText, unsigned int _maxLength);
	void AddLongString(const char* _txt);
	void Notify(const char* _text, int _color);
}

#endif // !HUD_H