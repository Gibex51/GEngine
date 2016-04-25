#pragma once

#ifndef GAUDIOMGR_H
#define GAUDIOMGR_H

#include "GSourceVoice.h"
#include "GWAVSound.h"

class GAudioMgr
{
public:
  void Play(g_uint32 index);
  //void Play(g_uint32 id);
  void Release();

  std::vector <GBaseSound*>   List_Sounds;
  std::vector <GSourceVoice*> List_SrcVoices;

  GXAudio *XAudio;

  GSourceVoice* AddVoice(GBaseSound *Sound);
  GWAVSound* AddWAVSound(g_char *WavFileName);

  //Конструктор и деструктор
  GAudioMgr(void);
  ~GAudioMgr(void);
private:

};

#endif //GAUDIOMGR_H