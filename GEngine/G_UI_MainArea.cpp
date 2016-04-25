#include "G_UI_MainArea.h"

//-----------------------------------------------

const g_uint16 UI_Set_Count = 14;
g_char UI_Set[UI_Set_Count][20] =  {"UI_DEF_BUTTON",
                                    "UI_DEF_EDIT",
						            "UI_DEF_SCROLLBAR",
						            "UI_DEF_CHECKBOX",
						            "UI_DEF_RADIOBUTTON",
						            "UI_DEF_PROGRESSBAR",
						            "UI_DEF_TRACKBAR",
						            "UI_DEF_WINDOW",
						            "UI_DEF_MOUSE",
						            "UI_DEF_LISTBOX",
                                    "UI_DEF_SCROLLLIST",
                                    "UI_DEF_BUTTONGRID",
                                    "UI_DEF_DESKTOP",
                                    "UI_DEF_PANEL"};

//-----------------------------------------------

G_UI_MainArea::G_UI_MainArea(GD3DDevice *Device)
{
  //Init Variables
  MouseVisible = true;
  
  _device = Device;
  _releasing = false;
  _focused_obj = NULL;    
  _event_callback = NULL;

  //Load Config
  G_CONFIG *CurrConfig = _device->ConfMgr->GetConfig(G_CUID_UICONFIG);

  //Init Font Sprite
  _device->ResMgr->AddSprite(RES_UID_UI_DRAWSPRITE, "UI Draw Sprite");

  //Create Def Font
  _device->ResMgr->AddFont(RES_UID_UI_DEF_FONT, "Times New Roman", 12, g_color(0.1f,0.1f,0.1f,1.0f));
  
  //Create Def Effects
  GEffect *UI_Effect;
  GC_KEY *FxPath = CurrConfig->GetSectionByName("UI_DEF_EFFECT")->GetKeyByName("Path");
  if (FxPath != NULL)
	UI_Effect = _device->ResMgr->AddEffect(RES_UID_UI_DEF_EFFECT, "UI_EFFECT_0", FxPath->Value);
  else
	UI_Effect = _device->ResMgr->AddEffect(RES_UID_UI_DEF_EFFECT, "UI_EFFECT_0", "..\\Shaders\\DefaultUI\\DefUI.fx");

  //Init Input Layout
  D3D10_INPUT_ELEMENT_DESC vertex_layout[] =
  {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
  };
  _device->ILayoutList->AddInputLayout(IL_UID_UI_DEFAULT, UI_Effect->GetDXEffect()->GetTechniqueByName("Render"), vertex_layout, 2);

  //Create Def Materials
  GMaterial *UI_Material = _device->ResMgr->AddMaterial(RES_UID_UI_DEF_MATERIAL, "UI_MATERIAL_0", UI_Effect);

  //Create Def Textures
  if (CurrConfig != NULL)
  {
	GTexture *tempTexture;

	g_int32 texcount = 0;
	for (g_uint16 i = 0; i < UI_Set_Count; i++)
	{
	  GC_SECTION *CurrSec = CurrConfig->GetSectionByName(UI_Set[i]);
	  
	  GC_KEY *UseNameKey = CurrSec->GetKeyByName("Use");
	  if (UseNameKey == NULL)
		continue;

      g_int16 texindex = 0;
	  while(1)
	  {	
		if (texcount > RES_UID_UI_TEXTURE_MAX - RES_UID_UI_TEXTURE_0) break;

		g_char KeyName[256];	
		sprintf_s(KeyName, 256, "Tex%d\0", texindex);
	    GC_KEY *CurrKey = CurrSec->GetKeyByName(KeyName);

		if (CurrKey == NULL) break;

        g_char UseName[256];
		sprintf_s(UseName, 256, UseNameKey->Value, texindex);

		tempTexture = _device->ResMgr->AddTexture(RES_UID_UI_TEXTURE_0 + texcount, UseName, CurrKey->Value);

		UI_Material->AddTexture(tempTexture, 0);

		texindex++;
		texcount++;
	  }
	}
  }

  InitUIMesh();

  //Init Mouse
  Mouse = AddImage(NULL, 0,0,32,32, "", g_anchors_set(g_anst_low, g_anst_low));
  GTexture *MouseTex = UI_Material->GetTexture("MOUSE_TEXTURE_0");
  Mouse->SetFromTexture(&MouseTex);
}

//-----------------------------------------------

G_UI_MainArea::~G_UI_MainArea(void)
{

}

//-----------------------------------------------

GUI_BaseObject* G_UI_MainArea::FindUIObjectByUID(GUI_BaseObject* RootObject, g_int32 UID)
{
  if (RootObject == NULL)
  {
    for (g_uint32 i = 0; i<UIObjectsTree.size(); i++)
    {
      if (UIObjectsTree[i]->GetUID() == UID)
        return UIObjectsTree[i];
      GUI_BaseObject *ChildFound = FindUIObjectByUID(UIObjectsTree[i],UID);
      if (ChildFound != NULL)
        return ChildFound;
    }
  }
  else
  {
    for (g_uint32 i = 0; i<RootObject->_childs.size(); i++)
    {
      if (RootObject->_childs[i]->GetUID() == UID)
        return RootObject->_childs[i];
      GUI_BaseObject *ChildFound = FindUIObjectByUID(RootObject->_childs[i],UID);
      if (ChildFound != NULL)
        return ChildFound;
    }
  }

  return NULL;
}

