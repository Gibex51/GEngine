#pragma once

#include "GObjectTransform.h"
#include "GMatrixAnimation.h"
#include "GBoneAnimation.h"

#ifndef GMESHINST_H
#define GMESHINST_H

//-----------------------------------------------

class GModel;

typedef map <g_int32, GBaseAnimation*> g_animap;

struct G_INSTANCE_STATE
{
  g_mat4x4      *transform;
  g_vec4        *bones_pos;
  g_vec4        *bones_quat;

  void Release() {FREENULL_ARR(bones_pos); FREENULL_ARR(bones_quat);};
  G_INSTANCE_STATE() {bones_pos = NULL; bones_quat = NULL;};
};

struct G_ACTIVE_ANIM
{
  G_ANIM_DYNAMIC_INFO  anim_info;
  GBaseAnimation*  anim;

  G_ACTIVE_ANIM(g_uint32 StartTime, GBaseAnimation*  Anim, g_uint32 LoopCount, g_uint32 LoopTime)
  {
    anim = Anim;
	if (Anim == NULL)
	  anim_info._anim_state = g_as_inactive;
	else
	  anim_info._anim_state = g_as_active;
	anim_info._pause_time = 0;
	anim_info._start_time = StartTime;
	anim_info._loop_count = LoopCount;
	anim_info._loop_time = LoopTime;
  };
};

//-----------------------------------------------

class GMeshInstance : 
	public GObjectTransform
{
public:
    g_bool Visible;

	void GetInstanceState(const g_uint32 time, G_INSTANCE_STATE &state);
	
	void RunAnimation(g_int32 uid, g_uint32 LoopCount, g_uint32 LoopTime);
	void RunAnimationInd(g_uint32 index, g_uint32 LoopCount, g_uint32 LoopTime);
	void RunCustomAnimation(GBaseAnimation* cust_anim, g_uint32 LoopCount, 
		                    g_uint32 LoopTime);
	void StopAnimation(g_int32 uid);
	g_en_anim_state GetAnimationState(g_int32 uid);
	g_uint32 GetActiveAnimCount() {return _active_anim_list.size();};
    void ClearActiveAnimList();

	GModel* GetParent() {return _parent;};

	void SetTexIndex(g_uint32 index) {_tex_index = index;};
	g_uint32 GetTexIndex() {return _tex_index;};

	//Конструктор и деструктор
	GMeshInstance(GModel *ParentModel, g_animap *AnimList);
	~GMeshInstance(void);
private:
	GModel   *_parent;
	g_animap *_anim_list;

	g_uint32 _tex_index;

	vector <G_ACTIVE_ANIM> _active_anim_list;	
};

#endif //GMESHINST_H