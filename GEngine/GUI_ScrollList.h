#pragma once

#ifndef GUI_SCRLIST_H
#define GUI_SCRLIST_H

#include "GUI_BaseObject.h"

const g_vec2 GUI_SCRLIST_TEXOFFSET[4] = {g_vec2(0.0f, 0.0f),
                                        g_vec2(0.0f, 0.5f),
										g_vec2(0.5f, 0.0f),
                                        g_vec2(0.5f, 0.5f)};

const g_vec2 GUI_SCRLIST_TEXSCALE = g_vec2(0.5f, 0.5f);

/*struct G_UISCROLLLIST_SAVE_DATA
{
  G_UIOBJECT_SAVE_DATA CommonData;
  //Personal Data
  g_bool  Loop;
  g_int32 SelectedIndex;
  g_int32 ItemsCount;
};*/

class GUI_ScrollList :
  public GUI_BaseObject
{
public:
  vector<string> Items;

  g_bool Loop;

  void ClearItems();
  
  void SetSelected(g_uint16 index);
  g_int32 GetSelected();

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
  virtual void OnMouseDown(g_int32 Button, g_int32 X, g_int32 Y) ;
  virtual void OnMouseUp(g_int32 Button, g_int32 X, g_int32 Y) ;
  virtual void OnDraw() {};

  void OnSelectItem(g_int32 ItemIndex); //????

  virtual void UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite);

  //Импорт и экспорт
  //virtual void GetRawSettings(void **pBuffer, g_uint32 &BufSize);
  //virtual void SetRawSettings(void *Buffer, g_uint32 BufSize);

  //Конструктор и деструктор
  GUI_ScrollList(const g_string &Name, GD3DDevice *Device);
  GUI_ScrollList(GModel *Model, GUI_BaseObject *Parent, const g_string &Name, GD3DDevice *Device);
  ~GUI_ScrollList(void);
private:
  g_uint16 _index;

  g_rect _inc_rect;
  g_rect _dec_rect;

  g_int8 _inc_stage;
  g_int8 _dec_stage;

  g_bool  _inc_downed;
  g_bool  _dec_downed;

  void _init_scrlist();
};

#endif //GUISCRLIST_H