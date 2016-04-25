#include "GUI_CheckBox.h"

//-----------------------------------------------

GUI_CheckBox::GUI_CheckBox(const g_string &Name, GD3DDevice *Device):GUI_BaseObject(Name, Device)
{
  _init_chkbox("");
}

//-----------------------------------------------

GUI_CheckBox::GUI_CheckBox(GModel *Model, GUI_BaseObject *Parent, const g_string &Caption, const g_string &Name, GD3DDevice *Device):
               GUI_BaseObject(Model, Parent, Name, Device)
{
  _init_chkbox(Caption);
}

//-----------------------------------------------

GUI_CheckBox::~GUI_CheckBox(void)
{

}

//-----------------------------------------------

void GUI_CheckBox::_init_chkbox(const g_string &Caption)
{
  SetType(G_OT_UI_CHECKBOX);
  SetCaption(Caption);
  _checked = false;
  _stage = 0;
}

//-----------------------------------------------

void GUI_CheckBox::SetCheck(g_bool Is_Checked) 
{
  if (_checked != Is_Checked) 
  {
	_changed = true; 
    if (Is_Checked) _stage = 3; else _stage = 1; 
  }
  _checked = Is_Checked;
};

//-----------------------------------------------

void GUI_CheckBox::OnMouseLeave(g_int32 X, g_int32 Y)
{
  _changed = true;
  _mouseinrect = false;
  if (_checked) 
	_stage = 2;
  else
    _stage = 0;
}

//-----------------------------------------------

void GUI_CheckBox::OnMouseEnter(g_int32 X, g_int32 Y)
{
  _changed = true;
  _mouseinrect = true; 
  if (_checked) 
	_stage = 3;
  else
    _stage = 1;
}

//-----------------------------------------------

void GUI_CheckBox::OnMouseDown(g_int32 Button, g_int32 X, g_int32 Y) 
{
  _mousedowned = true;
}

//-----------------------------------------------

void GUI_CheckBox::OnMouseUp(g_int32 Button, g_int32 X, g_int32 Y) 
{
  _mousedowned = false;
}

//-----------------------------------------------

void GUI_CheckBox::OnClick(g_int32 Button, g_int32 X, g_int32 Y) 
{
  if (Button == 1)
  {
	_checked = !_checked;
	if (_checked) _stage = 3; else _stage = 1; 
	_changed = true;
  }
}

//-----------------------------------------------

void GUI_CheckBox::UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite) 
{
  if (_begin_update(mWorld, mViewProj))
  {
	GModel *this_model = GetModel();
	g_ivec2 size = GetSize();

	if (this_model != NULL)
	{
	  _apply_ctrl_set(GUI_CHKBOX_TEXOFFSET[_stage], GUI_CHKBOX_TEXSCALE);
	  GMeshInstance *_minst = this_model->GetInstance(0);
	  _minst->SetScaling(g_vec3(g_float(size.y), g_float(size.y), 1));
	  this_model->Draw(false);
	}

	g_rect TextRect;
	SetRect(&TextRect, size.y + 4, 0, size.x, size.y);
    _draw_text(Sprite, TextRect, GetCaption(), DT_VCENTER | DT_SINGLELINE);

    _end_update();
  }
}

//-----------------------------------------------
/*
void GUI_CheckBox::GetRawSettings(void **pBuffer, g_uint32 &BufSize)
{
  
}

//-----------------------------------------------

void GUI_CheckBox::SetRawSettings(void *Buffer, g_uint32 BufSize)
{
  
}
*/
//-----------------------------------------------