//-----------------------------------------------

g_bool G_UI_MainArea::CheckChildEvents(g_int32 X, g_int32 Y, g_int32 mbD, g_int32 mbU, GUI_BaseObject *Child)
{
  g_bool result = false;
  if (Child->_visible)
  {
    if (Child->_childs.size() > 0)
      for (g_int32 i = g_int32(Child->_childs.size()-1); i>=0 ; i--)
	    if (CheckChildEvents(X,Y,mbD,mbU,Child->_childs[i]))
        {
          result = true;
          break;
        }
    
    if (result == false)
    {
	  if (Child->GetType() == G_OT_UI_DESKTOP)
	  {
        _focused_obj = NULL;
		return false;
	  }

      g_rect objRect = Child->GetAbsMetrics();
      if (PointInRect(objRect,G_POINT(X,Y)))
      {
        //Вызов события OnMouseMove
        result = true;
		GUI_EVENTPROC_DESC moveDesc = {Child,G_MSG_ONMOUSEMOVE,2,{X,Y}};
	    if (_event_callback != NULL) _event_callback(&moveDesc);
	    Child->OnMouseMove(X,Y);

		if (Child->ShowHint)
		  CurrentHint = Child->GetHint();
		else
		  CurrentHint.clear();
	    //Вызов события OnMouseEnter
	    if (!Child->_mouseinrect)
	    {
		  GUI_EVENTPROC_DESC entDesc = {Child,G_MSG_ONMOUSEENTER,2,{X,Y}};
	      if (_event_callback != NULL) _event_callback(&entDesc);
	      Child->OnMouseEnter(X,Y);	
	    }
	    if (mbD > 0) 
	    {
	      if (_focused_obj != Child)
	      {
	  	    //Вызов события OnFocusLost
		    if (_focused_obj != NULL)
		    {
			  GUI_EVENTPROC_DESC flostDesc = {Child,G_MSG_ONFOCUSLOST,0,{}};
		      if (_event_callback != NULL) _event_callback(&flostDesc);
		      _focused_obj->OnFocusLost();
		    }
		    _focused_obj = Child;
		    //Вызов события OnFocusSet
            GUI_EVENTPROC_DESC fsetDesc = {Child,G_MSG_ONFOCUSSET,0,{}};
		    if (_event_callback != NULL) _event_callback(&fsetDesc);
		    _focused_obj->OnFocusSet();
	      }
	      //Перемещение окна на верхний уровень
          if (Child->GetType() == G_OT_UI_WINDOW)
          {
            SetZOrder(Child, G_TOP_ZORDER);
          }
          else
          {
            GUI_BaseObject* win = GetParentWindow(Child);
            if (win != NULL)
              SetZOrder(win, G_TOP_ZORDER);
          }
          //Вызов события OnMouseDown
          GUI_EVENTPROC_DESC downDesc = {Child,G_MSG_ONMOUSEDOWN,3,{X,Y,mbD}};
	      if (_event_callback != NULL) _event_callback(&downDesc);
	      Child->OnMouseDown(mbD,X,Y);
	    }
	    //Вызов события OnClick
	    if ((mbU > 0)&&(Child->_mousedowned)) 
	    {
		  GUI_EVENTPROC_DESC clickDesc = {Child,G_MSG_ONCLICK,3,{X,Y,mbU}};
	      if (_event_callback != NULL) _event_callback(&clickDesc);
	      Child->OnClick(mbU,X,Y);
	    }
      } 
      else
      {
        //Вызов события OnMouseLeave
	    if (Child->_mouseinrect)
	    {
		  GUI_EVENTPROC_DESC leaveDesc = {Child,G_MSG_ONMOUSELEAVE,2,{X,Y}};
	      if (_event_callback != NULL) _event_callback(&leaveDesc);
          Child->OnMouseLeave(X,Y);  
	    }
	    //Вызов события OnMouseMove если зажата клавиша мыши
	    if (Child->_mousedowned)
	    {
		  GUI_EVENTPROC_DESC moveDesc = {Child,G_MSG_ONMOUSEMOVE,2,{X,Y}};
	      if (_event_callback != NULL) _event_callback(&moveDesc);
	      Child->OnMouseMove(X,Y);
	    }
      }
      //Вызов события OnMouseUp
      if ((mbU > 0)&&(Child->_mousedowned)) 
      {
		GUI_EVENTPROC_DESC upDesc = {Child,G_MSG_ONMOUSEUP,3,{X,Y,mbU}};
	    if (_event_callback != NULL) _event_callback(&upDesc);
	    Child->OnMouseUp(mbU,X,Y);
      }
    }
  }
  return result;
}

//-----------------------------------------------

