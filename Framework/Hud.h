#ifndef HUD_H
#define HUD_H
#include "Framework.h"
#include "../Shared/Vector.h"


namespace Hud
{
	void TextDisplay(Vector4, Vector2, Vector2, int, const char*);
	std::string TextBox(const char* _title, const char* _exampleText, unsigned int _maxLength);
	void AddLongString(const char* _txt);
	void Notify(const char* _text, int _color);
}

#endif // !HUD_H