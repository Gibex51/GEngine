#include "GUI_ScrollBar.h"

//-----------------------------------------------

GUI_ScrollBar::GUI_ScrollBar(const g_string &Name, GD3DDevice *Device):GUI_BaseObject(Name,Device)
{
  _init_scrbar(16,128);
}

//-----------------------------------------------

GUI_ScrollBar::GUI_ScrollBar(GModel *Model, GUI_BaseObject *Parent, const g_string &Name, GD3DDevice *Device):
                GUI_BaseObject(Model, Parent, Name, Device)
{
  _init_scrbar(16,128);
}

//-----------------------------------------------

GUI_ScrollBar::~GUI_ScrollBar(void)
{

}

//-----------------------------------------------

void GUI_ScrollBar::_init_scrbar(g_int32 Width, g_int32 Height)
{
  SetType(G_OT_UI_SCROLLBAR);

  _position = 0;
  _min = 0;
  _max = 100;

  SetSize(g_ivec2(Width,Height));

  _inc_stage = 0;
  _dec_stage = 0;
  _thumb_stage = 0;

  _inc_downed = false;
  _dec_downed = false;
  _thumb_downed = false;

  OnResize();
}

void GUI_ScrollBar::UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite)   
{
  if (_begin_update(mWorld, mViewProj))
  {
    GModel *this_model = GetModel();
	if (this_model != NULL)
	{
	  GMeshInstance* _minst = this_model->GetInstance(0);
	  g_ivec2 size = GetSize();

	  if (size.x > size.y)
	  {
        _minst->SetLocalCenter(g_vec3(size.y/2.0f,size.y/2.0f,0));

	  	if ((_max - _min) > 0)
	      _step = g_float(size.x - size.y*3)/g_float(_max-_min);
	    else
		  _step = 0;

		SetRect(&_thumb_rect,size.y+g_int32(_step*_position),0,size.y*2+g_int32(_step*_position),size.y);
	  } else {
        _minst->SetLocalCenter(g_vec3(size.x/2.0f,size.x/2.0f,0));

		if ((_max - _min) > 0)
	      _step = g_float(size.y - size.x*3)/g_float(_max-_min);
	    else
		  _step = 0;

		SetRect(&_thumb_rect,0,size.x+g_int32(_step*_position),size.x,size.x*2+g_int32(_step*_position));
	  }

	  // Draw Dec Button
	  _apply_ctrl_set(GUI_SCRBAR_TEXOFFSET[_dec_stage], GUI_SCRBAR_TEXSCALE);

	  _minst->SetPosition(g_vec3(g_float(_dec_rect.left),g_float(_dec_rect.top),0));	  
	  _minst->SetScaling(g_vec3(g_float(_dec_rect.right-_dec_rect.left),
		                        g_float(_dec_rect.bottom-_dec_rect.top),1.0f));
	  this_model->Draw(false);

	  // Draw Dec Arrow
	  _apply_ctrl_set(GUI_SCRBAR_TEXOFFSET[3], GUI_SCRBAR_TEXSCALE);

	  if (size.x > size.y)
	    _minst->RotateLocal(g_vec3(0,0,-90));
	  this_model->Draw(false);
	  _minst->RotateLocal(g_vec3(0,0,0));

	  // Draw Inc Button
	  _apply_ctrl_set(GUI_SCRBAR_TEXOFFSET[_inc_stage], GUI_SCRBAR_TEXSCALE);

      _minst->SetPosition(g_vec3(g_float(_inc_rect.left),g_float(_inc_rect.top),0));	  
	  _minst->SetScaling(g_vec3(g_float(_inc_rect.right-_inc_rect.left),
		                        g_float(_inc_rect.bottom-_inc_rect.top),1.0f));
	  this_model->Draw(false);

	  // Draw Inc Arrow
	  _apply_ctrl_set(GUI_SCRBAR_TEXOFFSET[3], GUI_SCRBAR_TEXSCALE);

	  if (size.x > size.y)
	    _minst->RotateLocal(g_vec3(0,0,90));
	  else
        _minst->RotateLocal(g_vec3(0,0,180));
	  this_model->Draw(false);
	  _minst->RotateLocal(g_vec3(0,0,0));

	  // Draw Thumb
	  _apply_ctrl_set(GUI_SCRBAR_TEXOFFSET[_thumb_stage], GUI_SCRBAR_TEXSCALE);

      _minst->SetPosition(g_vec3(g_float(_thumb_rect.left),g_float(_thumb_rect.top),0));	  
	  _minst->SetScaling(g_vec3(g_float(_thumb_rect.right-_thumb_rect.left),
		                        g_float(_thumb_rect.bottom-_thumb_rect.top),1.0f));
	  this_model->Draw(false);
	}
	_end_update();
  }  
}

