#pragma once

#include "GSprite.h"

using namespace std;

#ifndef GFONT_H
#define GFONT_H

class GFont :
  public GBaseObject
{
public:
  g_color FontColor;

  g_bool GDrawText(const g_string &Text, g_rect *TextRect);
  g_bool GDrawText(const g_string &Text, g_rect *TextRect, g_uint32 TextFormat);
  g_bool GDrawText(GSprite *Sprite, const g_string &Text, g_rect *TextRect, g_uint32 TextFormat);

  g_int32 GetFontHeight();

  void SetWeight(g_uint32 Weight);
  void SetItalic(g_bool IsItalic);

  HDC GetFontDC();

  g_bool IsCreated() {return _created;};

  //Конструктор и деструктор
  GFont(const g_string &Name, GD3DDevice *Device);
  GFont(const g_string &FontName, g_int32 FontSize, g_color FontColor, g_uint32 FontWeight, g_bool Italic, const g_string &Name, GD3DDevice *Device);
  ~GFont(void);
private:
  ID3DX10Font *_d3dxfont;

  g_int32  _f_size;
  g_uint32 _f_weight;
  g_bool   _f_italic;
  g_string _f_name;

  g_bool   _created;

  g_bool CreateFont(const g_string &FontName, g_int32 FontSize, g_color _FontColor, g_uint32 FontWeight, g_bool Italic);
  void   ReleaseFont();
};

#endif //GFONT_H