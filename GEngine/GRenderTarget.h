#pragma once

#ifndef GRENDTARG_H
#define GRENDTARG_H

#include "GStructs.h"

using namespace std;

class GRenderTarget
{
public:
  g_bool CreateDepthStensilView(ID3D10Resource *pRes, DXGI_FORMAT Format);
  g_bool AddRenderTargetView(ID3D10Resource *pRes, D3D10_RENDER_TARGET_VIEW_DESC *pDesc);

  g_bool SetRenderToTexture(g_int32 width, g_int32 height, ID3D10Texture2D *ppTexture);

  void Set();
  void Clear(g_bool RT, g_bool Depth, g_bool Stencil, g_color clear_color = def_clearcolor);
  //Release
  void ReleaseDepthStensil();
  void ReleaseRenderTarget(g_uint32 index);
  void ReleaseAllRenderTarget();
  void Release();
  //Конструктор и деструктор
  GRenderTarget(ID3D10Device* GDevice);
  ~GRenderTarget(void);  
private:
  ID3D10Device *_device;

  vector <ID3D10RenderTargetView*> _p_rt_view; //render target array
  ID3D10DepthStencilView*          _p_ds_view; //depth stensil 
};

class GRenderTargetList
{
public:
  GRenderTarget* AddRenderTarget(string Name);
  GRenderTarget* GetRenderTarget(string Name);
  void RemoveRenderTarget(string Name);
  void Release();
  //Конструктор и деструктор
  GRenderTargetList(ID3D10Device *Device);
  ~GRenderTargetList();
private:
  ID3D10Device *_device;
  map <string, GRenderTarget*> _rt_list;
};

#endif //GRENDTARG_H