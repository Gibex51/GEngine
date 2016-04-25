#include "GSourceVoice.h"

//-----------------------------------------------

GSourceVoice::GSourceVoice(GXAudio *_xaudio, GBaseSound* _sound)
{
  _iscreated = Init(_xaudio, _sound);
}

//-----------------------------------------------

GSourceVoice::GSourceVoice(GXAudio *_xaudio, WAVEFORMATEX *_wave_desc)
{
  _iscreated = Init(_xaudio, _wave_desc);
}

//-----------------------------------------------

GSourceVoice::~GSourceVoice(void)
{
  if (_iscreated)
  {
	Stop();
	_iscreated = false;	
    _source_voice->DestroyVoice();
  }
}

//-----------------------------------------------

void GSourceVoice::ClearQueue()
{
  while (_buffer_queue.size() > 0)
  {
	XAUDIO2_BUFFER *curr_buf = _buffer_queue.front();
	_buffer_queue.pop_front();
	delete[]curr_buf->pAudioData;
	delete curr_buf;
  }

  while (_release_buffer_queue.size() > 0)
  {
    XAUDIO2_BUFFER *curr_buf = _release_buffer_queue.front();
	_release_buffer_queue.pop_front();
	delete[]curr_buf->pAudioData;
	delete curr_buf;
  }
}

//-----------------------------------------------

g_bool GSourceVoice::Init(GXAudio *_xaudio, GBaseSound* _sound)
{
  if ((_xaudio == NULL) || (_sound == NULL))
    return false;

  if (!_xaudio->IsCreated())
    return false;

  WAVEFORMATEX *wave_desc;
  _sound->GetDesc((void**)&wave_desc);

  if(_xaudio->GetAudio()->CreateSourceVoice(&_source_voice,wave_desc, 0,
                                            XAUDIO2_DEFAULT_FREQ_RATIO/*XAUDIO2_MAX_FREQ_RATIO*/, 0, 0, 0) != S_OK)
    return false;

  memset(&_buffer,0,sizeof(XAUDIO2_BUFFER));
  _buffer.Flags = XAUDIO2_END_OF_STREAM;

  _sound->GetBuffer((void**)&_buffer.pAudioData, &_buffer.AudioBytes); 

  /*// Проигрываем все аудио-данные
  _buffer.PlayBegin  = 0;
  _buffer.PlayLength  = 0;

  // Цикл не нужен
  _buffer.LoopBegin  = 0;
  _buffer.LoopLength  = 0;
  _buffer.LoopCount  = 0;*/

   // Добавляем аудио-данные для проигрывания
  if(_source_voice->SubmitSourceBuffer(&_buffer) != S_OK)
  {
    _source_voice->DestroyVoice();
    return false;
  }

  return true;
}

//-----------------------------------------------


g_bool GSourceVoice::Init(GXAudio *_xaudio, WAVEFORMATEX *_wave_desc)
{
  if ((_xaudio == NULL) || (_wave_desc == NULL))
    return false;

  if (!_xaudio->IsCreated())
    return false;

  if(_xaudio->GetAudio()->CreateSourceVoice(&_source_voice, _wave_desc) != S_OK)
    return false;

  _curr_wave_desc = _wave_desc;

  return true;
}

//-----------------------------------------------

g_bool GSourceVoice::Play()
{
  if (_iscreated)
  {
    _source_voice->Stop();
    _source_voice->FlushSourceBuffers();
    _source_voice->SubmitSourceBuffer(&_buffer);
    _source_voice->SetVolume(0.5f); //<--DEL
    HRESULT res = _source_voice->Start();
    return true;
  }
  return false;
}

//-----------------------------------------------

void GSourceVoice::Stop()
{
  if (_iscreated)
  {
	_source_voice->Stop();
    _source_voice->FlushSourceBuffers();
	ClearQueue();
  }
}

//-----------------------------------------------

void GSourceVoice::SubmitBuffer(void *buffer, g_int32 buf_sz)
{
  if (_iscreated)
  {
	if ((buffer == NULL) || (buf_sz <= 0))
	  return;

	XAUDIO2_BUFFER *new_buffer = new XAUDIO2_BUFFER;
	memset(new_buffer, 0, sizeof(XAUDIO2_BUFFER));
	new_buffer->AudioBytes = buf_sz;
	new_buffer->pAudioData = new BYTE[buf_sz];
	memcpy((void*)new_buffer->pAudioData, buffer, buf_sz);
	
	_buffer_queue.push_back(new_buffer);
  }
}

//-----------------------------------------------

void GSourceVoice::UpdateBuffersQueue()
{
  if (_iscreated)
  {
	XAUDIO2_VOICE_STATE state;
    _source_voice->GetState(&state);

	while (_release_buffer_queue.size() > state.BuffersQueued)
	{
	  XAUDIO2_BUFFER *curr_buf = _release_buffer_queue.front();
	  _release_buffer_queue.pop_front();
	  _released_bytes += curr_buf->AudioBytes;
	  delete[]curr_buf->pAudioData;
	  delete curr_buf;
	}

	if (_buffer_queue.size() == 0)
	  return;

	g_int32 nSlots = XAUDIO2_MAX_QUEUED_BUFFERS - state.BuffersQueued;

    if (nSlots <= 0)
	  return;
	
	for (g_int32 i = 0; i < nSlots; i++)
	{
	  if (_buffer_queue.size() == 0) break;
      XAUDIO2_BUFFER *curr_buf = _buffer_queue.front();
	  _buffer_queue.pop_front();
	  _release_buffer_queue.push_back(curr_buf);
	  _source_voice->SubmitSourceBuffer(curr_buf);
	}

    //_source_voice->SubmitSourceBuffer(Buffer);
  }
}

//-----------------------------------------------

g_bool GSourceVoice::PlayBuffer()
{
  if (_iscreated)
  {
    _source_voice->Stop();
    _source_voice->FlushSourceBuffers();
    _source_voice->SetVolume(0.5f); //<--DEL
	_released_bytes = 0;
    HRESULT res = _source_voice->Start();
    return true;
  }

  return false;
}

//-----------------------------------------------

g_uint64 GSourceVoice::GetPlayedBytes()
{
  XAUDIO2_VOICE_STATE state;
  _source_voice->GetState(&state);
  return state.SamplesPlayed * g_uint64(_curr_wave_desc->wBitsPerSample * _curr_wave_desc->nChannels / 8);
}

//-----------------------------------------------


g_uint32 GSourceVoice::GetQueueSize()
{
  XAUDIO2_VOICE_STATE state;
  _source_voice->GetState(&state);
  return _buffer_queue.size() + state.BuffersQueued;
}

//-----------------------------------------------