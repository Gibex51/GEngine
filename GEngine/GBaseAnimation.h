#pragma once

#ifndef GBASEANIM_H
#define GBASEANIM_H

#include "GBaseObject.h"

using namespace std;

struct G_ANIM_DYNAMIC_INFO
{
  g_en_anim_state _anim_state;
  g_uint32        _start_time;
  g_uint32        _pause_time;
  g_uint32        _loop_time;
  g_uint32        _loop_count;
};

class GBaseAnimation :
	public GBaseObject
{
public:
    void SetLength(g_uint32 Length) {_length = Length;};
    g_uint32 GetLength() {return _length;};

    //void SetLoop(g_bool Loop, g_int32 LoopCount) {_loop = Loop; _loop_count = LoopCount;};

    g_bool GetCurrentAnimationTime(const g_uint32 current_time, const G_ANIM_DYNAMIC_INFO &anim_info, 
		                           g_uint32 &anim_time);

	g_int32 GetCurrentLoop(const g_uint32 current_time, const G_ANIM_DYNAMIC_INFO &anim_info);
	//Конструктор и деструктор
	GBaseAnimation(const g_string &Name);
	~GBaseAnimation(void);
private:
    //g_bool   _loop;
    //g_int32  _loop_count;
    g_uint32 _length;
};

#endif //GBASEANIM_H