#include "GUI_Edit.h"

//-----------------------------------------------

GUI_Edit::GUI_Edit(const g_string &Name, GD3DDevice *Device):GUI_BaseObject(Name,Device)
{
  _init_edit();
}

//-----------------------------------------------

GUI_Edit::GUI_Edit(GModel *Model, GUI_BaseObject *Parent, const g_string &Name, GD3DDevice *Device):
           GUI_BaseObject(Model, Parent, Name, Device)
{
  _init_edit();
}

//-----------------------------------------------

void GUI_Edit::_init_edit()
{
  SetType(G_OT_UI_EDIT);
  Text = "";
  ReadOnly = false;
  Multiline = false;
  _caret_pos = 0;
  _first_char_pos = 1;
  _n_visible_chars = 1;
  _max_length = 40;
  _is_input_field = true;
  _stage = 0;
}

//-----------------------------------------------

GUI_Edit::~GUI_Edit(void)
{

}

//-----------------------------------------------

void GUI_Edit::OnFocusSet() 
{
  SetFocused(true);
}

//-----------------------------------------------

void GUI_Edit::OnFocusLost()
{
  SetFocused(false);
}

//-----------------------------------------------

void GUI_Edit::OnShow() 
{

}

//-----------------------------------------------

void GUI_Edit::OnHide() 
{

}

//-----------------------------------------------

void GUI_Edit::OnKeyDown(g_int32 Key)
{
  if (GetVisible() && GetEnabled() && (!ReadOnly))
  {
    _changed = true;
  }
}

//-----------------------------------------------

void GUI_Edit::OnKeyUp(g_int32 Key)
{
  if (GetVisible() && GetEnabled() && (!ReadOnly))
  {
    _changed = true;
  }
}

//-----------------------------------------------

void GUI_Edit::OnKeyPress(g_int32 Key)
{
  if (GetVisible() && GetEnabled() && (!ReadOnly))
  {
    _changed = true;
  }
}

//-----------------------------------------------

void GUI_Edit::OnChar(g_int32 Key)
{
  if (GetVisible() && GetEnabled() && (!ReadOnly))
  {
    if (Key == VK_BACK)
	  if (Text.size() > _first_char_pos)
	  {
	    Text.erase(Text.size()-1);
		_changed = true;
	  }
	  else
	  {
		if (_first_char_pos > 5)
		  _first_char_pos -= 5;
		if (_first_char_pos > 1)
		  _first_char_pos--;
        if (Text.size() > 0)
		{
		  Text.erase(Text.size()-1);
		  _changed = true;
		}
	  }

    if (Key > 31)
	{
	  if ((Text.size() < _max_length) || (_max_length <= 0))
	  {
	    _changed = true;
		Text = Text + g_char(Key);
  
		g_rect client_area = GetClientArea();
	    Text.SetFont(GetFont());
	    Text.SetTextBoxWidth(client_area.right - client_area.left);

	    do
	    {
	      Text.SS_Analyse(_first_char_pos-1);
		  _n_visible_chars = Text.SS_pcOutChars();
	      if (_n_visible_chars < Text.GetText(_first_char_pos,Text.size()-_first_char_pos+1).size())
		    _first_char_pos = Text.size() - _n_visible_chars + 1;
		  else
		    break;
	    }
	    while (1);
	  }
	}
	//
  }
}

//-----------------------------------------------

void GUI_Edit::OnClick(g_int32 Button, g_int32 X, g_int32 Y)
{

}

//-----------------------------------------------

void GUI_Edit::OnMouseDown(g_int32 Button, g_int32 X, g_int32 Y)
{
  _mousedowned = true;
}

//-----------------------------------------------

void GUI_Edit::OnMouseUp(g_int32 Button, g_int32 X, g_int32 Y)
{
  _mousedowned = false;
}

//-----------------------------------------------

void GUI_Edit::OnMouseLeave(g_int32 X, g_int32 Y)
{
  _changed = true;
  _mouseinrect = false;
  _stage = 0;
}

//-----------------------------------------------

void GUI_Edit::OnMouseEnter(g_int32 X, g_int32 Y)
{
  _changed = true;
  _mouseinrect = true;
  _stage = 1;
}

//-----------------------------------------------

void GUI_Edit::UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite) 
{
  if (_begin_update(mWorld, mViewProj))
  {
    GModel *this_model = GetModel();
	g_ivec2 size = GetSize();

	if (this_model != NULL)
	{
	  _apply_ctrl_set(GUI_EDIT_TEXOFFSET[_stage], GUI_EDIT_TEXSCALE);
	  this_model->Draw(false);
	}

	g_rect TextRect;
	g_rect bord = GetBordersSize();
	SetRect(&TextRect, 0, 0, size.x, size.y);
	ReduceRect(TextRect, bord);
	if (Multiline) 
	  _draw_text(Sprite, TextRect, Text.GetText(), DT_WORDBREAK);
	else 
	  _draw_text(Sprite, TextRect, Text.GetText(_first_char_pos,_n_visible_chars), DT_VCENTER | DT_SINGLELINE);
	_end_update();
  }
}

//-----------------------------------------------

void GUI_Edit::UpdateText()
{
  g_rect client_area = GetClientArea();
  Text.SetFont(GetFont());
  Text.SetTextBoxWidth(client_area.right - client_area.left);

  do
  {
	Text.SS_Analyse(_first_char_pos-1);
	_n_visible_chars = Text.SS_pcOutChars();
	if (_n_visible_chars < Text.GetText(_first_char_pos, Text.size()-_first_char_pos+1).size())
	  _first_char_pos = Text.size() - _n_visible_chars + 1;
	else
	  break;
  }
  while (1);

  _changed = true;
}

//-----------------------------------------------
/*
void GUI_Edit::GetRawSettings(void **pBuffer, g_uint32 &BufSize)
{
  
}

//-----------------------------------------------

void GUI_Edit::SetRawSettings(void *Buffer, g_uint32 BufSize)
{
  
}
*/
//-----------------------------------------------
