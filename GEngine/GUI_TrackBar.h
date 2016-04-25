#pragma once

#ifndef GUI_TRACKBAR_H
#define GUI_TRACKBAR_H

#include "GUI_BaseObject.h"

const g_vec2 GUI_TRKBAR_TEXOFFSET[3] = {g_vec2(0, 0),
	                                    g_vec2(0, 0.5f),
                                        g_vec2(0.5f, 0)};

const g_vec2 GUI_TRKBAR_TEXSCALE[2] = {g_vec2(0.5f, 0.5f),
	                                   g_vec2(1.0f, 0.5f)};

/*struct G_UITRACKBAR_SAVE_DATA
{
  G_UIOBJECT_SAVE_DATA CommonData;
  //Personal Data
  g_int32 Min; 
  g_int32 Max;
  g_int32 Position;
};*/

class GUI_TrackBar :
  public GUI_BaseObject
{
public:
  void SetBarPosition(g_int32 Pos);
  g_int32 GetBarPosition() {return _position;};
  void SetRange(g_int32 Min, g_int32 Max);

  void OnSetValue();

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
  virtual void OnClick(g_int32 Button, g_int32 X, g_int32 Y) {};
  virtual void OnMouseDown(g_int32 Button, g_int32 X, g_int32 Y);
  virtual void OnMouseUp(g_int32 Button, g_int32 X, g_int32 Y);
  virtual void OnDraw() {};

  virtual void UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite);

  //Импорт и экспорт
  //virtual void GetRawSettings(void **pBuffer, g_uint32 &BufSize);
  //virtual void SetRawSettings(void *Buffer, g_uint32 BufSize);

  //Конструктор и деструктор
  GUI_TrackBar(const g_string &Name, GD3DDevice *Device);
  GUI_TrackBar(GModel *Model, GUI_BaseObject *Parent, const g_string &Name, GD3DDevice *Device);
  ~GUI_TrackBar(void);
private:
  g_int32 _min;
  g_int32 _max;
  g_int32 _position;
  g_float _step;

  g_rect _thumb_rect;
  g_int8 _thumb_stage;
  g_bool _thumb_downed;
  g_rect _thumb_bord;

  void _init_trkbar();
  g_float _get_step(g_int32 width, g_int32 height) const;
};

#endif //GUI_TRACKBAR_H