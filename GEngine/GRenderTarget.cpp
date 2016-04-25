#include "GRenderTarget.h"

//-----------------------------------------------

GRenderTargetList::GRenderTargetList(ID3D10Device *Device)
{
  _device = Device;
}

//-----------------------------------------------

GRenderTargetList::~GRenderTargetList(void)
{
  Release();
}

//-----------------------------------------------

GRenderTarget* GRenderTargetList::AddRenderTarget(string Name)
{
  GRenderTarget* NewRT = new GRenderTarget(_device);
  _rt_list[Name] = NewRT;
  return NewRT;
}

//-----------------------------------------------

GRenderTarget* GRenderTargetList::GetRenderTarget(string Name)
{
  GRenderTarget* result = _rt_list[Name];
  if (result == NULL) _rt_list.erase(Name);
  return result;
}

//-----------------------------------------------

void GRenderTargetList::RemoveRenderTarget(string Name)
{
  if (_rt_list[Name] != NULL) delete _rt_list[Name];
  _rt_list[Name] = NULL;
  _rt_list.erase(Name);
}

//-----------------------------------------------

void GRenderTargetList::Release()
{
  map<string, GRenderTarget*>::iterator it;

  for (it = _rt_list.begin() ;it != _rt_list.end(); it++ )
  {
    if ((*it).second != NULL) delete (*it).second;
  }

  _rt_list.clear();
}

//-----------------------------------------------
//  Render Target Class
//-----------------------------------------------

GRenderTarget::GRenderTarget(ID3D10Device* GDevice)
{
  _device = GDevice;
  _p_ds_view = NULL;
}


//-----------------------------------------------

GRenderTarget::~GRenderTarget(void)
{
  Release();
}

//-----------------------------------------------

void GRenderTarget::Release()
{
  ReleaseDepthStensil();
  ReleaseAllRenderTarget();
}

//-----------------------------------------------

void GRenderTarget::ReleaseDepthStensil()
{
  if (_p_ds_view != NULL) _p_ds_view->Release();
  _p_ds_view = NULL;
}

//-----------------------------------------------

void GRenderTarget::ReleaseRenderTarget(g_uint32 index)
{
  if (index < _p_rt_view.size())
  {
	_p_rt_view[index]->Release();
	_p_rt_view.erase(_p_rt_view.begin() + index);
  }
}

//-----------------------------------------------

void GRenderTarget::ReleaseAllRenderTarget()
{
  for (g_uint32 i = 0; i < _p_rt_view.size(); i++)
    _p_rt_view[i]->Release();
  _p_rt_view.clear();
}

//-----------------------------------------------

g_bool GRenderTarget::CreateDepthStensilView(ID3D10Resource *pRes, DXGI_FORMAT Format)
{
  D3D10_DEPTH_STENCIL_VIEW_DESC _desc_dsv;
  _desc_dsv.Format = DXGI_FORMAT_D32_FLOAT;
  _desc_dsv.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
  _desc_dsv.Texture2D.MipSlice = 0;
  
  if(FAILED(_device->CreateDepthStencilView(pRes, &_desc_dsv, &_p_ds_view)))
    return false;

  return true;
}

//-----------------------------------------------

g_bool GRenderTarget::AddRenderTargetView(ID3D10Resource *pRes, D3D10_RENDER_TARGET_VIEW_DESC *pDesc)
{
  _p_rt_view.push_back(NULL);
  g_int32 curr = _p_rt_view.size() - 1;

  if(FAILED(_device->CreateRenderTargetView(pRes, pDesc, &_p_rt_view[curr])))
    return false;

  return true;
}

//-----------------------------------------------

g_bool GRenderTarget::SetRenderToTexture(g_int32 width, g_int32 height, ID3D10Texture2D *ppTexture)
{
  D3D10_TEXTURE2D_DESC descRTT;
  ppTexture->GetDesc(&descRTT);

  D3D10_RENDER_TARGET_VIEW_DESC descviewRTT;
  descviewRTT.Format = descRTT.Format;
  descviewRTT.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
  descviewRTT.Texture2D.MipSlice = 0;

  ReleaseAllRenderTarget();

  if (!AddRenderTargetView(ppTexture, &descviewRTT))
	return false;

  return true;
}

//-----------------------------------------------

void GRenderTarget::Set()
{
  g_uint32 n_rt = _p_rt_view.size();
  ID3D10RenderTargetView **_pp_rt_view = NULL;
  if (n_rt > 0)
    _pp_rt_view = &_p_rt_view[0];

  _device->OMSetRenderTargets(n_rt, _pp_rt_view, _p_ds_view);
}

//-----------------------------------------------

void GRenderTarget::Clear(g_bool RT, g_bool Depth, g_bool Stencil, g_color clear_color)
{
  //Очищаем Render Target
  if (RT) 
	for (g_uint32 i = 0; i<_p_rt_view.size(); i++)
      _device->ClearRenderTargetView(_p_rt_view[i], (g_float*)&clear_color);
  //Очищаем Depth Stensil
  if (_p_ds_view != NULL)
  {
    g_uint32 flag = 0;
    if (Depth) flag += D3D10_CLEAR_DEPTH;
    if (Stencil) flag += D3D10_CLEAR_STENCIL;
    if (flag > 0)
      _device->ClearDepthStencilView(_p_ds_view, flag, def_cleardepth, def_clearstencil);
  }
}

//-----------------------------------------------
