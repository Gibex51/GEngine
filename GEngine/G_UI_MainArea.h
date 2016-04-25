#pragma once

#include "GUI_BaseObject.h"
#include "GUI_Button.h"
#include "GUI_ListBox.h"
#include "GUI_ScrollBar.h"
#include "GUI_Window.h"
#include "GUI_Edit.h"
#include "GUI_Image.h"
#include "GUI_Label.h"
#include "GUI_CheckBox.h"
#include "GUI_RadioButton.h"
#include "GUI_TrackBar.h"
#include "GUI_Panel.h"
#include "GUI_ScrollList.h"
#include "GUI_ProgressBar.h"
#include "GUI_Desktop.h"
#include "GUI_ButtonGrid.h"

#include "GInput.h"
#include "GResourceManager.h"

#ifndef GUIMAIN_H
#define GUIMAIN_H

class G_UI_MainArea
{
public:
  g_bool MouseVisible;
  
  //UI Objects
  vector<GUI_BaseObject*> UIObjectsTree;
  
  GUI_BaseObject* FindUIObjectByUID(GUI_BaseObject* RootObject, g_int32 UID);

  void SetZOrder(GUI_BaseObject *Object, g_uint16 z_ord); 
  GUI_BaseObject* GetParentWindow(GUI_BaseObject *Object);

  const g_string& GetCurrentHint() const {return CurrentHint;};

  void UpdateAllUIObject();

  //UI Object Mgmt
  /*Default objects*/
  GUI_Button* AddButton(GUI_BaseObject *Parent, const g_string &Caption, 
	                       g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, g_uint32 texindex, g_anchors_set anchors);

  GUI_ListBox* AddListBox(GUI_BaseObject *Parent, g_uint32 x, g_uint32 y, g_uint32 width, 
	                          g_uint32 height, g_uint32 texindex, g_anchors_set anchors);

  GUI_Window* AddWindow(GUI_BaseObject *Parent, const string &Caption, g_uint32 x, g_uint32 y, 
	                       g_uint32 width, g_uint32 height, g_uint32 texindex, g_anchors_set anchors);

  GUI_Edit* AddEdit(GUI_BaseObject *Parent, g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, g_uint32 texindex, g_anchors_set anchors);

  GUI_Image* AddImage(GUI_BaseObject *Parent, g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, const g_string &FileName, g_anchors_set anchors);

  GUI_Label* AddLabel(GUI_BaseObject *Parent,g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, const g_string &Caption, g_anchors_set anchors);

  GUI_CheckBox* AddCheckBox(GUI_BaseObject *Parent,g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, const g_string &Caption, g_uint32 texindex, g_anchors_set anchors);

  GUI_RadioButton* AddRadioButton(GUI_BaseObject *Parent,g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, const g_string &Caption, g_uint32 texindex, g_anchors_set anchors);
  
  GUI_TrackBar* AddTrackBar(GUI_BaseObject *Parent,g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, g_uint32 texindex, g_anchors_set anchors);

  GUI_Panel* AddPanel(GUI_BaseObject *Parent, g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, g_uint32 texindex, g_anchors_set anchors);

  GUI_ScrollList* AddScrollList(GUI_BaseObject *Parent,g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, g_uint32 texindex, g_anchors_set anchors);

  GUI_ProgressBar* AddProgressBar(GUI_BaseObject *Parent,g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, g_uint32 texindex, g_anchors_set anchors);
    
  GUI_ScrollBar* AddScrollBar(GUI_BaseObject *Parent,g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, g_uint32 texindex, g_anchors_set anchors);

  GUI_Desktop* AddDesktop();

  GUI_ButtonGrid* AddButtonGrid(GUI_BaseObject *Parent, g_uint32 x, g_uint32 y, g_uint32 grid_cols, g_uint32 grid_rows, 
                                   g_uint32 nButtons, g_uint32 texindex, g_anchors_set anchors);

  void InitCustomObject(GUI_BaseObject *UIObject, GFont *Font,g_int32 x,g_int32 y,g_uint32 width,g_uint32 height,
	                  g_anchors_set anchors, g_rect borders, g_uint32 TexIndex, g_char *ConfSectName); //New

  //Events
  g_bool CheckMouseEvents(GInput *GInputDevice);
  g_bool CheckKeyEvents();
  void MsgProc(HWND hwnd, g_uint32 message, WPARAM wParam, LPARAM lParam);
  void SetEventCallback(void (*_eventproc)(GUI_EVENTPROC_DESC*)) {_event_callback = _eventproc;};

  //Draw
  void DrawUI(g_mat4x4 &mWorld, GCamera *Camera);

  //Clear
  void Release();

  //Save/Load
  g_bool LoadUIFromFile(g_char *filename);

  //Конструктор и деструктор
  G_UI_MainArea(GD3DDevice *Device);
  ~G_UI_MainArea(void);
private:
  g_bool _releasing;

  GD3DDevice *_device;

  GUI_Image *Mouse;

  GUI_BaseObject *_focused_obj;

  g_string CurrentHint;

  void (*_event_callback)(GUI_EVENTPROC_DESC*);
 
  void InitUIMesh(); 
  void SetTexName(g_char *InSectName, g_char *OutTexName);
  
  g_bool CheckChildEvents(g_int32 X, g_int32 Y, g_int32 mbD, g_int32 mbU, GUI_BaseObject *Child);
  void CheckChildChange(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GUI_BaseObject *Child, GSprite *UISprite); 

  void _update_childs(GUI_BaseObject *parent);
};

//-----------------------------------------------

struct G_UIOBJECT_SET
{
  g_uint32 Visible;
  g_uint32 Drag;
  g_uint32 ShowHint;
  g_uint32 AlignFlag;
  g_uint32 ID;
  g_uint32 ParentID;
  g_uint32 X,Y,W,H;
  g_uint32 TexIndex;
  g_float Alpha;
  g_char ClassName[255];
  g_char Caption[255];
  g_char Hint[256];

  G_UIOBJECT_SET() 
  {
    Visible = true;
    Drag = false;
	ShowHint = false;
    AlignFlag = 0;
    ID = 0;
    ParentID = 0;
    X=Y=0;
    W=H=100;
    TexIndex = 0;
    Alpha = 1.0f;
    memset(Caption,0,255);
    memset(ClassName,0,255);
	memset(Hint,0,256);
  };
};

//-----------------------------------------------



//-----------------------------------------------

#endif //GUIMAIN_H