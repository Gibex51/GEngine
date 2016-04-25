#include "GFont.h"

//-----------------------------------------------

GFont::GFont(const g_string &Name, GD3DDevice *Device):GBaseObject(Name,Device)
{
  _d3dxfont = NULL;
  _created = CreateFont("Arial", 10, g_color(0.0f,0.0f,0.0f,1.0f), FW_BOLD, false);
}

//-----------------------------------------------

GFont::GFont(const g_string &FontName, g_int32 FontSize, g_color FontColor, g_uint32 FontWeight, g_bool Italic, 
             const g_string &Name, GD3DDevice *Device):GBaseObject(Name,Device)
{
  _d3dxfont = NULL;
  _created = CreateFont(FontName, FontSize, FontColor, FontWeight, Italic); 
}
//-----------------------------------------------

GFont::~GFont(void)
{
  ReleaseFont();
}

//-----------------------------------------------

void GFont::ReleaseFont()
{
  if (!_created) return;
  NULLRELEASE(_d3dxfont);
  _created = false;
}

//-----------------------------------------------

g_bool GFont::CreateFont(const g_string &FontName, g_int32 FontSize, g_color _FontColor, g_uint32 FontWeight, g_bool FontItalic)
{
  SetType(G_OT_FONT);

  HDC hDC = GetDC( NULL );
  g_int32 nLogPixelsY = GetDeviceCaps( hDC, LOGPIXELSY );
  ReleaseDC( NULL, hDC );

  g_int32 nHeight = g_int32((-FontSize * nLogPixelsY) / 72.0f);  

  HRESULT hr = D3DX10CreateFont(_device->D3DDevice,           // D3D device
                                nHeight,                      // Height
                                0,                            // Width
                                FontWeight,                   // Weight
                                1,                            // MipLevels, 0 = autogen mipmaps
                                FontItalic,                   // Italic
                                DEFAULT_CHARSET,              // CharSet
                                OUT_DEFAULT_PRECIS,           // OutputPrecision
                                CLEARTYPE_NATURAL_QUALITY,    // Quality
                                DEFAULT_PITCH | FF_DONTCARE,  // PitchAndFamily
                                FontName.c_str(),             // pFaceName
                                &_d3dxfont);                  // ppFont

  if (hr != S_OK)
  {
	_d3dxfont = NULL;
	return false;
  }

  _f_size = FontSize;
  _f_weight = FontWeight;
  _f_italic = FontItalic;
  _f_name = FontName;
  FontColor = _FontColor;

  return true;
}

//-----------------------------------------------

g_bool GFont::GDrawText(const g_string &Text, g_rect *TextRect)
{ 
  if (this == NULL) return false;
  return GDrawText(NULL,Text,TextRect,DT_NOCLIP);
}

//-----------------------------------------------

g_bool GFont::GDrawText(const g_string &Text, g_rect *TextRect, g_uint32 TextFormat)
{
  if (this == NULL) return false;
  return GDrawText(NULL,Text,TextRect,TextFormat);
}

//-----------------------------------------------

g_bool GFont::GDrawText(GSprite *Sprite, const g_string &Text, g_rect *TextRect, g_uint32 TextFormat)
{
  if (this == NULL) return false;
  g_bool Result = false;
  if (_created)
  {
	if (Sprite != NULL)
	  _d3dxfont->DrawTextA(Sprite->_sprite,Text.c_str(),-1,TextRect,TextFormat,FontColor);
	else
	  _d3dxfont->DrawTextA(NULL,Text.c_str(),-1,TextRect,TextFormat,FontColor);
	Result = true;
  }  
  return Result;
}

//-----------------------------------------------

g_int32 GFont::GetFontHeight()
{
  TEXTMETRICA _tm;
  _d3dxfont->GetTextMetricsA(&_tm);
  return _tm.tmHeight;
}

//-----------------------------------------------

void GFont::SetWeight(g_uint32 Weight)
{
  ReleaseFont();
  _created = CreateFont(_f_name, _f_size, FontColor, Weight, _f_italic); 
}

//-----------------------------------------------

void GFont::SetItalic(g_bool IsItalic)
{
  ReleaseFont();
  _created = CreateFont(_f_name, _f_size, FontColor, _f_weight, IsItalic);
}

//-----------------------------------------------

HDC GFont::GetFontDC()
{
  if (_d3dxfont == NULL) return NULL;
  return _d3dxfont->GetDC();
}

//-----------------------------------------------
