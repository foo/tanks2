#ifndef _BAND_H
#define _BAND_H

#include "useful.h"
#include "texture.h"

class band : public entity
{
public:
	static CTexture skin[4];
	float health;
	static bool textured;
	int index;

	band()
	{
		size = coord(20,20);
		color = ccolor(1,0,0);
		health = 24;
		index = rand() % 5;
	}
	void update()
	{
		if (health <= 0)
		{
			isDead = true;
		}
	}
	void onDraw()
	{
		if (textured)
		{
			color.r = color.g = color.b = (health / 25) * 1.2;
			drawTextured(skin[index].texID);
		}
		else
		{
			color.r = (health / 25) * 1.2;
			color.g = color.b = 0;
			draw();
		}
	}
	void handleCollision(entity *e)
	{
		/*if (typeid(*e) == typeid(warrior))
		{
			isDead = true;
		}*/
	}
};
CTexture band::skin[4];
bool band::textured = true;
#endif