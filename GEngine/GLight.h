#pragma once

#include "GBaseObject.h"

#ifndef GLIGHT_H
#define GLIGHT_H

class GLight :
  public GBaseObject
{
public:
  G_LIGHT_SET LightSet;

  //Конструктор и деструктор
  GLight(g_char *Name, GD3DDevice *Device);
  ~GLight(void);
private:

};

#endif //GLIGHT_H