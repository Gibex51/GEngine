#pragma once

#ifndef GBASESOUND_H
#define GBASESOUND_H

#include "GXAudio.h"

class GBaseSound
{
public:
  virtual g_bool LoadFromFile(g_char *FileName) {return false;};
  virtual void Release() {};
  virtual void GetBuffer(void **buffer, g_uint32 *length) {};
  virtual void GetDesc(void **desc) {};

  //Конструктор и деструктор
  GBaseSound(void);
  virtual ~GBaseSound(void);
protected:
  g_uint8 _type;
};

#endif //GBASESOUND_H