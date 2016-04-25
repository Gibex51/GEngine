#include "GBoneAnimation.h"

//-----------------------------------------------

GBoneAnimation::GBoneAnimation(void):GBaseAnimation("")
{
  SetType(G_AT_BONE);
}

//-----------------------------------------------

GBoneAnimation::~GBoneAnimation(void)
{
  Release();
}

//-----------------------------------------------


G_BONE_FRAME* GBoneAnimation::AddKeyFrame(g_uint32 FrameTime)
{
  G_BONE_FRAME* NewFrame = new G_BONE_FRAME();
  NewFrame->_time = FrameTime;
  KeyFrames_List.push_back(NewFrame);
  return NewFrame;
}

//-----------------------------------------------

void GBoneAnimation::AssignFrames(const vector <G_BONE_FRAME*> &frames_list)
{
  for (g_uint32 i = 0; i < frames_list.size(); i++)
  {
    G_BONE_FRAME *new_frame = new G_BONE_FRAME();
    *new_frame = *frames_list[i];
    KeyFrames_List.push_back(new_frame);
  }
}

//-----------------------------------------------

g_bool GBoneAnimation::GetBonesFrame(g_uint32 _time, G_BONE_FRAME &out_frame)
{
  G_BONE_FRAME *frame_st = NULL, *frame_end = NULL;
  g_uint32 nFrames = KeyFrames_List.size();

  for (g_uint32 i = 0; i < KeyFrames_List.size(); i++)
  {
	if ((_time < KeyFrames_List[i]->_time) && (i > 0))
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
      _time = KeyFrames_List[nFrames-1]->_time;
    }

  if ((frame_st == NULL) || (frame_end == NULL))
    return false;

  g_uint32 ftime = frame_end->_time - frame_st->_time;
  g_uint32 ctime = _time - frame_st->_time;

  g_float frame_pos = g_float(ctime)/g_float(ftime);
  
  g_uint32 nbones = frame_st->Bones.size();

  out_frame._time = _time;
  out_frame.Bones.resize(nbones);
  for (g_uint32 i = 0; i < nbones; i++)
  {
	D3DXQuaternionSlerp(&out_frame.Bones[i].Orientation, &frame_st->Bones[i].Orientation,
		                &frame_end->Bones[i].Orientation, frame_pos);
    out_frame.Bones[i].Position = frame_st->Bones[i].Position + (frame_end->Bones[i].Position - frame_st->Bones[i].Position)*frame_pos;
  }
  
  return true;
}

//-----------------------------------------------


g_bool GBoneAnimation::GetBonesArrays(g_uint32 _time, g_vec4 *bpos,  g_vec4 *bquat)
{
  G_BONE_FRAME *frame_st = NULL, *frame_end = NULL;
  g_uint32 nFrames = KeyFrames_List.size();

  for (g_uint32 i = 0; i < KeyFrames_List.size(); i++)
  {
	if ((_time < KeyFrames_List[i]->_time) && (i > 0))
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
      _time = KeyFrames_List[nFrames-1]->_time;
    }

  if ((frame_st == NULL) || (frame_end == NULL))
    return false;

  g_uint32 ftime = frame_end->_time - frame_st->_time;
  g_uint32 ctime = _time - frame_st->_time;

  g_float frame_pos = g_float(ctime)/g_float(ftime);
  
  g_uint32 nbones = frame_st->Bones.size();

  for (g_uint32 i = 0; i < nbones; i++)
  {
	D3DXQuaternionSlerp((g_quaternion*)&bquat[i], &frame_st->Bones[i].Orientation,
		                &frame_end->Bones[i].Orientation, frame_pos);
    bpos[i] = g_vec4(frame_st->Bones[i].Position + (frame_end->Bones[i].Position - frame_st->Bones[i].Position)*frame_pos, 0);
  }
  
  return true;
}

//-----------------------------------------------

void GBoneAnimation::Release()
{
  for (g_uint32 i = 0; i < KeyFrames_List.size(); i++)
    delete KeyFrames_List[i];
  KeyFrames_List.clear();
}

//-----------------------------------------------

