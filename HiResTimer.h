/****************************************************************************
 HiResTimer.h
 
 Klasa obudowuj¹ca licznik czasu wysokiej rozdzielczoœci. 
 Nie mo¿e byæ u¿ywana, jeœli licznik taki nie jest dostêpny.
  
 Autor   :   Dave Astle
 Data     :   2/1/2001

 Opracowano dla potrzeb ksi¹¿ki "Programowanie gier w OpenGL"
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

 Jeœli dostepny jest licznik czasu o du¿ej dok³adnoœci, to funkcja Init()
 zapamiêtuje jego rozdzielczoœæ i zwraca wartoœæ true. 
 W przeciwnym razie zwraca wartoœæ false i wtedy licznik nie mo¿e byæ u¿ywany.
*****************************************************************************/
bool Init()
{
  if (!QueryPerformanceFrequency(&m_ticksPerSecond))
  {
    // system nie posiada licznika czasu o du¿ej dok³adnoœci
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

 Zwraca œredni¹ liczbê klatek w okresie czasu elapsedFrames 
 (domyœlnie równym 1). Jeœli metoda nie jest wywo³ywana podczas tworzenia 
 ka¿dej ramki, to nale¿y samemu zliczaæ liczbê ramek oraz zerowaæ j¹ po
 wywo³aniu metody.
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

 Blokuje wykonanie programu tak, by prêdkoœæ tworzenia klatek nie wzros³a
 ponad wartoœæ okreœlona przez parametr metody. Metoda ta nie zapewnie 
 utrzymania sta³ej prêdkoœci tworzenia klatek, a jedynie zapobiega jej 
 wzrostowi ponad podan¹ wartoœæ. Natomiast prêdkoœæ ta w praktyce mo¿e spaœæ
 poni¿ej tej wartoœci. Zak³ada siê, ¿e metoda ta wywo³ywana jest podczas
 tworzenia ka¿dej klatki. Metoda zwraca chwilow¹ wartoœæ prêdkoœci tworzenie
 klatek (w klatkach na sekundê) <= targetFPS.
***************************************************************************/
float LockFPS(unsigned char targetFPS)
{
  if (targetFPS == 0)
    targetFPS = 1;

  static LARGE_INTEGER s_lastTime = m_startTime;
  LARGE_INTEGER currentTime;
  float   fps;

  // odczekuje w pêtli okres czasu, by osi¹gn¹æ zadan¹ prêdkoœæ
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
