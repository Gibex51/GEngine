#include "GMatrixAnimation.h"

//-----------------------------------------------

GMatrixAnimation::GMatrixAnimation(void):GBaseAnimation("")
{
  SetType(G_AT_MATRIX);
}

//-----------------------------------------------

GMatrixAnimation::~GMatrixAnimation(void)
{
  Release();
}

//-----------------------------------------------

g_bool GMatrixAnimation::GetMatrixFrame(g_uint32 _time, G_MATRIX_FRAME &out_frame)
{
  G_MATRIX_FRAME *frame_st = NULL, *frame_end = NULL;
  g_uint32 nFrames = KeyFrames_List.size();

  for (g_uint32 i = 0; i<nFrames; i++)
  {
    if ((_time < KeyFrames_List[i]->_start_time) && (i > 0))
    {
      frame_st = KeyFrames_List[i-1];
      frame_end = KeyFrames_List[i];
      break;
    }
  }

  if (nFrames > 1)
    if (frame_st == NULL)
    {
      frame_st = KeyFrames_List[nFrames-2];
      frame_end = KeyFrames_List[nFrames-1];
      _time = KeyFrames_List[nFrames-1]->_start_time;
    }

  if ((frame_st == NULL) || (frame_end == NULL))
    return false;

  g_uint32 ftime = frame_end->_start_time - frame_st->_start_time;
  g_uint32 ctime = _time - frame_st->_start_time;

  g_float frame_pos = g_float(ctime)/g_float(ftime);

  out_frame._start_time = _time;
  if (frame_end->_use_pos)
    out_frame._position = frame_st->_position + (frame_end->_position - frame_st->_position)*frame_pos;
  if (frame_end->_use_rot)
    out_frame._rotation = frame_st->_rotation + (frame_end->_rotation - frame_st->_rotation)*frame_pos;
  if (frame_end->_use_scale)
    out_frame._scale = frame_st->_scale + (frame_end->_scale - frame_st->_scale)*frame_pos;
  
  out_frame._use_pos = frame_end->_use_pos;
  out_frame._use_scale = frame_end->_use_scale;
  out_frame._use_rot = frame_end->_use_rot;

  return true;
}

//-----------------------------------------------

G_MATRIX_FRAME* GMatrixAnimation::AddKeyFrame(g_uint32 FrameTime)
{
  G_MATRIX_FRAME *NewFrame = new G_MATRIX_FRAME();
  NewFrame->_start_time = FrameTime;
  KeyFrames_List.push_back(NewFrame);
  return NewFrame;
}

//-----------------------------------------------

void GMatrixAnimation::Release()
{
  for (g_uint32 i = 0; i< KeyFrames_List.size(); i++)
    delete KeyFrames_List[i];
  KeyFrames_List.clear();
}

//-----------------------------------------------

void GMatrixAnimation::AddKeyFrameArray(G_MATRIX_FRAME *FrameArray, g_int32 nFrames)
{
  for (g_int32 i = 0; i<nFrames; i++)
  {
    G_MATRIX_FRAME *NewFrame = new G_MATRIX_FRAME();
    *NewFrame = FrameArray[i];
    KeyFrames_List.push_back(NewFrame);
  }
}

//-----------------------------------------------
