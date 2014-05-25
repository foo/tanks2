#ifndef _USEFUL_H
#define _USEFUL_H

class coord
{
public:
	coord() {}
	coord(float x_p, float y_p) : x(x_p), y(y_p) {}
	float x,y;
};
class ccolor
{
public:
	ccolor(float r_p = 1,float g_p = 1,float b_p = 1) : r(r_p), g(g_p), b(b_p) {}
	float r,g,b;
};
class rectangular
{
public:
	coord pos, size;
	
	void slide(rectangular *c)
	{
		float margin = 6;
		float depth = 6;

		rectangular top, bottom, left, right;

		right.pos.x = c->pos.x + c->size.x - depth;
		right.pos.y = c->pos.y + margin;
		right.size.x = depth;
		right.size.y = c->size.y - 2*margin;

		left.pos.x = c->pos.x;
		left.pos.y = c->pos.y + margin;
		left.size.x = depth;
		left.size.y = c->size.y - 2*margin;

		bottom.pos.x = c->pos.x + margin;
		bottom.pos.y = c->pos.y;
		bottom.size.x = c->size.x - 2*margin;
		bottom.size.y = depth;

		top.pos.x = c->pos.x + margin;
		top.pos.y = c->pos.y + c->size.y - depth;
		top.size.x = c->size.x - 2*margin;
		top.size.y = depth;

		if (isCollisionWith(&left)) pos.x = left.pos.x - size.x;
		if (isCollisionWith(&right)) pos.x = right.pos.x + right.size.x;
		if (isCollisionWith(&bottom)) pos.y = bottom.pos.y - size.y;
		if (isCollisionWith(&top)) pos.y = top.pos.y + top.size.y;
	}
	bool isCollisionWith(rectangular *c)
	{
		if (
			pos.x + size.x > c->pos.x &&
			c->pos.x + c->size.x > pos.x &&
			
			pos.y + size.y > c->pos.y &&
			c->pos.y + c->size.y > pos.y
			)
		{
			return true;
		}
		return false;
	}
};

class entity : public rectangular
{
public:
	entity() : isDead(false) {}
	virtual ~entity() {}

public:
	bool isDead;
	ccolor color;
public:
	virtual void update() {}
	virtual void handleCollision(entity *c) {}
	virtual void onDraw() { draw(); }

public:
	void draw()
	{
		glPushMatrix();
		glColor3f(color.r,color.g,color.b);
		glTranslatef(pos.x,pos.y,0);
		glScalef(size.x,size.y,0);
		glBegin(GL_QUADS);
			glVertex2f(0,0);
			glVertex2f(1,0);
			glVertex2f(1,1);
			glVertex2f(0,1);
		glEnd();
		glPopMatrix();
	}
	void drawTextured(GLuint texID)
	{
		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, texID);
		glTranslatef(pos.x,pos.y,0);
		glScalef(size.x,size.y,0);
		glColor3f(color.r,color.g,color.b);
		glBegin(GL_QUADS);
			glTexCoord2i(0,0); glVertex2f(0,0);
			glTexCoord2i(1,0); glVertex2f(1,0);
			glTexCoord2i(1,1); glVertex2f(1,1);
			glTexCoord2i(0,1); glVertex2f(0,1);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
	void drawArrow(float direction, float length)
	{
		glPushMatrix();
		glColor3f(1,0,0);
		glTranslatef(pos.x + size.x/2,pos.y + size.y/2,0);
		glRotatef(direction,0,0,1);
		glScalef(length,length,0);
			glBegin(GL_LINES);
				glVertex2f(0,0);
				glVertex2f(1,0);
				
				glVertex2f(1,0);
				glVertex2f(0.75,0.16);

				glVertex2f(1,0);
				glVertex2f(0.75,-0.16);
			glEnd();
		glPopMatrix();
	}

};
#endif