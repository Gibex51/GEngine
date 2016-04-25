#pragma once

#ifndef GUI_EDIT_H
#define GUI_EDIT_H

#include "GUI_BaseObject.h"

const g_vec2 GUI_EDIT_TEXOFFSET[2] = {g_vec2(0, 0),
                                      g_vec2(0, 0.5f)};

const g_vec2 GUI_EDIT_TEXSCALE = g_vec2(1.0f, 0.5f);

/*struct G_UIEDIT_SAVE_DATA
{
  G_UIOBJECT_SAVE_DATA CommonData;
  //Personal Data
  g_bool ReadOnly;
  g_bool Multiline;
  g_int32 MaxLength;
  g_int32 TextLength;
};*/

class GUI_Edit :
  public GUI_BaseObject
{
public:
  GBuffer Text;
  bool   ReadOnly;
  bool   Multiline;
 
  virtual void OnFocusSet();
  virtual void OnFocusLost();
  virtual void OnShow();
  virtual void OnHide();
  virtual void OnResize() {};
  virtual void OnSetPosition() {};

  virtual void OnKeyDown(g_int32 Key);
  virtual void OnKeyUp(g_int32 Key);
  virtual void OnKeyPress(g_int32 Key);
  virtual void OnChar(g_int32 Key);

  virtual void OnMouseLeave(g_int32 X, g_int32 Y);
  virtual void OnMouseEnter(g_int32 X, g_int32 Y);
  virtual void OnClick(g_int32 Button, g_int32 X, g_int32 Y);
  virtual void OnMouseDown(g_int32 Button, g_int32 X, g_int32 Y);
  virtual void OnMouseUp(g_int32 Button, g_int32 X, g_int32 Y);
  virtual void OnDraw() {};

  virtual void UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite);

  void SetMaxLength(g_uint32 MaxLength) {_max_length = MaxLength;};
  void UpdateText();

  //Импорт и экспорт
  //virtual void GetRawSettings(void **pBuffer, g_uint32 &BufSize);
  //virtual void SetRawSettings(void *Buffer, g_uint32 BufSize);

  //Конструктор и деструктор
  GUI_Edit(const g_string &Name, GD3DDevice *Device);
  GUI_Edit(GModel *Model, GUI_BaseObject *Parent, const g_string &Name, GD3DDevice *Device);
  ~GUI_Edit(void);
private:
  g_int32  _first_char_pos;  // 1 based index
  g_uint32 _n_visible_chars;
  g_int32  _caret_pos;       // 0 based index

  g_int32  _max_length;

  g_int32  _stage;

  void _init_edit();
};

#endif //GUI_EDIT_H