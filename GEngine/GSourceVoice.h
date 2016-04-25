#pragma once

#ifndef GXSRCVOICE_H
#define GXSRCVOICE_H

#include "GBaseSound.h"

class GSourceVoice
{
public:
  g_bool Play();
  g_bool PlayBuffer();
  g_bool PlayLoop(g_uint32 count, g_bool inf = false);
  g_bool Pause();
  void   Stop();

  void   SubmitBuffer(void *buffer, g_int32 buf_sz);
  void   UpdateBuffersQueue();
  g_uint64 GetPlayedBytes();
  g_uint32 GetQueueSize();

  g_bool SetVolume(const g_float _volume);
  g_bool GetVolume(g_float *_volume);

  inline g_bool IsCreated() {return _iscreated;};
  //Конструктор и деструктор
  GSourceVoice(GXAudio *_xaudio, GBaseSound* _sound);
  GSourceVoice(GXAudio *_xaudio, WAVEFORMATEX *_wave_desc);
  ~GSourceVoice(void);
private:
  g_bool _iscreated;
  g_bool _pause;
  g_bool _looped;

  g_uint32 _released_bytes;
  WAVEFORMATEX *_curr_wave_desc;

  XAUDIO2_BUFFER _buffer;
  std::deque <XAUDIO2_BUFFER*> _buffer_queue; 
  std::deque <XAUDIO2_BUFFER*> _release_buffer_queue;

  IXAudio2SourceVoice* _source_voice;

  g_bool Init(GXAudio *_xaudio, GBaseSound* _sound);
  g_bool Init(GXAudio *_xaudio, WAVEFORMATEX *_wave_desc);

  void ClearQueue();
};

#endif //GXSRCVOICE_H
