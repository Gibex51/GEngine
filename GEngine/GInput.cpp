#include "GInput.h"

//-----------------------------------------------

GInput::GInput()
{
  _amz = _amx = _amy = 0;
  _l = _r = _t = _b = 0;
  _inited = false;
  memset(_kbuff,0,sizeof(_kbuff));
  memset(_kprevbuff,0,sizeof(_kprevbuff));
  memset(_mstate.rgbButtons,0,sizeof(_mstate.rgbButtons));
  memset(_mprevbuff,0,sizeof(_mprevbuff));
  GDirectInput = NULL;
  GKeyboard = NULL;
  GMouse = NULL;
}

//-----------------------------------------------

GInput::~GInput(void)
{

}

//-----------------------------------------------

g_bool GInput::InitDevices(HINSTANCE hInst)
{
  if FAILED(DirectInput8Create(hInst,DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&GDirectInput,NULL))
    return false;
		
  if FAILED(GDirectInput->CreateDevice(GUID_SysKeyboard, &GKeyboard, NULL))
    return false;

  if FAILED(GKeyboard->SetDataFormat(&c_dfDIKeyboard))
    return false;

  if FAILED(GDirectInput->CreateDevice(GUID_SysMouse, &GMouse, NULL))
    return false;

  if FAILED(GMouse->SetDataFormat(&c_dfDIMouse))
    return false;

  _inited = true;

  return true;
}

//-----------------------------------------------

g_bool GInput::SetCoopLevel(HWND hwnd, g_int32 gcooplvl)
{
  g_uint32 CoopLvl = DISCL_FOREGROUND;
  switch (gcooplvl)
  {
    case G_EXCLUSIVE: CoopLvl |= DISCL_EXCLUSIVE; break;
    case G_NONEXCLUSIVE: CoopLvl |= DISCL_NONEXCLUSIVE; break;
  }
  
  if FAILED(GKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))
    return false;

  if FAILED(GMouse->SetCooperativeLevel(hwnd, CoopLvl))
    return false;

  return true;
}

//-----------------------------------------------

g_bool GInput::Acquire()
{
  if (_inited)
  {
    if FAILED(GKeyboard->Acquire())
      return false;

    if FAILED(GMouse->Acquire())
      return false;
  
    return true;
  }
  else
	return false;
}

//-----------------------------------------------

g_bool GInput::Unacquire()
{
  if (_inited)
  {
    if FAILED(GKeyboard->Unacquire())
      return false;

    if FAILED(GMouse->Unacquire())
      return false;

    return true;
  }
  else
	return false;
}

//-----------------------------------------------

void GInput::GetDeviceState()
{
  memcpy(_kprevbuff,_kbuff,sizeof(_kbuff));
  memcpy(_mprevbuff,_mstate.rgbButtons,sizeof(_mprevbuff));
  GKeyboard->GetDeviceState(sizeof(_kbuff),(LPVOID)&_kbuff);
  GMouse->GetDeviceState(sizeof(_mstate),(LPVOID)&_mstate);
  RX = _mstate.lX;
  RY = _mstate.lY;
  RZ = _mstate.lZ;
  _amx += RX;
  _amy += RY;
  _amz += RZ;
  if (_l != _r)
  {
	if (_amx < _l) _amx = _l;
	if (_amx > _r) _amx = _r;
  }  
  if (_t != _b)
  {
	if (_amy < _t) _amy = _t;
	if (_amy > _b) _amy = _b;
  } 
  AX = _amx;
  AY = _amy;
}

//-----------------------------------------------

void GInput::SetCoords(g_long _x, g_long _y, g_long _z)
{
  _amx = _x;
  _amy = _y;
  _amz = _z;
}

//-----------------------------------------------

void GInput::SetMouseRect(g_int32 r_left, g_int32 r_right, g_int32 r_top, g_int32 r_bottom)
{
  _l = r_left;
  _r = r_right;
  _t = r_top;
  _b = r_bottom;
}

//-----------------------------------------------

void GInput::GetMouseRect(g_rect &mouse_rect)
{
  mouse_rect.left = _l;
  mouse_rect.right = _r;
  mouse_rect.top = _t;
  mouse_rect.bottom = _b;
}

