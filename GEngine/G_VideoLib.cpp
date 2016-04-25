#include "G_VideoLib.h"

//-----------------------------------------------

G_VideoLib::G_VideoLib(GD3DDevice *Device, GXAudio *XAudio)
{
  _device = Device;
  _xaudio = XAudio;

  av_register_all();
}

//-----------------------------------------------

G_VideoLib::~G_VideoLib(void)
{
  CloseClip();
}

//-----------------------------------------------

g_bool G_VideoLib::OpenClip(const string FileName)
{
  if (_clip_context.IsInit())
    CloseClip();

  AVFormatContext* _format_context = NULL;

  if (avformat_open_input(&_format_context, FileName.c_str(), NULL, NULL) < 0)
	return false;

  if (avformat_find_stream_info(_format_context, NULL) < 0)
	return false;

  if (_clip_context.Init(_format_context, _xaudio, _device) != 0)
	return false;

  return true;
}

//-----------------------------------------------

void G_VideoLib::CloseClip()
{
  _clip_context.Release();
}

//----------------------------------------------

void G_VideoLib::Play()
{

  _clip_context.Start();
}

//----------------------------------------------

void G_VideoLib::Stop()
{
  _clip_context.Stop();
}

//----------------------------------------------

void G_VideoLib::Draw()
{
  _clip_context.Decode();
}

//----------------------------------------------

g_int32 G_VideoLib::GetWidth()
{
  if (_clip_context._vid_context._codec_context != NULL)
	return _clip_context._vid_context._codec_context->width;
  return -1;
}

//----------------------------------------------

g_int32 G_VideoLib::GetHeigth()
{
  if (_clip_context._vid_context._codec_context != NULL)
	return _clip_context._vid_context._codec_context->height;
  return -1;
}

//----------------------------------------------
// Packet Queue
//----------------------------------------------

void G_PACKET_QUEUE::Clear()
{
  while (PacketQueue.size() > 0)
  {
	AVPacket* packet = PacketQueue.front();
	PacketQueue.pop();
	av_free_packet(packet);
	delete packet;	
  }
}

//----------------------------------------------

g_bool G_PACKET_QUEUE::Put(AVPacket* pkt)
{
  if (pkt == NULL)
	return false;

  AVPacket *npkt = new AVPacket();
  av_copy_packet(npkt, pkt);

  PacketQueue.push(npkt);
  return true;
}

//----------------------------------------------

g_bool G_PACKET_QUEUE::Get(AVPacket* pkt)
{
  if (PacketQueue.size() == 0)
	return false;
  AVPacket* packet = PacketQueue.front();
  *pkt = *packet;
  PacketQueue.pop();
  delete packet;
  return true;
}

//----------------------------------------------
// Video Context
//----------------------------------------------

G_VIDEO_CONTEXT::G_VIDEO_CONTEXT()
{
  _video_clock = 0;
  _video_pts = 0;
  _vstream_ind = -1;

  _codec_context = NULL;
  _conv_context = NULL;

  memset(&_out_pict, 0, sizeof(AVPicture));

  _initialized = false;
}

//----------------------------------------------

void G_VIDEO_CONTEXT::Release()
{
  _video_queue.Clear();

  _video_clock = 0;
  _video_pts = 0;
  _vstream_ind = -1;

  if (_conv_context != NULL) sws_freeContext(_conv_context);
  if (_codec_context != NULL) avcodec_close(_codec_context);
  avpicture_free(&_out_pict);
  memset(&_out_pict, 0, sizeof(AVPicture));

  _conv_context = NULL;
  _codec_context = NULL;

  _initialized = false;
}

//----------------------------------------------
// Audio Context
//----------------------------------------------

G_AUDIO_CONTEXT::G_AUDIO_CONTEXT()
{
  _n_astreams = 0;
  _curr_astream = 0;

  for (g_uint32 i = 0; i < 10; i++)
    _astream_ind[i] = -1;

  _codec_context = NULL;
  _conv_context = NULL;

  _wave_desc = new WAVEFORMATEX();
  memset(_wave_desc, 0, sizeof(WAVEFORMATEX));

  _initialized = false;
}

//----------------------------------------------

G_AUDIO_CONTEXT::~G_AUDIO_CONTEXT()
{
  delete _wave_desc;
}

//----------------------------------------------

