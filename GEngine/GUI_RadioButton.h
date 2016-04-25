#pragma once

#ifndef GUI_RADBUT_H
#define GUI_RADBUT_H

#include "GUI_BaseObject.h"

const g_vec2 GUI_RADBTN_TEXOFFSET[4] = {g_vec2(0.0f, 0.0f),
                                        g_vec2(0.0f, 0.5f),
										g_vec2(0.5f, 0.0f),
										g_vec2(0.5f, 0.5f)};

const g_vec2 GUI_RADBTN_TEXSCALE = g_vec2(0.5f, 0.5f);

/*struct G_UIRADIOBUTTON_SAVE_DATA
{
  G_UIOBJECT_SAVE_DATA CommonData;
  //Personal Data
  g_bool  Checked;
  g_int32 Group;
  g_char  Caption[256];
};*/

class GUI_RadioButton :
  public GUI_BaseObject
{
public:
  void SetCheck(g_bool IsChecked);
  g_bool IsChecked() {return _checked;};

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
  GUI_RadioButton(const g_string &Name, GD3DDevice *Device);
  GUI_RadioButton(GModel *Model, GUI_BaseObject *Parent, const g_string &Caption, const g_string &Name, GD3DDevice *Device);
  ~GUI_RadioButton(void);
private:
  g_bool  _checked;
  g_int32 _group;
  g_int32 _stage;

  void _init_radbtn(const g_string &Caption);
  void _uncheck_another();
};

#endif //GUIRADBUT_H