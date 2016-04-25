#include "GUI_TrackBar.h"

//-----------------------------------------------

GUI_TrackBar::GUI_TrackBar(const g_string &Name, GD3DDevice *Device):GUI_BaseObject(Name,Device)
{
  _init_trkbar();
}

//-----------------------------------------------

GUI_TrackBar::GUI_TrackBar(GModel *Model, GUI_BaseObject *Parent, const g_string &Name, GD3DDevice *Device):
              GUI_BaseObject(Model, Parent, Name, Device)
{
  _init_trkbar();
}

//-----------------------------------------------

GUI_TrackBar::~GUI_TrackBar(void)
{

}

//-----------------------------------------------

void GUI_TrackBar::_init_trkbar()
{
  SetType(G_OT_UI_TRACKBAR);

  _position = 0;
  _min = 0;
  _max = 100;

  _thumb_stage = 0;
  _thumb_downed = false;
  SetRect(&_thumb_bord, 0, 0, 0, 0);

  OnSetValue();
}

//-----------------------------------------------

g_float GUI_TrackBar::_get_step(g_int32 width, g_int32 height) const
{
  if (_max - _min > 0)
    return g_float(abs(width - height))/g_float(_max - _min);
  else
	return 0;
}

//-----------------------------------------------

void GUI_TrackBar::SetRange(g_int32 Min, g_int32 Max)
{
  if (Max < Min) Max = Min;
  if ((_min != Min) || (_max != Max)) _changed = true;
  _min = Min;
  _max = Max;
  g_ivec2 size = GetSize();
  _step = _get_step(size.x, size.y);
}

//-----------------------------------------------

void GUI_TrackBar::SetBarPosition(g_int32 Pos)
{
  CheckRange(&Pos,_min,_max);

  if (_position != Pos) _changed = true;
  _position = Pos;
  g_ivec2 size = GetSize(); 
  
  if (size.y > size.x)
    SetRect(&_thumb_rect,0,g_int32(_step*(_position-_min)),size.x,size.x+g_int32(_step*(_position-_min)));
  else
    SetRect(&_thumb_rect,g_int32(_step*(_position-_min)),0,size.y+g_int32(_step*(_position-_min)),size.y);

  if (_changed) OnSetValue();
}

//-----------------------------------------------

void GUI_TrackBar::OnResize()
{
  _changed = true;
  g_ivec2 size = GetSize(); 

  if (size.y > size.x)
  {
	if (size.y < size.x*2) 
	  size.y = size.x*2;
	_set_size(size);
	_step = _get_step(size.x, size.y);	  
	SetRect(&_thumb_rect, 0, g_int32(_step*(_position-_min)), size.x, size.x+g_int32(_step*(_position-_min)));
  }
  else
  {
	if (size.x < size.y*2) 
	  size.x = size.y*2;
	_set_size(size);
	_step = _get_step(size.x, size.y);
	SetRect(&_thumb_rect, g_int32(_step*(_position-_min)), 0, size.y+g_int32(_step*(_position-_min)), size.y);
  }
}

//-----------------------------------------------

void GUI_TrackBar::OnMouseLeave(g_int32 X, g_int32 Y)
{
  _mouseinrect = false;
  _changed = true;

  if (_thumb_downed)
	_thumb_stage = 2;
  else
    _thumb_stage = 0;
}

//-----------------------------------------------

void GUI_TrackBar::OnMouseEnter(g_int32 X, g_int32 Y)
{
  _changed = true;
  _mouseinrect = true;
}

//-----------------------------------------------

void GUI_TrackBar::OnMouseDown(g_int32 Button, g_int32 X, g_int32 Y)
{
  _changed = true;
  _mousedowned = true;
  g_rect abs_metrics = GetAbsMetrics();

  if (PointInRect(_thumb_rect, G_POINT(X - abs_metrics.left, Y - abs_metrics.top)))
	_thumb_downed = true;
}

//-----------------------------------------------

void GUI_TrackBar::OnMouseUp(g_int32 Button, g_int32 X, g_int32 Y)
{
  _changed = true;
  _mousedowned = false;
  _thumb_downed = false;
  g_rect abs_metrics = GetAbsMetrics();

  if (PointInRect(_thumb_rect,G_POINT(X - abs_metrics.left, Y - abs_metrics.top)))
    _thumb_stage = 2;
  else
	_thumb_stage = 0;
}

//-----------------------------------------------

void GUI_TrackBar::OnMouseMove(g_int32 X, g_int32 Y)
{
  g_int32 prev_stage = _thumb_stage;
  g_rect abs_metrics = GetAbsMetrics();
  g_ivec2 size = GetSize();

  if (PointInRect(_thumb_rect,G_POINT(X - abs_metrics.left,Y - abs_metrics.top)))
  {
    _thumb_stage = 2;
  }
  else
  {
	if (_thumb_downed)
	  _thumb_stage = 2;
	else
      _thumb_stage = 0;
  }

  if (_thumb_downed)
  {
	if (size.y > size.x)
	  SetBarPosition(g_int32(g_float(Y - abs_metrics.top - size.x*0.5f)/_step) + _min);
    else
	  SetBarPosition(g_int32(g_float(X - abs_metrics.left - size.y*0.5f)/_step) + _min);
  }

  if (prev_stage != _thumb_stage) _changed = true;
}

//-----------------------------------------------

void GUI_TrackBar::UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite) 
{
  if (_begin_update(mWorld, mViewProj))
  {
	GModel *this_model = GetModel();
	g_ivec2 size = GetSize();
    if (this_model != NULL)
    {
	  GMeshInstance *_minst = this_model->GetInstance(0);
	  /*Draw Thumb Line*/
	  _minst->SetPosition(g_vec3(5,0,0));
	  _minst->SetScaling(g_vec3(g_float(size.x - 10), g_float(size.y), 1));
	  _apply_ctrl_set(GUI_TRKBAR_TEXOFFSET[1], GUI_TRKBAR_TEXSCALE[1]); 
	  this_model->Draw(false);

	  /*Draw Thumb*/  
	  g_rect bord = GetBordersSize();
	  SetBordersSize(_thumb_bord);
	    
      _minst->SetPosition(g_vec3(g_float(_thumb_rect.left),g_float(_thumb_rect.top),0));
	  _minst->SetScaling(g_vec3(g_float(_thumb_rect.right-_thumb_rect.left),
	  	                        g_float(_thumb_rect.bottom-_thumb_rect.top), 1.0f));
	  _apply_ctrl_set(GUI_TRKBAR_TEXOFFSET[_thumb_stage], GUI_TRKBAR_TEXSCALE[0]);
	  this_model->Draw(false);
	  SetBordersSize(bord);
	}

    _end_update();
  }
}

//-----------------------------------------------

void GUI_TrackBar::OnSetValue()
{
  GUI_EVENTPROC_DESC eDesc = {this, G_MSG_ONSETVALUE, 1, {_position}};
  if (_eventcallback != NULL) _eventcallback(&eDesc);
}

//-----------------------------------------------
/*
void GUI_TrackBar::GetRawSettings(void **pBuffer, g_uint32 &BufSize)
{
  
}

//-----------------------------------------------

void GUI_TrackBar::SetRawSettings(void *Buffer, g_uint32 BufSize)
{
  
}
*/
//-----------------------------------------------