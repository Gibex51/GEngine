#include "GTexture.h"

//-----------------------------------------------

GTexture::GTexture(const g_string &Name, GD3DDevice *Device):GBaseObject(Name,Device)
{
  Texture = NULL;
  TextureRes = NULL;
  _metrics = g_ivec3(0,0,0);
  SetType(G_OT_TEXTURE);
}

//-----------------------------------------------

GTexture::GTexture(const g_string &Name, const g_string &FileName, GD3DDevice *Device):GBaseObject(Name,Device)
{
  Texture = NULL;
  TextureRes = NULL;
  _metrics = g_ivec3(0,0,0);
  if (!CreateFromFile(FileName)) 
	MessageBox(0,":(","Eror textura",MB_OK); //{P
  SetType(G_OT_TEXTURE);
}

//-----------------------------------------------

GTexture::GTexture(const g_string &Name, const g_string &FileName, g_uint8 Type, g_bool CreateSRV, GD3DDevice *Device):GBaseObject(Name,Device)
{
  Texture = NULL;
  TextureRes = NULL;
  _metrics = g_ivec3(0,0,0);

  switch (Type)
  {
    case 1: if (!CreateFromFile_S(FileName,CreateSRV)) MessageBox(0,":(","Eror textura",MB_OK);
	        break;
	default:if (!CreateFromFile(FileName)) 
			  MessageBox(0,":(","Eror textura",MB_OK); //{P
	        break;
  }
  //{P
  SetType(G_OT_TEXTURE);
}

//-----------------------------------------------

GTexture::~GTexture(void)
{
  EmptyTexture();
}

//-----------------------------------------------

g_bool GTexture::CreateFromFile(const g_string &FileName)
{ 
  if (Texture != NULL) Texture->Release();
  
  D3DX10_IMAGE_LOAD_INFO tex_loadinfo;
  ZeroMemory( &tex_loadinfo, sizeof( D3DX10_IMAGE_LOAD_INFO ) );

  tex_loadinfo.Width = D3DX10_FROM_FILE;
  tex_loadinfo.Height = D3DX10_FROM_FILE;
  tex_loadinfo.Depth = D3DX10_FROM_FILE;
  tex_loadinfo.FirstMipLevel = 0;
  tex_loadinfo.MipLevels = 1;
  tex_loadinfo.Usage = D3D10_USAGE_DEFAULT;
  tex_loadinfo.BindFlags = D3D10_BIND_SHADER_RESOURCE;
  tex_loadinfo.CpuAccessFlags = 0;
  tex_loadinfo.MiscFlags = 0;
  tex_loadinfo.Format = DXGI_FORMAT_FROM_FILE;
  tex_loadinfo.Filter = D3DX10_FILTER_NONE;
  tex_loadinfo.MipFilter = D3DX10_FILTER_NONE;

  if(FAILED(D3DX10CreateShaderResourceViewFromFile(_device->D3DDevice, FileName.c_str(), &tex_loadinfo, 
	                                               NULL, &Texture, NULL)))
  {
    Texture = NULL;
	return false;
  }

  FillInfo();
  return true;
}

//-----------------------------------------------

g_bool GTexture::CreateFromFile_S(const g_string &FileName, g_bool CreateSRV)
{ 
  if (TextureRes != NULL) TextureRes->Release();
  
  D3DX10_IMAGE_LOAD_INFO tex_loadinfo;
  ZeroMemory( &tex_loadinfo, sizeof( D3DX10_IMAGE_LOAD_INFO ) );

  tex_loadinfo.Width = D3DX10_FROM_FILE;
  tex_loadinfo.Height = D3DX10_FROM_FILE;
  tex_loadinfo.Depth = D3DX10_FROM_FILE;
  tex_loadinfo.FirstMipLevel = 0;
  tex_loadinfo.MipLevels = D3DX10_FROM_FILE;
  tex_loadinfo.Usage = D3D10_USAGE_STAGING;
  tex_loadinfo.BindFlags = 0;
  tex_loadinfo.CpuAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
  tex_loadinfo.MiscFlags = 0;
  tex_loadinfo.Format = DXGI_FORMAT_FROM_FILE;
  tex_loadinfo.Filter = D3DX10_FILTER_NONE;
  tex_loadinfo.MipFilter = D3DX10_FILTER_NONE;

  HRESULT hr = D3DX10CreateTextureFromFile(_device->D3DDevice, FileName.c_str(), &tex_loadinfo, 
	                                       NULL, &TextureRes, NULL);
  if(FAILED(hr))
  {
    TextureRes = NULL;
	return false;
  }

  FillInfo();

  if (CreateSRV)
    return CreateFromFile(FileName);
  
  return true;
}

