#include <list>
#include <fstream>
#include <iostream>
#include <string>
//#include <algorithm>
#include <sstream>
//#include <iterator>
//#include <iomanip>

using namespace std;

#include <math.h>

#include "inputsystem.h"
#include "hirestimer.h"

CKeyboard *keyboard;
CHiResTimer timer;

#include "useful.h"

list<entity*> container;
bool menu;

class warrior;
warrior *w1,*w2;

#include "ground.h"
#include "band.h"
#include "arrow.h"
#include "warrior.h"

int level = 0;

void init()
{
	CInputSystem input;
	input.Initialize(g_hwnd, g_hInstance, true, IS_USEKEYBOARD);
	keyboard = input.GetKeyboard();

	ifstream cfg("data/cfg.txt");
	string property = "";
	int value = 0;

	glClearColor(0,0,0.5,1);

	while (cfg >> property >> value)
	{
		if (property == "texturedbands")	band::textured = value;
		if (property == "texturedground")	ground::textured = value;
		if (property == "startlevel")		level = value;
	}
	
	container.push_back(new ground);

	if (ground::textured)
		ground::skin.LoadTexture("data/textures/ground.bmp");

	w1 = new warrior;
	w1->pos = w1->startPos = coord(50, 50);
	w1->skin.LoadTexture("data/textures/player1.bmp");
	w1->keys.left	=	DIK_A;
	w1->keys.right	=	DIK_D;
	w1->keys.up		=	DIK_W;
	w1->keys.down	=	DIK_S;
	w1->keys.fire	=	DIK_LCONTROL;
	w1->keys.nextWeapon	=	DIK_Z;
	w1->keys.strefeLeft	=	DIK_Q;
	w1->keys.strefeRight=	DIK_E;
	container.push_back(w1);

	w2 = new warrior;
	w2->pos = w2->startPos = coord(900, 700);
	w2->skin.LoadTexture("data/textures/player2.bmp");
	w2->keys.left	=	DIK_J;
	w2->keys.right	=	DIK_L;
	w2->keys.up		=	DIK_I;
	w2->keys.down	=	DIK_K;
	w2->keys.fire	=	DIK_N;
	w2->keys.nextWeapon	=	DIK_M;
	w2->keys.strefeLeft	=	DIK_U;
	w2->keys.strefeRight=	DIK_O;
	container.push_back(w2);

	{
		stringstream s;
		s << "data/band" << level << ".txt";
		string ss;
		s >> ss;

		coord temp2;
		ifstream file2(ss.c_str());
		while (file2 >> temp2.x >> temp2.y)
		{
			band *b = new band;
			b->pos = temp2;
			container.push_back(b);
		}
	}

	if (band::textured)
	{
		band::skin[0].LoadTexture("data/textures/band0.bmp");
		band::skin[1].LoadTexture("data/textures/band1.bmp");
		band::skin[2].LoadTexture("data/textures/band2.bmp");
		band::skin[3].LoadTexture("data/textures/band3.bmp");
		band::skin[4].LoadTexture("data/textures/band4.bmp");
	}

	menu = false;
}

void displayPlayer(warrior *w, coord pos,coord size)
{
	glViewport(pos.x, pos.y, size.x, size.y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, size.x, 0, size.y);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(
		-w->pos.x + size.x/2 - w->size.x/2,
		-w->pos.y + size.y/2 - w->size.y/2,
		0.0f
		);

	for (list<entity*>::iterator i = container.begin(); i != container.end(); ++i)
	{
		if (
			(*i)->pos.x < w->pos.x + w->size.x/2 + size.x/2 &&
			(*i)->pos.x + (*i)->size.x > w->pos.x - w->size.x/2 - size.x/2 &&
			(*i)->pos.y < w->pos.y + w->size.y/2 + size.y/2 &&
			(*i)->pos.y + (*i)->size.y > w->pos.y - w->size.y/2 - size.y/2
			)
		{
			(*i)->onDraw();
		}
	}
}

