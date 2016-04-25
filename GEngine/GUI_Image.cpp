#include "GUI_Image.h"

//-----------------------------------------------

GUI_Image::GUI_Image(const g_string &Name, GD3DDevice *Device):GUI_BaseObject(Name, Device)
{
  _init_image("");
}

//-----------------------------------------------

GUI_Image::GUI_Image(GModel *Model, GUI_BaseObject *Parent, const g_string &FileName, const g_string &Name, 
                       GD3DDevice *Device):GUI_BaseObject(Model, Parent, Name, Device)

{
  _init_image(FileName);
}

//-----------------------------------------------

GUI_Image::~GUI_Image(void)
{
  EmptyImage();
}

//-----------------------------------------------

void GUI_Image::_init_image(const g_string &filename)
{
  px = 0; py = 0;
  _image = NULL;
  LoadFromFile(filename);
  SetType(G_OT_UI_IMAGE);
}

//-----------------------------------------------

void GUI_Image::OnMouseMove(g_int32 X, g_int32 Y)
{
  g_rect abs_metrics = GetAbsMetrics();
  if ((_mousedowned) && (DragEnabled))
	SetPosition(g_ivec2(abs_metrics.left - (px - X), abs_metrics.top - (py - Y)));
  px = X; py = Y;
}

//-----------------------------------------------

void GUI_Image::OnMouseDown(g_int32 Button, g_int32 X, g_int32 Y) 
{
  if ((Button & 2) == 2)
    _mousedowned = true;
}

//-----------------------------------------------

void GUI_Image::OnMouseUp(g_int32 Button, g_int32 X, g_int32 Y) 
{
  if ((Button & 2) == 2)
    _mousedowned = false;
}

//-----------------------------------------------

g_bool GUI_Image::LoadFromFile(const g_string &FileName)
{
  if (FileExists(FileName.c_str()))
  {
    EmptyImage();
    _image = new GTexture("IMG",FileName.c_str(),_device);
    _filename = FileName;
	_set_custom_rtt(_image);
	return true;
  }
  return false;
}

//-----------------------------------------------

void GUI_Image::SetFromTexture(GTexture **Texture)
{
  EmptyImage();
  _image = *Texture;
  _filename = "tex";
  _set_custom_rtt(_image);
}

//-----------------------------------------------

void GUI_Image::EmptyImage()
{
  _set_custom_rtt(NULL);
  if (_filename == "tex")
    _image = NULL;
  FREENULL_VAR(_image);	 
}

//-----------------------------------------------

void GUI_Image::UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite)  
{

}

//-----------------------------------------------
/*
void GUI_Image::GetRawSettings(void **pBuffer, g_uint32 &BufSize)
{
  
}

//-----------------------------------------------

void GUI_Image::SetRawSettings(void *Buffer, g_uint32 BufSize)
{
  
}
*/
//-----------------------------------------------
