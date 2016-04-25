#pragma once

#include "GModel.h"
#include "GFont.h"
#include "GBuffer.h"

#ifndef GUIOBJECT2_H
#define GUIOBJECT2_H

//-----------------------------------------------
class GUI_BaseObject;

struct GUI_EVENTPROC_DESC
{
  GUI_BaseObject *Object;
  g_uint16       Message;
  g_uint16       nPrms;
  g_uint32       Prms[5];
};

struct GUI_CONTROL_SET
{
  g_vec2  tex_offset;
  g_vec2  tex_scale;
  g_ivec2 tex_size;
  g_ivec2 ctrl_size;
  g_rect  borders;
};

//-----------------------------------------------

class GUI_BaseObject :
	public GBaseObject
{
  friend class G_UI_MainArea;
public:
  //Properties
  g_bool  DragEnabled;
  g_bool  ShowHint;

  //Properties Setters/Getters
  void SetPosition(g_ivec2 &Position);
  g_ivec2 GetPosition() const;
  g_ivec2 GetAbsPosition() const;

  void SetSize(g_ivec2 &Size);
  g_ivec2 GetSize() const;

  g_rect GetClientArea() const;
  g_rect GetAbsClientArea() const;
  g_rect GetAbsMetrics() const;

  void SetCaption(const g_string &Caption);
  const g_string& GetCaption() const {return _caption;};

  void SetHint(const g_string &Hint);
  const g_string& GetHint() const {return _hint;};

  void SetEnabled(g_bool Enabled);
  g_bool GetEnabled() const {return _enabled;};

  void SetVisible(g_bool Visible);
  g_bool GetVisible() const {return _visible;};

  void SetFocused(g_bool Focused);
  g_bool GetFocused() const {return _focused;};

  void SetScaleFactor(g_float ScaleFactor);
  g_float GetScaleFactor() const {return _scale_factor;};

  void SetAlphaLevel(g_float Level);
  g_float GetAlphaLevel() const {return _alpha_level;};

  void SetBordersSize(g_rect Borders);
  g_rect GetBordersSize() const {return _borders;};

  void SetAnchors(g_anchors_set Anchors);
  g_anchors_set GetAnchors() const {return _anchors_set;};

  g_bool IsInputField() const {return _is_input_field;};
  g_bool IsFocusable() const {return _is_focusable;};
  g_bool IsResizeable() const {return _is_resizeable;};

  g_uint16 GetZOrder() const {return _z_order;};

  void SetZOrderLevel(g_uint8 ZOrderLevel);
  g_uint8 GetZOrderLevel() const {return _z_order_level;};
  
  void SetParent(GUI_BaseObject *Parent);
  GUI_BaseObject* GetParent() const {return _parent;};

  g_bool SetModel(GModel *Model);
  GModel* GetModel() {return _model;};

  g_bool SetFont(GFont *Font);
  GFont* GetFont() {return _font;};

  const vector <GUI_BaseObject*> GetChildsList() {return _childs;};

  void SetTexName(g_string Name);
  void SetTexIndex(g_int8 index);

  void SetChanged() {_changed = true;};

  //Events
  virtual void OnFocusSet() {};
  virtual void OnFocusLost() {};
  virtual void OnShow() {};
  virtual void OnHide() {};
  virtual void OnResize() {};
  virtual void OnSetPosition() {};

  virtual void OnKeyDown(g_int32 Key) {};
  virtual void OnKeyUp(g_int32 Key) {};
  virtual void OnKeyPress(g_int32 Key) {};
  virtual void OnChar(g_int32 Key) {};

  virtual void OnMouseLeave(g_int32 X, g_int32 Y) {};
  virtual void OnMouseEnter(g_int32 X, g_int32 Y) {};
  virtual void OnMouseMove(g_int32 X, g_int32 Y) {};
  virtual void OnClick(g_int32 Button, g_int32 X, g_int32 Y) {};
  virtual void OnMouseDown(g_int32 Button, g_int32 X, g_int32 Y) {};
  virtual void OnMouseUp(g_int32 Button, g_int32 X, g_int32 Y) {};

  virtual void OnDraw() {};

  //Draw
  virtual void UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite) {};
  void Draw(GSprite *Sprite);

  //Конструктор и деструктор
  GUI_BaseObject(GModel *Model, GUI_BaseObject *Parent, const g_string &Name, GD3DDevice *Device); 
  GUI_BaseObject(const g_string &Name, GD3DDevice *Device);
  ~GUI_BaseObject(void);
