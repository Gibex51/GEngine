#pragma once

#include "GEffect.h"

#ifndef GMATERIAL_H
#define GMATERIAL_H

enum g_tex_type {g_tt_single, g_tt_array};

struct G_TEX_BASE
{
  g_uint32 ResID;
  g_tex_type Type;
};

struct G_TEX_SINGLE : G_TEX_BASE
{
  GTexture *Texture;
};

struct G_TEX_ARRAY : G_TEX_BASE
{
  vector <GTexture*> TextureList;
};

//-----------------------------------------------

class GMaterial :
	public GBaseObject
{
public:
    GEffect *Effect;

    GEffectTech* GetTech(const g_string &name);
	GTexture* GetTexture(const g_string &name);
	void GetTextureArray(g_uint32 index, vector <GTexture*> &out_tex_array);

	void AddTexture(GTexture *Texture, g_int32 ResID);
	void AddTextureArray(const vector <GTexture*> &TextureArray, g_int32 ResID);

	void ApplyTexture(g_uint32 index);
	void ApplyTexture(const g_string &name);
	void ApplyTexture(GTexture *custom_tex, g_int32 res_id);
	void ApplyAllTextures();

	g_bool IsValidIndex(g_uint32 index) {return (index < _resources.size());};
	
	//Конструктор и деструктор
	GMaterial(const g_string &Name, GD3DDevice *Device);
	~GMaterial(void);
private:
	vector <G_TEX_BASE*> _resources;

	inline void _apply_res(GEffectVariable *res_var, G_TEX_BASE* res); 
	inline g_int32 _get_tex_index(const g_string &name);
};

#endif // GMATERIAL_H