g_bool G_UI_MainArea::CheckMouseEvents(GInput *GInputDevice)
{
  if ((!_releasing) && (MouseVisible))
  {
    g_int32 tX = GInputDevice->AX;
    g_int32 tY = GInputDevice->AY;

    g_int32 mButtonsD = 0;
    g_int32 mButtonsU = 0;

    //SetMouseMeshPosition
	Mouse->SetPosition(g_ivec2(GInputDevice->AX, GInputDevice->AY));
	Mouse->SetVisible(MouseVisible);

    //Check Mouse Down
    if (GInputDevice->OnMouseDown(GMOUSE_LEFT))
	  mButtonsD |= 1;
    if (GInputDevice->OnMouseDown(GMOUSE_RIGHT))
	  mButtonsD |= 2;
    if (GInputDevice->OnMouseDown(GMOUSE_MIDDLE))
	  mButtonsD |= 4;
    //Check Mouse Up
    if (GInputDevice->OnMouseUp(GMOUSE_LEFT))
	  mButtonsU |= 1;
    if (GInputDevice->OnMouseUp(GMOUSE_RIGHT))
	  mButtonsU |= 2;
    if (GInputDevice->OnMouseUp(GMOUSE_MIDDLE))
	  mButtonsU |= 4;

	if (UIObjectsTree.size() > 0)
      for (g_int32 i = g_int32(UIObjectsTree.size()-1); i>=0 ; i--)
	    if (CheckChildEvents(tX,tY,mButtonsD,mButtonsU,UIObjectsTree[i]))
          return true;	
  }
  return false;
} 

//-----------------------------------------------

g_bool G_UI_MainArea::CheckKeyEvents()
{
  return ((!_releasing) && (_focused_obj != NULL) &&
	      (_focused_obj->IsInputField()) && 
		  (_focused_obj->GetEnabled()) &&
		  (_focused_obj->GetVisible()));
} 

//-----------------------------------------------

void G_UI_MainArea::MsgProc(HWND hwnd, g_uint32 message, WPARAM wParam, LPARAM lParam)
{
  if (!_releasing)
  {
	switch (message)                
    {
      case WM_KEYDOWN:
		              if (_focused_obj != NULL)
						if ((lParam & G_KEYSTATEDOWN) == G_KEYSTATEDOWN)
						{
						  //Вызов события OnKeyPress
						  GUI_EVENTPROC_DESC pressDesc = {_focused_obj, G_MSG_ONKEYPRESS,1,{wParam}};
						  if (_event_callback != NULL) _event_callback(&pressDesc);
						  _focused_obj->OnKeyPress(wParam);
						}
						else
						{
						  //Вызов события OnKeyDown
						  GUI_EVENTPROC_DESC downDesc = {_focused_obj, G_MSG_ONKEYDOWN,1,{wParam}};
						  if (_event_callback != NULL) _event_callback(&downDesc);
						  _focused_obj->OnKeyDown(wParam);
						}
	                  break;
	  case WM_KEYUP:  
		              if (_focused_obj != NULL)
					  {
						//Вызов события OnKeyUp
						GUI_EVENTPROC_DESC upDesc = {_focused_obj, G_MSG_ONKEYUP,1,{wParam}};
						if (_event_callback != NULL) _event_callback(&upDesc);
						_focused_obj->OnKeyUp(wParam);
					  }
	                  break;
      case WM_CHAR:   
		              if (_focused_obj != NULL)
					  {
						//Вызов события OnChar
						GUI_EVENTPROC_DESC charDesc = {_focused_obj, G_MSG_ONCHAR,1,{wParam}};
						if (_event_callback != NULL) _event_callback(&charDesc);
						_focused_obj->OnChar(wParam);
					  }
	                  break;
    }
  }
}

//-----------------------------------------------

void G_UI_MainArea::SetTexName(g_char *InSectName, g_char *OutTexName)
{
  if (InSectName != NULL)
  {
    GC_KEY *TexNameKey = _device->ConfMgr->GetConfig(G_CUID_UICONFIG)->GetSectionByName(InSectName)->GetKeyByName("Use");
    if (TexNameKey != NULL)
      strcpy_s(OutTexName,256,TexNameKey->Value);
  }
}

//-----------------------------------------------

void G_UI_MainArea::InitCustomObject(GUI_BaseObject *UIObject, GFont *Font,g_int32 x,g_int32 y,g_uint32 width,g_uint32 height,
	                  g_anchors_set anchors, g_rect borders, g_uint32 TexIndex, g_char *ConfSectName)
{
  UIObject->SetFont(Font);
  UIObject->SetPosition(g_ivec2(x,y));
  UIObject->SetSize(g_ivec2(width, height));
  UIObject->SetVisible(true);
  UIObject->SetBordersSize(borders);
  UIObject->SetAnchors(anchors);
  UIObject->_tex_index = TexIndex;
  UIObject->_eventcallback = _event_callback;
  g_char TexName[256];
  SetTexName(ConfSectName, TexName);
  UIObject->_tex_name = TexName;

  if (UIObject->_parent == NULL)
  {
    UIObject->_z_order = UIObjectsTree.size();
	UIObjectsTree.push_back(UIObject);
  }
  else
    UIObject->_z_order = UIObject->_parent->_childs.size() - 1;
}

