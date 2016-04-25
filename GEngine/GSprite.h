#pragma once

#ifndef GSPRITE_H
#define GSPRITE_H

#include "GBaseObject.h"

class GSprite :
  public GBaseObject
{
public:
  ID3DX10Sprite *_sprite;

  g_bool Begin(g_uint32 Flags);
  g_bool End();

  g_bool DrawBuffered(D3DX10_SPRITE *Sprites, g_uint32 nSprites);

  //Конструктор и деструктор
  GSprite(const g_string &Name, GD3DDevice *Device);
  ~GSprite(void);
private:

};

#endif //GSPRITE_H