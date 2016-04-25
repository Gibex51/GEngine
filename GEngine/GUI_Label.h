#pragma once

#ifndef GUI_LABEL_H
#define GUI_LABEL_H

#include "GUI_BaseObject.h"

/*struct G_UILABEL_SAVE_DATA
{
  G_UIOBJECT_SAVE_DATA CommonData;
  //Personal Data
  g_uint32 TextFormat;
  g_char   Caption[256];
};*/

class GUI_Label :
  public GUI_BaseObject
{
public:
  g_uint32 TextFormat;

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
  GUI_Label(const g_string &Name, GD3DDevice *Device);
  GUI_Label(GModel *Model, GUI_BaseObject *Parent, const g_string &Caption, const g_string &Name, GD3DDevice *Device);
  ~GUI_Label(void);
private:
  void _init_label(const g_string &Caption);
};

#endif //GUILABEL_H