//-----------------------------------------------

GUI_Button* G_UI_MainArea::AddButton(GUI_BaseObject *Parent, const g_string &Caption, 
	                          g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, g_uint32 texindex, g_anchors_set anchors)
{
  GModel *UIModel = _device->ResMgr->GetModelByUID(RES_UID_UI_DRAWMODEL);
  GUI_Button *NewButton = new GUI_Button(UIModel, Parent, Caption, "Button", _device); 

  g_rect Borders = {1,1,1,1};
  InitCustomObject(NewButton, _device->ResMgr->GetFontByUID(RES_UID_UI_DEF_FONT), x, y, width, height, anchors,
  Borders, texindex, UI_Set[0]);

  return NewButton;
}

//-----------------------------------------------

GUI_ListBox* G_UI_MainArea::AddListBox(GUI_BaseObject *Parent, g_uint32 x, g_uint32 y, g_uint32 width, 
	                                      g_uint32 height, g_uint32 texindex, g_anchors_set anchors)

{ 
  GModel *UIModel = _device->ResMgr->GetModelByUID(RES_UID_UI_DRAWMODEL);
  GUI_ListBox *NewListBox = new GUI_ListBox(UIModel, Parent, "ListBox", _device);
  
  g_rect Borders = {G_UI_DEF_BORDER_SIZE, G_UI_DEF_BORDER_SIZE,
	                G_UI_DEF_BORDER_SIZE, G_UI_DEF_BORDER_SIZE};
  InitCustomObject(NewListBox, _device->ResMgr->GetFontByUID(RES_UID_UI_DEF_FONT), x, y, width, height, anchors,
	               Borders, texindex, UI_Set[9]);

  return NewListBox;
}

//-----------------------------------------------

GUI_Window* G_UI_MainArea::AddWindow(GUI_BaseObject *Parent, const string &Caption, g_uint32 x, g_uint32 y, 
                                        g_uint32 width, g_uint32 height, g_uint32 texindex, g_anchors_set anchors)
{ 
  GModel *UIModel = _device->ResMgr->GetModelByUID(RES_UID_UI_DRAWMODEL);
  GUI_Window *NewWindow = new GUI_Window(UIModel, Parent, Caption, "Window", _device);
  
  g_rect Borders = {G_UI_WINDOW_BORDER_SIZE, G_UI_WINDOW_CAPTION_SIZE,
	                G_UI_WINDOW_BORDER_SIZE, G_UI_WINDOW_BORDER_SIZE};
  InitCustomObject(NewWindow, _device->ResMgr->GetFontByUID(RES_UID_UI_DEF_FONT), x, y, width, height, anchors,
	               Borders, texindex, UI_Set[7]);

  return NewWindow;
}

//-----------------------------------------------

GUI_Edit* G_UI_MainArea::AddEdit(GUI_BaseObject *Parent, g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, g_uint32 texindex, g_anchors_set anchors)
{ 
  GModel *UIModel = _device->ResMgr->GetModelByUID(RES_UID_UI_DRAWMODEL);
  GUI_Edit *NewEdit = new GUI_Edit(UIModel, Parent, "Edit", _device);

  g_rect Borders = {G_UI_DEF_BORDER_SIZE, G_UI_DEF_BORDER_SIZE,
	                G_UI_DEF_BORDER_SIZE, G_UI_DEF_BORDER_SIZE};
  InitCustomObject(NewEdit, _device->ResMgr->GetFontByUID(RES_UID_UI_DEF_FONT), x, y, width, height, anchors,
	               Borders, texindex, UI_Set[1]);

  return NewEdit;
}

//-----------------------------------------------

GUI_Image* G_UI_MainArea::AddImage(GUI_BaseObject *Parent, g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, 
									  const g_string &FileName, g_anchors_set anchors)
{ 
  GModel *UIModel = _device->ResMgr->GetModelByUID(RES_UID_UI_DRAWMODEL);
  GUI_Image *NewImage = new GUI_Image(UIModel, Parent, FileName, "Image", _device);
  
  g_rect Borders = {0,0,0,0};
  InitCustomObject(NewImage, NULL, x, y, width, height, anchors, Borders, 0, NULL);

  return NewImage;
}

//-----------------------------------------------

GUI_Label* G_UI_MainArea::AddLabel(GUI_BaseObject *Parent,g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, 
									  const g_string &Caption, g_anchors_set anchors)
{ 
  GUI_Label *NewLabel = new GUI_Label(NULL, Parent, Caption, "Label", _device);

  g_rect Borders = {0,0,0,0};
  InitCustomObject(NewLabel, _device->ResMgr->GetFontByUID(RES_UID_UI_DEF_FONT), x, y, width, height, anchors,
	               Borders, 0, NULL);

  return NewLabel;
}

//-----------------------------------------------

