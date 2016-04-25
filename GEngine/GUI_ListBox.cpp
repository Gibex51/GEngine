#include "GUI_ListBox.h"

//-----------------------------------------------

GUI_ListBox::GUI_ListBox(const g_string &Name, GD3DDevice *Device):GUI_BaseObject(Name,Device)
{
  _init_listbox(80,128);
}

//-----------------------------------------------

GUI_ListBox::GUI_ListBox(GModel *Model, GUI_BaseObject *Parent, const g_string &Name, GD3DDevice *Device):
             GUI_BaseObject(Model,Parent,Name,Device)
{
  _init_listbox(80,128);
}

//-----------------------------------------------

GUI_ListBox::~GUI_ListBox(void)
{

}

//-----------------------------------------------

void GUI_ListBox::_init_listbox(g_int32 width, g_int32 height)
{
  SetType(G_OT_UI_LISTBOX);

  _stage = 0;
  _n_visible_items = 0;
  _prev_pos = 0;
  SetRect(&_item_bord, 0,0,0,0);
  
  SelectedItem = -1;

  _scrollbar = new GUI_ScrollBar(GetModel(), this, "ScrBar", _device);
  _scrollbar->SetVisible(true);
  _scrollbar->SetRange(0,0);

  SetSize(g_ivec2(width, height));

  GC_KEY *TexNameKey = _device->ConfMgr->GetConfig(G_CUID_UICONFIG)->GetSectionByName("UI_DEF_SCROLLBAR")->GetKeyByName("Use");
  if (TexNameKey != NULL)
	_scrollbar->SetTexName(TexNameKey->Value);
}

//-----------------------------------------------

void GUI_ListBox::UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite)  
{
  if (_begin_update(mWorld, mViewProj))
  {
	GModel *_this_model = GetModel();
	g_ivec2 size = GetSize();

	if (_this_model != NULL)
    {
	  GMeshInstance* _minst = _this_model->GetInstance(0);
	  
	  _apply_ctrl_set(GUI_LSTBOX_TEXOFFSET[_stage], GUI_LSTBOX_TEXSCALE);
	  _this_model->Draw(false);

	  //Draw Selected Item
	  if ((SelectedItem > -1) && (SelectedItem < g_int32(Items.size())))
	  {
		g_int32 itemonscr = SelectedItem - _scrollbar->GetThumbPosition();

		if ((itemonscr >= 0) && (itemonscr <_n_visible_items))
		{      
		  g_rect _bord = GetBordersSize();
          _minst->SetPosition(g_vec3(g_float(_bord.left), g_float(_bord.top + 20*itemonscr),0));
		  _minst->SetScaling(g_vec3(g_float(size.x - _bord.right*2 - 24), 20, 1));

		  SetBordersSize(_item_bord);
		  _apply_ctrl_set(GUI_LSTBOX_TEXOFFSET[0], GUI_LSTBOX_ITEMSCALE);
		  _this_model->Draw(false);
		  SetBordersSize(_bord);
		}
	  }

	  //Update ScrollBar
	  if (_scrollbar != NULL)
		_scrollbar->SetRange(0, Items.size() - _n_visible_items);
	}
 
    g_int32 _item_height = 20;
    GFont *font = GetFont();
    if (font != NULL)
	  _item_height = font->GetFontHeight();
    _n_visible_items = (size.y-6) / _item_height;

	g_rect TextRect;
	SetRect(&TextRect, 0, 0, size.x, size.y);
	ReduceRect(TextRect, GetBordersSize());
	for (g_uint32 i = _scrollbar->GetThumbPosition(); i<Items.size(); i++)
	{
	  g_int32 ItemOffset = i - _scrollbar->GetThumbPosition();
	  if (ItemOffset >= _n_visible_items) break;
	  g_rect ItemRect;
	  SetRect(&ItemRect, TextRect.left, TextRect.top+20*ItemOffset, 
		                 TextRect.right-24, TextRect.top+20*(ItemOffset+1));
      _draw_text(Sprite, ItemRect, Items[i], DT_VCENTER | DT_SINGLELINE);
	}

    _end_update();
  }
}

//-----------------------------------------------

void GUI_ListBox::OnDraw()
{
  if (_prev_pos != _scrollbar->GetThumbPosition())
  {
	_prev_pos = _scrollbar->GetThumbPosition();
	_changed = true;
  }
}

//-----------------------------------------------

void GUI_ListBox::OnResize()
{
  _changed = true;
  g_rect client_size = GetClientArea();
  if (_scrollbar != NULL)
  {
    _scrollbar->SetSize(g_ivec2(24, client_size.bottom - client_size.top + 3)); 
    _scrollbar->SetPosition(g_ivec2(client_size.right - client_size.left - 23, -1));   
  }
}

//-----------------------------------------------

void GUI_ListBox::OnSetPosition()
{
  _changed = true;
  g_ivec2 size = GetSize();
  _scrollbar->SetPosition(g_ivec2(size.x - 27, 3));
}

//-----------------------------------------------

void GUI_ListBox::OnKeyPress(g_int32 Key) 
{ 
  if (Key == VK_UP)
    OnSelectItem(SelectedItem - 1);

  if (Key == VK_DOWN)
    OnSelectItem(SelectedItem + 1);
}