//-----------------------------------------------

g_bool GInput::Release()
{
  if (_inited)
  {
    if FAILED(GKeyboard->Unacquire())
      return false;

    if FAILED(GMouse->Unacquire())
      return false;

    _inited = false;

    GKeyboard->Release();

    GMouse->Release();

    return true;
  }
  else
	return false;
}

//-----------------------------------------------

g_bool GInput::OnKeyPress(const g_int32 key)
{
  return ((_kbuff[key] & 0x80) != 0);
}

//-----------------------------------------------

g_bool GInput::OnKeyDown(const g_int32 key)
{
  return (((_kbuff[key] & 0x80) != 0 ) && ((_kprevbuff[key] & 0x80) == 0));
}

//-----------------------------------------------

g_bool GInput::OnKeyUp(const g_int32 key)
{
  return (((_kbuff[key] & 0x80) == 0 ) && ((_kprevbuff[key] & 0x80) != 0));
}

//-----------------------------------------------

g_bool GInput::OnMousePress(const g_int32 key)
{
  return ((_mstate.rgbButtons[key] & 0x80) != 0);
}

//-----------------------------------------------

g_bool GInput::OnMouseDown(const g_int32 key)
{
  return (((_mstate.rgbButtons[key] & 0x80) != 0 ) && ((_mprevbuff[key] & 0x80) == 0));
}

//-----------------------------------------------

g_bool GInput::OnMouseUp(const g_int32 key)
{
  return (((_mstate.rgbButtons[key] & 0x80) == 0 ) && ((_mprevbuff[key] & 0x80) != 0));
}

//-----------------------------------------------

void GInput::BindKey(g_uint32 Action_UID, g_uint8 GIKeyCode, g_uint8 GIKeyMod, g_uint8 GI_ActType)
{
  Actions[Action_UID].KeyCode = GIKeyCode;
  Actions[Action_UID].KeyMod = GIKeyMod;
  Actions[Action_UID].KeyAction = GI_ActType;
}

//-----------------------------------------------

void GInput::UnBindKey(g_uint32 Action_UID)
{
  Actions[Action_UID].KeyCode = GI_KC_NOKEY;
  Actions[Action_UID].KeyMod = GMOD_NOCHECK;
}

//-----------------------------------------------

g_bool GInput::CheckAction(g_uint32 Action_UID)
{
  g_uint8 keymod = CheckKeyMods();

  if ((Actions[Action_UID].KeyMod & GMOD_NOCHECK) != GMOD_NOCHECK)
  {
    if (Actions[Action_UID].KeyMod != keymod)
	  return false;
  }

  g_uint8 keycode = Actions[Action_UID].KeyCode;
  if (keycode == GI_KC_NOKEY)
    return false;
 
  g_char key_st, prev_key_st;
  if (keycode <= 251)
  {
    key_st = _kbuff[keycode];
    prev_key_st = _kprevbuff[keycode];
  }
  else
  {
    key_st = _mstate.rgbButtons[keycode - 252];
    prev_key_st = _mprevbuff[keycode - 252];
  }

  switch (Actions[Action_UID].KeyAction)
  {
    case GI_AT_KEYPRESS: return ((key_st & 0x80) != 0);
    case GI_AT_KEYUP:    return (((key_st & 0x80) == 0 ) && ((prev_key_st & 0x80) != 0));
    case GI_AT_KEYDOWN:  return (((key_st & 0x80) != 0 ) && ((prev_key_st & 0x80) == 0));
    default: return false;
  }

  return false;
}

//-----------------------------------------------

g_int8 GInput::CheckKeyMods()
{
  g_int8 mres = 0;
  if (OnKeyPress(GI_KC_LSHIFT) || OnKeyPress(GI_KC_RSHIFT))
    mres += GMOD_SHIFT;

  if (OnKeyPress(GI_KC_LCONTROL) || OnKeyPress(GI_KC_RCONTROL))
    mres += GMOD_CTRL;

  if (OnKeyPress(GI_KC_LMENU) || OnKeyPress(GI_KC_RMENU))
    mres += GMOD_ALT;

  return mres;
}

//-----------------------------------------------
