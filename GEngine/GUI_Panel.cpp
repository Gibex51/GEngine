#include "GUI_Panel.h"

//-----------------------------------------------

GUI_Panel::GUI_Panel(const g_string &Name, GD3DDevice *Device):GUI_BaseObject(Name,Device)
{
  _init_panel();
}

//-----------------------------------------------

GUI_Panel::GUI_Panel(GModel *Model, GUI_BaseObject *Parent, const g_string &Name, GD3DDevice *Device):
            GUI_BaseObject(Model,Parent,Name,Device)
{
  _init_panel();
}

//-----------------------------------------------

GUI_Panel::~GUI_Panel(void)
{

}

//-----------------------------------------------

void GUI_Panel::_init_panel()
{
  SetType(G_OT_UI_PANEL);
}

//-----------------------------------------------

void GUI_Panel::UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite)  
{
  if (_begin_update(mWorld, mViewProj))
  {
    GModel *this_model = GetModel();
	g_ivec2 size = GetSize();	

	if (this_model != NULL)
    {
	  _apply_ctrl_set(GUI_PANEL_TEXOFFSET, GUI_PANEL_TEXSCALE);
	  this_model->Draw(false);
	}

    _end_update();
  }
}

//-----------------------------------------------
/*
void GUI_Panel::GetRawSettings(void **pBuffer, g_uint32 &BufSize)
{
  
}

//-----------------------------------------------

void GUI_Panel::SetRawSettings(void *Buffer, g_uint32 BufSize)
{
  
}
*/
//-----------------------------------------------