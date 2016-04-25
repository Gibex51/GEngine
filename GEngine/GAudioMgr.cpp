#include "GAudioMgr.h"

//-----------------------------------------------

GAudioMgr::GAudioMgr(void)
{
  XAudio = new GXAudio();
  XAudio->Init();
}

//-----------------------------------------------

GAudioMgr::~GAudioMgr(void)
{
  Release();
}

//-----------------------------------------------

void GAudioMgr::Play(g_uint32 index) 
{
  if (index < List_SrcVoices.size()) 
    List_SrcVoices[index]->Play();
}

//-----------------------------------------------

void GAudioMgr::Release()
{
  for (g_uint32 i = 0; i<List_SrcVoices.size(); i++)
    delete List_SrcVoices[i];
  List_SrcVoices.clear();

  for (g_uint32 i = 0; i<List_Sounds.size(); i++)
    delete List_Sounds[i];
  List_Sounds.clear();

  delete XAudio; XAudio = NULL;
}

//-----------------------------------------------

GSourceVoice* GAudioMgr::AddVoice(GBaseSound *Sound)
{
  GSourceVoice* NewVoice = new GSourceVoice(XAudio,Sound);
  List_SrcVoices.push_back(NewVoice);
  return NewVoice;
}

//-----------------------------------------------

GWAVSound* GAudioMgr::AddWAVSound(g_char *WavFileName)
{
  GWAVSound *NewSound = new GWAVSound();
  NewSound->LoadFromFile(WavFileName);
  List_Sounds.push_back(NewSound);
  return NewSound;
}

//-----------------------------------------------
