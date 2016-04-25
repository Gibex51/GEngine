#pragma once

#ifndef GXAUDIO_H
#define GXAUDIO_H

#include "GStructs.h"

class GXAudio
{
public:
  g_bool Init();
  void Release();
  
  inline g_bool IsCreated() {return _iscreated;};
  
  inline IXAudio2* GetAudio() {return _audio;};

  //Конструктор и деструктор
  GXAudio(void);
  ~GXAudio(void);
private:
  g_bool _iscreated;

  IXAudio2*               _audio;
  IXAudio2MasteringVoice* _mastering_voice;
};

#endif //GXAUDIO_H