GUI_CheckBox* G_UI_MainArea::AddCheckBox(GUI_BaseObject *Parent,g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, 
						                 const g_string &Caption, g_uint32 texindex, g_anchors_set anchors)
{  
  GModel *UIModel = _device->ResMgr->GetModelByUID(RES_UID_UI_DRAWMODEL);
  GUI_CheckBox *NewChkBox = new GUI_CheckBox(UIModel, Parent, Caption, "ChkBox", _device);
  
  g_rect Borders = {0,0,0,0};
  InitCustomObject(NewChkBox, _device->ResMgr->GetFontByUID(RES_UID_UI_DEF_FONT), x, y, width, height, anchors,
	               Borders, texindex, UI_Set[3]);

  return NewChkBox;
}

//-----------------------------------------------

GUI_RadioButton* G_UI_MainArea::AddRadioButton(GUI_BaseObject *Parent,g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, 
								                  const g_string &Caption, g_uint32 texindex, g_anchors_set anchors)

{  
  GModel *UIModel = _device->ResMgr->GetModelByUID(RES_UID_UI_DRAWMODEL);
  GUI_RadioButton *NewRadBtn = new GUI_RadioButton(UIModel, Parent, Caption, "RadBtn", _device);
  
  g_rect Borders = {0,0,0,0};
  InitCustomObject(NewRadBtn, _device->ResMgr->GetFontByUID(RES_UID_UI_DEF_FONT), x, y, width, height, anchors,
	               Borders, texindex, UI_Set[4]);

  return NewRadBtn;
}

//-----------------------------------------------

GUI_TrackBar* G_UI_MainArea::AddTrackBar(GUI_BaseObject *Parent,g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, 
											g_uint32 texindex, g_anchors_set anchors)
{  
  GModel *UIModel = _device->ResMgr->GetModelByUID(RES_UID_UI_DRAWMODEL);
  GUI_TrackBar *NewTrkBar = new GUI_TrackBar(UIModel, Parent, "TrackBar", _device);
  
  g_rect Borders = {13,0,13,0};
  InitCustomObject(NewTrkBar, _device->ResMgr->GetFontByUID(RES_UID_UI_DEF_FONT), x, y, width, height, anchors,
	               Borders, texindex, UI_Set[6]);

  return NewTrkBar;
}

//-----------------------------------------------

GUI_Panel* G_UI_MainArea::AddPanel(GUI_BaseObject *Parent, g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, 
                                      g_uint32 texindex, g_anchors_set anchors)
{  
  GModel *UIModel = _device->ResMgr->GetModelByUID(RES_UID_UI_DRAWMODEL);
  GUI_Panel *NewPanel = new GUI_Panel(UIModel, Parent, "Panel", _device);
  
  g_rect Borders = {G_UI_WINDOW_BORDER_SIZE, G_UI_WINDOW_BORDER_SIZE,
	                G_UI_WINDOW_BORDER_SIZE, G_UI_WINDOW_BORDER_SIZE};
  InitCustomObject(NewPanel, NULL, x, y, width, height, anchors, Borders, texindex, UI_Set[13]);

  return NewPanel;
}

//-----------------------------------------------

GUI_ScrollList* G_UI_MainArea::AddScrollList(GUI_BaseObject *Parent,g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, 
								                g_uint32 texindex, g_anchors_set anchors)
{  
  GModel *UIModel = _device->ResMgr->GetModelByUID(RES_UID_UI_DRAWMODEL);
  GUI_ScrollList *NewScrList = new GUI_ScrollList(UIModel, Parent, "ScrList", _device);
  
  g_rect Borders = {0,0,0,0};
  InitCustomObject(NewScrList, _device->ResMgr->GetFontByUID(RES_UID_UI_DEF_FONT), x, y, width, height, anchors,
	               Borders, texindex, UI_Set[10]);

  return NewScrList;
}

//-----------------------------------------------

GUI_ProgressBar* G_UI_MainArea::AddProgressBar(GUI_BaseObject *Parent,g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, 
												  g_uint32 texindex, g_anchors_set anchors)
{  
  GModel *UIModel = _device->ResMgr->GetModelByUID(RES_UID_UI_DRAWMODEL);
  GUI_ProgressBar *NewProgBar = new GUI_ProgressBar(UIModel, Parent, "ProgBar", _device);
   
  g_rect Borders = {G_UI_DEF_BORDER_SIZE, G_UI_DEF_BORDER_SIZE,
	                G_UI_DEF_BORDER_SIZE, G_UI_DEF_BORDER_SIZE};
  InitCustomObject(NewProgBar, NULL, x, y, width, height, anchors,
	               Borders, texindex, UI_Set[5]);

  return NewProgBar;
}

//-----------------------------------------------

GUI_Desktop* G_UI_MainArea::AddDesktop()
{
  GUI_Desktop* NewDesktop = new GUI_Desktop("Desktop", _device);

  g_rect Borders = {0, 0, 0, 0};
  g_anchors_set anchors = g_anchors_set(g_anst_both, g_anst_both);
  InitCustomObject(NewDesktop, NULL, 0, 0, _device->GetWidth(), _device->GetHeight(), anchors, Borders, 0, UI_Set[12]);

  return NewDesktop;
}

//-----------------------------------------------

