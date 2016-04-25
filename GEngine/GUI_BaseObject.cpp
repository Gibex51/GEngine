#include "GUI_BaseObject.h"

//-----------------------------------------------

GUI_BaseObject::GUI_BaseObject(GModel *Model, GUI_BaseObject *Parent, const g_string &Name, GD3DDevice *Device):
	            GBaseObject(Name, Device)
{
  _init(Model, Parent);
}

//-----------------------------------------------

GUI_BaseObject::GUI_BaseObject(const g_string &Name, GD3DDevice *Device):GBaseObject(Name, Device)
{
  _init(NULL, NULL);
}

//-----------------------------------------------

GUI_BaseObject::~GUI_BaseObject(void)
{
  for (g_uint32 i=0; i<_childs.size();i++)
	delete _childs[i];
  _childs.clear();

  FREENULL_VAR(_rtt_control);
}

//-----------------------------------------------

void GUI_BaseObject::_init(GModel *Model, GUI_BaseObject *Parent)
{
  DragEnabled = false;
  ShowHint = false;

  _is_input_field = false;
  _is_focusable = true;
  _is_resizeable = false;

  _z_order = 0;
  _changed = true;
  _last_change_time = 0;

  _parent = NULL;
  SetParent(Parent);

  _caption = "";
  _hint = "";
  SetRect(&_borders, 0, 0, 0, 0); 
  SetRect(&_local_metrics, 0, 0, 0, 0);
  _anchors_set = g_anchors_set(g_anst_low, g_anst_low);  
  _center_aspect = g_vec2(0,0);
  _scale_factor = 1.0f;
  _alpha_level = 1.0f;
  _enabled = true;
  _visible = true;
  _focused = false;
  _z_order_level = G_UI_DEF_ZORD_LEVEL;

  _mousedowned = false;
  _mouseinrect = false;

  _tex_index = 0;
  _tex_name = "";
  _tex_sprite_scale = g_vec2(1,1);

  _font = NULL;
  SetModel(Model);
  _rtt_control = new GTexture("_rtt", _device);
  _custom_rtt = NULL;

  SetRect(&_prev_metrics, 0, 0, 0, 0);
  D3DXMatrixScaling(&_sprite_mscaling, 1, 1, 1);
  D3DXMatrixTranslation(&_sprite_mtrans, 1, 1, 0.1f);

  _control_sprite.pTexture = NULL;
  _control_sprite.TexSize = g_vec2(1,1);
  _control_sprite.TextureIndex = 0;
  _control_sprite.TexCoord = g_vec2(0,0);
  _control_sprite.ColorModulate = g_color(1.0f, 1.0f, 1.0f, 1.0f);
  _control_sprite.matWorld = _sprite_mscaling * _sprite_mtrans;

  _eventcallback = NULL;
}

//-----------------------------------------------

void GUI_BaseObject::_get_low_aligned_metrics(g_rect &metrics) const
{
  g_int32 width = _device->GetWidth();
  g_int32 height = _device->GetHeight();

  if (_parent != NULL)
  {
	g_rect pca = _parent->GetClientArea();
	width = pca.right - pca.left;
	height = pca.bottom - pca.top;
  }

  g_ivec2 x_metr = _get_low_aligned_pos(_anchors_set.x, width, _local_metrics.left, 
	                                    _local_metrics.right, _center_aspect.x);

  g_ivec2 y_metr = _get_low_aligned_pos(_anchors_set.y, height, _local_metrics.top, 
	                                    _local_metrics.bottom, _center_aspect.y);

  SetRect(&metrics, x_metr.x, y_metr.x, x_metr.y, y_metr.y);
}

//-----------------------------------------------

g_ivec2 GUI_BaseObject::_get_low_aligned_pos(g_anchors_state curr_as, g_int32 area_size,
				                             g_long lo_pos, g_long hi_pos, g_float center_aspect) const
{
  g_ivec2 low_aligned_pos = g_ivec2(lo_pos, hi_pos);

  switch (curr_as)
  {
	case g_anst_high: 
		low_aligned_pos.x = area_size - lo_pos;
    case g_anst_both: 
		low_aligned_pos.y = area_size - hi_pos;
		break;
	case g_anst_center: 
		g_int32 center_pos = g_int32(center_aspect * g_float(area_size));
		low_aligned_pos.x = center_pos - lo_pos;
	    low_aligned_pos.y = center_pos + hi_pos;
		break;
  }

  if (low_aligned_pos.x > low_aligned_pos.y) 
  {
	g_int32 center = low_aligned_pos.x + (low_aligned_pos.x - low_aligned_pos.y)/2;
    low_aligned_pos = g_ivec2(center, center);
  }

  return low_aligned_pos;
}

