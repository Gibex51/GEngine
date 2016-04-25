#pragma once

#include "GConfigMgr.h"
#include "GD3DBuffer.h"
#include "GRenderTarget.h"
#include "GInputLayout.h"
#include "GInfoCollector.h"

#ifndef GDEVICE_H
#define GDEVICE_H

typedef g_uint32 RTT_RESULT;

class GD3DDevice
{
public:
  g_bool v_sync;
  //Переменные
  ID3D10Device             *D3DDevice;
  IDXGISwapChain           *pSwapChain;

  ID3D10Texture2D          *pDepthStencil;

  ID3D10StateBlock         *pStateBlock1;
  ID3D10StateBlock         *pStateBlock2;  

  RECT                     ScreenRect;

  // Config Manager
  GConfigMgr *ConfMgr;

  // Resource Manager
  GResourceManager *ResMgr;

  //Функции
  g_bool InitDevice(g_uint32 Width, g_uint32 Height, HWND Handle, g_bool Windowed);
  g_bool InitRenderTarget();

  g_bool SetResolution(DXGI_MODE_DESC mode_desc, g_bool Windowed);

  void ClearView();
  void SwapBuffer();
  void SetPrimitiveTopology(g_uint8 PrimitiveTopology);

  void SetWireframe(g_bool Flag);

  void Release();

  g_uint32 GetWidth() {return _width;};
  g_uint32 GetHeight() {return _height;};

  //Buffers
  GD3DBuffer *BufferList;

  //Render Targets
  GRenderTargetList *RTList;

  //Input Layouts
  GInputLayoutList  *ILayoutList;

  //Info
  GInfoCollector    *Info;

  //Конструктор и деструктор
  GD3DDevice(void);
  ~GD3DDevice(void);
private:
  g_uint32 _width;
  g_uint32 _height;

  ID3D10RasterizerState    *_raster_state;

  GRenderTarget *_rt_main;

  g_bool _init_main_rt(g_uint32 width, g_uint32 height);
};

#endif /* GDEVICE_H */