#include "GUI_ScrollList.h"

//-----------------------------------------------

GUI_ScrollList::GUI_ScrollList(const g_string &Name, GD3DDevice *Device):GUI_BaseObject(Name,Device)
{
  _init_scrlist();
}

//-----------------------------------------------

GUI_ScrollList::GUI_ScrollList(GModel *Model, GUI_BaseObject *Parent, const g_string &Name, GD3DDevice *Device):
                 GUI_BaseObject(Model,Parent,Name,Device)
{
  _init_scrlist();
}

//-----------------------------------------------

GUI_ScrollList::~GUI_ScrollList(void)
{

}

//-----------------------------------------------

void GUI_ScrollList::_init_scrlist()
{
  SetType(G_OT_UI_SCROLLLIST);

  _index = 0;

  _inc_stage = 0;
  _dec_stage = 0;

  _inc_downed = false;
  _dec_downed = false;

  Loop = true;
}

//-----------------------------------------------

void GUI_ScrollList::UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite)  
{
  if (_begin_update(mWorld, mViewProj))
  {
	GModel *this_model = GetModel();
	g_ivec2 size = GetSize();

	if (this_model != NULL)
    {
	  GMeshInstance* _minst = this_model->GetInstance(0);

	  _minst->SetLocalCenter(g_vec3(size.y/2.0f, size.y/2.0f,0));

	  /*Draw Dec Button*/
	  _apply_ctrl_set(GUI_SCRLIST_TEXOFFSET[_dec_stage], GUI_SCRLIST_TEXSCALE);
	  
      _minst->SetPosition(g_vec3(g_float(_dec_rect.left),g_float(_dec_rect.top),0));	  
	  _minst->SetScaling(g_vec3(g_float(_dec_rect.right-_dec_rect.left),
		                          g_float(_dec_rect.bottom-_dec_rect.top),1.0f));
	  this_model->Draw(false);

	  //Draw arrow
      _apply_ctrl_set(GUI_SCRLIST_TEXOFFSET[3], GUI_SCRLIST_TEXSCALE);

	  if (size.x > size.y)
        _minst->RotateLocal(g_vec3(0,0,-90));

      this_model->Draw(false);	  
	  _minst->RotateLocal(g_vec3(0,0,0));

	  /*Draw Inc Button*/
	  _apply_ctrl_set(GUI_SCRLIST_TEXOFFSET[_inc_stage], GUI_SCRLIST_TEXSCALE);

	  _minst->SetPosition(g_vec3(g_float(_inc_rect.left),g_float(_inc_rect.top),0)); 
	  _minst->SetScaling(g_vec3(g_float(_inc_rect.right-_inc_rect.left),
		                          g_float(_inc_rect.bottom-_inc_rect.top),1.0f));

	  this_model->Draw(false);

	  //Draw arrow
	  _apply_ctrl_set(GUI_SCRLIST_TEXOFFSET[3], GUI_SCRLIST_TEXSCALE);

	  if (size.x > size.y)
        _minst->RotateLocal(g_vec3(0,0,90));
	  else
	    _minst->RotateLocal(g_vec3(0,0,180));

	  this_model->Draw(false);
	}

	if (Items.size() > 0)
	{
	  g_rect TextRect;
	  SetRect(&TextRect, size.y + 2, 0, size.x - size.y - 2, size.y);
      _draw_text(Sprite, TextRect, Items[_index], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

    _end_update();
  }
}

//-----------------------------------------------

void GUI_ScrollList::SetSelected(g_uint16 index)
{
  if (index != _index)
	OnSelectItem(index);
}

//-----------------------------------------------

g_int32 GUI_ScrollList::GetSelected() 
{
  if (Items.size() == 0)
	return -1;
  if (_index >= Items.size())
	_index = 0;
  return _index;
}

//-----------------------------------------------

void GUI_ScrollList::OnResize() 
{
  _changed = true;
  g_ivec2 size = GetSize();

  if (size.x < size.y*4) 
  {
	size.x = size.y*4;
	_set_size(size);
  }
  
  SetRect(&_dec_rect, 0, 0, size.y, size.y);
  SetRect(&_inc_rect, size.x-size.y, 0, size.x, size.y);
}

//-----------------------------------------------

void GUI_ScrollList::OnMouseLeave(g_int32 X, g_int32 Y)
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
}

//-----------------------------------------------

void GUI_ScrollList::OnMouseEnter(g_int32 X, g_int32 Y)
{
  _changed = true;
  _mouseinrect = true;
}

//-----------------------------------------------

void GUI_ScrollList::OnMouseMove(g_int32 X, g_int32 Y)
{
  g_int32 prev_stage = _dec_stage;
  g_rect abs_metrics = GetAbsMetrics();
  
  if (PointInRect(_dec_rect,G_POINT(X - abs_metrics.left, Y - abs_metrics.top)))
  {
	if (_dec_downed)
	  _dec_stage = 1;
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

  if (PointInRect(_inc_rect,G_POINT(X - abs_metrics.left, Y - abs_metrics.top)))
  {
	if (_inc_downed)
	  _inc_stage = 1;
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
}

//-----------------------------------------------

void GUI_ScrollList::OnMouseDown(g_int32 Button, g_int32 X, g_int32 Y)
{
  _changed = true;
  _mousedowned = true;
  g_rect abs_metrics = GetAbsMetrics();

  if (PointInRect(_dec_rect,G_POINT(X - abs_metrics.left, Y - abs_metrics.top)))
  {
	_dec_stage = 1;
	_dec_downed = true;
  }

  if (PointInRect(_inc_rect,G_POINT(X - abs_metrics.left, Y - abs_metrics.top)))
  {
	_inc_stage = 1;
	_inc_downed = true;
  }
}

//-----------------------------------------------

void GUI_ScrollList::OnMouseUp(g_int32 Button, g_int32 X, g_int32 Y)
{
  _mousedowned = false;
  g_uint16 old_index = _index;
  g_rect abs_metrics = GetAbsMetrics();

  _dec_downed = false;
  if (PointInRect(_dec_rect,G_POINT(X - abs_metrics.left, Y - abs_metrics.top)))
  {
    _dec_stage = 2;
	if (_index > 0) 
	{
	  _index--;
	}
	else
	  if (Loop)
		_index = Items.size()-1;
  }
  else
	_dec_stage = 0;

  _inc_downed = false;
  if (PointInRect(_inc_rect,G_POINT(X - abs_metrics.left,Y - abs_metrics.top)))
  {
    _inc_stage = 2;
	if (_index < (Items.size()-1)) 
	{
	  _index++;
	}
	else
	  if (Loop)
		_index = 0;
  }
  else
	_inc_stage = 0;

  if (old_index != _index)
	OnSelectItem(_index);
}

//-----------------------------------------------

void GUI_ScrollList::OnSelectItem(g_int32 ItemIndex) 
{
  if ((ItemIndex>-1) && (ItemIndex <  g_int32(Items.size())))
  {    
    _index = ItemIndex;
    _changed = true;
	GUI_EVENTPROC_DESC eDesc = {this, G_MSG_ONSELECTITEM, 1, {ItemIndex}};
	if (_eventcallback != NULL) _eventcallback(&eDesc);
  }
  else
    _index = 0;
}

//-----------------------------------------------

void GUI_ScrollList::ClearItems()
{
  Items.clear();
  _index = 0;
  _changed = true;
}

//-----------------------------------------------
/*
void GUI_ScrollList::GetRawSettings(void **pBuffer, g_uint32 &BufSize)
{
  
}

//-----------------------------------------------

void GUI_ScrollList::SetRawSettings(void *Buffer, g_uint32 BufSize)
{
  
}
*/
//-----------------------------------------------