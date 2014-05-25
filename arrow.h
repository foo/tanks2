#ifndef _ARROW_H
#define _ARROW_H

#include "useful.h"

class arrow : public entity
{
protected:
	float DD;
public:
	float direction;
	float speed;
	float damage;
	float lifeTime;
		
	arrow()
	{
		DD = 0;
		lifeTime = 2;
	}

	void onDraw()
	{
		draw();
		drawArrow(direction, 10);
	}

	void update()
	{
		pos.x += cosf(direction * 0.0174) * time::delta * 60 * speed;
		pos.y += sinf(direction * 0.0174) * time::delta * 60 * speed;

		DD += time::delta;
		if (DD > lifeTime) isDead = true;
	}
	void handleCollision(entity *e)
	{
		if (typeid(*e) == typeid(band))
		{
			band *b = dynamic_cast<band*>(e);
			b->health -= damage;

			isDead = true;
		}
		if (typeid(*e) == typeid(warrior))
		{}
	}
};

class Weapon
{
protected:
	int ammo;
	void fire(arrow *n, coord startPos, float direction)
	{
		if (ammo <= 0) return;
		ammo --;
		arrow *a = n;
		a->pos = startPos;
		a->pos.x -= a->size.x/2;
		a->pos.y -= a->size.y/2;
		a->direction = direction;
		container.push_back(a);
	}
public:
	Weapon() {}
	float frequency;
	virtual void shot(coord startPos, float direction) = 0;
	inline int GetAmmo() { return ammo; }
};

class normalArrow : public arrow
{
public:
	normalArrow()
	{
		size = coord(5,5);
		color = ccolor(1,1,1);
		speed = 7;
		damage = 15;
		lifeTime = 2.5;
	}
};

class normalWeapon : public Weapon
{
public:
	normalWeapon()
	{
		ammo = 750;
		frequency = 0.4;
	}

	void shot(coord startPos, float direction)
	{
		fire(new normalArrow,startPos, direction);
	}
};


class bubbleArrow : public arrow
{
public:
	int maxDiff;
	bubbleArrow()
	{
		size = coord(5,5);
		color.r = float((rand() % 50) + 50)/100;
		color.g = 0;
		color.b = float((rand() % 50) + 50)/100;
		speed = 3.2;
		damage = 3;
		lifeTime = 0.9;
		maxDiff = 8;
	}

	void update()
	{
		pos.x += cosf(direction * 0.0174) * time::delta * 60 * speed;
		pos.y += sinf(direction * 0.0174) * time::delta * 60 * speed;

		DD += time::delta;
		if (DD > lifeTime) isDead = true;
		direction += (rand()%(maxDiff*2))-maxDiff+0.75;
	}
	void onDraw()
	{
		draw();
	}
};

class bubbleWeapon : public Weapon
{
public:
	bubbleWeapon()
	{
		ammo = 300;
		frequency = 0.04;
	}

	void shot(coord startPos, float direction)
	{
		fire(new bubbleArrow,startPos, direction);
	}
};


class superBubbleArrow : public bubbleArrow
{
public:
	superBubbleArrow()
	{
		size = coord(5,5);

		color.r = 0;
		color.g = 0;
		color.b = float((rand() % 80) + 20)/100;

		speed = 3.5;
		damage = 5;
		lifeTime = 2;
		maxDiff = 3;
	}
};

class superBubbleWeapon : public bubbleWeapon
{
public:
	superBubbleWeapon()
	{
		ammo = 100;
		frequency = 0;
	}
	void shot(coord startPos, float direction)
	{
		fire(new superBubbleArrow,startPos, direction);
	}
};

class sniperArrow : public arrow
{
public:
	sniperArrow()
	{
		size = coord(5,5);
		color = ccolor(0.5,0.5,0.5);
		speed = 12.5;
		damage = 60;
		lifeTime = 10.0;
	}
};

class sniperWeapon : public Weapon
{
public:
	sniperWeapon()
	{
		ammo = 20;
		frequency = 2.75;
	}
	void shot(coord startPos, float direction)
	{
		fire(new sniperArrow,startPos, direction);
	}
};

class superSniperArrow : public sniperArrow
{
public:
	superSniperArrow()
	{
		color = ccolor(1,1,0);
		damage = 100;
	}
	void handleCollision(entity *e)
	{
		if (typeid(*e) == typeid(band))
		{
			e->isDead = true;
		}
	}
};

class superSniperWeapon : public sniperWeapon
{
public:
	superSniperWeapon()
	{
		ammo = 4;
		frequency = 2.75;
	}
	void shot(coord startPos, float direction)
	{
		fire(new superSniperArrow,startPos, direction);
	}
};

class machineArrow : public arrow
{
public:
	machineArrow()
	{
		size = coord(5,5);
		speed = 9.5;
		lifeTime = 0.3;

		color = ccolor(0,0,1);
		damage = 3.5;
	}
};

class machineWeapon : public Weapon
{
public:
	machineWeapon()
	{
		ammo = 300;
		frequency = 0.12;
	}
	void shot(coord startPos, float direction)
	{
		fire(new machineArrow,startPos, direction);
	}
};

class superMachineArrow : public machineArrow
{
public:
	superMachineArrow()
	{
		size = coord(5,5);
		speed = 10;
		lifeTime = 0.8;

		color = ccolor(0,0,1);
		damage = 4;
	}
};

class superMachineWeapon : public machineWeapon
{
public:
	superMachineWeapon()
	{
		ammo = 120;
		frequency = 0.05;
	}
	void shot(coord startPos, float direction)
	{
		fire(new superMachineArrow,startPos, direction+2);
		fire(new superMachineArrow,startPos, direction-2);
	}
};

#endif