#include "GUI_ButtonGrid.h"

//-----------------------------------------------

GUI_ButtonGrid::GUI_ButtonGrid(GModel *Model, GUI_BaseObject *Parent, const g_string &Name, GD3DDevice *Device):
                 GUI_BaseObject(Model,Parent,Name,Device)
{
  _init_btngrid();
}

//-----------------------------------------------

GUI_ButtonGrid::GUI_ButtonGrid(const g_string &Name, GD3DDevice *Device):
                 GUI_BaseObject(NULL,NULL,Name,Device)
{
  _init_btngrid();
}

//-----------------------------------------------

GUI_ButtonGrid::~GUI_ButtonGrid(void)
{

}

//-----------------------------------------------

void GUI_ButtonGrid::_init_btngrid()
{
  SetType(G_OT_UI_BUTTONGRID);
  _img_tex = NULL;
  _pressed = -1;
  _separator_size = 2;
  _grid_size = g_ivec2(5,5);
  _button_size = g_ivec2(36,36);
  _image_size = g_ivec2(16,16);
  _image_count = g_ivec2(0,0);
  _mode = g_btgm_radiobtn;
}

//-----------------------------------------------

g_int32 GUI_ButtonGrid::GetButtonIndex(g_int32 X, g_int32 Y)
{
  div_t gx = div(X, _button_size.x + _separator_size);
  div_t gy = div(Y, _button_size.y + _separator_size);

  if ((gx.rem >= _button_size.x) || (gy.rem >= _button_size.y))
    return -1;

  g_int32 index = gy.quot * _grid_size.x + gx.quot;

  if ((index < 0) || (index >= g_int32(_buttons.size())))
    return -1;
  else
    return index;
}

//-----------------------------------------------

void GUI_ButtonGrid::OnClick(g_int32 Button, g_int32 X, g_int32 Y)
{

}

//-----------------------------------------------

void GUI_ButtonGrid::OnMouseDown(g_int32 Button, g_int32 X, g_int32 Y)
{
  _changed = true;
  _mousedowned = true;
  g_rect abs_metrics = GetAbsMetrics();

  g_int32 ind = GetButtonIndex(X - abs_metrics.left, Y - abs_metrics.top);

  if (ind > -1)
  {
    for (g_uint32 i = 0; i<_buttons.size(); i++)
      _buttons[i]._pressed = false;

    _buttons[ind]._pressed = true;
    _buttons[ind]._state = 3;

    _pressed = ind;
  }
}

//-----------------------------------------------

void GUI_ButtonGrid::OnMouseUp(g_int32 Button, g_int32 X, g_int32 Y)
{
  _changed = true;
  _mousedowned = false;
}

//-----------------------------------------------

void GUI_ButtonGrid::OnMouseLeave(g_int32 X, g_int32 Y)
{
  _changed = true;
  _mouseinrect = false;

  for (g_uint32 i = 0; i<_buttons.size(); i++)
    if (_buttons[i]._pressed)
      _buttons[i]._state = 1;
    else
      _buttons[i]._state = 0;
}

//-----------------------------------------------

void GUI_ButtonGrid::OnMouseEnter(g_int32 X, g_int32 Y)
{
  _changed = true;
  _mouseinrect = true; 
}

//-----------------------------------------------

void GUI_ButtonGrid::OnMouseMove(g_int32 X, g_int32 Y) 
{
  _changed = true;
  g_rect abs_metrics = GetAbsMetrics();

  g_int32 ind = GetButtonIndex(X - abs_metrics.left, Y - abs_metrics.top);

  for (g_uint32 i = 0; i<_buttons.size(); i++)
    if (_buttons[i]._pressed)
      _buttons[i]._state = 1;
    else
      _buttons[i]._state = 0;

  if (ind > -1)
  {
    if (_buttons[ind]._pressed)
      _buttons[ind]._state = 3;
    else
      _buttons[ind]._state = 2;
  }
}

//-----------------------------------------------

