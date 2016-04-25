#include "GInputLayout.h"

//-----------------------------------------------
// GInputLayout
//-----------------------------------------------

GInputLayout::GInputLayout(ID3D10Device *Device, ID3D10EffectTechnique *Tech, 
						   D3D10_INPUT_ELEMENT_DESC *VertexLayout, g_uint32 LayoutElements)
{
  _device = Device;
  _ilayout = NULL;

  if ((Tech != NULL) && (Tech->IsValid()))
  {
	D3D10_PASS_DESC PassDesc;
    Tech->GetPassByIndex(0)->GetDesc(&PassDesc);
  
    if(FAILED(_device->CreateInputLayout(VertexLayout,LayoutElements,PassDesc.pIAInputSignature, 
                                        PassDesc.IAInputSignatureSize,&_ilayout)))
      _ilayout = NULL;
  }
}

//-----------------------------------------------

GInputLayout::~GInputLayout(void)
{
  _ilayout->Release();
}

//-----------------------------------------------

void GInputLayout::Set()
{
  if (_ilayout != NULL)
	_device->IASetInputLayout(_ilayout);
}

//-----------------------------------------------
// GInputLayoutList
//-----------------------------------------------

GInputLayoutList::GInputLayoutList(ID3D10Device *Device)
{
  _device = Device;
}

//-----------------------------------------------

GInputLayoutList::~GInputLayoutList(void)
{
  Release();
}

//-----------------------------------------------

GInputLayout* GInputLayoutList::AddInputLayout(g_uint32 UID, ID3D10EffectTechnique *Tech, 
											   D3D10_INPUT_ELEMENT_DESC *VertexLayout, 
							                   g_uint32 LayoutElements)
{
  if (_device == NULL) return NULL;
 
  GInputLayout *NewIL = new GInputLayout(_device, Tech, VertexLayout, LayoutElements);
  if (NewIL != NULL)
    _il_list[UID] = NewIL;
  return NewIL;
}

//-----------------------------------------------

void GInputLayoutList::AddInputLayout(g_uint32 UID, GInputLayout* InputLayout)
{
  if (InputLayout != NULL)
    _il_list[UID] = InputLayout;
}

//-----------------------------------------------

GInputLayout* GInputLayoutList::CreateInputLayout(ID3D10EffectTechnique *Tech, 
												  D3D10_INPUT_ELEMENT_DESC *VertexLayout, 
												  g_uint32 LayoutElements)
{
  if (_device == NULL) return NULL;
  GInputLayout *NewIL = new GInputLayout(_device, Tech, VertexLayout, LayoutElements);
  return NewIL;
}	

//-----------------------------------------------

void GInputLayoutList::Release()
{
  map<g_uint32, GInputLayout*>::iterator it;

  for (it = _il_list.begin() ;it != _il_list.end(); it++ )
  {
    if ((*it).second != NULL) delete (*it).second;
  }

  _il_list.clear();
}

//-----------------------------------------------

void GInputLayoutList::RemoveInputLayout(g_uint32 UID)
{
  if (_il_list[UID] != NULL) delete _il_list[UID];
  _il_list[UID] = NULL;
  _il_list.erase(UID);
}

//-----------------------------------------------
