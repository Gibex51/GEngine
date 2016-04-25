#pragma once

#include "GUI_BaseObject.h"

#ifndef GUI_IMAGE_H
#define GUI_IMAGE_H

/*struct G_UIIMAGE_SAVE_DATA
{
  G_UIOBJECT_SAVE_DATA CommonData;
  //Personal Data
  g_char Filename[256];
};*/

class GUI_Image :
  public GUI_BaseObject
{
public:
  g_bool LoadFromFile(const g_string &FileName);
  void SetFromTexture(GTexture **Texture);

  void EmptyImage();

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
  GUI_Image(const g_string &Name, GD3DDevice *Device);
  GUI_Image(GModel *Model, GUI_BaseObject *Parent, const g_string &FileName, const g_string &Name, GD3DDevice *Device);
  ~GUI_Image(void);
private:
  g_string _filename;

  GTexture *_image;
  
  g_int32 px;
  g_int32 py;

  void _init_image(const g_string &filename);
};

#endif //GUI_IMAGE_H