void GUI_ButtonGrid::UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite) 
{
  //Set Real Size
  g_ivec2 real_grid_size = _grid_size;
  div_t divres = div(_buttons.size(), _grid_size.x);
  if (divres.quot == 0) real_grid_size.x = _buttons.size();
  real_grid_size.y = divres.quot;
  if (divres.rem > 0) real_grid_size.y++;

  SetSize(g_ivec2(real_grid_size.x*_button_size.x + (real_grid_size.x-1)*_separator_size,
                  real_grid_size.y*_button_size.y + (real_grid_size.y-1)*_separator_size));

  //Update
  if (_begin_update(mWorld, mViewProj))
  {
    GModel *this_model = GetModel();
	g_ivec2 size = GetSize();

    if (this_model != NULL)
    {
	  GMeshInstance* _minst = this_model->GetInstance(0);

	  g_vec3 *button_pos = new g_vec3[_buttons.size()];
	  g_vec3 *img_pos = new g_vec3[_buttons.size()];
	  g_vec2 *img_offs = new g_vec2[_buttons.size()];

	  //Calc Positions
	  for (g_uint32 i = 0; i < _buttons.size(); i++)
      { 
        div_t divres = div(i, _grid_size.x);
        button_pos[i].x = g_float(divres.rem * (_button_size.x + _separator_size));
        button_pos[i].y = g_float(divres.quot * (_button_size.y + _separator_size));
        button_pos[i].z = 0;

		if (_img_tex != NULL)
	    {
		  div_t divimg = div(i,_image_count.x);
          img_offs[i] = g_vec2(divimg.rem * (1.0f/_image_count.x),
                               divimg.quot * (1.0f/_image_count.y));

		  img_pos[i].x = button_pos[i].x + (_button_size.x - _image_size.x)/2.0f;
          img_pos[i].y = button_pos[i].y + (_button_size.y - _image_size.y)/2.0f;
          img_pos[i].z = 0;
		}
	  }

	  //Draw Buttons
	  _minst->SetScaling(g_vec3(g_float(_button_size.x),g_float(_button_size.y),1));
      for (g_uint32 i = 0; i < _buttons.size(); i++)
      {        
		_apply_ctrl_set(GUI_BTNGRID_TEXOFFSET[_buttons[i]._state], GUI_BTNGRID_TEXSCALE);
		_minst->SetPosition(button_pos[i]);

        this_model->Draw(false);
	  }

	  //Draw Pics
	  if (_img_tex != NULL)
	  {
        g_vec2 pic_scale = g_vec2(1.0f/_image_count.x, 1.0f/_image_count.y);
		_minst->SetScaling(g_vec3(g_float(_image_size.x), g_float(_image_size.y), 1));
		this_model->GetMaterial()->ApplyTexture(_img_tex, 0);

	    for (g_uint32 i = 0; i < _buttons.size(); i++)
        {   
		  _apply_ctrl_set(img_offs[i], pic_scale);
		  _minst->SetPosition(img_pos[i]);		  	  
          this_model->Draw(false);
        }
	  }

	  delete[]button_pos;
	  delete[]img_pos;
	  delete[]img_offs;
	}

    _end_update();
  } 
}

//-----------------------------------------------

void GUI_ButtonGrid::AddButtons(g_uint32 nButtons)
{
  for (g_uint32 i = 0; i < nButtons; i++)
  {
    GUI_BUTTONINFO NewBut;
    NewBut._state = 0;
    NewBut._pressed = false;
    _buttons.push_back(NewBut);
  }
  _changed = true;
}

//-----------------------------------------------

void GUI_ButtonGrid::SetPressedButton(g_int32 index)
{
  _pressed = -1;
  if ((index >= 0) && (index < g_int32(_buttons.size()))) //Переделать
  {
    for (g_uint32 i = 0; i<_buttons.size(); i++)
      _buttons[i]._pressed = false;

    _buttons[index]._pressed = true;
    _buttons[index]._state = 3;
    _pressed = index;
  }
  _changed = true;
}

//-----------------------------------------------

void GUI_ButtonGrid::SetImagesTexture(GTexture *ImgTex, g_ivec2 ImgSize, g_ivec2 ImgCount)
{
  if (CheckImgSize(ImgSize) && CheckImgCount(ImgCount))
  {
    _img_tex = ImgTex;
    _image_size = ImgSize;
    _image_count = ImgCount;
  }
  else
    _img_tex = NULL;
  _changed = true;
}

//-----------------------------------------------

g_bool GUI_ButtonGrid::CheckImgSize(g_ivec2 ImgSize)
{
  return (((ImgSize.x > 0) && (ImgSize.x <= G_UI_MAX_ICON_SIZE)) && 
          ((ImgSize.y > 0) && (ImgSize.y <= G_UI_MAX_ICON_SIZE)));
}

//-----------------------------------------------

g_bool GUI_ButtonGrid::CheckImgCount(g_ivec2 ImgCount)
{
  return ((ImgCount.x > 0) && (ImgCount.y > 0));
}

//-----------------------------------------------

void GUI_ButtonGrid::SetMode(g_btng_mode Mode)
{
  _mode = Mode;
  _changed = true;
}

//-----------------------------------------------
/*
void GUI_ButtonGrid::GetRawSettings(void **pBuffer, g_uint32 &BufSize)
{
  
}

//-----------------------------------------------

void GUI_ButtonGrid::SetRawSettings(void *Buffer, g_uint32 BufSize)
{
  
}
*/
//-----------------------------------------------