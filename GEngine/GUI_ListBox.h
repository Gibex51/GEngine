#pragma once

#ifndef GUI_LISTBOX_H
#define GUI_LISTBOX_H

#include "GUI_BaseObject.h"
#include "GUI_ScrollBar.h"

const g_vec2 GUI_LSTBOX_TEXOFFSET[3] = {g_vec2(0.0f, 0.0f),
                                        g_vec2(0.0f, 0.5f)};

const g_vec2 GUI_LSTBOX_TEXSCALE = g_vec2(1.0f, 0.5f);
const g_vec2 GUI_LSTBOX_ITEMSCALE = g_vec2(0.0f, 0.0f);

/*struct G_UILISTBOX_SAVE_DATA
{
  G_UIOBJECT_SAVE_DATA CommonData;
  //Personal Data
  g_int32 SelectedIndex;
  g_int32 ItemsCount;
};*/

#define G_LB_SCROLL_BEG  -1
#define G_LB_SCROLL_END  -2
#define G_LB_SCROLL_CURR -3

class GUI_ListBox :
  public GUI_BaseObject
{
public:
  vector<string> Items;
  g_int32 SelectedItem;

  void ClearItems();

  virtual void OnFocusSet() {};
  virtual void OnFocusLost() {};
  virtual void OnShow() {};
  virtual void OnHide() {};
  virtual void OnResize();
  virtual void OnSetPosition();

  virtual void OnKeyDown(g_int32 Key);
  virtual void OnKeyUp(g_int32 Key) {};
  virtual void OnKeyPress(g_int32 Key);
  virtual void OnChar(g_int32 Key) {};

  virtual void OnMouseLeave(g_int32 X, g_int32 Y);
  virtual void OnMouseEnter(g_int32 X, g_int32 Y);
  virtual void OnMouseMove(g_int32 X, g_int32 Y) {};
  virtual void OnClick(g_int32 Button, g_int32 X, g_int32 Y);
  virtual void OnMouseDown(g_int32 Button, g_int32 X, g_int32 Y);
  virtual void OnMouseUp(g_int32 Button, g_int32 X, g_int32 Y);
  
  virtual void OnDraw();

  void OnSelectItem(g_int32 ItemIndex);
  void SetScrollPos(g_int32 Pos);

  void FillFromDirectory(g_char *FindMask, g_bool Numerate, 
	                     g_bool OnlyDirs, g_uint8 RemoveExtSize);

  virtual void UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite);
  
  //Импорт и экспорт
  //virtual void GetRawSettings(void **pBuffer, g_uint32 &BufSize);
  //virtual void SetRawSettings(void *Buffer, g_uint32 BufSize);

  //Конструктор и деструктор
  GUI_ListBox(const g_string &Name, GD3DDevice *Device);
  GUI_ListBox(GModel *Model, GUI_BaseObject *Parent, const g_string &Name, GD3DDevice *Device);
  ~GUI_ListBox(void);
private:
  GUI_ScrollBar  *_scrollbar;

  g_uint32       _stage;
  g_int32        _n_visible_items;
  g_int32        _prev_pos;
  g_rect         _item_bord;

  void _init_listbox(g_int32 width, g_int32 height);
};

#endif //GUILISTBOX_H