//-----------------------------------------------

void GUI_ListBox::OnKeyDown(g_int32 Key)
{
  if (Key == VK_UP)
    OnSelectItem(SelectedItem - 1);

  if (Key == VK_DOWN)
    OnSelectItem(SelectedItem + 1);
}

//-----------------------------------------------

void GUI_ListBox::OnMouseLeave(g_int32 X, g_int32 Y) 
{
  _changed = true;
  _mouseinrect = false;
  _stage = 0;
}

//-----------------------------------------------

void GUI_ListBox::OnMouseEnter(g_int32 X, g_int32 Y) 
{
  _changed = true;
  _mouseinrect = true;
  _stage = 1;
}

//-----------------------------------------------

void GUI_ListBox::OnClick(g_int32 Button, g_int32 X, g_int32 Y)
{
  if (Button == 1)
  {
	g_ivec2 size = GetSize();
	g_ivec2 pos = GetAbsPosition();
	g_rect bord = GetBordersSize();

	g_int32 dx = X - pos.x - bord.left;
	g_int32 dy = Y - pos.y - bord.top;

	if ((dy > 0) && (dy < _n_visible_items*20))
	  if ((dx > 0) && (dx < (size.x - bord.right - 24)))
	  {
		g_int32 ts = _scrollbar->GetThumbPosition() + g_int32(floor(dy / 20.0f));
		if (SelectedItem != ts)
		  OnSelectItem(ts);
	  }
  }
}

//-----------------------------------------------

void GUI_ListBox::OnMouseDown(g_int32 Button, g_int32 X, g_int32 Y) 
{
  _mousedowned = true;
}

//-----------------------------------------------

void GUI_ListBox::OnMouseUp(g_int32 Button, g_int32 X, g_int32 Y) 
{
  _mousedowned = false;
}

//-----------------------------------------------

void GUI_ListBox::OnSelectItem(g_int32 ItemIndex) 
{
  if ((ItemIndex>-1) && (ItemIndex <  g_int32(Items.size())))
  {    
    SelectedItem = ItemIndex;

    g_int32 tp = _scrollbar->GetThumbPosition();

    if (SelectedItem >= tp + _n_visible_items - 1)
      _scrollbar->SetThumbPosition(SelectedItem - _n_visible_items + 1);

    if (SelectedItem < tp)
      _scrollbar->SetThumbPosition(SelectedItem);

    _changed = true;

	GUI_EVENTPROC_DESC eDesc = {this, G_MSG_ONSELECTITEM, 1, {ItemIndex}};
	if (_eventcallback != NULL) _eventcallback(&eDesc);
  }
}

//-----------------------------------------------

void GUI_ListBox::SetScrollPos(g_int32 Pos)
{
  if (_scrollbar == NULL) return;

  switch (Pos)
  {
    case G_LB_SCROLL_BEG: {
		                    _scrollbar->SetThumbPosition(0);
						  } break;
    case G_LB_SCROLL_END: {		           
		                    _scrollbar->SetRange(0, Items.size() - _n_visible_items);
		                    _scrollbar->SetThumbPosition(Items.size());
						  } break;
    case G_LB_SCROLL_CURR:{
		                    
						  } break;
	default:{
		      _scrollbar->SetThumbPosition(Pos);           
			} break;
  }
}

//-----------------------------------------------

void GUI_ListBox::ClearItems()
{
  Items.clear();
  SelectedItem = -1;
  _scrollbar->SetThumbPosition(0);
  _changed = true;
}

//-----------------------------------------------

void GUI_ListBox::FillFromDirectory(g_char *FindMask, g_bool Numerate, 
                                     g_bool OnlyDirs, g_uint8 RemoveExtSize)
{
  ClearItems();
                           
  WIN32_FIND_DATA find_data;
  HANDLE hFind;
  memset(&find_data,0,sizeof(WIN32_FIND_DATA));

  hFind = FindFirstFile(FindMask,&find_data);
  if (hFind != INVALID_HANDLE_VALUE)
  {
    char obj_name[300];
	memset(obj_name,0,300);
    g_int32 Num = 0;

    while(1)
    {
      if ((OnlyDirs == false) || (IsDir(&find_data) == true))
      {
        Num++;
        if (Numerate)
        {
          g_char tmp_name[300];
		  memset(tmp_name,0,300);
          strncpy_s(&tmp_name[0], 300, find_data.cFileName,strlen(find_data.cFileName)-RemoveExtSize);
          sprintf_s(&obj_name[0], 300,"%d.%s", Num, tmp_name);
        }
        else
          strncpy_s(&obj_name[0], 300, find_data.cFileName,strlen(find_data.cFileName)-RemoveExtSize);

        Items.push_back(obj_name);
      }
      if (FindNextFile(hFind,&find_data) == FALSE)
        break;      
    }
  }
  FindClose(hFind);

  _changed = true;
}

//-----------------------------------------------
/*
void GUI_ListBox::GetRawSettings(void **pBuffer, g_uint32 &BufSize)
{
  
}

//-----------------------------------------------

void GUI_ListBox::SetRawSettings(void *Buffer, g_uint32 BufSize)
{
  
}
*/
//-----------------------------------------------