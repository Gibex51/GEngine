#include "GMeshInstance.h"

//-----------------------------------------------

GMeshInstance::GMeshInstance(GModel *ParentModel, g_animap *AnimList)
{
  _parent = ParentModel;
  _anim_list = AnimList;

  _tex_index = 0;

  Visible = true;
}

//-----------------------------------------------

GMeshInstance::~GMeshInstance(void)
{

}

//-----------------------------------------------

void GMeshInstance::ClearActiveAnimList()
{
  _active_anim_list.clear();
}

//-----------------------------------------------

void GMeshInstance::GetInstanceState(const g_uint32 time, G_INSTANCE_STATE &state)
{
  for (g_uint32 i = 0; i<_active_anim_list.size(); i++)
  {
	g_en_anim_state &anim_state = _active_anim_list[i].anim_info._anim_state;
	GBaseAnimation *curr_anim = _active_anim_list[i].anim;

	if (curr_anim == NULL) anim_state = g_as_inactive;

	switch (anim_state)
	{
	  case g_as_active:
		  {
			g_uint32 anim_time;
	  
            g_bool IsEnd = !curr_anim->GetCurrentAnimationTime(time, _active_anim_list[i].anim_info, anim_time);
	  	  
	        switch (curr_anim->GetType())
	        {
	          case G_AT_MATRIX:
                  {
		            GMatrixAnimation* mat_anim = (GMatrixAnimation*)curr_anim;
                    G_MATRIX_FRAME curr_frame;
                    mat_anim->GetMatrixFrame(anim_time, curr_frame);
			        //----обработка фрейма
					if (curr_frame._use_pos)
					  SetPosition(curr_frame._position);
					if (curr_frame._use_rot)
					  RotateLocal(curr_frame._rotation);
					if (curr_frame._use_scale)
					  SetScaling(curr_frame._scale);
                  }
			      break;		 
	          case G_AT_BONE:
                  {
		            GBoneAnimation* bon_anim = (GBoneAnimation*)curr_anim;			   
			        G_BONE_FRAME curr_frame;
					bon_anim->GetBonesArrays(anim_time, state.bones_pos, state.bones_quat);
                  }
			      break;
	        }

            if (IsEnd)
              anim_state = g_as_inactive;
		  }
		  break;
	  case g_as_inactive:
	      _active_anim_list.erase(_active_anim_list.begin() + i);
		  i--;
	      break;
	  case g_as_pause:
	      //Изменение времени чтобы после включения продолжилось с тогоже места
	      break;
	}
  }
  // Получаем финальную матрицу трансформации
  GetTransform(*state.transform);
}

//-----------------------------------------------

void GMeshInstance::RunAnimation(g_int32 uid, g_uint32 LoopCount, g_uint32 LoopTime)
{
  _active_anim_list.push_back(G_ACTIVE_ANIM(g_uint32(GetTicks()), (*_anim_list)[uid], LoopCount, LoopTime));
}

//-----------------------------------------------

void GMeshInstance::RunAnimationInd(g_uint32 index, g_uint32 LoopCount, g_uint32 LoopTime)
{
  if (index < _anim_list->size())
  {
	g_animap::iterator it = _anim_list->begin();
    for (g_uint32 i = 0; i < index; i++)
	  it++;
	  
    _active_anim_list.push_back(G_ACTIVE_ANIM(g_uint32(GetTicks()), (*it).second, LoopCount, LoopTime));
  }
}

//-----------------------------------------------

void GMeshInstance::RunCustomAnimation(GBaseAnimation* cust_anim, g_uint32 LoopCount, 
									   g_uint32 LoopTime)
{
  _active_anim_list.push_back(G_ACTIVE_ANIM(g_uint32(GetTicks()), cust_anim, LoopCount, LoopTime));
}

//-----------------------------------------------

void GMeshInstance::StopAnimation(g_int32 uid)
{
  for (g_uint32 i = 0; i < _active_anim_list.size(); i++)
    if (_active_anim_list[i].anim->GetUID() == uid)
	{
	  _active_anim_list.erase(_active_anim_list.begin() + i);
      return;
	}
}

//-----------------------------------------------

g_en_anim_state GMeshInstance::GetAnimationState(g_int32 uid)
{
  for (g_uint32 i = 0; i < _active_anim_list.size(); i++)
    if (_active_anim_list[i].anim->GetUID() == uid)
      return _active_anim_list[i].anim_info._anim_state;
  return g_as_inactive;
}

//-----------------------------------------------
