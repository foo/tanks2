#ifndef _GROUND_H
#define _GROUND_H

#include "useful.h"
#include "texture.h"

class ground : public entity
{
public:
	static bool textured;
	static CTexture skin;

	ground()
	{
		pos = coord(0,0);
		size = border;
	}
	void onDraw()
	{
		if (textured)
		{
			color = ccolor(1,1,1);
			drawTextured(skin.texID);
		}
		else
		{
			color = ccolor(0.1,1.0,0.1);
			draw();
		}
	}
	static coord border;
};
coord ground::border(1000,800);
bool ground::textured = true;
CTexture ground::skin;
#endif