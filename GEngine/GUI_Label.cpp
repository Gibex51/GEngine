#include "GUI_Label.h"

//-----------------------------------------------

GUI_Label::GUI_Label(const g_string &Name, GD3DDevice *Device):GUI_BaseObject(Name,Device)
{
  _init_label("");
}

//-----------------------------------------------

GUI_Label::GUI_Label(GModel *Model, GUI_BaseObject *Parent, const g_string &Caption, const g_string &Name, 
					 GD3DDevice *Device):GUI_BaseObject(Model, Parent, Name, Device)

{
  _init_label(Caption);
}

//-----------------------------------------------

GUI_Label::~GUI_Label(void)
{

}

//-----------------------------------------------

void GUI_Label::_init_label(const g_string &Caption)
{
  SetCaption(Caption);
  TextFormat = DT_NOCLIP;
  SetType(G_OT_UI_LABEL);
}

//-----------------------------------------------

void GUI_Label::UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite)   
{
  if (_begin_update(mWorld, mViewProj))
  {
	g_rect TextRect;
	g_ivec2 size = GetSize();
	SetRect(&TextRect, 0, 0, size.x, size.y);
    _draw_text(Sprite, TextRect, GetCaption(), TextFormat);

    _end_update();
  }
}

//-----------------------------------------------
/*
void GUI_Label::GetRawSettings(void **pBuffer, g_uint32 &BufSize)
{
  
}

//-----------------------------------------------

void GUI_Label::SetRawSettings(void *Buffer, g_uint32 BufSize)
{
  
}
*/
//-----------------------------------------------
