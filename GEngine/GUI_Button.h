#pragma once

#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

#include "GUI_BaseObject.h"

const g_vec2 GUI_BUTTON_TEXOFFSET[3] = {g_vec2(0, 0),
                                        g_vec2(0, 85.0f/256.0f),
										g_vec2(0, 170.0f/256.0f)};

const g_vec2 GUI_BUTTON_TEXSCALE = g_vec2(1.0f, 85.0f/256.0f);

/*struct G_UIBUTTON_SAVE_DATA
{
  G_UIOBJECT_SAVE_DATA CommonData;
  //Personal Data
  g_char Caption[256];
};*/

class GUI_Button :
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

  virtual void OnMouseLeave(g_int32 X, g_int32 Y);
  virtual void OnMouseEnter(g_int32 X, g_int32 Y);
  virtual void OnMouseMove(g_int32 X, g_int32 Y) {}; 
  virtual void OnClick(g_int32 Button, g_int32 X, g_int32 Y);
  virtual void OnMouseDown(g_int32 Button, g_int32 X, g_int32 Y);
  virtual void OnMouseUp(g_int32 Button, g_int32 X, g_int32 Y);
  virtual void OnDraw() {};

  virtual void UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite);

  //Импорт и экспорт
  //virtual void GetRawSettings(void **pBuffer, g_uint32 &BufSize);
  //virtual void SetRawSettings(void *Buffer, g_uint32 BufSize);

  //Конструктор и деструктор
  GUI_Button(const g_string &Caption, const g_string &Name, GD3DDevice *Device);
  GUI_Button(GModel *Model, GUI_BaseObject *Parent, const g_string &Caption, const g_string &Name, GD3DDevice *Device);
  ~GUI_Button(void);
private:
  g_uint32  _stage;

  void      _init_button(const g_string &Caption);
};

#endif //GUI_BUTTON_H