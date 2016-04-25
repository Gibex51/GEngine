#include "GUI_Window.h"

//-----------------------------------------------

GUI_Window::GUI_Window(const g_string &Name, GD3DDevice *Device):GUI_BaseObject(Name,Device)
{
  _init_window("It Is Caption");
}

//-----------------------------------------------

GUI_Window::GUI_Window(GModel *Model, GUI_BaseObject *Parent, const g_string &Caption, const g_string &Name, GD3DDevice *Device):
             GUI_BaseObject(Model,Parent,Name,Device)
{
  _init_window(Caption);
}

//-----------------------------------------------

GUI_Window::~GUI_Window(void)
{

}

//-----------------------------------------------

void GUI_Window::_init_window(string sCaption)
{
  SetType(G_OT_UI_WINDOW);
  SetCaption(sCaption);
  px = 0; py = 0;
  _mouseincaption = false;
}

//-----------------------------------------------

void GUI_Window::UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite)  
{
  if (_begin_update(mWorld, mViewProj))
  {
    GModel *this_model = GetModel();
	g_ivec2 size = GetSize();	

	if (this_model != NULL)
    {
	  _apply_ctrl_set(GUI_WIN_TEXOFFSET, GUI_WIN_TEXSCALE);
	  this_model->Draw(false);
	}

	g_rect TextRect;
	g_rect bord = GetBordersSize();
	SetRect(&TextRect, bord.left, 3, size.x - bord.right, bord.top - 3);
    _draw_text(Sprite, TextRect, GetCaption(), DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    _end_update();
  }
}

//-----------------------------------------------

void GUI_Window::OnMouseDown(g_int32 Button, g_int32 X, g_int32 Y) 
{
  _mousedowned = true;
  _mouseincaption = false;
  g_rect abs_metrics = GetAbsMetrics();
  g_rect bord = GetBordersSize();
  if ((Y > abs_metrics.top) && (Y < abs_metrics.top + bord.top))
    if ((X > abs_metrics.left+bord.left) && (X < abs_metrics.right-bord.right))
	  _mouseincaption = true;
}

//-----------------------------------------------

void GUI_Window::OnMouseUp(g_int32 Button, g_int32 X, g_int32 Y) 
{
  _mousedowned = false;
  _mouseincaption = false;
}

//-----------------------------------------------

void GUI_Window::OnMouseLeave(g_int32 X, g_int32 Y) 
{
  _mouseinrect = false;
}

//-----------------------------------------------

void GUI_Window::OnMouseEnter(g_int32 X, g_int32 Y) 
{
  _mouseinrect = true;
}

//-----------------------------------------------

void GUI_Window::OnMouseMove(g_int32 X, g_int32 Y) 
{
  g_rect abs_metrics = GetAbsMetrics();
  if ((_mousedowned) && (DragEnabled) && (_mouseincaption))
    SetPosition(g_ivec2(abs_metrics.left-(px-X),abs_metrics.top-(py-Y)));
  px = X; py = Y;
}

//-----------------------------------------------
/*
void GUI_Window::GetRawSettings(void **pBuffer, g_uint32 &BufSize)
{
  
}

//-----------------------------------------------

void GUI_Window::SetRawSettings(void *Buffer, g_uint32 BufSize)
{
  
}
*/
//-----------------------------------------------