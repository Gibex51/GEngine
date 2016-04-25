#include "GUI_Button.h"

//-----------------------------------------------

GUI_Button::GUI_Button(const g_string &Caption, const g_string &Name, GD3DDevice *Device):
            GUI_BaseObject(NULL,NULL,Name,Device)
{
  _init_button(Caption);
}

//-----------------------------------------------

GUI_Button::GUI_Button(GModel *Model, GUI_BaseObject *Parent, const g_string &Caption, 
					   const g_string &Name, GD3DDevice *Device):
            GUI_BaseObject(Model,Parent,Name,Device)
{
  _init_button(Caption);
}

//-----------------------------------------------

GUI_Button::~GUI_Button(void)
{
  
}

//-----------------------------------------------

void GUI_Button::_init_button(const g_string &Caption)
{
  _stage = 0;
  SetCaption(Caption);
  SetType(G_OT_UI_BUTTON);
}

//-----------------------------------------------

void GUI_Button::OnClick(g_int32 Button, g_int32 X, g_int32 Y)
{
  
}

//-----------------------------------------------

void GUI_Button::OnMouseDown(g_int32 Button, g_int32 X, g_int32 Y)
{
  _changed = true;
  _mousedowned = true;
  _stage = 1;
}

//-----------------------------------------------

void GUI_Button::OnMouseUp(g_int32 Button, g_int32 X, g_int32 Y)
{
  _changed = true;
  _mousedowned = false;
  if (_mouseinrect) _stage = 2;
  else _stage = 0;
}

//-----------------------------------------------

void GUI_Button::OnMouseLeave(g_int32 X, g_int32 Y)
{
  _changed = true;
  _mouseinrect = false;
  if (_mousedowned) _stage = 2;
  else _stage = 0;
}

//-----------------------------------------------

void GUI_Button::OnMouseEnter(g_int32 X, g_int32 Y)
{
  _changed = true;
  _mouseinrect = true; 
  if (_mousedowned) _stage = 1;
  else _stage = 2;
}

//-----------------------------------------------

void GUI_Button::UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite) 
{
  if (_begin_update(mWorld, mViewProj))
  {
    GModel *this_model = GetModel();
	g_ivec2 size = GetSize();

	if (this_model != NULL)
	{
	  _apply_ctrl_set(GUI_BUTTON_TEXOFFSET[_stage], GUI_BUTTON_TEXSCALE);
	  this_model->Draw(false);
	}

	g_rect TextRect;
	SetRect(&TextRect, 0, 0, size.x, size.y);
    _draw_text(Sprite, TextRect, GetCaption(), DT_CENTER | DT_VCENTER);

	_end_update();
  } 
}

//-----------------------------------------------
/*
void GUI_Button::GetRawSettings(void **pBuffer, g_uint32 &BufSize)
{
  if (pBuffer != NULL)
  {
	//g_uint32 name_sz = GetName().size();
    //if (name_sz > 256) name_sz = 256;
    //memcpy(UIData.Name, GetName().c_str(), name_sz);
  }
}
*/
//-----------------------------------------------
/*
void GUI_Button::SetRawSettings(void *Buffer, g_uint32 BufSize)
{
  
}
*/
//-----------------------------------------------