GUI_ButtonGrid* G_UI_MainArea::AddButtonGrid(GUI_BaseObject *Parent, g_uint32 x, g_uint32 y, g_uint32 grid_cols, g_uint32 grid_rows, 
                                                g_uint32 nButtons, g_uint32 texindex, g_anchors_set anchors)
{ 
  GModel *UIModel = _device->ResMgr->GetModelByUID(RES_UID_UI_DRAWMODEL);
  GUI_ButtonGrid *NewButGrid = new GUI_ButtonGrid(UIModel, Parent, "ButtonGrid", _device);
  
  g_rect Borders = {0, 0, 0, 0};
  InitCustomObject(NewButGrid, _device->ResMgr->GetFontByUID(RES_UID_UI_DEF_FONT), x, y, 32, 32, anchors,
	               Borders, texindex, UI_Set[11]);

  NewButGrid->SetGridSize(g_ivec2(grid_cols,grid_rows));
  NewButGrid->AddButtons(nButtons);

  return NewButGrid;
}

//-----------------------------------------------

GUI_ScrollBar* G_UI_MainArea::AddScrollBar(GUI_BaseObject *Parent,g_uint32 x, g_uint32 y, g_uint32 width, g_uint32 height, 
										   g_uint32 texindex, g_anchors_set anchors)
{  
  GModel *UIModel = _device->ResMgr->GetModelByUID(RES_UID_UI_DRAWMODEL);
  GUI_ScrollBar *NewScrBar = new GUI_ScrollBar(UIModel, Parent, "ScrBar", _device);
  
  g_rect Borders = {0, 0, 0, 0};
  InitCustomObject(NewScrBar, NULL, x, y, width, height, anchors,
	               Borders, texindex, UI_Set[2]);

  return NewScrBar;
}

//-----------------------------------------------

void G_UI_MainArea::CheckChildChange(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GUI_BaseObject *Child, GSprite *UISprite)
{
  if (Child->_changed) Child->UpdateObj(mWorld, mViewProj, UISprite);

  for (g_uint32 i=0; i<Child->_childs.size(); i++)
	CheckChildChange(mWorld, mViewProj, Child->_childs[i], UISprite);
}

//-----------------------------------------------

void G_UI_MainArea::DrawUI(g_mat4x4 &mWorld, GCamera *Camera)
{
  if (!_releasing)
  {
    _device->SetPrimitiveTopology(G_PT_TRIANGLE_LIST);
	Camera->SetViewport();
	GEffect *DefEffect = _device->ResMgr->GetEffectByUID(RES_UID_UI_DEF_EFFECT);
	GSprite *UISprite = _device->ResMgr->GetSpriteByUID(RES_UID_UI_DRAWSPRITE);

    //Draw Objects 
	for (g_uint32 i=0; i<UIObjectsTree.size(); i++) 
      CheckChildChange(mWorld, Camera->GetViewProj2DMatrix(), UIObjectsTree[i], UISprite);

    UISprite->Begin(D3DX10_SPRITE_SAVE_STATE);
	DefEffect->GetTech(0)->ApplyPass(0);

	for (g_uint32 i=0; i<UIObjectsTree.size(); i++)
	  if (UIObjectsTree[i] != Mouse)
	    UIObjectsTree[i]->Draw(UISprite);
	Mouse->Draw(UISprite);

    UISprite->End();
	DefEffect->GetTech(1)->ApplyPass(0);
  }
}

//-----------------------------------------------

void G_UI_MainArea::Release()
{
  _releasing = true;
  clear_del_vector(UIObjectsTree);
}

//-----------------------------------------------

void G_UI_MainArea::InitUIMesh()
{
  G_LVERTEX vertices[] =
  {
    G_LVERTEX(g_vec3( 0.0f, 0.0f, 0.1f ), g_vec2( 0.0f, 0.0f )),
    G_LVERTEX(g_vec3( 0.0f, 1.0f, 0.1f ), g_vec2( 0.0f, 1.0f )),
    G_LVERTEX(g_vec3( 1.0f, 0.0f, 0.1f ), g_vec2( 1.0f, 0.0f )),
    G_LVERTEX(g_vec3( 1.0f, 1.0f, 0.1f ), g_vec2( 1.0f, 1.0f ))
  };

  g_uint32 indices[] =
  {
    2,1,0,   3,1,2,
  };
   
  GModel *UIModel = _device->ResMgr->AddModel(RES_UID_UI_DRAWMODEL, "UI Model", 
	                                          _device->ResMgr->GetMaterialByUID(RES_UID_UI_DEF_MATERIAL), 0,
											  sizeof(G_LVERTEX), IL_UID_UI_DEFAULT);

  UIModel->AddVertexArray(vertices, 4);
  UIModel->AddIndexArray(indices, 6);
  UIModel->CreateVIBuffers();
  UIModel->AddInstance();
}

//-----------------------------------------------

