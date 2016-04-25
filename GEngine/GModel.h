#pragma once

#include "GMeshObject.h"
#include "GMeshInstance.h"
#include "GMaterial.h"

#ifndef GMODEL_H
#define GMODEL_H

//-----------------------------------------------

class GModel :
	public GMeshObject
{
public:
	//��������
	GMeshInstance* AddInstance();
	GMeshInstance* GetInstance(g_uint32 index);
	g_uint32 GetInstanceCount() {return _instances.size();};
	void ClearInstances();

	//��������
	g_bool SetMaterial(GMaterial *Material, g_uint32 ShaderTechID);
	GMaterial *GetMaterial() {return _material;};
	
	//������������� ��������� ����������
	void SetInstanceVariables(g_int32 first_var_id);
	void SetBonesCount(g_uint32 n_bones);

	//��������� ������
	void SetWorldMatrix(const g_mat4x4 &mWorld);
	void SetTransfWorldMatrix(const g_mat4x4 &mWorld);
    void SetViewProjMatrix(const g_mat4x4 &mViewProj);

	//��������
	GBaseAnimation* AddAnimation(g_int32 uid, g_int32 anim_type);
	void ClearAnimations();

	//������
	void Draw(g_bool set_textures = true, g_uint32 _startindex = 0, g_uint32 _indexcount = 0);

	//�������
	void Release();

	//����������� � ����������
	GModel(const g_string &Name, GD3DDevice *Device);
	~GModel(void);
private:
	//��������
	vector <GMeshInstance*> _instances;
	void _build_instances();
	g_uint32 _n_vis_inst;

	//������
	g_uint32             _n_bones;

	g_vec4    *_bones_and_transf_buf;
	g_uint32  *_texind_buf;

	GEffectVariable *_ev_n_bones;
	GEffectVariable *_ev_n_inst;
	GEffectVariable *_ev_bones_buffer;
	GEffectVariable *_ev_texind_buffer;
	
	void _update_buffers_size();
	void _update_buffer();

	//��������
	GMaterial *_material;
	GEffectTech *_et_render;

	//��������
	g_animap _anim_list;
};

#endif //GMODEL_H