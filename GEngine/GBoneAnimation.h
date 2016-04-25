#pragma once

#ifndef GBONEANIM_H
#define GBONEANIM_H

#include "GBaseAnimation.h"

struct G_BONE_SET
{
  g_vec3 Position;
  g_quaternion Orientation; 
};

struct G_BONE_FRAME
{
  g_uint32 _time;
  vector <G_BONE_SET> Bones;
};

class GBoneAnimation :
	public GBaseAnimation
{
public:
    vector <G_BONE_FRAME*> KeyFrames_List;
  
    G_BONE_FRAME* AddKeyFrame(g_uint32 FrameTime);
    void AssignFrames(const vector <G_BONE_FRAME*> &frames_list);
	//g_bool LoadKeyFramesFromFile(g_string FileName);
    //g_bool SaveKeyFramesToFile(g_string FileName);

    g_bool GetBonesFrame(g_uint32 _time, G_BONE_FRAME &out_frame); 
	g_bool GetBonesArrays(g_uint32 _time, g_vec4 *bpos,  g_vec4 *bquat);

    void Release();
    //Конструктор и деструктор
    GBoneAnimation(void);
    ~GBoneAnimation(void);
private:
};

#endif //GBONEANIM_H