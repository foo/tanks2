#ifndef _WARRIOR_H
#define _WARRIOR_H

#include "band.h"
#include "arrow.h"
#include "useful.h"
#include "texture.h"

class warrior : public entity
{
private:
	float DA;
	float DD;
	float DSH;
	float DNX;
public:
	CTexture skin;
	float speed;
	float strefe;
	float direction;
	list<Weapon*> weapons;
	list<Weapon*>::iterator actualWeapon;
	coord startPos;

	int score;

	class Keys
	{
	public:
		char left,right,up,down,fire,nextWeapon,strefeLeft,strefeRight;
	};
	Keys keys;

	float health;

	warrior()
	{
		size = coord(35,35);
		speed = 0;
		strefe = 0;
		direction = 0;

		DA = 0;
		DD = 0;
		DSH = 0.9;
		DNX = 0;

		health = 100;
		score = 0;

		reloadWeapons();
	}
	void reloadWeapons()
	{
		for (list<Weapon*>::iterator i = weapons.begin(); i != weapons.end(); ++i)
		{
			delete (*i);
			i = weapons.erase(i);
		}
		weapons.push_back(new normalWeapon);
		weapons.push_back(new machineWeapon);
		weapons.push_back(new bubbleWeapon);
		weapons.push_back(new sniperWeapon);
		weapons.push_back(new superMachineWeapon);
		weapons.push_back(new superBubbleWeapon);
		weapons.push_back(new superSniperWeapon);
		actualWeapon = weapons.begin();
	}
	void update()
	{
		DSH += time::delta;
		if (keyboard->KeyDown(keys.fire))
		{
			if (DSH > (*actualWeapon)->frequency)
			{
				DSH = 0;
					
				coord temp;
				temp.x = pos.x + size.x/2 + 35 * cosf(direction * 0.0174);
				temp.y = pos.y + size.y/2 + 35 * sinf(direction * 0.0174);
					
				(*actualWeapon)->shot(temp, direction);
			}
		}

		DNX += time::delta;
		if (DNX > 0.45)
		{
			if (keyboard->KeyDown(keys.nextWeapon))
			{
				++actualWeapon;
				if (actualWeapon == weapons.end())
				{
					actualWeapon = weapons.begin();
				}
				DNX = 0.0f;
				// new weapon is ready to fire
				DSH = 5.0f;
			}
		}
		if ((*actualWeapon)->GetAmmo() <= 0)
		{
			delete (*actualWeapon);
			actualWeapon = weapons.erase(actualWeapon);
			++actualWeapon;
			if (actualWeapon == weapons.end())
				{
					actualWeapon = weapons.begin();
				}
		}
/***********************************************************************************/
		const float rotateSpeed = 4.0f;

		if (keyboard->KeyDown(keys.left))
		{
			direction += time::delta * 60 * rotateSpeed * DA;
// { Stopniowe skrêcanie
			DA += time::delta*4; if (DA > 1) DA = 1;
		} else { DA = 0; }
// }

		if (keyboard->KeyDown(keys.right))
		{
			direction -= time::delta * 60 * rotateSpeed * DD;
// { Stopniowe skrêcanie
			DD += time::delta*4; if (DD > 1) DD = 1;
		} else { DD = 0; }
// }

		color.r = color.g = color.b = (health / 100) * 1.2;

		if (health <= 0)
		{
			menu = true;

			if (this == w1) w2->score ++;
			else			w1->score ++;

			reset();
		}

		/****************************************************************/
		const float tolerance = 0.01f;

		{
			const float accelerate = 0.225;
			const float backAccelerate = accelerate/2;
			const float maxSpeed = 2.8;

			if (keyboard->KeyDown(keys.up)) speed += accelerate * time::delta * 60;
			if (keyboard->KeyDown(keys.down)) speed -= backAccelerate * time::delta * 60;

			//tarcie
			if (speed > 0 + tolerance)
				speed -= accelerate/3 * time::delta * 60;
			else if (speed < 0 - tolerance)
				speed += accelerate/3 * time::delta * 60;
			else
				speed = 0.0f;

			if (speed <= -maxSpeed) speed = -maxSpeed;
			if (speed >=  maxSpeed) speed =  maxSpeed;

			pos.x += cosf(direction * 0.0174) * time::delta * 60 * speed;
			pos.y += sinf(direction * 0.0174) * time::delta * 60 * speed;
		}
		/****************************************************************/
		{
			const float strefeAccel = 0.1;
			const float maxStrefe = 2.0f;

			if (keyboard->KeyDown(keys.strefeLeft)) strefe += strefeAccel * time::delta * 60;
			if (keyboard->KeyDown(keys.strefeRight)) strefe -= strefeAccel * time::delta * 60;
			
			if (strefe > 0 + tolerance)
				strefe -= strefeAccel/3 * time::delta * 60;
			else if (strefe < 0 - tolerance)
				strefe += strefeAccel/3 * time::delta * 60;
			else
				strefe = 0.0f;

			if (strefe <= -maxStrefe) strefe = -maxStrefe;
			if (strefe >=  maxStrefe) strefe =  maxStrefe;		

			pos.x += cosf((90 + direction) * 0.0174) * time::delta * 60 * strefe;
			pos.y += sinf((90 + direction) * 0.0174) * time::delta * 60 * strefe;
		}
		/****************************************************************/

		if (pos.x < 0) pos.x = 0;
		if (pos.x > ground::border.x - size.x) pos.x = ground::border.x - size.x;
		if (pos.y < 0) pos.y = 0;
		if (pos.y > ground::border.y - size.y) pos.y = ground::border.y - size.y;
	}
	void handleCollision(entity *e)
	{
		if (typeid(*e) == typeid(band))
		{
			slide(e);
		}
		else if (
			typeid(*e) == typeid(normalArrow)		||
			typeid(*e) == typeid(bubbleArrow)		||
			typeid(*e) == typeid(superBubbleArrow)	||
			typeid(*e) == typeid(sniperArrow)		||
			typeid(*e) == typeid(superSniperArrow)	||
			typeid(*e) == typeid(machineArrow)		||
			typeid(*e) == typeid(superMachineArrow)
			)
		{
			arrow *a = dynamic_cast<arrow*>(e);
			health -= a->damage;
			a->isDead = true;
		}
	}

	void onDraw()
	{
		drawTextured(skin.texID);
		drawArrow(direction, 35);
	}

	static void reset()
	{
		
		w1->health = 100; w1->color = ccolor(1,1,1);
		w2->health = 100; w2->color = ccolor(1,1,1);

		w1->pos = w1->startPos;
		w2->pos = w2->startPos;

		w1->reloadWeapons();
		w2->reloadWeapons();

		w1->direction = w2->direction = 0;

		for (list<entity*>::iterator i = container.begin(); i != container.end(); ++i)
		{
			if (
					typeid(*(*i)) == typeid(normalArrow)		||
					typeid(*(*i)) == typeid(bubbleArrow)		||
					typeid(*(*i)) == typeid(superBubbleArrow)	||
					typeid(*(*i)) == typeid(sniperArrow)		||
					typeid(*(*i)) == typeid(superSniperArrow)	||
					typeid(*(*i)) == typeid(machineArrow)		||
					typeid(*(*i)) == typeid(superMachineArrow)
				)
			{
				(*i)->isDead = true;
			}
		}
	}
};
#endif