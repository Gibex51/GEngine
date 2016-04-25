#pragma once

#ifndef GWAVSOUND_H
#define GWAVSOUND_H

#include "GBaseSound.h"

class GWAVSound :
  public GBaseSound
{
public:
  g_bool LoadFromFile(g_char *FileName);
  void Release();
  void GetBuffer(void **buffer, g_uint32 *length);

  void GetDesc(void **desc);

  //Конструктор и деструктор
  GWAVSound(void);
  ~GWAVSound(void);
private:
  g_uint8   *_buffer;
  g_uint32  _length;

  WAVEFORMATEX  _data_desc;
};

#endif //GWAVSOUND_H