//-----------------------------------------------

g_bool GTexture::CreateFromTexture2D(ID3D10Texture2D **ppTexture2D, DXGI_FORMAT override_srv_format)
{ 
  D3D10_TEXTURE2D_DESC Tex2Ddesc;
  (*ppTexture2D)->GetDesc(&Tex2Ddesc);

  D3D10_SHADER_RESOURCE_VIEW_DESC descviewSR;
  ZeroMemory( &descviewSR, sizeof(descviewSR) );
  if (override_srv_format != DXGI_FORMAT_UNKNOWN)
    descviewSR.Format = override_srv_format;
  else
    descviewSR.Format = Tex2Ddesc.Format;
  descviewSR.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
  descviewSR.Texture2D.MipLevels = Tex2Ddesc.MipLevels;
  descviewSR.Texture2D.MostDetailedMip = 0;

  if (Texture != NULL) Texture->Release();
  
  if(FAILED(_device->D3DDevice->CreateShaderResourceView(*ppTexture2D,&descviewSR,&Texture)))
  {
    Texture = NULL;
	return false;
  }

  FillInfo();
  return true;
}

//-----------------------------------------------

g_bool GTexture::CreateCustomTexture2D(D3D10_TEXTURE2D_DESC &Tex2DDesc, g_bool CreateSRV, DXGI_FORMAT override_srv_format)
{
  ID3D10Texture2D *NewTexture;
  if(FAILED(_device->D3DDevice->CreateTexture2D(&Tex2DDesc, NULL, &NewTexture)))
    return false;
  
  if (TextureRes != NULL) TextureRes->Release();
  TextureRes = NewTexture;

  if (CreateSRV)
  {
    D3D10_SHADER_RESOURCE_VIEW_DESC descviewSR;
    ZeroMemory( &descviewSR, sizeof(descviewSR) );
    if (override_srv_format != DXGI_FORMAT_UNKNOWN)
      descviewSR.Format = override_srv_format;
    else
      descviewSR.Format = Tex2DDesc.Format;
    descviewSR.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    descviewSR.Texture2D.MipLevels = Tex2DDesc.MipLevels;
    descviewSR.Texture2D.MostDetailedMip = 0;

	if (Texture != NULL) Texture->Release();
  
    if(FAILED(_device->D3DDevice->CreateShaderResourceView(NewTexture, &descviewSR, &Texture)))
      Texture = NULL;  
  }

  FillInfo();

  return true;
}

//-----------------------------------------------

g_bool GTexture::CreateCustomTexture1D(D3D10_TEXTURE1D_DESC &Tex1DDesc, g_bool CreateSRV, DXGI_FORMAT override_srv_format)
{
  ID3D10Texture1D *NewTexture;
  if(FAILED(_device->D3DDevice->CreateTexture1D(&Tex1DDesc, NULL, &NewTexture)))
    return false;
  
  if (TextureRes != NULL) TextureRes->Release();
  TextureRes = NewTexture;

  if (CreateSRV)
  {
    D3D10_SHADER_RESOURCE_VIEW_DESC descviewSR;
    ZeroMemory( &descviewSR, sizeof(descviewSR) );
    if (override_srv_format != DXGI_FORMAT_UNKNOWN)
      descviewSR.Format = override_srv_format;
    else
      descviewSR.Format = Tex1DDesc.Format;
    descviewSR.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE1D;
    descviewSR.Texture1D.MipLevels = Tex1DDesc.MipLevels;
    descviewSR.Texture1D.MostDetailedMip = 0;

	if (Texture != NULL) Texture->Release();
  
    if(FAILED(_device->D3DDevice->CreateShaderResourceView(NewTexture, &descviewSR, &Texture)))
      Texture = NULL;  
  }

  FillInfo();

  return true;
}

//-----------------------------------------------

