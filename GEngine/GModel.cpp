#include "GModel.h"

//-----------------------------------------------

GModel::GModel(const g_string &Name, GD3DDevice *Device):GMeshObject(Name,Device)
{
  _material = NULL;
  _et_render = NULL;

  _n_vis_inst = 0;

  _n_bones = 0;

  _ev_n_bones = NULL;
  _ev_n_inst = NULL;
  _ev_bones_buffer = NULL;
  _ev_texind_buffer = NULL;

  _bones_and_transf_buf = NULL;
  _texind_buf = NULL;

  SetType(G_OT_MODEL);
}

//-----------------------------------------------

GModel::~GModel(void)
{
  Release();
}

//-----------------------------------------------

void GModel::Release()
{
  ClearInstances();
  ClearAnimations();

  FREENULL_ARR(_bones_and_transf_buf);
  FREENULL_ARR(_texind_buf);

  _n_vis_inst = 0;
  _n_bones = 0;

  _ev_n_bones = NULL;
  _ev_n_inst = NULL;
  _ev_bones_buffer = NULL;
  _ev_texind_buffer = NULL;

  _material = NULL;
  _et_render = NULL;
}

//-----------------------------------------------

g_bool GModel::SetMaterial(GMaterial *Material, g_uint32 ShaderTechID)
{
  if ((Material == NULL) || (Material->Effect == NULL))
    return false;

  _material = Material;
  _et_render = _material->Effect->GetTech(ShaderTechID);

  return (_et_render != NULL);
}

//-----------------------------------------------

GMeshInstance* GModel::AddInstance()
{
  GMeshInstance *NewInstance = new GMeshInstance(this, &_anim_list);
  _instances.push_back(NewInstance);
  
  _update_buffers_size();
  return NewInstance;
}

//-----------------------------------------------

GMeshInstance* GModel::GetInstance(g_uint32 index)
{
  if (this == NULL) return NULL;
  if (index < _instances.size())
    return _instances[index];
  return NULL;
}

//-----------------------------------------------

void GModel::ClearInstances()
{
  for (g_uint32 i = 0; i<_instances.size(); i++)
	delete _instances[i];
  _instances.clear();
}

//-----------------------------------------------

void GModel::SetInstanceVariables(g_int32 first_var_id)
{
  if (_material != NULL)
  {
	_ev_n_bones = _material->Effect->GetVar(G_SVT_CUSTOM, first_var_id);
	_ev_n_inst = _material->Effect->GetVar(G_SVT_CUSTOM, first_var_id+1);
	_ev_bones_buffer = _material->Effect->GetVar(G_SVT_CUSTOM, first_var_id+2);
    _ev_texind_buffer = _material->Effect->GetVar(G_SVT_CUSTOM, first_var_id+3);
  }
}

//-----------------------------------------------

GBaseAnimation* GModel::AddAnimation(g_int32 uid, g_int32 anim_type)
{
  GBaseAnimation* NewAnim;
  switch (anim_type)
  {
    case G_AT_MATRIX: NewAnim = new GMatrixAnimation(); break;
	case G_AT_BONE:   NewAnim = new GBoneAnimation();   break;
	default:          return NULL;
  }
  NewAnim->SetUID(uid);
  _anim_list[uid] = NewAnim;

  return NewAnim;
}

//-----------------------------------------------

void GModel::ClearAnimations()
{
  g_animap::iterator it;
  for (it = _anim_list.begin(); it != _anim_list.end(); it++)
  {
    GBaseAnimation *CurrRes = (*it).second;
    if (CurrRes != NULL)
      switch (CurrRes->GetType())
	  {
	    case G_AT_MATRIX: delete dynamic_cast<GMatrixAnimation*>(CurrRes); break;
	    case G_AT_BONE:   delete dynamic_cast<GBoneAnimation*>(CurrRes); break;
	  }
  }
  _anim_list.clear();
}

//-----------------------------------------------

void GModel::SetBonesCount(g_uint32 n_bones)
{
  _n_bones = n_bones;
  _update_buffers_size();
}

//------------------------------------------------

