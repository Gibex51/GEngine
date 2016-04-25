#include "GUI_RadioButton.h"

//-----------------------------------------------

GUI_RadioButton::GUI_RadioButton(const g_string &Name, GD3DDevice *Device):GUI_BaseObject(Name,Device)
{
  _init_radbtn("");
}

//-----------------------------------------------

GUI_RadioButton::GUI_RadioButton(GModel *Model, GUI_BaseObject *Parent, const g_string &Caption, const g_string &Name, GD3DDevice *Device):
                  GUI_BaseObject(Model,Parent,Name,Device)
{
  _init_radbtn(Caption);
}

//-----------------------------------------------

GUI_RadioButton::~GUI_RadioButton(void)
{

}

//-----------------------------------------------

void GUI_RadioButton::_init_radbtn(const g_string &Caption)
{
  SetType(G_OT_UI_RADIOBUTTON);
  SetCaption(Caption);
  _checked = false;
  _group = 0;
  _stage = 0;
}

//-----------------------------------------------

void GUI_RadioButton::SetCheck(g_bool IsChecked) 
{
  if (_checked != IsChecked) 
	_changed = true; 
  _checked = IsChecked;

  if (_checked) 
  {
	_stage = 2; 
	_uncheck_another();
  }
  else 
    _stage = 0; 
  _changed = true;
};

//-----------------------------------------------

void GUI_RadioButton::_uncheck_another()
{
  if (_parent != NULL)
  {
	const vector <GUI_BaseObject*> childs = _parent->GetChildsList();
    for (g_uint32 i=0; i<childs.size(); i++)
	  if ((childs[i] != this) && (childs[i]->GetType() == G_OT_UI_RADIOBUTTON))
        if (((GUI_RadioButton*)childs[i])->_group == _group)
		  ((GUI_RadioButton*)childs[i])->SetCheck(false);
  }
}

//-----------------------------------------------

void GUI_RadioButton::OnMouseLeave(g_int32 X, g_int32 Y)
{
  _changed = true;
  _mouseinrect = false;
  if (_checked) 
	_stage = 2;
  else
    _stage = 0;
}

//-----------------------------------------------

void GUI_RadioButton::OnMouseEnter(g_int32 X, g_int32 Y)
{
  _changed = true;
  _mouseinrect = true; 
  if (_checked) 
	_stage = 3;
  else
    _stage = 1;
}

//-----------------------------------------------

void GUI_RadioButton::OnMouseDown(g_int32 Button, g_int32 X, g_int32 Y) 
{
  _mousedowned = true;
}

//-----------------------------------------------

void GUI_RadioButton::OnMouseUp(g_int32 Button, g_int32 X, g_int32 Y) 
{
  _mousedowned = false;
}

//-----------------------------------------------

void GUI_RadioButton::OnClick(g_int32 Button, g_int32 X, g_int32 Y) 
{
  if (Button == 1)
  {
	_checked = true;
	_stage = 3; 
	_uncheck_another();
	_changed = true;
  }
}

//-----------------------------------------------

void GUI_RadioButton::UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite)  
{
  if (_begin_update(mWorld, mViewProj))
  {
	GModel *this_model = GetModel();
	g_ivec2 size = GetSize();

	if (this_model != NULL)
	{
	  _apply_ctrl_set(GUI_RADBTN_TEXOFFSET[_stage], GUI_RADBTN_TEXSCALE);
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
void GUI_RadioButton::GetRawSettings(void **pBuffer, g_uint32 &BufSize)
{
  
}

//-----------------------------------------------

void GUI_RadioButton::SetRawSettings(void *Buffer, g_uint32 BufSize)
{
  
}
*/
//-----------------------------------------------