#include "GXAudio.h"

//-----------------------------------------------

GXAudio::GXAudio(void)
{
  _iscreated = false;

  _audio = NULL;
  _mastering_voice = NULL;
}

//-----------------------------------------------

GXAudio::~GXAudio(void)
{
  Release();
}

//-----------------------------------------------

g_bool GXAudio::Init()
{
  if (!_iscreated)
  {
    // Иницилизируем COM
    CoInitializeEx(0, COINIT_MULTITHREADED);

    // Создаём основной интерфейс
    if(XAudio2Create(&_audio, 0, XAUDIO2_DEFAULT_PROCESSOR) != S_OK)
    {
      CoUninitialize();
      return false;
    }

    // Создаём главный голос
    if(_audio->CreateMasteringVoice(&_mastering_voice,XAUDIO2_DEFAULT_CHANNELS,
                                    XAUDIO2_DEFAULT_SAMPLERATE,0,0,0) != S_OK)
    {
      _audio->Release();
      CoUninitialize();
      return false;
    }

    _iscreated = true;

    return true;
  }
  return false;
}

//-----------------------------------------------

void GXAudio::Release()
{
  if (_iscreated)
  {
    // Освобождаем голоса
    if (_mastering_voice != NULL) _mastering_voice->DestroyVoice();

    // Освобождаем основной интерфейс
    if (_audio != NULL) _audio->Release();

    // Деинициализируем COM
    CoUninitialize();

    _iscreated = false;
  }
}

//-----------------------------------------------
