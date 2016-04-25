#include "GDevice.h"

//-----------------------------------------------

GD3DDevice::GD3DDevice(void)
{
  v_sync = false;

  D3DDevice = NULL;
  pSwapChain = NULL;
  pDepthStencil = NULL;
  pStateBlock1 = NULL;
  pStateBlock2 = NULL;
  _raster_state = NULL;
  ConfMgr = new GConfigMgr;
  ResMgr = NULL;

  BufferList = new GD3DBuffer;
  ILayoutList = NULL;
  RTList = NULL;

  _rt_main = NULL;

  Info = new GInfoCollector();
  Info->CollectInfo();
}

//-----------------------------------------------

GD3DDevice::~GD3DDevice(void)
{
  FREENULL_VAR(ConfMgr);
  FREENULL_VAR(BufferList);
  FREENULL_VAR(RTList);
  FREENULL_VAR(ILayoutList);
  FREENULL_VAR(Info);
}

//-----------------------------------------------

g_bool GD3DDevice::InitDevice(g_uint32 Width, g_uint32 Height, HWND Handle, g_bool Windowed)
{
  _width = Width;
  _height = Height;

  SetRect(&ScreenRect,0,0,Width,Height);

  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory( &sd, sizeof(sd) );
  sd.BufferCount = 1;
  sd.BufferDesc.Width = Width;
  sd.BufferDesc.Height = Height;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = Handle;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = Windowed;
  sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

  #ifdef _DEBUG
  if (FAILED(D3D10CreateDeviceAndSwapChain(NULL,D3D10_DRIVER_TYPE_HARDWARE,NULL,
                          D3D10_CREATE_DEVICE_DEBUG,D3D10_SDK_VERSION,&sd,&pSwapChain,&D3DDevice)))
    return false;
  #else
  if (FAILED(D3D10CreateDeviceAndSwapChain(NULL,D3D10_DRIVER_TYPE_HARDWARE,NULL,
                          0,D3D10_SDK_VERSION,&sd,&pSwapChain,&D3DDevice)))
    return false;
  #endif

  BufferList->_device = D3DDevice;
  if (RTList == NULL) RTList = new GRenderTargetList(D3DDevice);
  if (ILayoutList == NULL) ILayoutList = new GInputLayoutList(D3DDevice);

  return true;
}

//-----------------------------------------------

g_bool GD3DDevice::_init_main_rt(g_uint32 width, g_uint32 height)
{
  // Получаем backbuffer
  ID3D10Texture2D *pBackBuffer;
  if(FAILED(pSwapChain->GetBuffer(0,__uuidof(ID3D10Texture2D),(LPVOID*)&pBackBuffer)))
    return false;

  // Создаем Render Target View
  if (_rt_main == NULL)
    _rt_main = RTList->AddRenderTarget("rt_main");   
 
  g_bool res = _rt_main->AddRenderTargetView(pBackBuffer, NULL);
  pBackBuffer->Release();

  if (!res)
	return false;

  // Создаем Depth Stencil Texture
  D3D10_TEXTURE2D_DESC descDepth = DEF_DESC;
  descDepth.Width = width;
  descDepth.Height = height;
  descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
  descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;
  
  NULLRELEASE(pDepthStencil);
  HRESULT hr = D3DDevice->CreateTexture2D(&descDepth, NULL, &pDepthStencil);
  if(FAILED(hr))
    return false;
  
  // Создаем Depth Stencil
  if (!_rt_main->CreateDepthStensilView(pDepthStencil, DXGI_FORMAT_D32_FLOAT))
	return false;
  
  // Устанавливаем Render Target
  _rt_main->Set();

  return true;
}

//-----------------------------------------------

