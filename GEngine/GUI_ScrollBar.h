#pragma once

#ifndef GUI_SCRBAR_H
#define GUI_SCRBAR_H

#include "GUI_BaseObject.h"

const g_vec2 GUI_SCRBAR_TEXOFFSET[4] = {g_vec2(0.0f, 0.0f),
                                        g_vec2(0.0f, 0.5f),
										g_vec2(0.5f, 0.0f),
                                        g_vec2(0.5f, 0.5f)};

const g_vec2 GUI_SCRBAR_TEXSCALE = g_vec2(0.5f, 0.5f);

/*struct G_UISCROLLBAR_SAVE_DATA
{
  G_UIOBJECT_SAVE_DATA CommonData;
  //Personal Data
  g_int32 Min;
  g_int32 Max;
  g_int32 Position;
};*/

class GUI_ScrollBar :
  public GUI_BaseObject
{
public:
  void SetThumbPosition(g_int32 Pos);
  g_int32 GetThumbPosition() {return _position;};
  void SetRange(g_int32 Min, g_int32 Max);

  virtual void OnFocusSet() {};
  virtual void OnFocusLost() {};
  virtual void OnShow() {};
  virtual void OnHide() {};
  virtual void OnResize();
  virtual void OnSetPosition() {};

  virtual void OnKeyDown(g_int32 Key) {};
  virtual void OnKeyUp(g_int32 Key) {};
  virtual void OnKeyPress(g_int32 Key) {};
  virtual void OnChar(g_int32 Key) {};

  virtual void OnMouseLeave(g_int32 X, g_int32 Y);
  virtual void OnMouseEnter(g_int32 X, g_int32 Y);
  virtual void OnMouseMove(g_int32 X, g_int32 Y);
  virtual void OnClick(g_int32 Button, g_int32 X, g_int32 Y);
  virtual void OnMouseDown(g_int32 Button, g_int32 X, g_int32 Y);
  virtual void OnMouseUp(g_int32 Button, g_int32 X, g_int32 Y);
  virtual void OnDraw() {};

  virtual void UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite);

  //Импорт и экспорт
  //virtual void GetRawSettings(void **pBuffer, g_uint32 &BufSize);
  //virtual void SetRawSettings(void *Buffer, g_uint32 BufSize);

  //Конструктор и деструктор
  GUI_ScrollBar(const g_string &Name, GD3DDevice *Device);
  GUI_ScrollBar(GModel *Model, GUI_BaseObject *Parent, const g_string &Name, GD3DDevice *Device);
  ~GUI_ScrollBar(void);
private:
  g_int32 _min;
  g_int32 _max;
  g_int32 _position;
  g_float _step;

  g_rect _inc_rect;
  g_rect _dec_rect;
  g_rect _thumb_rect;

  g_int8 _inc_stage;
  g_int8 _dec_stage;
  g_int8 _thumb_stage;

  g_bool  _inc_downed;
  g_bool  _dec_downed;
  g_bool  _thumb_downed;

  g_uint32 _lastscrolltime;

  void _init_scrbar(g_int32 Width, g_int32 Height);
};

#endif //GUISCRBAR_H