void G_AUDIO_CONTEXT::Release()
{
  _n_astreams = 0;
  _curr_astream = 0;

  for (g_uint32 i = 0; i < 10; i++)
    _astream_ind[i] = -1;

  if (_conv_context != NULL) swr_free(&_conv_context);
  if (_codec_context != NULL) avcodec_close(_codec_context);

  _codec_context = NULL;
  _conv_context = NULL;

  memset(_wave_desc, 0, sizeof(WAVEFORMATEX));

  _initialized = false;
}

//----------------------------------------------
// Clip Context
//----------------------------------------------

G_CLIP_CONTEXT::G_CLIP_CONTEXT()
{
  _frame_timer = 0;
  _frame_last_pts = 0;
  _frame_last_delay = 0;
  _curr_delay_start = 0;
  _curr_delay = 0;

  _format_context = NULL;
  _frame = NULL;

  _device = NULL;
  _tex_buf = NULL;
  _buf = NULL;
  _xaudio = NULL;
  _voice_out = NULL;

  _initialized = false;
  _playing = false;
}

//----------------------------------------------

g_int32 G_CLIP_CONTEXT::Init(AVFormatContext* FormatContext, GXAudio *XAudio, GD3DDevice *D3DDevice)
{
  // Clear 
  Release();

  g_int32 res = 0; 
  _format_context = FormatContext;
  _device = D3DDevice;
  _xaudio = XAudio;

  // Find video & audio streams
  for (g_uint32 i = 0; i < _format_context->nb_streams; i++) 
  {
    if (_format_context->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) 
	  _vid_context._vstream_ind = i;
    if (_format_context->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
	  if (_aud_context._n_astreams < 10)
	  {
	    _aud_context._astream_ind[_aud_context._n_astreams] = i;
	    _aud_context._n_astreams++;
	  }
  }

  if ((_vid_context._vstream_ind < 0) && (_aud_context._astream_ind[0] < 0))
	return G_CC_NO_CONTENT;

  // Open Video Codec
  if (_vid_context._vstream_ind >= 0)
  {
    _vid_context._codec_context = _format_context->streams[_vid_context._vstream_ind]->codec;
    AVCodec* codec = avcodec_find_decoder(_vid_context._codec_context->codec_id);
    if (avcodec_open2(_vid_context._codec_context, codec, NULL) >= 0) 
	{
	  // Get Video Convert Context	
      _vid_context._conv_context = sws_getCachedContext(NULL,
                                        _vid_context._codec_context->width, 
										_vid_context._codec_context->height,
                                        _vid_context._codec_context->pix_fmt,
                                        _vid_context._codec_context->width, 
										_vid_context._codec_context->height,
                                        PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

      if (_vid_context._conv_context != NULL) 
	  {
        avpicture_alloc(&_vid_context._out_pict, PIX_FMT_RGB24, 
		                _vid_context._codec_context->width, _vid_context._codec_context->height);

		// Create Texture Buffer
		g_uint32 buf_sz = _vid_context._codec_context->width*_vid_context._codec_context->height*3;
        g_uint8 *empty_buf = new g_uint8[buf_sz];
        memset(empty_buf, 0, buf_sz);

        _buf = _device->BufferList->CreateBuffer(G_BT_SHADERRES, G_BF_DEFAULT, sizeof(g_uint8),
                                                 buf_sz, empty_buf, 0);

        delete empty_buf;

        if (_buf != NULL)
        {
          _tex_buf = new GTexture("video_buff", _device);
          if (_tex_buf->CreateFromBuffer(&_buf->Buffer, buf_sz, DXGI_FORMAT_R8_UINT))
			_vid_context._initialized = true;
		  else
		    res += G_CC_VID_TEXBUFF_ERR;        
        }
		else
		  res += G_CC_VID_BUFF_ERR;
	  }
	  else
        res += G_CC_VID_CONV_ERR;
	}
	else
	  res += G_CC_NO_VID_CODEC;
  }

  // Open Audio Codec
  if (_aud_context._astream_ind[_aud_context._curr_astream] >= 0)
  {
    _aud_context._codec_context = _format_context->streams[_aud_context._astream_ind[_aud_context._curr_astream]]->codec;
    AVCodec* acodec = avcodec_find_decoder(_aud_context._codec_context->codec_id);
    if (avcodec_open2(_aud_context._codec_context, acodec, NULL) >= 0) 
	{
	  // Get Audio Context
      _aud_context._conv_context = swr_alloc_set_opts(NULL, 
	                                        _aud_context._codec_context->channel_layout, 
											AV_SAMPLE_FMT_S16,
	                                        _aud_context._codec_context->sample_rate,
											_aud_context._codec_context->channel_layout,
											_aud_context._codec_context->sample_fmt,
											//AV_SAMPLE_FMT_FLTP,
	                                        _aud_context._codec_context->sample_rate, 0, NULL);
	  if (_aud_context._conv_context != NULL) 
	  {
        swr_init(_aud_context._conv_context);
	    
		// Init Wave Desc 
        _aud_context._wave_desc->cbSize = sizeof(WAVEFORMATEX);
        _aud_context._wave_desc->wFormatTag = WAVE_FORMAT_PCM;
        _aud_context._wave_desc->nChannels = _aud_context._codec_context->channels;
        _aud_context._wave_desc->wBitsPerSample = 16;  
        _aud_context._wave_desc->nBlockAlign = (_aud_context._codec_context->channels*_aud_context._wave_desc->wBitsPerSample)/8;
        _aud_context._wave_desc->nSamplesPerSec = _aud_context._codec_context->sample_rate;
        _aud_context._wave_desc->nAvgBytesPerSec = _aud_context._wave_desc->nSamplesPerSec*_aud_context._wave_desc->nBlockAlign;
	    
		_voice_out = new GSourceVoice(_xaudio, _aud_context._wave_desc);

		_aud_context._initialized = true;
	  }
	  else
        res += G_CC_AUD_CONV_ERR;
	}
	else
      res += G_CC_NO_AUD_CODEC;
  }

  _initialized = ((_vid_context._initialized) || (_aud_context._initialized));

  return res;
}

//----------------------------------------------

void G_CLIP_CONTEXT::Release()
{
  _initialized = false;
  _playing = false;

  _aud_context.Release();
  _vid_context.Release();

  _frame_timer = 0;
  _frame_last_pts = 0;
  _frame_last_delay = 0;
  _curr_delay_start = 0;
  _curr_delay = 0;

  if (_frame != NULL) av_free(_frame);
  if (_format_context != NULL) avformat_close_input(&_format_context);  

  _format_context = NULL;
  _frame = NULL;

  _device = NULL;
  FREENULL_VAR(_tex_buf);
  FREENULL_VAR(_buf);
  _xaudio = NULL;
  FREENULL_VAR(_voice_out);
}

//----------------------------------------------

void G_CLIP_CONTEXT::SyncVideo(g_double &pts)
{
  if(pts != 0)
    _vid_context._video_clock = pts;
  else
    pts = _vid_context._video_clock;

  double frame_delay = av_q2d(_vid_context._codec_context->time_base);

  frame_delay += _frame->repeat_pict * (frame_delay * 0.5f);
  _vid_context._video_clock += frame_delay;
}

//----------------------------------------------

g_double G_CLIP_CONTEXT::GetAudioClock() 
{     
  g_int32 bytes_per_sec = _aud_context._codec_context->sample_rate *
                          _aud_context._codec_context->channels *
                          av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
  
  g_uint32 buffer_size = g_uint32(_voice_out->GetPlayedBytes());

  if (bytes_per_sec == 0)
	return 0;

  return (g_double)buffer_size / (g_double)bytes_per_sec;
}

//----------------------------------------------

g_double G_CLIP_CONTEXT::ComputeDelay()
{
  double delay = _vid_context._video_pts - _frame_last_pts;
  if (delay <= 0.0f || delay >= 1.0f)
    delay = _frame_last_delay;

  _frame_last_pts = _vid_context._video_pts;
  _frame_last_delay = delay;

  double ref_clock = GetAudioClock();
  double diff = _vid_context._video_pts - ref_clock;
  double sync_threshold = FFMAX(AV_SYNC_THRESHOLD, delay);
  if (fabs(diff) < AV_NOSYNC_THRESHOLD) 
  {
    if (diff <= -sync_threshold) {
      delay = 0;
    } else if (diff >= sync_threshold) {
      delay = 2 * delay;
    }
  }

  _frame_timer += delay;
    
  g_double curr_timer = g_double(av_gettime()) / 1000000.0f;
  double actual_delay = _frame_timer - curr_timer;
  if(actual_delay < 0.010f) 
    actual_delay = 0.010f;

  return actual_delay;
}

//----------------------------------------------

void G_CLIP_CONTEXT::Decode()
{
  if ((!_playing) || (!_initialized))
	return;

  _voice_out->UpdateBuffersQueue();

  if (_frame == NULL)
	_frame = avcodec_alloc_frame();

  if (_frame == NULL)
	return;

  if (g_uint32(GetTicks()) - _curr_delay_start >= _curr_delay)
  {
	AVPacket vpacket;

	if (_vid_context._video_queue.Get(&vpacket))
	{
      g_int32 frame_finished;
      avcodec_decode_video2(_vid_context._codec_context, _frame, &frame_finished, &vpacket);

      if (frame_finished) 
	  {                    
        sws_scale(_vid_context._conv_context,
                  _frame->data, _frame->linesize,
                  0, _vid_context._codec_context->height,
                  _vid_context._out_pict.data, _vid_context._out_pict.linesize);

	    g_uint32 buf_size = _vid_context._out_pict.linesize[0]*_vid_context._codec_context->height;

	    if (_tex_buf->TextureRes != NULL)
        {
          D3D10_BOX box = {0, 0, 0, buf_size, 1, 1};
          _device->D3DDevice->UpdateSubresource(_tex_buf->TextureRes, 0, &box, 
	 	                                        _vid_context._out_pict.data[0], 1, 0);
        }

	    g_double pts = (g_double)_frame->pkt_dts;
        if (pts == AV_NOPTS_VALUE)
          pts = (g_double)_frame->pkt_pts;
        if (pts == AV_NOPTS_VALUE)
          pts = 0;
        pts *= av_q2d(_format_context->streams[_vid_context._vstream_ind]->time_base);
        SyncVideo(pts);
	    _vid_context._video_pts = pts;

		_curr_delay = g_uint32(ComputeDelay()*1000.0f + 0.5f);
	    _curr_delay_start = g_uint32(GetTicks());
      }

	  av_free_packet(&vpacket);
	}
  }

  if (_vid_context._video_queue.PacketQueue.size() > AV_MAX_VIDEO_QUEUE_SIZE)
  {
	avcodec_free_frame(&_frame);
	return;
  }

  AVPacket packet;

  if (av_read_frame(_format_context, &packet) < 0)
  {
	av_free_packet(&packet);
	avcodec_free_frame(&_frame);
	return;
  }

  if (packet.stream_index == _vid_context._vstream_ind) 
  {
	_vid_context._video_queue.Put(&packet);
  }

  if (packet.stream_index == _aud_context._astream_ind[_aud_context._curr_astream]) 
  {
    g_int32 got_frame;
    g_int32 len = avcodec_decode_audio4(_aud_context._codec_context, _frame, &got_frame, &packet);

    if ((got_frame) && (len>=0))
	{
	  g_int32 data_size = av_samples_get_buffer_size(NULL, _aud_context._codec_context->channels,
                                                     _frame->nb_samples,
													 AV_SAMPLE_FMT_S16, 1);

	  uint8_t* outputBuffer[SWR_CH_MAX] = {new uint8_t[data_size], NULL};
	  
	  g_int32 res = swr_convert(_aud_context._conv_context, outputBuffer, 
		                        _aud_context._codec_context->sample_rate,
		                        (const uint8_t**)_frame->extended_data, 
		                        _frame->nb_samples);

	  _voice_out->SubmitBuffer(outputBuffer[0], data_size);

	  delete[]outputBuffer[0];
    }
  }

  av_free_packet(&packet);
  avcodec_free_frame(&_frame);
}

//----------------------------------------------

void G_CLIP_CONTEXT::Start()
{
  if (!_initialized)
	return;

  if (_playing)
    Stop();

  if (_vid_context._vstream_ind > 0)
    av_seek_frame(_format_context, _vid_context._vstream_ind, 0, 0);

  for (g_uint32 i = 0; i < _aud_context._n_astreams; i++)
	if (_aud_context._astream_ind[i] > 0)
	  av_seek_frame(_format_context, _aud_context._astream_ind[i], 0, 0);

  _playing = true;
  _voice_out->PlayBuffer(); 

  _frame_timer = double(av_gettime()) / 1000000.0f;
}

//----------------------------------------------

void G_CLIP_CONTEXT::Stop()
{
  if (!_initialized)
	return;

  _playing = false;
  _voice_out->Stop();

  _vid_context._video_pts = 0;
  _vid_context._video_clock = 0;
  _vid_context._video_pts = 0;
  _vid_context._video_queue.Clear();

  _frame_last_pts = 0;
  _frame_last_delay = 40e-3;
  _curr_delay = 0;
  _curr_delay_start = g_uint32(GetTicks());
}

//----------------------------------------------
