#include "GMaterial.h"

//-----------------------------------------------

GMaterial::GMaterial(const g_string &Name, GD3DDevice *Device):GBaseObject(Name,Device)
{
  SetType(G_OT_MATERIAL);
  Effect = NULL;
}

//-----------------------------------------------

GMaterial::~GMaterial(void)
{
  for (g_uint32 i = 0; i < _resources.size(); i++)
  {
	switch (_resources[i]->Type)
	{
	  case g_tt_single: delete (G_TEX_SINGLE*)_resources[i]; break;
      case g_tt_array: delete (G_TEX_ARRAY*)_resources[i]; break;
	}
  }
  _resources.clear();
}

//-----------------------------------------------

inline g_int32 GMaterial::_get_tex_index(const g_string &name)
{
  for (g_uint32 i=0; i<_resources.size(); i++)
  {
	if (_resources[i]->Type != g_tt_single) continue;
	if (name == ((G_TEX_SINGLE*)_resources[i])->Texture->GetName())
	  return i;
  }
  return -1;
}

//-----------------------------------------------

inline void GMaterial::_apply_res(GEffectVariable *res_var, G_TEX_BASE* res)
{
  if ((res_var == NULL) || (res == NULL)) return;
  switch (res->Type)
  {
    case g_tt_single:{
		Effect->GetVar(G_SVT_RESOURCE, res->ResID)->SetTexture(((G_TEX_SINGLE*)res)->Texture);
		}break; 
	case g_tt_array:{
		Effect->GetVar(G_SVT_RESOURCE, res->ResID)->SetTextureArray(((G_TEX_ARRAY*)res)->TextureList, _device);
		}break; 
  }
}

//-----------------------------------------------

GEffectTech* GMaterial::GetTech(const g_string &name)
{
  if (Effect == NULL) return NULL;
  return Effect->GetTech(name.c_str());
}

//-----------------------------------------------

GTexture* GMaterial::GetTexture(const g_string &name)
{
  g_int32 index = _get_tex_index(name);
  if ((index < 0) && (_resources[index]->Type != g_tt_single))
	return NULL;

  return ((G_TEX_SINGLE*)_resources[index])->Texture;
}

//-----------------------------------------------

void GMaterial::GetTextureArray(g_uint32 index, vector <GTexture*> &out_tex_array)
{
  if (index >= _resources.size())
	return;

  if (_resources[index]->Type != g_tt_array)
	return;

  out_tex_array = ((G_TEX_ARRAY*)_resources[index])->TextureList;
}

//-----------------------------------------------

void GMaterial::AddTexture(GTexture *Texture, g_int32 ResID)
{
  if (Texture == NULL) return;
  G_TEX_SINGLE *NewTex = new G_TEX_SINGLE();
  NewTex->Type = g_tt_single;
  NewTex->ResID = ResID;
  NewTex->Texture = Texture;
  _resources.push_back(NewTex);
}

//-----------------------------------------------

void GMaterial::AddTextureArray(const vector <GTexture*> &TextureArray, g_int32 ResID)
{
  if (TextureArray.size() == 0) return;
  G_TEX_ARRAY *NewTex = new G_TEX_ARRAY();
  NewTex->Type = g_tt_array;
  NewTex->ResID = ResID;
  NewTex->TextureList = TextureArray;
  _resources.push_back(NewTex);
}

//-----------------------------------------------

void GMaterial::ApplyTexture(g_uint32 index)
{
  if (index >= _resources.size())
	return;

  _apply_res(Effect->GetVar(G_SVT_RESOURCE, _resources[index]->ResID), _resources[index]);
}

//-----------------------------------------------

void GMaterial::ApplyTexture(const g_string &name)
{
  g_int32 index = _get_tex_index(name);
  if (index < 0)
	return;

  _apply_res(Effect->GetVar(G_SVT_RESOURCE, _resources[index]->ResID), _resources[index]);
}

//-----------------------------------------------
	
void GMaterial::ApplyTexture(GTexture *custom_tex, g_int32 res_id)
{
  Effect->GetVar(G_SVT_RESOURCE, res_id)->SetTexture(custom_tex);
}

//-----------------------------------------------
	
void GMaterial::ApplyAllTextures()
{
  for (g_uint32 index = 0; index < _resources.size(); index++)
	_apply_res(Effect->GetVar(G_SVT_RESOURCE, _resources[index]->ResID), _resources[index]);
}

//-----------------------------------------------