//-----------------------------------------------

void GUI_BaseObject::_set_from_low_aligned_pos(g_ivec2 curr_la_pos, g_anchors_state curr_as, g_int32 area_size,
				                               g_long &lo_pos, g_long &hi_pos, g_float &center_aspect) const
{
  lo_pos = curr_la_pos.x;
  hi_pos = curr_la_pos.y;
  switch (curr_as)
  {
	case g_anst_high: 
		lo_pos = area_size - curr_la_pos.x;	  
	case g_anst_both: 		
        hi_pos = area_size - curr_la_pos.y;
		break;
	case g_anst_center: 
		{
		  g_int32 center_pos = curr_la_pos.x + (curr_la_pos.y - curr_la_pos.x)/2;
	      lo_pos = center_pos - curr_la_pos.x;
		  hi_pos = curr_la_pos.y - center_pos;
		  center_aspect = g_float(center_pos)/g_float(area_size);
		}
		break;
  }
}

//-----------------------------------------------

void GUI_BaseObject::_set_from_low_aligned_metrics(const g_rect &la_metrics, const g_anchors_set &anchors, g_rect &dest_metrics, g_vec2 &dest_center_aspect) const
{
  g_int32 width = _device->GetWidth();
  g_int32 height = _device->GetHeight();

  if (_parent != NULL)
  {
	g_rect pca = _parent->GetClientArea();
	width = pca.right - pca.left;
	height = pca.bottom - pca.top;
  }

  _set_from_low_aligned_pos(g_ivec2(la_metrics.left, la_metrics.right), anchors.x, width, 
	                        dest_metrics.left, dest_metrics.right, dest_center_aspect.x);
  _set_from_low_aligned_pos(g_ivec2(la_metrics.top, la_metrics.bottom), anchors.y, height, 
	                        dest_metrics.top, dest_metrics.bottom, dest_center_aspect.y);
}

//-----------------------------------------------

void GUI_BaseObject::_change_anch(g_anchors_state &curr_as, const g_anchors_state &new_as, g_int32 area_size,
				                  g_long &lo_pos, g_long &hi_pos, g_float &center_aspect)
{
  if (curr_as == new_as) return;
  g_ivec2 low_aligned_pos = _get_low_aligned_pos(curr_as, area_size, lo_pos, hi_pos, center_aspect);
  _set_from_low_aligned_pos(low_aligned_pos, new_as, area_size, lo_pos, hi_pos, center_aspect);
  curr_as = new_as;
}

//-----------------------------------------------

g_bool GUI_BaseObject::_begin_update(const g_mat4x4 &mWorld, const g_mat4x4 &mViewProj)
{
  if (!_check_upd_interval()) 
	return false;

  g_ivec2 obj_size = GetSize();

  D3D10_TEXTURE2D_DESC descRTT = DEF_DESC;
  descRTT.Width = obj_size.x;
  descRTT.Height = obj_size.y;
  descRTT.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;

  _rtt_control->CreateCustomTexture2D(descRTT, true); 

  GRenderTarget *_rt_ui = _device->RTList->GetRenderTarget("rt_ui");
  if (_rt_ui == NULL)
    return false;

  if (!_rt_ui->SetRenderToTexture(obj_size.x, obj_size.y, (ID3D10Texture2D*)_rtt_control->TextureRes))
	return false;

  _rt_ui->Set();
  _rt_ui->Clear(true, false, false, g_color(0.0f,0.0f,0.0f,0.0f));

  if (_model != NULL)
  {
	char tmpTexName[G_MAX_NAME_LEN];
    sprintf_s(tmpTexName, _tex_name.c_str(), _tex_index);

	_model->SetWorldMatrix(mWorld);
	_model->SetViewProjMatrix(mViewProj);
    _model->GetMaterial()->ApplyTexture(tmpTexName);

	GMeshInstance* _minst = _model->GetInstance(0);
	g_ivec2 size = GetSize();
	//Reset Transform
    _minst->SetPosition(g_vec3(0,0,0));
	_minst->SetScaling(g_vec3(g_float(size.x), g_float(size.y), 1));
	_minst->RotateLocal(g_vec3(0,0,0));
  }

  return true;
}

