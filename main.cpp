#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winuser.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <ctime>

#define WND_CLASS_NAME  "OpenGL Window Class"

HDC       g_hdc;					// globalny kontekst urz�dzenia
HGLRC     g_hrc;					// globalny kontekst tworzenia grafiki
BOOL      g_isFullscreen;			// prze��cza tryb pe�noekranowy i okienkowy
HWND      g_hwnd = NULL;			// uchwyt okna  
HINSTANCE g_hInstance;				// instancja aplikacji

GLuint g_width, g_height;
bool  done;
void mb(const char *str, ...);

#include "game.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL SetupWindow(char *title, int width, int height, int bits, bool isFullscreen);
void InitializeScene();
GLvoid ResizeScene(GLsizei width, GLsizei height);
BOOL KillWindow();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	MSG msg;

	SetupWindow("game", 800, 600, 32, true);

	time::delta = timer.GetElapsedSeconds();

	{
		time_t tt;
		time(&tt);
		tm *td = localtime(&tt);

		srand(td->tm_sec * td->tm_hour * td->tm_min);
	}

	init();

	done = false;

	while (!done)
	{
		if(PeekMessage(&msg, g_hwnd, NULL, NULL, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				done = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		gamecycle();

		SwapBuffers(g_hdc);
	}
	KillWindow();

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_SIZE:
		ResizeScene(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}

	return (DefWindowProc(hwnd, message, wParam, lParam));
}

BOOL SetupWindow(char *title, int width, int height, int bits, bool isFullscreen)
{
	g_isFullscreen = isFullscreen;

	g_hInstance	= GetModuleHandle(NULL);

	WNDCLASSEX  wc;
	wc.cbSize         = sizeof(WNDCLASSEX);
	wc.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc    = WndProc;
	wc.cbClsExtra     = 0;
	wc.cbWndExtra     = 0;
	wc.hInstance      = g_hInstance;
	wc.hIcon          = LoadIcon(NULL, IDI_APPLICATION);	// domy�lna ikona
	wc.hIconSm        = LoadIcon(NULL, IDI_WINLOGO);		// logo windows 
	wc.hCursor        = LoadCursor(NULL, IDC_ARROW);		// strza�ka (kursor domy�lny)
	wc.hbrBackground  = NULL;								//(HBRUSH) GetStockObject(BLACK_BRUSH);   // t�o w kolorze czarnym
	wc.lpszMenuName   = NULL;								// bez menu
	wc.lpszClassName  = WND_CLASS_NAME;
	
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL,"Nie mo�na zarejestrowa� klasy okna", "B��d", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;										// zwraca warto�� FALSE
	}
	
	if (g_isFullscreen)
	{
		// wype�nia struktur� opisuj�c� tryb wy�wietlania
		DEVMODE screenSettings;
		memset(&screenSettings,0,sizeof(screenSettings));

		screenSettings.dmSize       = sizeof(screenSettings);	
		screenSettings.dmPelsWidth  = width;			// szeroko�� ekranu
		screenSettings.dmPelsHeight = height;			// wysoko�� ekranu
		screenSettings.dmBitsPerPel = bits;				// bit�w na piksel
		screenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// pr�buje prze�aczy� kart� na zdefiniowany wy�ej tryb graficzny
		if (ChangeDisplaySettings(&screenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
		// je�li prze��czenie si� nie powiod�o, to pozwala wybra� tryb okienkowy
			if (MessageBox(NULL, "Prze��czenie do wybranego trybu graficznego nie jest mo�liwe\n"
								"Czy przej�� do tryby okinkowego?",
								"err",
								MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				g_isFullscreen = FALSE;	
			}
			else
			{
				return FALSE;
			}
		}
	}

	DWORD dwExStyle;
	DWORD dwStyle;

	// konfiguruje styl okna
	if (g_isFullscreen)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;	// okno bez ramki i paska tytu�u
		ShowCursor(FALSE);            // ukrywa kursor
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
	}

	RECT  windowRect;
	windowRect.left = 0;
	windowRect.right = (LONG) width;
	windowRect.top = 0;
	windowRect.bottom = (LONG) height;


	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	// tworzy okno
	g_hwnd = CreateWindowEx(dwExStyle,          // styl rozszerzony
							WND_CLASS_NAME,     // nazwa klasy
							title,              // nazwa aplikacji
							dwStyle |           // styl okna
							WS_CLIPCHILDREN |   // wymagane przez Opengl
							WS_CLIPSIBLINGS,    
							0, 0,               // wsp�rz�dne x,y coordinate
							windowRect.right - windowRect.left, // szeroko��
							windowRect.bottom - windowRect.top, // wysoko��
							NULL,               // uchwyt okna nadrz�dnego 
							NULL,               // uchwyt menu
							g_hInstance,        // instancja aplikacji
							NULL);              // bez dodatkowych parametr�w

	// sprawdza poprawno�� uchwytu okna
	if (!g_hwnd)
	{
	    
		KillWindow();
		MessageBox(NULL, "Nie mo�na utworzy� okna", "B��d", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	// pobiera kontekst urz�dzenia
	if (!(g_hdc = GetDC(g_hwnd)))
	{

		KillWindow();
		MessageBox(NULL,"Nie mo�na utworzy� kontekstu urz�dzenia", "B��d", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	// konfiguruje format pikseli
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// rozmiar struktury
		1,                              // domy�lna wersja
		PFD_DRAW_TO_WINDOW |            // okno grafiki OpenGL
		PFD_SUPPORT_OPENGL |            
		PFD_DOUBLEBUFFER,               // podw�jne buforowanie
		PFD_TYPE_RGBA,                  // tryb kolor�w RGBA 
		bits,                           // 32-bitowy opis kolor�w
		0, 0, 0, 0, 0, 0,               // ignoruje bity koloru, bez palety
		0,                              // bez bufora alfa
		0,                              // ignoruje bit przesuni�cia
		0,                              // bez bufora akumulacji
		0, 0, 0, 0,                     // ignoruje bity akumulacji
		16,                             // 16-bitowy bufor z
		0,                              // bez bufora powielania
		0,                              // bez bufora pomocniczego
		PFD_MAIN_PLANE,                 // g��wny plan rysowania
		0,                              // zarezerwowane
		0, 0, 0							// ignoruje maski warst
	};
	      
	GLuint  pixelFormat;

	// wybiera najodpowiedniejszy format pikseli
	if (!(pixelFormat = ChoosePixelFormat(g_hdc, &pfd)))
	{
	    
		KillWindow();
		MessageBox(NULL, "Nie mo�na dopasowa� formatu pikseli", "B��d", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	// wybiera format pikseli dla kontekstu urz�dzenia
	if(!SetPixelFormat(g_hdc, pixelFormat,&pfd))
		{
	    
			KillWindow();
			MessageBox(NULL, "Nie mo�na wybra� formatu pikseli", "B��d", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;
	}

	// tworzy kontekst tworzenia grafiki OpenGL
	if (!(g_hrc = wglCreateContext(g_hdc)))
	{
    	KillWindow();
		MessageBox(NULL, "Nie mo�na utworzy� kontekstu grafiki OpenGL", "B��d",MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	// aktywuje kontekst tworzenia grafiki
	if(!wglMakeCurrent(g_hdc, g_hrc))
	{
		KillWindow();
		MessageBox(NULL,"Nie mo�na aktywowa� kontekstu grafiki OpenGL", "B��d", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	// aktywuje okno
	ShowWindow(g_hwnd, SW_SHOW);
	SetForegroundWindow(g_hwnd);
	SetFocus(g_hwnd);

	// zmienia rozmiary sceny
	ResizeScene(width, height);

	// wykonuje jednorazow� inicjacj�
	InitializeScene();

	return TRUE;
}

void InitializeScene()
{
	glShadeModel(GL_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glHint(GL_POINT_SMOOTH, GL_NICEST);
	glHint(GL_LINE_SMOOTH, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH, GL_NICEST);
}

GLvoid ResizeScene(GLsizei width, GLsizei height)
{
	glViewport(0, 0, width, height);
	
	g_width = width;
	g_height = height;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(0, width, 0, height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

BOOL KillWindow()
{
	if (g_isFullscreen)
	{
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(TRUE);
	}

	if (g_hrc)
	{
		if (!wglMakeCurrent(NULL,NULL))
		{
		MessageBox(NULL, "Nie mo�na zwolni� kontekstu grafiki", "B��d", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(g_hrc))
		{
		MessageBox(NULL, "Nie mo�na usun�� kontekstu grafiki", "B��d", MB_OK | MB_ICONINFORMATION);
		}

		g_hrc = NULL;
	}

	if (g_hdc && !ReleaseDC(g_hwnd, g_hdc))
	{
		MessageBox(NULL, "Nie mo�na zwolni� kontekstu urz�dzenia", "B��d", MB_OK | MB_ICONINFORMATION);
		g_hdc = NULL;
	}

	if (g_hwnd && !DestroyWindow(g_hwnd))
	{
		MessageBox(NULL, "Nie mo�na usun�� okna", "B��d", MB_OK | MB_ICONINFORMATION);
		g_hwnd = NULL;
	}

	if (!UnregisterClass(WND_CLASS_NAME, g_hInstance))
	{
		MessageBox(NULL, "Nie mo�na wyrejestrowa� klasy okna", "B��d", MB_OK | MB_ICONINFORMATION);
		g_hInstance = NULL;
	}

	return TRUE;
}

void mb(const char *str, ...)
{
	char text[256];
	va_list args;

	va_start(args, str);
		vsprintf(text, str, args);
	va_end(args);

	MessageBox(g_hwnd,text,"info",MB_OK | MB_ICONINFORMATION);
}