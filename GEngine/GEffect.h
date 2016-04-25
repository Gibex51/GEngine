#pragma once

#include "GTexture.h"
#include "GEffectPool.h"

#ifndef GEFFECT_H
#define GEFFECT_H

class GEffectVariable
{
public:
  void GetRawData(void **ppdata, g_uint32 data_sz);
  void SetRawData(void *pdata, g_uint32 data_sz);

  template<typename T> void GetRawData(T& value)
  {
    _dx_effect_var->GetRawValue((void*)&value, 0, sizeof(value));
  }

  template<typename T> void SetRawData(T& value)
  {
    _dx_effect_var->SetRawValue((void*)&value, 0, sizeof(value));
  }

  void SetMatrix(const g_mat4x4 &matrix);
  void SetMatrixArray(g_mat4x4 *matrixarray, g_uint32 elements);

  //void GetTexture(GTexture *Texture);
  g_bool SetTexture(GTexture *Texture);

  //g_bool SetTextureArray(GTexture **TexArray, g_uint8 TexCount, GD3DDevice *Device);
  g_bool SetTextureArray(vector <GTexture*> TexArray, GD3DDevice *Device);

  ID3D10EffectVariable* GetDXVar() {if (this != NULL) return _dx_effect_var; else return NULL;};

  g_bool InitTextureBuffer(GD3DDevice *Device, g_uint32 elem_count, 
	                       g_uint32 elem_size = 4, DXGI_FORMAT elem_format = DXGI_FORMAT_R32_FLOAT);
  g_bool SetTextureBuffer(GD3DDevice *Device, void *Buffer);

  GEffectVariable();
  GEffectVariable(ID3D10EffectVariable *dx_effect_var);
  ~GEffectVariable();
private:
  ID3D10EffectVariable *_dx_effect_var;

  // Tex Buffer
  g_uint32 _buf_elem_count;
  g_uint32 _buf_elem_size;

  G_BUFFER  *_shader_buffer;
  GTexture  *_texture_buffer;

  inline void _release_buffers();
};


class GEffectTech
{
public:
  void ApplyPass(g_uint32 index);
  void ApplyPass(const g_string& name);

  g_uint32 PassCount() {return _pass_count;};
  const g_string& Name() {return _name;};

  ID3D10EffectTechnique* GetDXTech() {return _dx_effect_tech;};
  
  GEffectTech();
  GEffectTech(ID3D10EffectTechnique *dx_effect_tech);
  ~GEffectTech();
private:
  ID3D10EffectTechnique *_dx_effect_tech;
  g_uint32 _pass_count;
  g_string _name;
};


class GEffect:
  public GBaseObject
{
public:
  g_bool LoadFromFile(const g_string &filename, GEffectPool *effect_pool);
  
  GEffectVariable* GetVar(g_int32 type, g_int32 id);
  GEffectVariable* GetVarByName(const g_string &name);
  GEffectVariable* GetVarBySemantic(const g_string &semantic);
  GEffectTech* GetTech(g_uint32 id);
  GEffectTech* GetTech(const g_string &name);

  ID3D10Effect *GetDXEffect() {return _dx_effect;};

  void Release();

  GEffect(const g_string &Name, GD3DDevice *Device);
  GEffect(const g_string &FileName, GEffectPool *EffectPool, const g_string &Name, GD3DDevice *Device);
  ~GEffect(void);
private:
  ID3D10Effect *_dx_effect;

  g_string     _filename;
  GEffectPool  *_p_fx_pool_uid;

  map <g_uint32, GEffectVariable*> _vars;
  map <g_uint32, GEffectTech*>     _techs;

  void _init();
  void _load_variables();
  inline g_uint32 _make_var_uid(g_int32 type, g_int32 id);
  inline g_bool _get_variable_uid(ID3D10EffectVariable *var, g_uint32 &uid);
  inline g_bool _get_tech_uid(ID3D10EffectTechnique *tech, g_uint32 &uid);
};

#endif // GEFFECT_H