void GModel::Draw(g_bool set_textures, g_uint32 _startindex, g_uint32 _indexcount)
{ 
  _device->SetPrimitiveTopology(_prim_topology);
  
  //Build Instances Buffers
  _build_instances();

  if (_n_vis_inst == 0) return;

  g_uint32 n_inst = _instances.size();

  //Set Input Layout
  _il->Set(); 
  
  // Set Vertex & Index Buffers
  SetVIBuffers(); 

  //Set Textures
  if (set_textures)
    _material->ApplyAllTextures();

  //Set Instance Count
  if (_ev_n_inst != NULL)
    _ev_n_inst->SetRawData(n_inst);

  //Set Bones Count
  if (_ev_n_bones != NULL)
    _ev_n_bones->SetRawData(_n_bones);

  //Draw
  if (_et_render == NULL) return;

  if (_indexcount == 0) 
  {
	_indexcount = _nindices;
	_startindex = 0;
  }

  for (g_uint32 p = 0; p < _et_render->PassCount(); ++p)
  {
	_et_render->ApplyPass(p);
    _device->D3DDevice->DrawIndexedInstanced(_indexcount, _n_vis_inst, _startindex, 0, 0 );
  }
}

//------------------------------------------------

void GModel::_build_instances()
{
  g_uint32 n_inst = _instances.size();

  if ((_bones_and_transf_buf == NULL) || (_texind_buf == NULL))
	return;
	
  g_uint32 curr_time = g_uint32(GetTicks());
  g_uint32 _transf_size = n_inst*4;
  g_uint32 _bones_size = n_inst*_n_bones;
  g_uint32 buf_size = _transf_size + _bones_size*2;

  G_INSTANCE_STATE curr_state;
  _n_vis_inst = 0;
  for (g_uint32 i = 0; i < n_inst; i++)
  {
	//Fill Bones, Transform, TexIndex Buffers
	if (!_instances[i]->Visible) continue;
	curr_state.transform = (g_mat4x4*)&_bones_and_transf_buf[_n_vis_inst*4];
	curr_state.bones_pos = &_bones_and_transf_buf[_transf_size + _n_vis_inst*_n_bones];
	curr_state.bones_quat = &_bones_and_transf_buf[_transf_size + _bones_size + _n_vis_inst*_n_bones];
	_instances[i]->GetInstanceState(curr_time, curr_state);
	_texind_buf[i] = _instances[i]->GetTexIndex();

    _n_vis_inst++;
  }

  if (_ev_bones_buffer != NULL)
	_ev_bones_buffer->SetTextureBuffer(_device, _bones_and_transf_buf);

  if (_ev_texind_buffer != NULL)
	_ev_texind_buffer->SetTextureBuffer(_device, _texind_buf);
}

//-----------------------------------------------

void GModel::_update_buffers_size()
{
  g_uint32 n_inst = _instances.size();
  if (n_inst == 0) return;

  g_uint32 buf_size = n_inst*(4 + _n_bones*2);
  FREENULL_ARR(_bones_and_transf_buf);
  _bones_and_transf_buf = new g_vec4[buf_size];

  FREENULL_ARR(_texind_buf);
  _texind_buf = new g_uint32[n_inst];

  if (_ev_bones_buffer != NULL)
	_ev_bones_buffer->InitTextureBuffer(_device, buf_size, sizeof(g_vec4), 
	                                    DXGI_FORMAT_R32G32B32A32_FLOAT);

  if (_ev_texind_buffer != NULL)
	_ev_texind_buffer->InitTextureBuffer(_device, n_inst, sizeof(g_uint32), 
	                                     DXGI_FORMAT_R32_UINT);
}

//-----------------------------------------------

void GModel::SetWorldMatrix(const g_mat4x4 &mWorld)
{
  _material->Effect->GetVar(G_SVT_MWORLD, 0)->SetMatrix(mWorld);
}

//-----------------------------------------------

void GModel::SetViewProjMatrix(const g_mat4x4 &mViewProj)
{
  _material->Effect->GetVar(G_SVT_MVP, 0)->SetMatrix(mViewProj);
}

//-----------------------------------------------