//-----------------------------------------------

void GUI_BaseObject::_end_update()
{
  GRenderTarget *_rt_main = _device->RTList->GetRenderTarget("rt_main");
  if (_rt_main != NULL) _rt_main->Set();

  _changed = false;
  _last_change_time = GetTickCount();
}

//-----------------------------------------------

void GUI_BaseObject::_apply_ctrl_set(g_vec2 TexOffset, g_vec2 TexScale)
{
  if (_model == NULL) return;

  char tmpTexName[G_MAX_NAME_LEN];
  sprintf_s(tmpTexName, _tex_name.c_str(), _tex_index);

  g_ivec3 tm = _model->GetMaterial()->GetTexture(tmpTexName)->GetMetrics();
  GEffectVariable *_ctrl_set_var = _model->GetMaterial()->Effect->GetVar(G_SVT_CUSTOM, 9);

  GUI_CONTROL_SET CtrlSet;
  CtrlSet.borders = _borders;
  CtrlSet.ctrl_size = GetSize();
  CtrlSet.tex_offset = TexOffset;
  CtrlSet.tex_scale = TexScale;
  CtrlSet.tex_size = g_ivec2(g_int32(tm.x*TexScale.x), g_int32(tm.y*TexScale.y));
 
  if (_ctrl_set_var != NULL)
	_ctrl_set_var->SetRawData(CtrlSet);
}

//----------------------------------------------

void GUI_BaseObject::_set_extra_state(g_ivec2 state)
{
  if (_model == NULL) return;
  ID3D10EffectVariable *_extra_state_var = _model->GetMaterial()->Effect->GetVar(G_SVT_CUSTOM, 8)->GetDXVar();
  if (_extra_state_var->IsValid())
	_extra_state_var->AsVector()->SetIntVector((g_int32*)&state);
}

//----------------------------------------------

void GUI_BaseObject::_set_custom_rtt(GTexture *Texture)
{
  _custom_rtt = Texture;
}

//----------------------------------------------

void GUI_BaseObject::_draw_text(GSprite *Sprite, g_rect TextRect, const g_string &Text, g_int32 TextFormat)
{
  if (_font == NULL) return;
  if (Sprite != NULL) Sprite->Begin(D3DX10_SPRITE_SAVE_STATE);
  _font->GDrawText(Sprite, Text, &TextRect, TextFormat);
  if (Sprite != NULL) Sprite->End();
}

//-----------------------------------------------

void GUI_BaseObject::_set_size(g_ivec2 &Size)
{
  g_rect la_metrics;
  _get_low_aligned_metrics(la_metrics);
  ResizeRect(la_metrics, Size.x, Size.y);
  _set_from_low_aligned_metrics(la_metrics, _anchors_set, _local_metrics, _center_aspect);
}

//-----------------------------------------------

void GUI_BaseObject::_set_position(g_ivec2 &Position)
{
  g_rect la_metrics;
  _get_low_aligned_metrics(la_metrics);
  MoveRect(la_metrics, Position.x, Position.y);
  _set_from_low_aligned_metrics(la_metrics, _anchors_set, _local_metrics, _center_aspect);
}

//-----------------------------------------------

g_bool GUI_BaseObject::SetModel(GModel *Model)
{
  _model = NULL;
  if (Model == NULL) return false;
  GMaterial *_curr_material = Model->GetMaterial();
  if ((_curr_material == NULL) || (_curr_material->Effect == NULL) ||
	  (Model->GetInstanceCount() == 0)) return false;
  _model = Model;
  _changed = true;
  return true;
}

//-----------------------------------------------

g_bool GUI_BaseObject::SetFont(GFont *Font)
{
  _font = NULL;
  if ((Font == NULL) || (!Font->IsCreated())) return false;
  _font = Font;
  _changed = true;
  return true;
}

//-----------------------------------------------

void GUI_BaseObject::SetEnabled(g_bool Enabled)
{
  _enabled = Enabled;
  _changed = true;
}

//-----------------------------------------------