g_bool G_UI_MainArea::LoadUIFromFile(g_char *filename)
{
  if (FileExists(filename))
  { 
	//Reading Data From File
    g_char *buffer;
    g_uint32 fSize;
    if (!ReadEntireFile(filename,&buffer,&fSize))
      return false;

	//Parsing Data
	g_uint32 prevpos = 0;
    const g_uint16 max_len = 2000;
    const g_uint16 min_len = 35;
	g_char TextLine[max_len];
    
	for (g_uint32 i = 0; i < fSize; i++)
	{
	  if ((buffer[i] == 10) || (buffer[i] == 13))
	  {
		g_uint32 LineLen = i-prevpos;
		if (LineLen > min_len)
		{
		  memset(&TextLine[0],0,max_len);
		  strncpy_s(&TextLine[0],max_len,&buffer[prevpos],LineLen);
       
		  if (TextLine[0] != ';') //Parse Line
		  {
            G_UIOBJECT_SET UI_Set;
			char *pcurrstr;
            g_uint32 cnt = 0;
            g_char *context;
            pcurrstr = strtok_s(TextLine,"|",&context);
            while (pcurrstr != NULL)
            {
              if (cnt<12)
              {
                g_int32 str_len = strlen(pcurrstr)+1;
                g_char *trim_str = new g_char[str_len];
                trim(pcurrstr, trim_str, str_len);
                
                switch (cnt)
                {
                  case 0:strcpy_s(UI_Set.ClassName,255,trim_str);break;
                  case 1:sscanf_s(trim_str,"%d",&UI_Set.ParentID);break;
                  case 2:sscanf_s(trim_str,"%d",&UI_Set.ID);break;
                  case 3:strcpy_s(UI_Set.Caption,255,trim_str);break;
                  case 4:sscanf_s(trim_str,"%d,%d,%d,%d",&UI_Set.X,&UI_Set.Y,&UI_Set.W,&UI_Set.H);break;
                  case 5:sscanf_s(trim_str,"%d",&UI_Set.Visible);break;
                  case 6:sscanf_s(trim_str,"%d",&UI_Set.Drag);break;
                  case 7:sscanf_s(trim_str,"%d",&UI_Set.AlignFlag);break;
                  case 8:sscanf_s(trim_str,"%f",&UI_Set.Alpha);break;
                  case 9:sscanf_s(trim_str,"%d",&UI_Set.TexIndex);break;
                  case 10:sscanf_s(trim_str,"%d",&UI_Set.ShowHint);break;
                  case 11:strcpy_s(UI_Set.Hint,256,trim_str);break;
                  default:break;
                }
                
                delete[]trim_str;
                cnt++;
              }
              else
                break;

              pcurrstr = strtok_s(NULL, "|", &context);
            }//while (pcurrstr != NULL)

            if (cnt > 4)
            {
              GUI_BaseObject *ParentObject = NULL;
              GUI_BaseObject *ThisObject = NULL;
			  g_anchors_set anchors = g_anchors_set(g_anst_low, g_anst_low);

			  if ((UI_Set.AlignFlag & G_OAF_AL_RIGHT) == G_OAF_AL_RIGHT)
			  {
                anchors.x = g_anst_high;
				UI_Set.X = _device->GetWidth() - UI_Set.X;
			  }
			  if ((UI_Set.AlignFlag & G_OAF_AL_BOTTOM) == G_OAF_AL_BOTTOM)
			  {
                anchors.y = g_anst_high;
				UI_Set.Y = _device->GetHeight() - UI_Set.Y;
			  }
			  if ((UI_Set.AlignFlag & G_OAF_AL_HCENTER) == G_OAF_AL_HCENTER)
                anchors.x = g_anst_center;
			  if ((UI_Set.AlignFlag & G_OAF_AL_VCENTER) == G_OAF_AL_VCENTER)
                anchors.y = g_anst_center;

              if (UI_Set.ParentID != 0)
                ParentObject = FindUIObjectByUID(NULL,UI_Set.ParentID);

              if (strcmp(UI_Set.ClassName,"Window") == 0)
                ThisObject = AddWindow(ParentObject, UI_Set.Caption,UI_Set.X,UI_Set.Y,UI_Set.W,UI_Set.H,UI_Set.TexIndex,anchors);
              if (strcmp(UI_Set.ClassName,"Button") == 0)
                ThisObject = AddButton(ParentObject,UI_Set.Caption,UI_Set.X,UI_Set.Y,UI_Set.W,UI_Set.H,UI_Set.TexIndex,anchors);
              if (strcmp(UI_Set.ClassName,"Edit") == 0)
                ThisObject = AddEdit(ParentObject,UI_Set.X,UI_Set.Y,UI_Set.W,UI_Set.H,UI_Set.TexIndex,anchors);
              if (strcmp(UI_Set.ClassName,"CheckBox") == 0)
                ThisObject = AddCheckBox(ParentObject,UI_Set.X,UI_Set.Y,UI_Set.W,UI_Set.H,UI_Set.Caption,UI_Set.TexIndex,anchors);
              if (strcmp(UI_Set.ClassName,"Image") == 0)
                ThisObject = AddImage(ParentObject,UI_Set.X,UI_Set.Y,UI_Set.W,UI_Set.H,UI_Set.Caption,anchors);
              if (strcmp(UI_Set.ClassName,"Label") == 0)
                ThisObject = AddLabel(ParentObject,UI_Set.X,UI_Set.Y,UI_Set.W,UI_Set.H,UI_Set.Caption,anchors);
              if (strcmp(UI_Set.ClassName,"ListBox") == 0)
                ThisObject = AddListBox(ParentObject,UI_Set.X,UI_Set.Y,UI_Set.W,UI_Set.H,UI_Set.TexIndex,anchors);
              if (strcmp(UI_Set.ClassName,"ProgressBar") == 0)
                ThisObject = AddProgressBar(ParentObject,UI_Set.X,UI_Set.Y,UI_Set.W,UI_Set.H,UI_Set.TexIndex,anchors);
              if (strcmp(UI_Set.ClassName,"RadioButton") == 0)
                ThisObject = AddRadioButton(ParentObject,UI_Set.X,UI_Set.Y,UI_Set.W,UI_Set.H,UI_Set.Caption,UI_Set.TexIndex,anchors);
              if (strcmp(UI_Set.ClassName,"ScrollBar") == 0)
                ThisObject = AddScrollBar(ParentObject,UI_Set.X,UI_Set.Y,UI_Set.W,UI_Set.H,UI_Set.TexIndex,anchors);
              if (strcmp(UI_Set.ClassName,"ScrollList") == 0)
                ThisObject = AddScrollList(ParentObject,UI_Set.X,UI_Set.Y,UI_Set.W,UI_Set.H,UI_Set.TexIndex,anchors);
              if (strcmp(UI_Set.ClassName,"TrackBar") == 0)
                ThisObject = AddTrackBar(ParentObject,UI_Set.X,UI_Set.Y,UI_Set.W,UI_Set.H,UI_Set.TexIndex,anchors);
              if (strcmp(UI_Set.ClassName,"ButtonGrid") == 0)
                ThisObject = AddButtonGrid(ParentObject,UI_Set.X,UI_Set.Y,UI_Set.W,UI_Set.H,0,UI_Set.TexIndex,anchors);
              if (strcmp(UI_Set.ClassName,"Desktop") == 0)
                ThisObject = AddDesktop();
			  if (strcmp(UI_Set.ClassName,"Panel") == 0)
                ThisObject = AddPanel(ParentObject, UI_Set.X,UI_Set.Y,UI_Set.W,UI_Set.H,UI_Set.TexIndex,anchors);
              
              if (ThisObject != NULL)
              {
                ThisObject->SetAlphaLevel(UI_Set.Alpha);
                ThisObject->SetVisible(UI_Set.Visible > 0);
                ThisObject->DragEnabled = UI_Set.Drag > 0;
                ThisObject->SetUID(UI_Set.ID);
				ThisObject->SetHint(UI_Set.Hint);
				ThisObject->ShowHint = UI_Set.ShowHint > 0;
              }
            }
		  }//if (TextLine[0] != ';')
		}
		prevpos = i+1;
	  }
	}

	//Clear
	delete []buffer;
  }
  else
	return false;

  return true;
}