g_bool GTexture::CreateFromTexture1D(ID3D10Texture1D **ppTexture1D, DXGI_FORMAT override_srv_format)
{ 
  D3D10_TEXTURE1D_DESC Tex1Ddesc;
  (*ppTexture1D)->GetDesc(&Tex1Ddesc);

  D3D10_SHADER_RESOURCE_VIEW_DESC descviewSR;
  ZeroMemory( &descviewSR, sizeof(descviewSR) ); 
  if (override_srv_format != DXGI_FORMAT_UNKNOWN)
    descviewSR.Format = override_srv_format;
  else
    descviewSR.Format = Tex1Ddesc.Format;
  descviewSR.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE1D;
  descviewSR.Texture1D.MipLevels = Tex1Ddesc.MipLevels;
  descviewSR.Texture1D.MostDetailedMip = 0;

  if (Texture != NULL) Texture->Release();
  
  if(FAILED(_device->D3DDevice->CreateShaderResourceView(*ppTexture1D,&descviewSR,&Texture)))
  {
    Texture = NULL;
	return false;
  }

  FillInfo();
  return true;
}
//-----------------------------------------------

g_bool GTexture::CreateFromBuffer(ID3D10Buffer **ppBuffer, g_uint32 n_elements, DXGI_FORMAT Format)
{
  D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
  ZeroMemory( &SRVDesc, sizeof( SRVDesc ) );
  SRVDesc.Format = Format;
  SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_BUFFER;
  SRVDesc.Buffer.FirstElement = 0;
  SRVDesc.Buffer.NumElements = n_elements;

  if (Texture != NULL) Texture->Release();

  if(FAILED(_device->D3DDevice->CreateShaderResourceView(*ppBuffer, &SRVDesc, &Texture)))
  {
    Texture = NULL;
	return false;
  }

  FillInfo();
  return true;
}

//-----------------------------------------------

void GTexture::EmptyTexture()
{
  NULLRELEASE(TextureRes);
  NULLRELEASE(Texture);
}

//-----------------------------------------------

g_bool GTexture::SaveToFile(const g_string &FileName, D3DX10_IMAGE_FILE_FORMAT Format)
{
  if (Texture == NULL) 
	return false;

  ID3D10Resource *TRes = NULL;
  Texture->GetResource(&TRes);

  if (TRes == NULL)
	return false;

  HRESULT hres = D3DX10SaveTextureToFile(TRes, Format, FileName.c_str());

  return (hres == S_OK);
}

//-----------------------------------------------

void GTexture::FillInfo()
{
  if (TextureRes == NULL)
    if (Texture != NULL)
      Texture->GetResource(&TextureRes);
    else
      return;

  if (TextureRes != NULL)
  {
    _metrics = g_ivec3(0,0,0);

    D3D10_RESOURCE_DIMENSION trType;
    TextureRes->GetType(&trType);

    switch (trType)
    {
      case D3D10_RESOURCE_DIMENSION_BUFFER:
            {
              D3D10_BUFFER_DESC Buffdesc;
              ((ID3D10Buffer*)TextureRes)->GetDesc(&Buffdesc);
              _metrics = g_ivec3(Buffdesc.ByteWidth,0,0);
            }
            break;
      case D3D10_RESOURCE_DIMENSION_TEXTURE1D:
            {
              D3D10_TEXTURE1D_DESC Tex1Ddesc;
              ((ID3D10Texture1D*)TextureRes)->GetDesc(&Tex1Ddesc);
              _metrics = g_ivec3(Tex1Ddesc.Width,0,0);
            }
            break;
      case D3D10_RESOURCE_DIMENSION_TEXTURE2D:
            {
              D3D10_TEXTURE2D_DESC Tex2Ddesc;
              ((ID3D10Texture2D*)TextureRes)->GetDesc(&Tex2Ddesc);
              _metrics = g_ivec3(Tex2Ddesc.Width,Tex2Ddesc.Height,0);
            }
            break;
      case D3D10_RESOURCE_DIMENSION_TEXTURE3D:
            {
              D3D10_TEXTURE3D_DESC Tex3Ddesc;
              ((ID3D10Texture3D*)TextureRes)->GetDesc(&Tex3Ddesc);
              _metrics = g_ivec3(Tex3Ddesc.Width,Tex3Ddesc.Height,Tex3Ddesc.Depth);
            }
            break;
    }
  }
}