void GUI_BaseObject::SetVisible(g_bool Visible)
{
  _visible = Visible;

  if (_visible)
  {
    GUI_EVENTPROC_DESC eDesc = {this, G_MSG_ONSHOW, 0, {}};
	if (_eventcallback != NULL) _eventcallback(&eDesc);
    OnShow();
  }
  else
  {
    GUI_EVENTPROC_DESC eDesc = {this, G_MSG_ONHIDE, 0, {}};
    if (_eventcallback != NULL) _eventcallback(&eDesc);
    OnHide();
  }
  _changed = true;
}

//-----------------------------------------------

void GUI_BaseObject::SetFocused(g_bool Focused)
{
  if (_is_focusable)
    _focused = Focused;
  else
	_focused = false;
}

//-----------------------------------------------

void GUI_BaseObject::SetZOrderLevel(g_uint8 ZOrderLevel)
{
  _z_order_level = ZOrderLevel;
}

//-----------------------------------------------

void GUI_BaseObject::SetParent(GUI_BaseObject *Parent)
{
  if (_parent != NULL)
	for (g_uint32 i = 0; i < _parent->_childs.size(); i++)
	  if (_parent->_childs[i] == _parent)
	  {
        _parent->_childs.erase(_parent->_childs.begin() + i);
		break;
	  }

  _parent = Parent;
  if (_parent != NULL)
    _parent->_childs.push_back(this);
}

//-----------------------------------------------

void GUI_BaseObject::SetAlphaLevel(g_float Level)
{
  _alpha_level = Level;
}

//-----------------------------------------------

void GUI_BaseObject::SetCaption(const g_string &Caption)
{
  _caption = Caption;
  if (_caption.size() > G_UI_MAX_CAPTION_LENGTH)
	_caption.erase(_caption.begin()+G_UI_MAX_CAPTION_LENGTH, _caption.end());
  _changed = true;
}

//-----------------------------------------------

void GUI_BaseObject::SetHint(const g_string &Hint)
{
  _hint = Hint;
  if (_hint.size() > G_UI_MAX_HINT_LENGTH)
	_hint.erase(_hint.begin()+G_UI_MAX_HINT_LENGTH, _hint.end());
}

//-----------------------------------------------

void GUI_BaseObject::SetTexName(g_string Name)
{
  _tex_name = Name;
}

//-----------------------------------------------

void GUI_BaseObject::SetTexIndex(g_int8 index)
{
  if (_model == NULL) return;
  if (!_model->GetMaterial()->IsValidIndex(index))
	index = 0;
  _tex_index = index;
  _changed = true;
}

void GUI_BaseObject::SetScaleFactor(g_float ScaleFactor)
{
  _scale_factor = ScaleFactor;
  _changed = true;
}

//-----------------------------------------------

void GUI_BaseObject::SetAnchors(g_anchors_set Anchors)
{  
  g_int32 width = _device->GetWidth();
  g_int32 height = _device->GetHeight();

  if (_parent != NULL)
  {
	g_rect pca = _parent->GetClientArea();
	width = pca.right - pca.left;
	height = pca.bottom - pca.top;
  }

  _change_anch(_anchors_set.x, Anchors.x, width, 
	           _local_metrics.left, _local_metrics.right, _center_aspect.x);

  _change_anch(_anchors_set.y, Anchors.y, height, 
	           _local_metrics.top, _local_metrics.bottom, _center_aspect.y);
}

//-----------------------------------------------

void GUI_BaseObject::SetBordersSize(g_rect Borders)
{
  _borders = Borders; 
  GUI_EVENTPROC_DESC eDesc = {this, G_MSG_ONRESIZE, 0, {}};
  if (_eventcallback != NULL) _eventcallback(&eDesc);
  OnResize();
  _changed = true;
}

//-----------------------------------------------

void GUI_BaseObject::SetPosition(g_ivec2 &Position)
{
  _set_position(Position);
  GUI_EVENTPROC_DESC eDesc = {this, G_MSG_ONSETPOS, 0, {}};
  if (_eventcallback != NULL) _eventcallback(&eDesc);
  OnSetPosition();
  _changed = true;
}

//-----------------------------------------------

g_ivec2 GUI_BaseObject::GetPosition() const //Позиция относительно верхнего левого угла парента
{
  g_rect la_metrics;
  _get_low_aligned_metrics(la_metrics);

  return g_ivec2(la_metrics.left, la_metrics.top);
}

