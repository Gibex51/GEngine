#include "GEffectPool.h"

//-----------------------------------------------

GEffectPool::GEffectPool(const g_string &Name, GD3DDevice *Device):GBaseObject(Name,Device)
{
  _init();
}

//-----------------------------------------------

GEffectPool::GEffectPool(const g_string &FileName, const g_string &Name, GD3DDevice *Device):GBaseObject(Name,Device)
{
  _init();
  LoadFromFile(FileName);
}

//-----------------------------------------------

GEffectPool::~GEffectPool(void)
{
  Release();
}

//-----------------------------------------------

void GEffectPool::_init()
{
  _dx_effect_pool = NULL;
  SetType(G_OT_EFFECTPOOL);
}

//-----------------------------------------------

void GEffectPool::Release()
{
  NULLRELEASE(_dx_effect_pool);
}

//-----------------------------------------------

g_bool GEffectPool::LoadFromFile(const g_string &FileName)
{
  Release();

  return (S_OK == D3DX10CreateEffectPoolFromFile(FileName.c_str(), NULL, NULL, "fx_4_0",
	                                             D3D10_SHADER_ENABLE_STRICTNESS, 0, _device->D3DDevice, 
			                                     NULL, &_dx_effect_pool, NULL, NULL));
}

//-----------------------------------------------