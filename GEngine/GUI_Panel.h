#pragma once

#ifndef GUI_PANEL_H
#define GUI_PANEL_H

#include "GUI_BaseObject.h"

const g_vec2 GUI_PANEL_TEXOFFSET = g_vec2(0, 0);

const g_vec2 GUI_PANEL_TEXSCALE = g_vec2(1.0f, 1.0f);

class GUI_Panel :
  public GUI_BaseObject
{
public:
  virtual void OnFocusSet() {};
  virtual void OnFocusLost() {};
  virtual void OnShow() {};
  virtual void OnHide() {};
  virtual void OnResize() {};
  virtual void OnSetPosition() {};

  virtual void OnKeyDown(g_int32 Key) {};
  virtual void OnKeyUp(g_int32 Key) {};
  virtual void OnKeyPress(g_int32 Key) {};
  virtual void OnChar(g_int32 Key) {};

  virtual void OnMouseLeave(g_int32 X, g_int32 Y) {};
  virtual void OnMouseEnter(g_int32 X, g_int32 Y) {};
  virtual void OnMouseMove(g_int32 X, g_int32 Y) {};
  virtual void OnClick(g_int32 Button, g_int32 X, g_int32 Y) {};
  virtual void OnMouseDown(g_int32 Button, g_int32 X, g_int32 Y) {};
  virtual void OnMouseUp(g_int32 Button, g_int32 X, g_int32 Y) {};
  virtual void OnDraw() {};

  virtual void UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite);
  
  //Импорт и экспорт
  //virtual void GetRawSettings(void **pBuffer, g_uint32 &BufSize);
  //virtual void SetRawSettings(void *Buffer, g_uint32 BufSize);

  //Конструктор и деструктор
  GUI_Panel(const g_string &Name, GD3DDevice *Device);
  GUI_Panel(GModel *Model, GUI_BaseObject *Parent, const g_string &Name, GD3DDevice *Device);
  ~GUI_Panel(void);
private:
  void _init_panel();
};

#endif //GUIPANEL_H