//-----------------------------------------------

g_ivec2 GUI_BaseObject::GetAbsPosition() const //Позиция относительно верхнего левого угла экрана
{
  g_rect la_metrics;
  _get_low_aligned_metrics(la_metrics);

  g_rect parent_pos = {0,0,0,0};
  if (_parent != NULL)
    parent_pos = _parent->GetAbsClientArea();

  return g_ivec2(la_metrics.left + parent_pos.left, la_metrics.top + parent_pos.top);
}

//-----------------------------------------------

void GUI_BaseObject::SetSize(g_ivec2 &Size)
{
  _set_size(Size);

  GUI_EVENTPROC_DESC eDesc = {this, G_MSG_ONRESIZE, 0, {}};
  if (_eventcallback != NULL) _eventcallback(&eDesc);
  OnResize();
  _changed = true;
}

//-----------------------------------------------

g_ivec2 GUI_BaseObject::GetSize() const
{
  g_rect la_metrics;
  _get_low_aligned_metrics(la_metrics);

  return g_ivec2(la_metrics.right - la_metrics.left, la_metrics.bottom - la_metrics.top);
}

//-----------------------------------------------

g_rect GUI_BaseObject::GetClientArea() const
{
  g_rect la_metrics;
  _get_low_aligned_metrics(la_metrics);

  ReduceRect(la_metrics, _borders);

  return la_metrics;
}

//-----------------------------------------------

g_rect GUI_BaseObject::GetAbsClientArea() const
{
  g_rect la_metrics = GetAbsMetrics();
  ReduceRect(la_metrics, _borders);

  return la_metrics;
}

//-----------------------------------------------

g_rect GUI_BaseObject::GetAbsMetrics() const
{
  g_rect la_metrics;
  _get_low_aligned_metrics(la_metrics);

  g_rect parent_pos = {0,0,0,0};
  if (_parent != NULL)
    parent_pos = _parent->GetAbsClientArea();

  MoveRect(la_metrics, la_metrics.left + parent_pos.left, la_metrics.top + parent_pos.top);

  return la_metrics;
}

//-----------------------------------------------

void GUI_BaseObject::_check_metrics()
{
  g_ivec2 screen = g_ivec2(_device->GetWidth(), _device->GetHeight());
  g_rect la_metrics;
  _get_low_aligned_metrics(la_metrics);

  if (_parent != NULL)
  {
    g_rect pca = _parent->GetAbsClientArea();
	 MoveRect(la_metrics, la_metrics.left + pca.left, 
		      la_metrics.top + pca.top);
  }

  if ((EqualRect(&_prev_metrics, &la_metrics) == FALSE) ||  (screen != _prev_screen))
  {	   
	g_ivec2 ctrl_size = g_ivec2(la_metrics.right - la_metrics.left, la_metrics.bottom - la_metrics.top);
	D3DXMatrixTranslation(&_sprite_mtrans, la_metrics.left + ctrl_size.x/2.0f,
		                  _device->GetHeight() - (la_metrics.top + ctrl_size.y/2.0f), 0.1f);
	D3DXMatrixScaling(&_sprite_mscaling, g_float(ctrl_size.x), g_float(ctrl_size.y),1); 
	_control_sprite.matWorld = _sprite_mscaling * _sprite_mtrans;
	_prev_metrics = la_metrics;
	_prev_screen = screen;
	_changed = true;
  }
}

//-----------------------------------------------

void GUI_BaseObject::Draw(GSprite *Sprite)
{ 
  if (_visible)
  {
    //Вызов события OnDraw
	GUI_EVENTPROC_DESC eDesc = {this, G_MSG_ONDRAW, 0, {}};
    if (_eventcallback != NULL) _eventcallback(&eDesc);
    OnDraw();

	_check_metrics();

	//Отрисовка
	if (Sprite != NULL) 
	{  
	  if (_custom_rtt != NULL)
	    _control_sprite.pTexture = _custom_rtt->Texture;
	  else
	    _control_sprite.pTexture = _rtt_control->Texture;
	  _control_sprite.ColorModulate.a = _alpha_level;  
	  Sprite->DrawBuffered(&_control_sprite,1);  
	}

    for (g_uint32 i=0; i<_childs.size(); i++)
	  _childs[i]->Draw(Sprite);
  }
}

//-----------------------------------------------