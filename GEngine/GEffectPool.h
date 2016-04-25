#pragma once

#include "GBaseObject.h"

#ifndef GEFFECTPOOL_H
#define GEFFECTPOOL_H

class GEffectPool :
  public GBaseObject
{
public:
  g_bool LoadFromFile(const g_string &FileName);

  ID3D10EffectPool *GetDXEffectPool() {return _dx_effect_pool;};

  void Release();

  //Конструктор и деструктор
  GEffectPool(const g_string &Name, GD3DDevice *Device);
  GEffectPool(const g_string &FileName, const g_string &Name, GD3DDevice *Device);
  ~GEffectPool(void);
private:
  ID3D10EffectPool *_dx_effect_pool;

  void _init();
};

#endif //GEFFECTPOOL_H