g_bool GD3DDevice::InitRenderTarget()
{
  if (!_init_main_rt(_width, _height))
	return false;

  //Инициализация RasterState
  D3D10_RASTERIZER_DESC rasterizerState;
  rasterizerState.FillMode = D3D10_FILL_SOLID;
  rasterizerState.CullMode = D3D10_CULL_NONE;//D3D10_CULL_FRONT;
  rasterizerState.FrontCounterClockwise = true;
  rasterizerState.DepthBias = false;
  rasterizerState.DepthBiasClamp = 0;
  rasterizerState.SlopeScaledDepthBias = 0;
  rasterizerState.DepthClipEnable = true;
  rasterizerState.ScissorEnable = true;
  rasterizerState.MultisampleEnable = false;
  rasterizerState.AntialiasedLineEnable = false;

  D3DDevice->CreateRasterizerState( &rasterizerState, &_raster_state );

  D3DDevice->RSSetState(_raster_state);

  //Инициализация StateBlock
  D3D10_STATE_BLOCK_MASK StateBlockMask;
  D3D10StateBlockMaskEnableAll(&StateBlockMask);
  D3D10StateBlockMaskDisableCapture(&StateBlockMask,D3D10_DST_OM_RENDER_TARGETS, 0, 1 );
  D3D10CreateStateBlock(D3DDevice,&StateBlockMask,&pStateBlock1);
  D3D10CreateStateBlock(D3DDevice,&StateBlockMask,&pStateBlock2);
  return true;
}

//-----------------------------------------------

void GD3DDevice::ClearView()
{
  _rt_main->Clear(true, true, false);
}

//-----------------------------------------------

void GD3DDevice::SwapBuffer()
{
  //Вывод
  pSwapChain->Present(v_sync ? 1 : 0, 0);
}

//-----------------------------------------------

void GD3DDevice::SetPrimitiveTopology(g_uint8 PrimitiveTopology)
{
  D3DDevice->IASetPrimitiveTopology((D3D10_PRIMITIVE_TOPOLOGY)PrimitiveTopology);
}

//-----------------------------------------------

void GD3DDevice::SetWireframe(g_bool Flag)
{
  D3D10_RASTERIZER_DESC rast_desc;
  _raster_state->GetDesc(&rast_desc);
  if (Flag)
    rast_desc.FillMode = D3D10_FILL_WIREFRAME;
  else 
    rast_desc.FillMode = D3D10_FILL_SOLID;
  _raster_state->Release();
  D3DDevice->CreateRasterizerState(&rast_desc, &_raster_state); 
  D3DDevice->RSSetState(_raster_state);
}

//-----------------------------------------------

void GD3DDevice::Release()
{
  pSwapChain->SetFullscreenState(false, NULL);
  if (D3DDevice) D3DDevice->Release();
  if (pSwapChain) pSwapChain->Release();
  if (pStateBlock1) pStateBlock1->Release();
  if (pStateBlock2) pStateBlock2->Release();
  D3DDevice = NULL;
  pSwapChain = NULL;
  pStateBlock1 = NULL;
  pStateBlock2 = NULL;
  BufferList->_device = NULL;
}

//-----------------------------------------------

g_bool GD3DDevice::SetResolution(DXGI_MODE_DESC mode_desc, g_bool Windowed)
{
  DXGI_SWAP_CHAIN_DESC sc_desc;
  D3D10_RASTERIZER_DESC rast_desc;

  pSwapChain->GetDesc(&sc_desc);
  _raster_state->GetDesc(&rast_desc);

  //Release Rasterizer State, Render Target
  D3DDevice->RSSetState(NULL);
  _raster_state->Release();

  D3DDevice->OMSetRenderTargets(0, NULL, NULL);
  _rt_main->Release();

  //Resize
  HRESULT hres = pSwapChain->ResizeBuffers(sc_desc.BufferCount, 
	                        mode_desc.Width,
							mode_desc.Height,
							sc_desc.BufferDesc.Format,
							sc_desc.Flags);
  
  if (!Windowed)
  {
	IDXGIOutput *target;
	BOOL fs;
	pSwapChain->GetFullscreenState(&fs, &target);
	pSwapChain->SetFullscreenState(true, target);
    pSwapChain->ResizeTarget(&mode_desc);
  } 

  if (hres == S_OK)
  {
    _width = mode_desc.Width;
    _height = mode_desc.Height;
  }
    
  //Recreate Rasterizer State, Render Target
  _init_main_rt(_width, _height);

  D3DDevice->CreateRasterizerState( &rast_desc, &_raster_state );
  D3DDevice->RSSetState(_raster_state);

  return (hres == S_OK);
}

//-----------------------------------------------