void gamecycle()
{
	time::delta = timer.GetElapsedSeconds();
	time::sinceStart += time::delta;
	keyboard->Update();

	if (keyboard->KeyDown(DIK_ESCAPE))
	{
		done = true;
	}

	if (!menu)
	{
		list<entity*>::iterator i;
		for (i = container.begin(); i != container.end(); ++i)
		{
			(*i)->update();

			list<entity*>::iterator ci;
			ci = i;
			++ci;
			for (; ci != container.end(); ++ci)
			{
				if ((*i)->isCollisionWith((*ci)))
				{
					(*i)->handleCollision((*ci));
					(*ci)->handleCollision((*i));
				}
			}

			if ((*i)->isDead)
			{
				delete (*i);
				i = container.erase(i);
			}
		}
	}
	else
	{
		if (keyboard->KeyDown(DIK_SPACE))
		{
			menu = false;
		}
	}

	glClear(GL_COLOR_BUFFER_BIT);

	displayPlayer(w1, coord(0,0),coord(398,600));
	displayPlayer(w2, coord(402,0),coord(398,600));

	if (menu)
	{
		const coord sector(256,128);
		glViewport(800/2 - sector.x/2, 600/2 - sector.y/2,sector.x,sector.y);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, sector.x, 0, sector.y);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		static CTexture skin("data/textures/menu.bmp");

		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, skin.texID);
		glScalef(sector.x,sector.y,0);
		glColor3f(1,1,1);
		glBegin(GL_QUADS);
			glTexCoord2i(0,0); glVertex2f(0,0);
			glTexCoord2i(1,0); glVertex2f(1,0);
			glTexCoord2i(1,1); glVertex2f(1,1);
			glTexCoord2i(0,1); glVertex2f(0,1);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}

	{
		const coord sector(100, 30);
		glViewport(400 - 0.5*sector.x,0,sector.x,sector.y);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, sector.x, 0, sector.y);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glPushMatrix();
		glScalef(sector.x,sector.y,0);
		glColor3f(1,1,1);
		glBegin(GL_QUADS);
			glVertex2f(0,0);
			glVertex2f(1,0);
			glVertex2f(1,1);
			glVertex2f(0,1);
		glEnd();
		glPopMatrix();

		const float border = 2;
		glPushMatrix();
		glTranslatef(0,sector.y - border,0);
		glScalef(sector.x,border,0);
		glColor3f(0,0,0);
		glBegin(GL_QUADS);
			glVertex2f(0,0);
			glVertex2f(1,0);
			glVertex2f(1,1);
			glVertex2f(0,1);
		glEnd();
		glPopMatrix();

		glPushMatrix();
		glScalef(border,sector.y,0);
		glColor3f(0,0,0);
		glBegin(GL_QUADS);
			glVertex2f(0,0);
			glVertex2f(1,0);
			glVertex2f(1,1);
			glVertex2f(0,1);
		glEnd();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(sector.x - border,0,0);
		glScalef(border,sector.y,0);
		glColor3f(0,0,0);
		glBegin(GL_QUADS);
			glVertex2f(0,0);
			glVertex2f(1,0);
			glVertex2f(1,1);
			glVertex2f(0,1);
		glEnd();
		glPopMatrix();

		glColor3f(0,0.8,0);
		for (int i = 0; i < w1->score; ++i)
		{
			glPushMatrix();
			glTranslatef(2 + i * 10,5,0);
			glScalef(6,6,0);
			glBegin(GL_QUADS);
				glVertex2f(0,0);
				glVertex2f(1,0);
				glVertex2f(1,1);
				glVertex2f(0,1);
			glEnd();
			glPopMatrix();
		}

		glColor3f(0,0,1);
		for (int i = 0; i < w2->score; ++i)
		{
			glPushMatrix();
			glTranslatef(2 + i * 10,15,0);
			glScalef(6,6,0);
			glBegin(GL_QUADS);
				glVertex2f(0,0);
				glVertex2f(1,0);
				glVertex2f(1,1);
				glVertex2f(0,1);
			glEnd();
			glPopMatrix();
		}
		const int winLimit = 10;
		if (w1->score > winLimit || w2->score > winLimit)
		{
			if (w1->score > winLimit)
				MessageBox(g_hwnd,"gracz nr 1 (zielony) wygra³!","koniec gry",MB_OK | MB_ICONASTERISK);
			if (w2->score > winLimit)
				MessageBox(g_hwnd,"gracz nr 2 (niebieski) wygra³!","koniec gry",MB_OK | MB_ICONASTERISK);

			warrior::reset();
		}
	}
}