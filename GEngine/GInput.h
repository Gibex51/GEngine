#pragma once

#include "GStructs.h"

#ifndef GINPUT_H
#define GINPUT_H

struct GI_ActionKey
{
  g_uint8  KeyCode;
  g_uint8  KeyMod;
  g_uint8  KeyAction;
};

class GInput
{
public:
  g_long AX,AY,RX,RY,RZ;

  LPDIRECTINPUT8 GDirectInput;
  LPDIRECTINPUTDEVICE8 GKeyboard;
  LPDIRECTINPUTDEVICE8 GMouse;

  g_bool InitDevices(HINSTANCE hInst);
  g_bool SetCoopLevel(HWND hwnd, g_int32 gcooplvl);

  g_bool Acquire();
  g_bool Unacquire();

  void GetDeviceState();
  void SetCoords(g_long x, g_long y, g_long z);
  void SetMouseRect(g_int32 r_left, g_int32 r_right, g_int32 r_top, g_int32 r_bottom);
  void GetMouseRect(g_rect &mouse_rect);

  g_bool OnKeyPress(const g_int32 key);
  g_bool OnKeyDown(const g_int32 key);
  g_bool OnKeyUp(const g_int32 key);
  g_bool OnMousePress(const g_int32 key);
  g_bool OnMouseDown(const g_int32 key);
  g_bool OnMouseUp(const g_int32 key);

  void BindKey(g_uint32 Action_UID, g_uint8 GIKeyCode, g_uint8 GIKeyMod,  g_uint8 GI_ActType);
  void UnBindKey(g_uint32 Action_UID);
  g_bool CheckAction(g_uint32 Action_UID);

  g_int8 CheckKeyMods();

  g_bool Release();

  //Конструктор и деструктор
  GInput();
  ~GInput(void);
private:
  g_bool _inited;
  DIMOUSESTATE _mstate;
  g_char _mprevbuff[4];
  g_char _kbuff[256];
  g_char _kprevbuff[256];
  g_long _amx;
  g_long _amy;
  g_long _amz;
  g_long _l,_r,_t,_b;

  map <g_uint32,GI_ActionKey> Actions;
};

#define GMOUSE_LEFT   0
#define GMOUSE_RIGHT  1
#define GMOUSE_MIDDLE 2

#define GMOD_NONE      0
#define GMOD_SHIFT     1
#define GMOD_CTRL      2
#define GMOD_ALT       4
#define GMOD_NOCHECK   8

#endif // GINPUT_H