protected:
  g_bool          _is_input_field;      //-gi-
  g_bool          _is_focusable;        //-gi-
  g_bool          _is_resizeable;       //-gi-

  g_uint16        _z_order;             //-gi-
 
  g_bool          _changed;             //s-i-
  
  vector <GUI_BaseObject*> _childs;     //----
  GUI_BaseObject*          _parent;     //sgi-

  void _set_size(g_ivec2 &Size);
  void _set_position(g_ivec2 &Position);

  //event variables 
  g_bool          _mousedowned;         //--i-
  g_bool          _mouseinrect;         //--i-

  void (*_eventcallback)(GUI_EVENTPROC_DESC*);

  //draw functions
  g_bool          _begin_update(const g_mat4x4 &mWorld, const g_mat4x4 &mViewProj);
  void            _end_update();

  void            _apply_ctrl_set(g_vec2 TexOffset, g_vec2 TexScale);
  void            _set_custom_rtt(GTexture *Texture);
  void            _set_extra_state(g_ivec2 state);

  void            _draw_text(GSprite *Sprite, g_rect TextRect, const g_string &Text, g_int32 TextFormat);
private:
  g_string        _caption;             //sgi-
  g_string        _hint;                //sgi-
  
  g_rect          _borders;             //sgi-
  g_anchors_set   _anchors_set;         //sgi-
  g_rect          _local_metrics;       //sgi-
  g_float         _scale_factor;        //sgi-
  g_vec2          _center_aspect;       //--i-

  g_bool          _enabled;             //sgi-
  g_bool          _visible;             //sgi-
  g_bool          _focused;             //sgi-

  g_float         _alpha_level;         //sgi-
  g_uint8         _z_order_level;       //sgi-

  //draw variables
  GModel          *_model;              //sgi-
  GFont           *_font;               //sgi-
  GTexture        *_rtt_control;        //--ir
  GTexture        *_custom_rtt;         //s-i-

  D3DX10_SPRITE   _control_sprite;      //--i-
  g_mat4x4        _sprite_mscaling;     //--i-
  g_mat4x4        _sprite_mtrans;       //--i-
  g_rect          _prev_metrics;        //--i-
  g_ivec2         _prev_screen;         //--i-

  g_int8          _tex_index;           //s-i-
  g_string        _tex_name;            //s-i-
  g_vec2          _tex_sprite_scale;    //sgi-

  g_uint32        _last_change_time;    //--i-

  //functions
  void            _init(GModel *Model, GUI_BaseObject *Parent);

  void            _get_low_aligned_metrics(g_rect &metrics) const;
  void            _set_from_low_aligned_metrics(const g_rect &la_metrics, const g_anchors_set &anchors, 
	                                            g_rect &dest_metrics, g_vec2 &dest_center_aspect) const;

  g_ivec2         _get_low_aligned_pos(g_anchors_state curr_as, g_int32 area_size,
				                       g_long lo_pos, g_long hi_pos, g_float center_aspect) const;
  void            _set_from_low_aligned_pos(g_ivec2 curr_la_pos, g_anchors_state curr_as, g_int32 area_size,
				                            g_long &lo_pos, g_long &hi_pos, g_float &center_aspect) const;

  void            _change_anch(g_anchors_state &curr_as, const g_anchors_state &new_as, g_int32 area_size,
				               g_long &lo_pos, g_long &hi_pos, g_float &center_aspect); 

  void            _check_metrics();
  //draw functions
  g_bool          _check_upd_interval() {return (GetTickCount()-_last_change_time > G_UI_UPDATE_INTERVAL);};
};

#endif //GUIOBJECT2_H