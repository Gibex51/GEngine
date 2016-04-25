#pragma once

#include "GBaseObject.h"

#ifndef GTEXTURE_H
#define GTEXTURE_H

class GTexture:
	public GBaseObject
{
public:
	ID3D10ShaderResourceView *Texture;
    ID3D10Resource *TextureRes;
    g_bool CreateFromFile(const g_string &FileName);
	g_bool CreateFromFile_S(const g_string &FileName, g_bool CreateSRV);
	g_bool CreateFromTexture2D(ID3D10Texture2D **ppTexture2D, DXGI_FORMAT override_srv_format = DXGI_FORMAT_UNKNOWN);
	g_bool CreateFromTexture1D(ID3D10Texture1D **ppTexture1D, DXGI_FORMAT override_srv_format = DXGI_FORMAT_UNKNOWN);
	g_bool CreateFromBuffer(ID3D10Buffer **ppBuffer, g_uint32 n_elements, DXGI_FORMAT Format);
	g_bool CreateCustomTexture1D(D3D10_TEXTURE1D_DESC &Tex1DDesc, g_bool CreateSRV, DXGI_FORMAT override_srv_format = DXGI_FORMAT_UNKNOWN);	
	g_bool CreateCustomTexture2D(D3D10_TEXTURE2D_DESC &Tex2DDesc, g_bool CreateSRV, DXGI_FORMAT override_srv_format = DXGI_FORMAT_UNKNOWN);
	void EmptyTexture();

	g_bool SaveToFile(const g_string &FileName, D3DX10_IMAGE_FILE_FORMAT Format);

    g_ivec3 GetMetrics() {return _metrics;};

	//Конструктор и деструктор	
	GTexture(const g_string &Name, GD3DDevice *Device);
	GTexture(const g_string &Name, const g_string &FileName, GD3DDevice *Device);
	GTexture(const g_string &Name, const g_string &FileName, g_uint8 Type, g_bool CreateSRV, GD3DDevice *Device);
	~GTexture(void);
private:
    g_ivec3 _metrics;
    void FillInfo();
};

struct GTexItem
{
  GTexture *Texture;
  g_int32 ResID;
  // Конструктор
  inline GTexItem(GTexture *_texture, g_int32 _resid)
  {
    Texture = _texture;
	ResID = _resid;
  };
};

#endif // GTEXTURE_H