//-----------------------------------------------

void GUI_ScrollBar::OnResize()
{
  _changed = true;
  g_ivec2 size = GetSize();
  if (size.y > size.x)
  {
	  if (size.y < size.x*4) 
	    size.y = size.x*4;

	  if ((_max - _min) > 0)
	    _step = g_float(size.y - size.x*3)/g_float(_max-_min);
	  else
		_step = 0;

	  SetRect(&_dec_rect,0,0,size.x,size.x);
	  SetRect(&_inc_rect,0,size.y-size.x,size.x,size.y);
	  SetRect(&_thumb_rect,0,size.x+g_int32(_step*_position),size.x,size.x*2+g_int32(_step*_position));
  }
  else
  {
	  if (size.x < size.y*4) 
	    size.x = size.y*4;

      if ((_max - _min) > 0)
	    _step = g_float(size.x - size.y*3)/g_float(_max-_min);
	  else
		_step = 0;

      SetRect(&_dec_rect,0,0,size.y,size.y);
	  SetRect(&_inc_rect,size.x-size.y,0,size.x,size.y);
	  SetRect(&_thumb_rect,size.y+g_int32(_step*_position),0,size.y*2+g_int32(_step*_position),size.y);
  }
}

//-----------------------------------------------

void GUI_ScrollBar::OnMouseLeave(g_int32 X, g_int32 Y)
{
  _changed = true;
  _mouseinrect = false;

  if (_dec_downed)
	_dec_stage = 2;
  else
    _dec_stage = 0;

  if (_inc_downed)
	_inc_stage = 2;
  else
    _inc_stage = 0;

  if (_thumb_downed)
	_thumb_stage = 2;
  else
    _thumb_stage = 0;
}

//-----------------------------------------------

void GUI_ScrollBar::OnMouseEnter(g_int32 X, g_int32 Y)
{
  _changed = true;
  _mouseinrect = true;
}

//-----------------------------------------------

void GUI_ScrollBar::OnMouseMove(g_int32 X, g_int32 Y)
{
  g_int32 prev_stage = _dec_stage;
  g_int32 prev_pos = _position;
  g_ivec2 pos = GetAbsPosition();
  g_ivec2 size = GetSize();
  
  if (PointInRect(_dec_rect, G_POINT(X-pos.x, Y-pos.y)))
  {
	if (_dec_downed)
	{
      _dec_stage = 1;
      if (GetTickCount()-_lastscrolltime > G_UI_SCROLL_INTERVAL)
	    if (_position > _min)
	    {
          _lastscrolltime = GetTickCount();
	      _position--;
		  if (size.y>size.x)
		    SetRect(&_thumb_rect,0,size.x+g_int32(_step*_position),size.x,size.x*2+g_int32(_step*_position));
		  else
	        SetRect(&_thumb_rect,size.y+g_int32(_step*_position),0,size.y*2+g_int32(_step*_position),size.y);
	    }
	}
	else
      _dec_stage = 2;
  }
  else
  {
	if (_dec_downed)
	  _dec_stage = 2;
	else
      _dec_stage = 0;
  }

  if (prev_stage != _dec_stage) _changed = true;
  prev_stage = _inc_stage;

  if (PointInRect(_inc_rect, G_POINT(X-pos.x, Y-pos.y)))
  {
	if (_inc_downed)
	{
	  _inc_stage = 1;
      if (GetTickCount()-_lastscrolltime > G_UI_SCROLL_INTERVAL)
	    if (_position < _max)
	    {
          _lastscrolltime = GetTickCount();
	      _position++;
	      if (size.y>size.x)
		    SetRect(&_thumb_rect,0,size.x+g_int32(_step*_position),size.x,size.x*2+g_int32(_step*_position));
		  else
	        SetRect(&_thumb_rect,size.y+g_int32(_step*_position),0,size.y*2+g_int32(_step*_position),size.y);
	    }
	}
	else
      _inc_stage = 2;
  }
  else
  {
	if (_inc_downed)
	  _inc_stage = 2;
	else
      _inc_stage = 0;
  }

  if (prev_stage != _inc_stage) _changed = true;
  prev_stage = _thumb_stage;

  if (PointInRect(_thumb_rect, G_POINT(X-pos.x, Y-pos.y)))
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
	if (size.y>size.x)
	{
	  _position = g_int32(g_float(Y - pos.y - size.x*1.5)/_step);

	  CheckRange(&_position,_min,_max);
	  SetRect(&_thumb_rect,0,size.x+g_int32(_step*_position),size.x,size.x*2+g_int32(_step*_position));
	}
    else
	{
	  _position = g_int32(g_float(X - pos.x - size.y*1.5)/_step);

	  CheckRange(&_position,_min,_max);
	  SetRect(&_thumb_rect,size.y+g_int32(_step*_position),0,size.y*2+g_int32(_step*_position),size.y);
	}
  }

  if (prev_stage != _thumb_stage) _changed = true;
  if (prev_pos != _position) _changed = true;
}

