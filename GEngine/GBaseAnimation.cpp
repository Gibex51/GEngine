#include "GBaseAnimation.h"

//-----------------------------------------------

GBaseAnimation::GBaseAnimation(const g_string &Name):GBaseObject(Name,NULL)
{
  //_loop = false;
  //_loop_count = 0;
  _length = 0;
}

//-----------------------------------------------

GBaseAnimation::~GBaseAnimation(void)
{

}

//-----------------------------------------------


g_bool GBaseAnimation::GetCurrentAnimationTime(const g_uint32 current_time, 
											   const G_ANIM_DYNAMIC_INFO &anim_info, 
		                                       g_uint32 &anim_time)
{  
  if (anim_info._start_time > current_time) 
    return false;

  anim_time = current_time - anim_info._start_time;

  if (anim_time < _length)
	return true;

  if (anim_info._loop_count == 1)
	return false;

  g_int32 _current_loop = anim_time/_length + 1;
  if (((anim_info._loop_count > 0) && (anim_info._loop_count - _current_loop >= 0)) || 
	  (anim_info._loop_count == 0))
  {		
	if ((anim_info._loop_count == 0) && (anim_info._loop_time > 0))
      if (anim_time > anim_info._loop_time)
		return false;
    anim_time %= _length;       
    return true;
  }

  return false;
}

//-----------------------------------------------

g_int32 GBaseAnimation::GetCurrentLoop(const g_uint32 current_time, 
									   const G_ANIM_DYNAMIC_INFO &anim_info)
{
  if (anim_info._start_time > current_time) 
    return -1;

  return (current_time - anim_info._start_time)/_length + 1;
}

//-----------------------------------------------
