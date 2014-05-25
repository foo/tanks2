/****************************************************************************
 HiResTimer.h
 
 Klasa obudowuj�ca licznik czasu wysokiej rozdzielczo�ci. 
 Nie mo�e by� u�ywana, je�li licznik taki nie jest dost�pny.
  
 Autor   :   Dave Astle
 Data     :   2/1/2001

 Opracowano dla potrzeb ksi��ki "Programowanie gier w OpenGL"
*****************************************************************************/

#ifndef __TIMER_H_INCLUDED__
#define __TIMER_H_INCLUDED__

#include <windows.h>


class CHiResTimer
{
public:
  CHiResTimer() { Init(); }
  ~CHiResTimer() {}

/*****************************************************************************
 Init()

 Je�li dostepny jest licznik czasu o du�ej dok�adno�ci, to funkcja Init()
 zapami�tuje jego rozdzielczo�� i zwraca warto�� true. 
 W przeciwnym razie zwraca warto�� false i wtedy licznik nie mo�e by� u�ywany.
*****************************************************************************/
bool Init()
{
  if (!QueryPerformanceFrequency(&m_ticksPerSecond))
  {
    // system nie posiada licznika czasu o du�ej dok�adno�ci
    return false;
  }
  else
  {
    QueryPerformanceCounter(&m_startTime);
    return true;
  }
} // Init()


float GetElapsedSeconds(unsigned long elapsedFrames = 1)
{
  static LARGE_INTEGER s_lastTime = m_startTime;
  LARGE_INTEGER currentTime;

  QueryPerformanceCounter(&currentTime);

  float seconds =  ((float)currentTime.QuadPart - (float)s_lastTime.QuadPart) / (float)m_ticksPerSecond.QuadPart;

  // "zeruje" licznik czasu
  s_lastTime = currentTime;

  return seconds;
} // GetElapsedSeconds()


/***************************************************************************
 GetFPS()

 Zwraca �redni� liczb� klatek w okresie czasu elapsedFrames 
 (domy�lnie r�wnym 1). Je�li metoda nie jest wywo�ywana podczas tworzenia 
 ka�dej ramki, to nale�y samemu zlicza� liczb� ramek oraz zerowa� j� po
 wywo�aniu metody.
***************************************************************************/
float GetFPS(unsigned long elapsedFrames = 1)
{
  static LARGE_INTEGER s_lastTime = m_startTime;
  LARGE_INTEGER currentTime;

  QueryPerformanceCounter(&currentTime);

  float fps = (float)elapsedFrames * (float)m_ticksPerSecond.QuadPart / ((float)currentTime.QuadPart - (float)s_lastTime.QuadPart);

  // "zeruje" licznik czasu
  s_lastTime = currentTime;

  return fps;
} // GetFPS


/***************************************************************************
 LockFPS()

 Blokuje wykonanie programu tak, by pr�dko�� tworzenia klatek nie wzros�a
 ponad warto�� okre�lona przez parametr metody. Metoda ta nie zapewnie 
 utrzymania sta�ej pr�dko�ci tworzenia klatek, a jedynie zapobiega jej 
 wzrostowi ponad podan� warto��. Natomiast pr�dko�� ta w praktyce mo�e spa��
 poni�ej tej warto�ci. Zak�ada si�, �e metoda ta wywo�ywana jest podczas
 tworzenia ka�dej klatki. Metoda zwraca chwilow� warto�� pr�dko�ci tworzenie
 klatek (w klatkach na sekund�) <= targetFPS.
***************************************************************************/
float LockFPS(unsigned char targetFPS)
{
  if (targetFPS == 0)
    targetFPS = 1;

  static LARGE_INTEGER s_lastTime = m_startTime;
  LARGE_INTEGER currentTime;
  float   fps;

  // odczekuje w p�tli okres czasu, by osi�gn�� zadan� pr�dko��
  do {
    QueryPerformanceCounter(&currentTime);
    fps = (float)m_ticksPerSecond.QuadPart/((float)(currentTime.QuadPart - s_lastTime.QuadPart));
  } while (fps > (float)targetFPS);

  // "zeruje" licznik czasu
  s_lastTime = m_startTime;

  return fps;
} // LockFPS()

static float D;

private:
  LARGE_INTEGER   m_startTime;
  LARGE_INTEGER   m_ticksPerSecond;
};

class time
{
public:
	static float delta;
	static float sinceStart;
};
float time::delta = 0.0f;
float time::sinceStart = 0.0f;

#endif // __TIMER_H_INCLUDED_
