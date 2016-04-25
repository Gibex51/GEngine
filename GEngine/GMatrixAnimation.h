#pragma once

#ifndef GMATANIM_H
#define GMATANIM_H

#include "GBaseAnimation.h"

struct G_MATRIX_FRAME
{
  g_uint32 _start_time;
  g_vec3   _position;
  g_vec3   _scale;
  g_vec3   _rotation;
  g_bool   _use_rot;
  g_bool   _use_scale;
  g_bool   _use_pos;

  G_MATRIX_FRAME() {
             _start_time = 0;
             _position = _scale = _rotation = g_vec3(0,0,0);
			 _use_rot = _use_scale = _use_pos = false;
           };

  G_MATRIX_FRAME(g_uint32 start_time, g_vec3 position, g_vec3 scale, g_vec3 rotation, 
	             g_bool use_pos, g_bool use_scale, g_bool use_rot) 
           {
             _start_time = start_time;
             _position = position;
             _scale = scale;
             _rotation = rotation;
			 _use_pos = use_pos;		 
			 _use_scale = use_scale;
			 _use_rot = use_rot;
           };
};

class GMatrixAnimation :
	public GBaseAnimation
{
public:
    vector <G_MATRIX_FRAME*> KeyFrames_List;
  
    G_MATRIX_FRAME* AddKeyFrame(g_uint32 FrameTime);
    void AddKeyFrameArray(G_MATRIX_FRAME *FrameArray, g_int32 nFrames);
    //g_bool LoadKeyFramesFromFile(g_string FileName);
    //g_bool SaveKeyFramesToFile(g_string FileName);

    g_bool GetMatrixFrame(g_uint32 _time, G_MATRIX_FRAME &out_frame);
   
	void Release();
    //Конструктор и деструктор
	GMatrixAnimation(void);
	~GMatrixAnimation(void);
private:

};

#endif //GMATANIM_H