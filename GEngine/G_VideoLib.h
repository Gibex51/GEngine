#pragma once

#ifndef GVIDEOLIB_H
#define GVIDEOLIB_H

#include "GStructs.h"
#include "GTexture.h"
#include "GModel.h"
#include "GSourceVoice.h"

extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #include <libswscale/swscale.h>
  #include <libswresample/swresample.h>
  #include <libavutil/avutil.h>
  #include <libavdevice/avdevice.h>
  #include <libavfilter/avfilter.h>
}

#pragma comment (lib, "avcodec.lib")
#pragma comment (lib, "avformat.lib")
#pragma comment (lib, "swscale.lib")
#pragma comment (lib, "avutil.lib")
#pragma comment (lib, "avdevice.lib")
#pragma comment (lib, "avfilter.lib")
#pragma comment (lib, "postproc.lib")
#pragma comment (lib, "swresample.lib")

//---------------------------------------------

#define AV_SYNC_THRESHOLD        0.01f
#define AV_NOSYNC_THRESHOLD      10.0f

#define AV_MAX_VIDEO_QUEUE_SIZE  300

//---------------------------------------------

struct G_PACKET_QUEUE 
{
  std::queue <AVPacket*> PacketQueue;
  
  void Clear();
  g_bool Put(AVPacket* pkt);
  g_bool Get(AVPacket* pkt);

  ~G_PACKET_QUEUE() {Clear();};
};

//---------------------------------------------

struct G_VIDEO_CONTEXT
{
  g_bool             _initialized;

  g_double           _video_clock;
  g_double           _video_pts;

  g_int32            _vstream_ind;

  AVCodecContext*    _codec_context;
  struct SwsContext* _conv_context;
  AVPicture          _out_pict;

  G_PACKET_QUEUE     _video_queue;

  void Release();

  G_VIDEO_CONTEXT();
};

struct G_AUDIO_CONTEXT
{
  g_bool             _initialized;

  g_uint32           _n_astreams;
  g_uint32           _curr_astream;
  g_int32            _astream_ind[10];

  AVCodecContext*    _codec_context;
  struct SwrContext* _conv_context;

  WAVEFORMATEX       *_wave_desc;

  void Release();

  G_AUDIO_CONTEXT();
  ~G_AUDIO_CONTEXT();
};

//---------------------------------------------

#define G_CC_OK               0
#define G_CC_NO_VID_CODEC     1
#define G_CC_NO_AUD_CODEC     2
#define G_CC_NO_CONTENT       4
#define G_CC_VID_CONV_ERR     8
#define G_CC_AUD_CONV_ERR     16
#define G_CC_VID_BUFF_ERR     32
#define G_CC_VID_TEXBUFF_ERR  64

struct G_CLIP_CONTEXT
{
  g_double           _frame_timer;     
  g_double           _frame_last_pts;
  g_double           _frame_last_delay;
  g_uint32           _curr_delay_start;
  g_uint32           _curr_delay;

  AVFormatContext*   _format_context;
	
  AVFrame*           _frame;

  G_AUDIO_CONTEXT    _aud_context;
  G_VIDEO_CONTEXT    _vid_context;

  GD3DDevice         *_device;
  GTexture           *_tex_buf;
  G_BUFFER           *_buf;
  
  GXAudio            *_xaudio;
  GSourceVoice       *_voice_out;

  g_bool    IsInit() {return _initialized;};

  void      Start();
  void      Stop();

  void      Decode();

  g_int32   Init(AVFormatContext* FormatContext, GXAudio *XAudio, GD3DDevice *D3DDevice);
  void      Release();

  G_CLIP_CONTEXT();
private:
  g_bool             _initialized;
  g_bool             _playing;

  void      SyncVideo(g_double &pts);
  g_double  ComputeDelay();
  g_double  GetAudioClock();
};

//---------------------------------------------

class G_VideoLib
{
public:
	g_bool OpenClip(const string FileName);
	void   CloseClip();

	void Play();
	void Stop();

	void Draw();

	g_int32 GetWidth();
	g_int32 GetHeigth();

	GTexture* GetTexBuffer() {return _clip_context._tex_buf;};

	g_int32 GetVQSize() {if (_clip_context.IsInit()) return _clip_context._vid_context._video_queue.PacketQueue.size();return 0;};
	g_int32 GetAQSize() {if (_clip_context.IsInit()) return _clip_context._voice_out->GetQueueSize();return 0;};

	G_VideoLib(GD3DDevice *Device, GXAudio *XAudio);
	~G_VideoLib(void);
private:
	GD3DDevice    *_device;
	GXAudio       *_xaudio;

	G_CLIP_CONTEXT _clip_context; 
};

#endif //GVIDEOLIB_H