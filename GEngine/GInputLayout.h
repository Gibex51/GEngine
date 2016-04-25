#pragma once

#include "GStructs.h"

#ifndef GILAYOUT_H
#define GILAYOUT_H

class GInputLayout
{
public:
	void Set();
	//Конструктор и деструктор
	GInputLayout(ID3D10Device *Device, ID3D10EffectTechnique *Tech,
		         D3D10_INPUT_ELEMENT_DESC *VertexLayout, g_uint32 LayoutElements);
	~GInputLayout(void);
private:
	ID3D10Device *_device;
	ID3D10InputLayout *_ilayout;
};

class GInputLayoutList
{
public:
	map <g_uint32, GInputLayout*> ILayouts;

	GInputLayout* AddInputLayout(g_uint32 UID, ID3D10EffectTechnique *Tech, D3D10_INPUT_ELEMENT_DESC *VertexLayout, g_uint32 LayoutElements);
	void          AddInputLayout(g_uint32 UID, GInputLayout* InputLayout);
	GInputLayout* CreateInputLayout(ID3D10EffectTechnique *Tech, D3D10_INPUT_ELEMENT_DESC *VertexLayout, g_uint32 LayoutElements);	
	GInputLayout* GetInputLayout(g_uint32 UID) {return _il_list[UID];};
	void          RemoveInputLayout(g_uint32 UID);

	void Release();
    //Конструктор и деструктор
	GInputLayoutList(ID3D10Device *Device);
	~GInputLayoutList(void);
private:
    map <g_uint32, GInputLayout*> _il_list;
	ID3D10Device *_device;
};

#endif //GILAYOUT_H