//-----------------------------------------------

void G_UI_MainArea::SetZOrder(GUI_BaseObject *Object, g_uint16 z_ord)
{
  vector <GUI_BaseObject*> *list;
  if (Object->_parent != NULL)
    list = &Object->_parent->_childs;
  else
    list = &UIObjectsTree;

  if (z_ord >= list->size()) z_ord = list->size() - 1;

  if  (z_ord > Object->_z_order)
  {
    for (g_uint32 i = 0; i<list->size(); i++)
      if ((*list)[i]->_z_order > Object->_z_order)
        (*list)[i]->_z_order--;
  }

  if  (z_ord < Object->_z_order)
  {
    for (g_uint32 i = 0; i<list->size(); i++)
      if ((*list)[i]->_z_order < Object->_z_order)
        (*list)[i]->_z_order++;
  }

  Object->_z_order = z_ord;

  vector <GUI_BaseObject*> sortedlist = (*list);
  for (g_uint32 i = 0; i<list->size(); i++)
    (*list)[sortedlist[i]->_z_order] = sortedlist[i];
}

//-----------------------------------------------

GUI_BaseObject* G_UI_MainArea::GetParentWindow(GUI_BaseObject *Object)
{
  GUI_BaseObject* Parent = Object->_parent;
  while(true)
  {
    if (Parent == NULL) break;
    if (Parent->GetType() == G_OT_UI_WINDOW) return Parent;
    Parent = Parent->_parent;
  }
  return NULL;
}

//-----------------------------------------------

void G_UI_MainArea::_update_childs(GUI_BaseObject *parent)
{
  if (parent == NULL) return;
  parent->_changed = true;
  for (g_uint32 i = 0; i < parent->_childs.size(); i++)
	_update_childs(parent->_childs[i]);
}

//-----------------------------------------------

void G_UI_MainArea::UpdateAllUIObject()
{
  for (g_uint32 i = 0; i < UIObjectsTree.size(); i++)
	_update_childs(UIObjectsTree[i]);
}

//-----------------------------------------------
