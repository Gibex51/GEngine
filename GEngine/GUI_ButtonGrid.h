#pragma once

#include "GUI_BaseObject.h"

#ifndef GUI_BUTTONGRID_H
#define GUI_BUTTONGRID_H

enum g_btng_mode {g_btgm_radiobtn, g_btgm_togglebtn, g_btgm_simplebtn};

const g_vec2 GUI_BTNGRID_TEXOFFSET[4] = {g_vec2(0.0f, 0.0f),
                                         g_vec2(0.0f, 0.5f),
										 g_vec2(0.5f, 0.0f),
                                         g_vec2(0.5f, 0.5f)};

const g_vec2 GUI_BTNGRID_TEXSCALE = g_vec2(0.5f, 0.5f);

/*struct G_UIBUTTONGRID_SAVE_DATA
{
  G_UIOBJECT_SAVE_DATA CommonData;
  //Personal Data
  g_bg_mode Mode;
  g_ivec2   ButtonSize;
  g_ivec2   ImageSize;
  g_ivec2   ImageCount;
  g_ivec2   GridSize;
  g_int32   SeparatorSize;
};*/

struct GUI_BUTTONINFO
{
  g_uint8 _state;
  g_bool  _pressed;
  //image
};


class GUI_ButtonGrid :
  public GUI_BaseObject
{
public:
  void SetGridSize(g_ivec2 grid_size) {_grid_size = grid_size;_changed = true;};
  void SetMode(g_btng_mode Mode);

  g_int32 GetPressedButton() {return _pressed;};
  void SetPressedButton(g_int32 index);
  void AddButtons(g_uint32 nButtons);

  void SetImagesTexture(GTexture *ImgTex, g_ivec2 ImgSize, g_ivec2 ImgCount);

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
  GUI_ButtonGrid(const g_string &Name, GD3DDevice *Device);
  GUI_ButtonGrid(GModel *Model, GUI_BaseObject *Parent, const g_string &Name, GD3DDevice *Device);
  ~GUI_ButtonGrid(void);
private:
  g_btng_mode _mode;

  g_ivec2  _button_size;
  g_ivec2  _image_size;
  g_ivec2  _image_count;
  g_ivec2  _grid_size;
  g_int32  _separator_size;
  g_int32  _pressed;

  GTexture *_img_tex;

  vector <GUI_BUTTONINFO> _buttons;

  void _init_btngrid();
  g_int32 GetButtonIndex(g_int32 X, g_int32 Y);

  g_bool CheckImgSize(g_ivec2 ImgSize);
  g_bool CheckImgCount(g_ivec2 ImgCount);
};

#endif //GUI_BUTTONGRID_H
