#include "InputSystem.h"


CKeyboard::CKeyboard(LPDIRECTINPUT8 pDI, HWND hwnd)
{
  if (FAILED(pDI->CreateDevice(GUID_SysKeyboard, &m_pDIDev, NULL)))
  {
    // obs�uga b�edu
  }

  if (FAILED(m_pDIDev->SetDataFormat(&c_dfDIKeyboard)))
  {
    // obs�uga b�edu
  }

  if (FAILED(m_pDIDev->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
  {
    // obs�uga b�edu
  }

  if (FAILED(m_pDIDev->Acquire()))
  {
    // obs�uga b�edu
  }

  Clear();
}


CKeyboard::~CKeyboard()
{
  if (m_pDIDev)
  {
    m_pDIDev->Unacquire();
    m_pDIDev->Release();
  }
}


bool CKeyboard::Acquire()
{
  Clear();
  return (!FAILED(m_pDIDev->Acquire()));
}


bool CKeyboard::Unacquire()
{
  Clear();
  return (!FAILED(m_pDIDev->Unacquire()));
}


bool CKeyboard::Update()
{
  if (FAILED(m_pDIDev->GetDeviceState(sizeof(m_keys), (LPVOID)m_keys)))
  {
    if (FAILED(m_pDIDev->Acquire()))
    {
      return false;
    }
    if (FAILED(m_pDIDev->GetDeviceState(sizeof(m_keys), (LPVOID)m_keys)))
    {
      return false;
    }
  }
  return true;
}


CMouse::CMouse(LPDIRECTINPUT8 pDI, HWND hwnd, bool isExclusive)
{
  if (FAILED(pDI->CreateDevice(GUID_SysMouse, &m_pDIDev, NULL)))
  {
    // obs�uga b�edu
  }

  if (FAILED(m_pDIDev->SetDataFormat(&c_dfDIMouse)))
  {
    // obs�uga b�edu
  }

  DWORD flags;

  if (isExclusive)
    flags = DISCL_FOREGROUND | DISCL_EXCLUSIVE | DISCL_NOWINKEY;
  else
    flags = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;


  if (FAILED(m_pDIDev->SetCooperativeLevel(hwnd, flags)))
  {
    // obs�uga b�edu
  }

  if (FAILED(m_pDIDev->Acquire()))
  {
    // obs�uga b�edu
  }

  if (FAILED(m_pDIDev->GetDeviceState(sizeof(DIMOUSESTATE), &m_state)))
  {
    // obs�uga b�edu
  }
}


bool CMouse::Update()
{
  if (FAILED(m_pDIDev->GetDeviceState(sizeof(DIMOUSESTATE), &m_state)))
  {
    if (FAILED(m_pDIDev->Acquire()))
    {
      return false;
    }
    if (FAILED(m_pDIDev->GetDeviceState(sizeof(DIMOUSESTATE), &m_state)))
    {
      return false;
    }
  }

  return true;
}




CMouse::~CMouse()
{
  if (m_pDIDev)
  {
    m_pDIDev->Unacquire();
    m_pDIDev->Release();
  }
}


bool CMouse::Acquire()
{
  return (!FAILED(m_pDIDev->Acquire()));
}


bool CMouse::Unacquire()
{
  return (!FAILED(m_pDIDev->Unacquire()));
}


CJoystick::CJoystick(LPDIRECTINPUT8 pDI, HINSTANCE appInstance)
{
}


CJoystick::~CJoystick()
{
  if (m_pDIDev)
  {
    m_pDIDev->Unacquire();
    m_pDIDev->Release();
  }
}


bool CInputSystem::Initialize(HWND hwnd, HINSTANCE appInstance, bool isExclusive, DWORD flags)
{
  if (FAILED(DirectInput8Create(appInstance,
                               DIRECTINPUT_VERSION,
                               IID_IDirectInput8,
                               (void **)&m_pDI,
                               NULL)))
    return false;

  if (flags & IS_USEKEYBOARD)
  {
    m_pKeyboard = new CKeyboard(m_pDI, hwnd);
  }
  if (flags & IS_USEMOUSE)
  {
    m_pMouse = new CMouse(m_pDI, hwnd, true);
  }
  if (flags & IS_USEJOYSTICK)
  {
    m_pJoystick = new CJoystick(m_pDI, appInstance);
  }
  return true;
}


bool CInputSystem::Shutdown()
{
  UnacquireAll();
  if (m_pKeyboard)
  {
    delete m_pKeyboard;
    m_pKeyboard = NULL;
  }

  if (m_pKeyboard)
  {
    delete m_pMouse;
    m_pMouse = NULL;
  }

  if (m_pJoystick)
  {
    delete m_pJoystick;
    m_pJoystick = NULL;
  }

  if (FAILED(m_pDI->Release()))
    return false;

  return true;
}

bool CInputSystem::Update()
{
  if (m_pKeyboard)
    m_pKeyboard->Update();
  if (m_pMouse)
    m_pMouse->Update();
  if (m_pJoystick)
    m_pJoystick->Update();

  return true;
}


void CInputSystem::AcquireAll()
{
  if (m_pKeyboard)
    m_pKeyboard->Acquire();
  if (m_pMouse)
    m_pMouse->Acquire();
  if (m_pJoystick)
    m_pJoystick->Acquire();
}


void CInputSystem::UnacquireAll()
{
  if (m_pKeyboard)
    m_pKeyboard->Unacquire();
  if (m_pMouse)
    m_pMouse->Unacquire();
  if (m_pJoystick)
    m_pJoystick->Unacquire();
}


bool CJoystick::Acquire()
{
  return (!FAILED(m_pDIDev->Acquire()));
}


bool CJoystick::Unacquire()
{
  return (!FAILED(m_pDIDev->Unacquire()));
}


bool CJoystick::Update()
{
  return true;
}