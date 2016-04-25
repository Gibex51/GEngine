#include "GUI_ProgressBar.h"

//-----------------------------------------------

GUI_ProgressBar::GUI_ProgressBar(const g_string &Name, GD3DDevice *Device):GUI_BaseObject(Name,Device)
{
  _init_progbar();
}

//-----------------------------------------------

GUI_ProgressBar::GUI_ProgressBar(GModel *Model, GUI_BaseObject *Parent, const g_string &Name, GD3DDevice *Device):
                  GUI_BaseObject(Model, Parent, Name, Device)
{
  _init_progbar();
}

//-----------------------------------------------

GUI_ProgressBar::~GUI_ProgressBar(void)
{
  
}

//-----------------------------------------------

void GUI_ProgressBar::_init_progbar()
{
  SetType(G_OT_UI_PROGRESSBAR);
  SetRect(&_line_bord, 0, 0, 0, 0);
  _min = 0;
  _max = 100;
  _position = 0;
}

//-----------------------------------------------

void GUI_ProgressBar::SetRange(g_int32 Min, g_int32 Max)
{
  if (Max < Min) Max = Min;
  if ((_min != Min) || (_max != Max)) _changed = true;
  _min = Min;
  _max = Max;
}

//-----------------------------------------------

void GUI_ProgressBar::SetBarPosition(g_int32 Pos)
{
  CheckRange(&Pos,_min,_max);
  if (_position != Pos) _changed = true;
  _position = Pos;
}

//-----------------------------------------------

void GUI_ProgressBar::UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite)  
{
  if (_begin_update(mWorld, mViewProj))
  {
    GModel *this_model = GetModel();
	g_ivec2 size = GetSize();

    if (this_model != NULL)
    {
	  //Draw Border
	  GMeshInstance* _minst = this_model->GetInstance(0);

	  _apply_ctrl_set(GUI_PROGBAR_TEXOFFSET[0], GUI_PROGBAR_TEXSCALE);
	  this_model->Draw(false);

	  //Draw Line
	  g_float _step = 0;
	  g_rect bord = GetBordersSize();
	  SetBordersSize(_line_bord);
	  
	  _set_extra_state(g_ivec2(1,0));
	  _apply_ctrl_set(GUI_PROGBAR_TEXOFFSET[1], GUI_PROGBAR_TEXSCALE);

	  if ((_max - _min) > 0)
	    _step = g_float(size.x - (bord.left + bord.right))/g_float(_max - _min);

	  _minst->SetPosition(g_vec3(g_float(bord.left),g_float(bord.top),0));
	  _minst->SetScaling(g_vec3(g_float(_position)*_step, g_float(size.y - (bord.bottom + bord.top)), 1));

	  this_model->Draw(false);
	  SetBordersSize(bord);

      //Reset Extra State
      _set_extra_state(g_ivec2(0,0));
	}

    _end_update();
  } 
}

//-----------------------------------------------
/*
void GUI_ProgressBar::GetRawSettings(void **pBuffer, g_uint32 &BufSize)
{
  
}

//-----------------------------------------------

void GUI_ProgressBar::SetRawSettings(void *Buffer, g_uint32 BufSize)
{
  
}
*/
//-----------------------------------------------