//-----------------------------------------------

void GUI_ScrollBar::OnClick(g_int32 Button, g_int32 X, g_int32 Y)
{

}

//-----------------------------------------------

void GUI_ScrollBar::OnMouseDown(g_int32 Button, g_int32 X, g_int32 Y)
{
  _changed = true;
  _mousedowned = true;
  g_ivec2 pos = GetAbsPosition();

  if (PointInRect(_dec_rect, G_POINT(X-pos.x, Y-pos.y)))
  {
	_dec_stage = 1;
	_dec_downed = true;
  }

  if (PointInRect(_inc_rect, G_POINT(X-pos.x, Y-pos.y)))
  {
	_inc_stage = 1;
	_inc_downed = true;
  }

  if (PointInRect(_thumb_rect, G_POINT(X-pos.x, Y-pos.y)))
  {
	_thumb_downed = true;
  }
}

//-----------------------------------------------

void GUI_ScrollBar::OnMouseUp(g_int32 Button, g_int32 X, g_int32 Y)
{
  _changed = true;
  _mousedowned = false;
  g_ivec2 pos = GetAbsPosition();

  _dec_downed = false;
  if (PointInRect(_dec_rect, G_POINT(X-pos.x, Y-pos.y)))
  {
    _dec_stage = 2;
  }
  else
	_dec_stage = 0;

  _inc_downed = false;
  if (PointInRect(_inc_rect, G_POINT(X-pos.x, Y-pos.y)))
  {
    _inc_stage = 2;
  }
  else
	_inc_stage = 0;

  _thumb_downed = false;
  if (PointInRect(_thumb_rect, G_POINT(X-pos.x, Y-pos.y)))
  {
    _thumb_stage = 2;
  }
  else
	_thumb_stage = 0;
}

//-----------------------------------------------

void GUI_ScrollBar::SetRange(g_int32 Min, g_int32 Max)
{ 
  if (Max < Min) Max = Min;
  if ((_min != Min) || (_max != Max)) _changed = true;
  _min = Min;
  _max = Max;
}

//-----------------------------------------------

void GUI_ScrollBar::SetThumbPosition(g_int32 Pos)
{
  if (this == NULL) return;

  CheckRange(&Pos,_min,_max);
  if (_position != Pos) _changed = true;
  _position = Pos;

  g_ivec2 size = GetSize();

  if (size.y>size.x)
	SetRect(&_thumb_rect,0,size.x+g_int32(_step*_position),size.x,size.x*2+g_int32(_step*_position));
  else
	SetRect(&_thumb_rect,size.y+g_int32(_step*_position),0,size.y*2+g_int32(_step*_position),size.y);

  _changed = true;
}

//-----------------------------------------------
/*
void GUI_ScrollBar::GetRawSettings(void **pBuffer, g_uint32 &BufSize)
{
  
}

//-----------------------------------------------

void GUI_ScrollBar::SetRawSettings(void *Buffer, g_uint32 BufSize)
{
  
}
*/
//-----------------------------------------------