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
    // ������������� COM
    CoInitializeEx(0, COINIT_MULTITHREADED);

    // ������ �������� ���������
    if(XAudio2Create(&_audio, 0, XAUDIO2_DEFAULT_PROCESSOR) != S_OK)
    {
      CoUninitialize();
      return false;
    }

    // ������ ������� �����
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
    // ����������� ������
    if (_mastering_voice != NULL) _mastering_voice->DestroyVoice();

    // ����������� �������� ���������
    if (_audio != NULL) _audio->Release();

    // ���������������� COM
    CoUninitialize();

    _iscreated = false;
  }
}

//-----------------------------------------------
