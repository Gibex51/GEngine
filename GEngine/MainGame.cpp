#include "MainGame.h"

//-----------------------------------------------
// For Test

g_uint32 def_layout_elements = 4;
g_uint32 bone_layout_elements = 6;
g_uint32 lbone_layout_elements = 5;

D3D10_INPUT_ELEMENT_DESC def_vertex_layout[] =
{
  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 28, D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 36, D3D10_INPUT_PER_VERTEX_DATA, 0 }
};

D3D10_INPUT_ELEMENT_DESC bone_vertex_layout[] =
{
  { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,       0, 24,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "COLOR",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "WEIGHTS",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "BONES",      0, DXGI_FORMAT_R32G32B32A32_UINT,  0, 64,  D3D10_INPUT_PER_VERTEX_DATA, 0 }
};

D3D10_INPUT_ELEMENT_DESC lbone_vertex_layout[] =
{
  { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,       0, 24,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "WEIGHTS",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "BONES",      0, DXGI_FORMAT_R32G32B32A32_UINT,  0, 48,  D3D10_INPUT_PER_VERTEX_DATA, 0 }
};

//-----------------------------------------------
// Main Game Functions
//-----------------------------------------------

MainGame::MainGame(void)
{
  InitVariables();
}

//-----------------------------------------------

MainGame::~MainGame(void)
{
  FREENULL_VAR(MainWindow);
}

//-----------------------------------------------

void MainGame::InitVariables()
{  
  _settings.scr_width = GetSystemMetrics(SM_CXSCREEN);
  _settings.scr_height = GetSystemMetrics(SM_CYSCREEN);
  _settings.FOV = 65;
  _settings.fullscreen = false;
  _settings.use_shadowmap = true;
  _settings.use_fog = true;
  _settings.use_wireframe = false;
  _settings.use_sound = true;

  closeapp = false;
  closethread = false;
  pause_render = false;
  is_paused_render = false;

  DeviceCreated = false;

  game_mode = G_GAMEMODE_MAIN_MENU;

  cam_heightalign = true;
  sel_cubealign = true;

  sel_started = false;

  tool_type = -1;

  MainWindow = NULL;
  myGDevice  = NULL;
  myGInput   = NULL;
  myAudioMgr = NULL;
  MainScene  = NULL;
  MyUI       = NULL;

  MyPlayer   = NULL;

  MyCT       = NULL;

  //Physics    = NULL;

  /* Variables For Test */
  LightAng = g_vec2(50,50);
  maxfps = 0;
  minfps = 0;

  FPSLabel = NULL;
  CubesLB = NULL;
  PreviewImg = NULL;

  SaveLoadWindow = NULL;
  SaveLoadEdit = NULL;
  SaveLoadLabel = NULL;
  SaveLoadList = NULL;

  ConsoleEdit = NULL;

  ClipModel = NULL;

  mtrSelect = NULL;
  mtrObjPreview = NULL;

  fxCT = NULL;
  fxDef = NULL;
  
  fxpCT = NULL;
  fxpDef = NULL;

  sVoice = NULL;
  rVoice = NULL;

  DESC_CO_PREV = DEF_DESC;
  DESC_CO_PREV.Width = 96;
  DESC_CO_PREV.Height = 96;
  DESC_CO_PREV.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;

  DESC_LIGHT_CAM = DEF_DESC;
  DESC_LIGHT_CAM.Width = G_SHDMAP_RES;
  DESC_LIGHT_CAM.Height = G_SHDMAP_RES*G_SHDMAP_CASC;
  DESC_LIGHT_CAM.Format = DXGI_FORMAT_R32_TYPELESS;
  DESC_LIGHT_CAM.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;
}

//-----------------------------------------------

void MainGame::ConsoleInput(const string &CmdLine)
{
  GCommandParser Parser;
  G_PARSE_RESULT ParseRes;
  Parser.ParseConsoleCmdLine(CmdLine, ParseRes);

  g_bool ValidCommand = false;

  if (ParseRes.Command == "exit")
  {
    closethread = true;
	ValidCommand = true;
  }

  if (ParseRes.Command == "test")
  {

	ValidCommand = true;
  }

  if (ParseRes.Command == "settings")
  {
	if (ParseRes.ParamList.size() > 0)
	{
	  if (ParseRes.ParamList[0].PrmString == "save")
	  {
        if (SaveSettings())
          WriteToLogProc("OK");
		else 
		  WriteToLogProc("Failed");
	  }
	  if (ParseRes.ParamList[0].PrmString == "load")
	  {
        if (LoadSettings())
          WriteToLogProc("OK");
		else 
		  WriteToLogProc("Failed");
	  }
	}
	else
	  WriteToLogProc("Need Params");

	ValidCommand = true;
  }

  if (ParseRes.Command == "info")
  {
    if (ParseRes.ParamList.size() > 0)
	{
	  if (ParseRes.ParamList[0].PrmString == "tempdir")
        WriteToLogProc(GetFullTempPath());	  
	  else if (ParseRes.ParamList[0].PrmString == "adapters")
        OutputAdaptersInfo();
	  else
        WriteToLogProc("Invalid Param");
	}
	else
	  WriteToLogProc("Need Params");

	ValidCommand = true;
  }

  if (ParseRes.Command == "set_tool")
  {
    g_int32 tt = ParseRes.ParamList[0].AsInt();
	CheckRange(&tt, 0, 127);
	tool_type = g_int8(tt);
	ValidCommand = true;
  }

  if (ParseRes.Command == "run_anim")
  {
	g_int32 sel_npc = NPCLB->SelectedItem;
	g_int32 anim_ind = 0;
	if (ParseRes.ParamList.size() > 0)
	  anim_ind = ParseRes.ParamList[0].AsInt();
    if (sel_npc >= 0)
    {
	  G_GM_Unit *Curr_NPC = MyNPCMgr->GetNPCbyIndex(NPCLB->SelectedItem);
	  Curr_NPC->RunAnim(g_uint32(anim_ind));
	}
	ValidCommand = true;
  }

  if (ValidCommand)
	WriteToLogProc(CmdLine);
  else
	WriteToLogProc("Invalid Command");
}

//-----------------------------------------------

void MainGame::OutputAdaptersInfo()
{
  myGDevice->Info->Release();
  myGDevice->Info->CollectInfo();
  for (g_uint32 i = 0; i < myGDevice->Info->GetAdaptersCount(); i++)
  {
	G_INF_ADAPTER curr_adapt;
	myGDevice->Info->GetAdapter(i, curr_adapt);
			
	// Adapter Name
	g_char OemDesc[128];
	CharToOemW(curr_adapt.Desc.Description, OemDesc);

	WriteToLogProc(OemDesc);

	// Displays
	for (g_uint32 j = 0; j < curr_adapt._outputs.size(); j++)
	{
	  // Display Name
	  g_char OemDeviceName[32];
	  CharToOemW(curr_adapt._outputs[j].Desc.DeviceName, OemDeviceName);

	  g_rect DesktopCoord = curr_adapt._outputs[j].Desc.DesktopCoordinates;

	  g_char DisplayInfo[256];
	  sprintf_s(DisplayInfo, 256, "[%s] %dx%d Modes:%d", OemDeviceName, 
				DesktopCoord.right - DesktopCoord.left, DesktopCoord.bottom - DesktopCoord.top,
				curr_adapt._outputs[j].Modes.size());

	  g_float curr_aspect = g_float(DesktopCoord.right - DesktopCoord.left)/g_float(DesktopCoord.bottom - DesktopCoord.top);

	  WriteToLogProc(DisplayInfo);

	  vector <DXGI_MODE_DESC> ResList;
	  curr_adapt._outputs[j].GetShortResolutionList(ResList);

	  for (g_uint32 k = 0; k < ResList.size(); k++)
	  {
		g_char ModeInfo[256];

		g_float ModeAspect = ResList[k].Width/g_float(ResList[k].Height);
		  
		sprintf_s(ModeInfo, 256, "Mode%d: %dx%d Aspect:%.2f Rate: %d", k + 1, 
			      ResList[k].Width, ResList[k].Height, ModeAspect, ResList[k].RefreshRate.Numerator/ResList[k].RefreshRate.Denominator);

		WriteToLogProc(ModeInfo);
	  }
	}
  }
}

//-----------------------------------------------

g_bool MainGame::InitWindow(HINSTANCE _hInst, WNDPROC _wndProc, g_int32 _cmdShow)
{
  srand(g_uint32(time(NULL)));
  timeBeginPeriod(1);

  /* Init Window */
  MainWindow = new GWindow;
  if (!MainWindow->Create(G_CLASS_NAME, G_TITLE_NAME,
	                      _settings.scr_width, _settings.scr_height,
						  _hInst, _wndProc))
    return false;
  MainWindow->Show(_cmdShow);

  return true;
}

//-----------------------------------------------

g_bool MainGame::Init3DDevice()
{
  /* Init GD3DDevice */ 
  myGDevice = new GD3DDevice; 

  /* Load Configs */
  if (!myGDevice->ConfMgr->LoadConfig(G_DEFPATH_UICONF, G_CUID_UICONFIG))
    MessageBox(MainWindow->GetHandle(), "Failed Load UI Config", "Error", MB_OK);

  if (!myGDevice->ConfMgr->LoadConfig(G_DEFPATH_GAMECONF, G_CUID_GAMECONFIG))
    MessageBox(MainWindow->GetHandle(), "Failed Load Game Config", "Error", MB_OK);

  if (!myGDevice->ConfMgr->LoadConfig("..\\Settings\\CubeTerrain.conf", G_CUID_CUBE_TERRAIN))
	MessageBox(MainWindow->GetHandle(), "Failed Load Cube Config", "Error", MB_OK);

  LoadSettings();

  MainWindow->SetSize(_settings.scr_width, _settings.scr_height);

  /* Init Device */
  if (myGDevice->InitDevice(_settings.scr_width, _settings.scr_height,
	                        MainWindow->GetHandle(), !_settings.fullscreen))
  {
    myGDevice->InitRenderTarget();

	/* Create Resource Manager */
	myResMgr = new GResourceManager(myGDevice);
	myGDevice->ResMgr = myResMgr;

    return true;
  }
  else
    return false;
}

//-----------------------------------------------

g_bool MainGame::InitEngineSubSystems()
{
  /* Инициализация ввода */    
  if (!InitInput())
  {
    MessageBox(MainWindow->GetHandle(),"Ошибка инициализации DirectInput",G_TITLE_NAME,MB_OK);
    return false;
  }

  /* Инициализация звука */
  InitSound();

  /* Применяем настройки */
  SetWireframe(_settings.use_wireframe);

  GUI_CheckBox *TempCB = (GUI_CheckBox*)MyUI->FindUIObjectByUID(NULL, UID_CHECKBOX_SHDMAP);
  if (TempCB != NULL) TempCB->SetCheck(_settings.use_shadowmap);

  TempCB = (GUI_CheckBox*)MyUI->FindUIObjectByUID(NULL, UID_CHECKBOX_FOG);
  if (TempCB != NULL) TempCB->SetCheck(_settings.use_fog);

  /* Создаем сцену или загружаем из файла */
  MainScene = new GScene(myGDevice);

  /* Добавяем счетчики fps */
  MainScene->AddFPSInfo(); //0 - Render (frames/s)
  MainScene->AddFPSInfo(); //1 - Main cycle (cycles/s)
  
  /* Add Cameras */
  GCamera *camMenu; 
  camMenu = myResMgr->AddCamera(UID_CAM_MENU, "Menu Cam", myGDevice->GetWidth(), myGDevice->GetHeight(), false);
  camMenu->SetView(g_vec3(-10.0f,60.0f,-110.0f), g_vec3(-1, -1, -1));	  
  MainScene->SetDefaultCamera(camMenu);

  /* Инициализация FFMpeg */
  /*FFVideo = new G_VideoLib(myGDevice, myAudioMgr->XAudio);
  FFVideo->OpenClip("..\\Video\\Test.mkv");
  FFVideo->Play();
  
  fxDef->GetVarByID(10)->AsShaderResource()->SetResource(FFVideo->GetTexBuffer()->Texture);
  fxDef->GetVarByID(11)->AsScalar()->SetInt(FFVideo->GetHeigth());
  fxDef->GetVarByID(12)->AsScalar()->SetInt(FFVideo->GetWidth());*/

  /* Инициализация BulletPhysics */
  /*
  Physics = new G_BulletPhysics;
  Physics->Init();
  MyCT->SetPhysicsPlugin(Physics);
  */
  /* Создаем кубики ^_^ */
  /*
  btCollisionShape* GroundShape = Physics->AddBoxShape(g_vec3(100,5,100));
  btCollisionShape* BoxShape = Physics->AddBoxShape(g_vec3(1,1,1));

  Physics->AddBody(GroundShape, g_vec3(0,-5,0), 0.0f, 0);

  for (g_int32 i = 0; i<30; i++)
    Physics->AddBody(BoxShape, g_vec3((i%5)*2.0f,((i-i%10)/10)*2.0f+1,(i%2)*2.0f), 2.0f, 0);

  Physics->AddBody(BoxShape, g_vec3(0,0,0), 0.0f, G_BF_KINEMATIC | G_BF_DISABLE_DEACTIVATION);
  */

  minfps = 99999;
  maxfps = 0;

  DeviceCreated = true;

  return true;
}

//-----------------------------------------------

g_bool MainGame::InitInput()
{
  myGInput = new GInput;
  if (!myGInput->InitDevices(MainWindow->GetInstance())) return false;
  if (!myGInput->SetCoopLevel(MainWindow->GetHandle(),G_EXCLUSIVE)) return false;
  myGInput->Acquire();
  myGInput->SetMouseRect(0, myGDevice->GetWidth(), 0, myGDevice->GetHeight());
  myGInput->SetCoords(myGDevice->GetWidth()/2, myGDevice->GetHeight()/2, 0);

  /*Bind Keys*/
  myGInput->BindKey(GM_ACT_MAPMOVEUP,    GI_KC_W,           GMOD_NOCHECK, GI_AT_KEYPRESS);
  myGInput->BindKey(GM_ACT_MAPMOVEDOWN,  GI_KC_S,           GMOD_NOCHECK, GI_AT_KEYPRESS);
  myGInput->BindKey(GM_ACT_MAPMOVELEFT,  GI_KC_A,           GMOD_NOCHECK, GI_AT_KEYPRESS);
  myGInput->BindKey(GM_ACT_MAPMOVERIGHT, GI_KC_D,           GMOD_NOCHECK, GI_AT_KEYPRESS);
  myGInput->BindKey(GM_ACT_MAPROTENABLE, GI_KC_MOUSE_MID,   GMOD_NOCHECK, GI_AT_KEYPRESS);

  myGInput->BindKey(GM_ACT_LMTOOL_DOWN,  GI_KC_MOUSE_LEFT,  GMOD_NOCHECK, GI_AT_KEYDOWN);
  myGInput->BindKey(GM_ACT_RMTOOL_DOWN,  GI_KC_MOUSE_RIGHT, GMOD_NOCHECK, GI_AT_KEYDOWN);
  myGInput->BindKey(GM_ACT_LMTOOL_UP,    GI_KC_MOUSE_LEFT,  GMOD_NOCHECK, GI_AT_KEYUP);
  myGInput->BindKey(GM_ACT_RMTOOL_UP,    GI_KC_MOUSE_RIGHT, GMOD_NOCHECK, GI_AT_KEYUP);
  myGInput->BindKey(GM_ACT_LMTOOL_PRS,   GI_KC_MOUSE_LEFT,  GMOD_NOCHECK, GI_AT_KEYPRESS);
  myGInput->BindKey(GM_ACT_RMTOOL_PRS,   GI_KC_MOUSE_RIGHT, GMOD_NOCHECK, GI_AT_KEYPRESS);

  myGInput->BindKey(GM_ACT_SETNPC,       GI_KC_MOUSE_LEFT,  GMOD_NOCHECK, GI_AT_KEYDOWN);
  myGInput->BindKey(GM_ACT_MOVENPC,      GI_KC_MOUSE_RIGHT, GMOD_NOCHECK, GI_AT_KEYDOWN);

  myGInput->BindKey(GM_ACT_UNDO,         GI_KC_Z,           GMOD_CTRL,    GI_AT_KEYDOWN);
  myGInput->BindKey(GM_ACT_COPY,         GI_KC_C,           GMOD_CTRL,    GI_AT_KEYDOWN);
  myGInput->BindKey(GM_ACT_PASTE,        GI_KC_V,           GMOD_CTRL,    GI_AT_KEYDOWN);

  myGInput->BindKey(GM_ACT_WIREFRAME,    GI_KC_F1,          GMOD_NOCHECK, GI_AT_KEYDOWN);

  return true;
}

//-----------------------------------------------

void MainGame::InitSound()
{
  myAudioMgr = new GAudioMgr();
 
  rVoice = myAudioMgr->AddVoice(myAudioMgr->AddWAVSound("..\\SoundFX\\Remove.wav"));
  sVoice = myAudioMgr->AddVoice(myAudioMgr->AddWAVSound("..\\SoundFX\\Set.wav"));
}

//-----------------------------------------------

void MainGame::InitUI(void (*_uieventproc)(GUI_EVENTPROC_DESC*))
{
  MyUI = new G_UI_MainArea(myGDevice);
  MyUI->SetEventCallback(_uieventproc);
  MyUI->MouseVisible = true;

  /* Добавление элементов UI */
  Test_Add_UI_Elements();

  /* Создаем шрифт */
  GFont *TempFont = myResMgr->AddFont(UID_FONT_FPS, "Courier New", 12, g_color(0.9f,0.9f,0.9f,1.0f));	 

  /* Добавляем рендер таргет для UI*/
  myGDevice->RTList->AddRenderTarget("rt_ui");

  FPSLabel->SetFont(TempFont);
  MouseLabel->SetFont(TempFont);
}

//-----------------------------------------------

void MainGame::LogProc(const string &Message)
{
  time_t seconds = time(NULL);
  tm timeinfo; 
  char outmesg[256];

  localtime_s(&timeinfo, &seconds); 
  sprintf_s(outmesg, 256, "[%02d.%02d %02d:%02d:%02d] %s", timeinfo.tm_mday, timeinfo.tm_mon, 
	        timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, Message.c_str());

  LogLB->Items.push_back(outmesg);

  if (LogLB->Items.size() > G_MAX_LOG_ROWS)
    LogLB->Items.erase(LogLB->Items.begin());

  LogLB->SetScrollPos(G_LB_SCROLL_END);
  LogLB->SetChanged();
}

//-----------------------------------------------

void MainGame::UIEventProc(GUI_EVENTPROC_DESC *eDesc)
{
  GUI_BaseObject *UIObject = reinterpret_cast<GUI_BaseObject*>(eDesc->Object);

  if (UIObject == NULL) return;

  g_int32 UID = UIObject->GetUID();

  switch (eDesc->Message)
  {
    case G_MSG_ONCLICK:
	                   if ((UID == UID_BUTTON_EXIT) || (UID == UID_BUTTON_MM_EXIT))
						 closethread = true;

					   if (UID == UID_BUTTON_MM_EDITOR)
					   {
						 Editor_Init();
						 GUI_Desktop *CurrDesktop = (GUI_Desktop*)MyUI->FindUIObjectByUID(NULL, UID_DESKTOP_EDITOR);
						 CurrDesktop->SetVisible(true);
						 CurrDesktop = (GUI_Desktop*)MyUI->FindUIObjectByUID(NULL, UID_DESKTOP_MAINMENU);
						 CurrDesktop->SetVisible(false);
					   }

					   if (UID == UID_BUTTON_MM_NEWGAME)
					   {
						 Game_Init();
						 GUI_Desktop *CurrDesktop = (GUI_Desktop*)MyUI->FindUIObjectByUID(NULL, UID_DESKTOP_GAME);
						 CurrDesktop->SetVisible(true);
						 CurrDesktop = (GUI_Desktop*)MyUI->FindUIObjectByUID(NULL, UID_DESKTOP_MAINMENU);
						 CurrDesktop->SetVisible(false);
					   }
					   
					   if (UID == UID_BUTTON_EXITTOMENU)
					   {
						 GUI_Desktop *CurrDesktop = (GUI_Desktop*)MyUI->FindUIObjectByUID(NULL, UID_DESKTOP_EDITOR);
						 CurrDesktop->SetVisible(false);
						 CurrDesktop = (GUI_Desktop*)MyUI->FindUIObjectByUID(NULL, UID_DESKTOP_GAME);
						 CurrDesktop->SetVisible(false);
						 CurrDesktop = (GUI_Desktop*)MyUI->FindUIObjectByUID(NULL, UID_DESKTOP_MAINMENU);
						 CurrDesktop->SetVisible(true);
						 Editor_Release();
					   }

                       if (UID == UID_BUTTON_APPLYSET)
					   {
						 g_int32 sel_res = ResolutList->GetSelected();
						 if ((sel_res >= 0) && (sel_res < g_int32(_resolut_list.size())))
						 {
						   PauseRender();
						   SetScreen(_resolut_list[sel_res], false);	
						   ContinueRender();
						 }
					   }

                       if (UID == UID_BUTTON_LOADEDMAP_WW)
					   {
						 PauseRender();
                         MyCT->Release();
						 g_ivec2 mapsz = g_ivec2(0,0);
						 MyCT->InitMap_Test(5,mapsz);
						 MyCT->UpdateSectorsMesh();
						 mapsz = MyCT->GetMapSize();
                         UpdateClipMesh(mapsz);
                         MyPlayer->SetMoveClip(ClipBox);
						 ContinueRender();
					   }

					   if (UID == UID_BUTTON_UPDATEMAP)
					   {
                         g_double ticks = GetTicks();
                         //Start Check Load Speed
						 PauseRender();
                         MyCT->SectorsMgr.CheckAllMapCubeAdj();
                         MyCT->UpdateSectorsMesh();
						 ContinueRender();
                         //End Check Load Speed
                         CheckTimeAndWriteToFile(ticks, "update_map");
					   }

                       if (UID == UID_BUTTON_LOADEDMAP)
					   {
						 PauseRender();
						 MyCT->Release();
						 g_ivec2 mapsz = g_ivec2(0,0);
						 MyCT->InitMap_Test(6,mapsz);
						 MyCT->UpdateSectorsMesh();
						 mapsz = MyCT->GetMapSize();
                         UpdateClipMesh(mapsz);
                         MyPlayer->SetMoveClip(ClipBox);
						 ContinueRender();
					   }

                       if (UID == UID_BUTTON_TEST)
                       {          
						   
                       }

                       if (UID == UID_BUTTON_CLEARFPS)
                       {
                         minfps = 99999;
                         maxfps = 0;
                       }

					   if (UID == UID_BUTTON_CONSOLE)
                       {
                         WinLog->SetVisible(!WinLog->GetVisible());
                       }

                       if (UID == UID_BUTTON_DELOBJ)
                       {
                         g_int8 KeyMods = myGInput->CheckKeyMods();
						 if (((KeyMods & GMOD_CTRL) == GMOD_CTRL) && 
							 (CubeObjLB != NULL) && (CubeObjLB->SelectedItem > -1))
						 {
						   PauseRender();
						   g_string filename = MyCT->SaveMgr.MakeObjectFileName(CubeObjLB->Items[CubeObjLB->SelectedItem]);
						   DeleteFile(filename.c_str());
						   CubeObjLB->Items.erase(CubeObjLB->Items.begin()+CubeObjLB->SelectedItem);
						   CubeObjLB->SetChanged();
						   ContinueRender();
						 }
                       }

                       if (UID == UID_BUTTON_TOOLGRID)
                       {
                         tool_type = CEditorBGrid->GetPressedButton();
                       }

                       if (UID == UID_BUTTON_HIDESIDEBAR)
                       {
						 GUI_Desktop *CurrDesktop = (GUI_Desktop*)MyUI->FindUIObjectByUID(NULL, UID_DESKTOP_EDITOR);
                         GUI_Panel *CurrPanel = (GUI_Panel*)MyUI->FindUIObjectByUID(CurrDesktop, UID_PANEL_SIDEBAR);
                         GUI_Button *HideButton = (GUI_Button*)MyUI->FindUIObjectByUID(CurrDesktop, UID_BUTTON_HIDESIDEBAR);
						 if (HideButton != NULL)
						 {
						   if (CurrPanel->GetVisible() == true)
						   {
						     CurrPanel->SetVisible(false);
                             HideButton->SetPosition(g_ivec2(0, myGDevice->GetHeight() - 50));
							 FPSLabel->SetPosition(g_ivec2(2, 2));
						   }
						   else
						   {
						     CurrPanel->SetVisible(true);
                             HideButton->SetPosition(g_ivec2(CurrPanel->GetSize().x, myGDevice->GetHeight() - 50));
							 FPSLabel->SetPosition(g_ivec2(CurrPanel->GetSize().x + 2, 2));
						   }
						 }
                       }

					   if (UID == UID_BUTTON_MENUGRID)
                       {
                         g_int32 page = MenuBGrid->GetPressedButton();
						 
						 switch (page)
						 {
						   case 0:{
                                    GUI_RadioButton *MapEdRadBtn = (GUI_RadioButton*)MyUI->FindUIObjectByUID(NULL, UID_RADIOBTN_EDITMAP);
                                    if (MapEdRadBtn != NULL) 
									{
									  if ((GUI_RadioButton*)MapEdRadBtn->IsChecked())
                                        game_mode = G_GAMEMODE_EDITOR_CUBE;
									  else
										game_mode = G_GAMEMODE_EDITOR_OBJECT;
									}
								  } break;
						   case 1:{
							        game_mode = G_GAMEMODE_EDITOR_NPC;
								  } break;
						 }

						 GUI_Desktop *CurrDesktop = (GUI_Desktop*)MyUI->FindUIObjectByUID(NULL, UID_DESKTOP_EDITOR);
						 GUI_Panel   *CurrPanel;

						 CurrPanel = (GUI_Panel*)MyUI->FindUIObjectByUID(CurrDesktop, UID_WINDOW_EDITORMODE);
						 CurrPanel->SetVisible(page == 0);

						 CurrPanel = (GUI_Panel*)MyUI->FindUIObjectByUID(CurrDesktop, UID_WINDOW_TOOLS);
						 CurrPanel->SetVisible(page == 0);

						 CurrPanel = (GUI_Panel*)MyUI->FindUIObjectByUID(CurrDesktop, UID_WINDOW_OBJECTS);
						 CurrPanel->SetVisible(page == 0);

						 CurrPanel = (GUI_Panel*)MyUI->FindUIObjectByUID(CurrDesktop, UID_WINDOW_CUBEOBJ);
						 CurrPanel->SetVisible(page == 0);

						 CurrPanel = (GUI_Panel*)MyUI->FindUIObjectByUID(CurrDesktop, UID_WINDOW_NPC);
						 CurrPanel->SetVisible(page == 1);

						 CurrPanel = (GUI_Panel*)MyUI->FindUIObjectByUID(CurrDesktop, UID_WINDOW_SETTINGS);
						 CurrPanel->SetVisible(page == 2);

						 CurrPanel = (GUI_Panel*)MyUI->FindUIObjectByUID(CurrDesktop, UID_WINDOW_LIGHTCAM);
						 CurrPanel->SetVisible(page == 2);

						 CurrPanel = (GUI_Panel*)MyUI->FindUIObjectByUID(CurrDesktop, UID_WINDOW_OTHERSET);
						 CurrPanel->SetVisible(page == 2);

						 SaveLoadWindow->SetVisible(false);
                       }

					   if (UID == UID_RADIOBTN_EDITMAP)
                       {
                         game_mode = G_GAMEMODE_EDITOR_CUBE;
						 PauseRender();
                         MyCT->Release();
						 ContinueRender();
                       }

					   if (UID == UID_RADIOBTN_EDITOBJ)
                       {
                         game_mode = G_GAMEMODE_EDITOR_OBJECT;

						 PauseRender();
                         MyCT->Release();
						 MyCT->InitMap_Test(3, g_ivec2(9,9));
						 MyCT->UpdateSectorsMesh();
						 ContinueRender();

                         UpdateClipMesh(g_ivec2(9,9));
                         MyPlayer->SetMoveClip(ClipBox);
                       }

                       if (UID == UID_CHECKBOX_HEIGHT)
                       {
                         cam_heightalign = !((GUI_CheckBox*)eDesc->Object)->IsChecked();
                         MyPlayer->MoveInYCoord = !cam_heightalign;
                       }

                       if (UID == UID_CHECKBOX_CUBEALIGN)
                       {
                         sel_cubealign = !((GUI_CheckBox*)eDesc->Object)->IsChecked();
                       }

                       if (UID == UID_CHECKBOX_FIX_Y)
                       {
						 if (!cam_heightalign)
                           MyPlayer->MoveInYCoord = ((GUI_CheckBox*)eDesc->Object)->IsChecked();
						 else
						   ((GUI_CheckBox*)eDesc->Object)->SetCheck(true);
                       }

                       if (UID == UID_CHECKBOX_SHDMAP)
                       {
						 SetShadowmap(!((GUI_CheckBox*)eDesc->Object)->IsChecked());
					   }

					   if (UID == UID_CHECKBOX_FOG)
                       {
						 SetFog(!((GUI_CheckBox*)eDesc->Object)->IsChecked());
						 
					   }

                       if (game_mode == G_GAMEMODE_EDITOR_OBJECT)
                       {
                         if (UID == UID_BUTTON_SAVEMAP)
                         {
						   PauseRender();
                           SaveLoadLabel->SetCaption("-");
                           if (!MyCT->SaveCroppedObject(SaveLoadEdit->Text.GetText().c_str()))
                             SaveLoadLabel->SetCaption("Error");
                           else
                           {
                             SaveLoadWindow->SetVisible(false);
                             
                             g_char FindMask[300];
                             sprintf_s(FindMask,300,SAVE_OBJ_PATH_TEMPLATE,"*","pobj");
                             CubeObjLB->FillFromDirectory(FindMask,false,false,5);
                           }
						   ContinueRender();
                         }

                         if (UID == UID_BUTTON_LOADMAP)
                         {
						   PauseRender();
                           MyCT->Release();
						   MyCT->InitMap_Test(3, g_ivec2(9,9));
                           SaveLoadLabel->SetCaption("-");
                           if (!MyCT->LoadCubeObject(SaveLoadEdit->Text.GetText().c_str(), g_ivec3(-400,-120,-400)))
                             SaveLoadLabel->SetCaption("Error");
                           else
                             SaveLoadWindow->SetVisible(false);
						   ContinueRender();
                         }

                         if (UID == UID_BUTTON_SAVELOAD)
                         {
                           g_char FindMask[300];
                           sprintf_s(FindMask,300,SAVE_OBJ_PATH_TEMPLATE,"*","pobj");
						   PauseRender();
                           SaveLoadList->FillFromDirectory(FindMask,false,false,5);
                           SaveLoadLabel->SetCaption("-");
                           SaveLoadWindow->SetVisible(true);
						   ContinueRender();
                         }

                         if (UID == UID_BUTTON_CANCELSL)
                         {
                           SaveLoadWindow->SetVisible(false);
                         }
                       }

                       if (game_mode == G_GAMEMODE_EDITOR_CUBE)
                       {
                         if (UID == UID_BUTTON_SAVEMAP)
                         {               
						   g_double ticks = GetTicks();
                           //Start Check Load Speed
                           PauseRender();
                           SaveLoadLabel->SetCaption("-");
                           if (!MyCT->SaveMap(SaveLoadEdit->Text.GetText()))
                             SaveLoadLabel->SetCaption("Error");
                           else
                             SaveLoadWindow->SetVisible(false);
						   ContinueRender();
                           //End Check Load Speed
                           CheckTimeAndWriteToFile(ticks, "save_map");
                         }

                         if (UID == UID_BUTTON_LOADMAP)
                         {
                           g_double ticks = GetTicks();
                           //Start Check Load Speed
						   PauseRender();
                           MyCT->Release();
                           
                           SaveLoadLabel->SetCaption("-");
                           G_MAP_INFO map_info;
                           if (!MyCT->LoadMap(SaveLoadEdit->Text.GetText(),map_info))
                             SaveLoadLabel->SetCaption("Error");
                           else
                             SaveLoadWindow->SetVisible(false);

                           MyCT->UpdateSectorsMesh();
                           UpdateClipMesh(map_info.map_size);
                           MyPlayer->SetMoveClip(ClipBox);
						   ContinueRender();
                           //End Check Load Speed
                           CheckTimeAndWriteToFile(ticks, "load_map");
                         }

                         if (UID == UID_BUTTON_SAVELOAD)
                         {
                           g_char FindMask[300];
                           sprintf_s(FindMask,300,SAVE_MAP_PATH"%s","*.*");
						   PauseRender();
						   SaveLoadList->FillFromDirectory(FindMask,false,true,0);
                           SaveLoadLabel->SetCaption("-");
                           SaveLoadWindow->SetVisible(true); 
						   ContinueRender();
                         }

                         if (UID == UID_BUTTON_CANCELSL)
                         {
                           SaveLoadWindow->SetVisible(false);
                         }
                       }
	                   break;
	case G_MSG_ONSETVALUE:
	                   if (UID == UID_TRACKBAR_SPEED)
                         if (MyPlayer != NULL)
						   MyPlayer->SetSpeed(g_float(((GUI_TrackBar*)UIObject)->GetBarPosition()*5));
                       
					   if (UID == UID_TRACKBAR_LCAMX)
					   {			 
						 LightAng.x = g_float(((GUI_TrackBar*)UIObject)->GetBarPosition());
						 if (MainScene != NULL)
						 {
						   PauseRender();
						   GCamera *Light_Cam = myResMgr->GetCameraByUID(UID_CAM_LIGHTCAM);
						   if (Light_Cam != NULL)
						     Light_Cam->RotateAround(world_center, sun_dist, LightAng.x*G_PI_180, LightAng.y*G_PI_180);
						   ContinueRender();
						 }
						 
					   }

					   if (UID == UID_TRACKBAR_LCAMZ)
					   {				 
						 LightAng.y = g_float(((GUI_TrackBar*)UIObject)->GetBarPosition());
						 if (MainScene != NULL)
						 {
						   PauseRender();
						   GCamera *Light_Cam = myResMgr->GetCameraByUID(UID_CAM_LIGHTCAM);
						   if (Light_Cam != NULL)
						     Light_Cam->RotateAround(world_center, sun_dist, LightAng.x*G_PI_180, LightAng.y*G_PI_180);
						   ContinueRender();
						 }
					   }

                       if (UID == UID_TRACKBAR_FOV)
                       {
						 SetFOV(((GUI_TrackBar*)UIObject)->GetBarPosition());
                       }

                       if (UID == UID_TRACKBAR_DIST)
                         if (MyCT != NULL)
                           MyCT->SetDrawDist(((GUI_TrackBar*)UIObject)->GetBarPosition());

	                   break;
    case G_MSG_ONSELECTITEM:
		               if (UID == UID_SCROLLLIST_GROUPS)
					   {
						 g_uint16 grp_id = GroupsList->GetSelected();
						 if (grp_id < GroupsList->Items.size())
						 {
					       CubesLB->ClearItems();
						   for (g_uint16 i = 0; i<MyCT->Objects.ObjGroups[grp_id].nObjects; i++) 
                           {
                             g_char itemname[200];
	                         sprintf_s(itemname,200,"%d.%s",i+1,
						  	     MyCT->Objects.Objects[MyCT->Objects.ObjGroups[grp_id].ObjIDList[i]].Name);
                             CubesLB->Items.push_back(itemname);
                           }
                           CubesLB->SelectedItem = 0;
						 }
					   }

                       if (UID == UID_LISTBOX_LISTOBJ)
                       {
                         if (eDesc->Prms[0] >= 0)
                         {
                           SaveLoadEdit->Text.SetText(SaveLoadList->Items[eDesc->Prms[0]]);
                           SaveLoadEdit->UpdateText();
                         }
                       }

                       if (UID == UID_LISTBOX_CUBEOBJ)
                       {
                         if (eDesc->Prms[0] >= 0)
						 {
						   g_string fn = MyCT->SaveMgr.MakeObjectFileName(CubeObjLB->Items[eDesc->Prms[0]]);
                           MyCT->SaveMgr.LoadObject(fn, loaded_object);
						 }
                       }

	                   if (UID == UID_LISTBOX_TEXTURE)
                       {
                         RenderCubeObjectPreview();
                       }
                       break;
    case G_MSG_ONKEYUP:
                       if (UID == UID_EDIT_CONSOLE)
					   {
                         if (eDesc->Prms[0] == VK_RETURN)
						 {
						   string inCmd = ConsoleEdit->Text.GetText();					   
                           ConsoleInput(inCmd);
						   ConsoleEdit->Text = "";

						   console_history.push_back(inCmd);
						   if (console_history.size() > G_MAX_CONSOLE_HISTORY_SIZE)
							 console_history.erase(console_history.begin());
						   console_history_index = 0;
						 }

						 if ((eDesc->Prms[0] == VK_DOWN) && (console_history.size() > 0))
						 {
						   if (console_history_index >= console_history.size())
                             console_history_index = 0;

						   ConsoleEdit->Text.SetText(console_history[console_history_index]);
						   ConsoleEdit->UpdateText();
						   console_history_index++;						 
						 }

						 if ((eDesc->Prms[0] == VK_UP) && (console_history.size() > 0))
						 {
						   if (console_history_index > 0)
						     console_history_index--;
						   else
							 console_history_index = console_history.size() - 1;

						   if (console_history_index >= console_history.size())
                             console_history_index = console_history.size() - 1;

						   ConsoleEdit->Text.SetText(console_history[console_history_index]);
						   ConsoleEdit->UpdateText();					   
						 }
					   }
                       break;
    default: break;
  }
}

//-----------------------------------------------

void MainGame::MsgProc(HWND hwnd, g_uint32 message, WPARAM wParam, LPARAM lParam)
{
  if (MyUI != NULL) MyUI->MsgProc(hwnd,message,wParam,lParam);

  switch (message)                 
  {
	case WM_DESTROY:
			if (DeviceCreated)
			{
		      DeviceCreated = false;
			  //FREENULLRELEASE(Physics);
			  //FREENULL_VAR(FFVideo);
			  FREENULLRELEASE(MyNPCMgr);
			  FREENULL_VAR(MyPlayer);
			  FREENULLRELEASE(MyCT);
              FREENULLRELEASE(MyUI);
			  FREENULLRELEASE(MainScene);
			  FREENULLRELEASE(myGInput);
			  FREENULLRELEASE(myAudioMgr);
              FREENULL_VAR(myResMgr);
			  FREENULLRELEASE(myGDevice);
			}
			timeEndPeriod(1);
            break; 
    case WM_ACTIVATE: 
		    if (DeviceCreated)
			{
              if(LOWORD(wParam)==WA_ACTIVE) myGInput->Acquire();
              if(LOWORD(wParam)==WA_CLICKACTIVE) myGInput->Acquire();
			  if(LOWORD(wParam)==WA_INACTIVE) myGInput->Unacquire();
			}
            break;
  }
}

//-----------------------------------------------

void MainGame::StartRenderCycle()
{
  while (!closethread)
  {
    if (pause_render)
    {
      is_paused_render = true;
      while (pause_render) Sleep(1);
      is_paused_render = false;
    }

	if (DeviceCreated)
	{
	  MainScene->CheckFPS(0);
	  
	  if (MyCT != NULL)
	    if (_settings.use_shadowmap)
	      RenderLightCam();

      myGDevice->ClearView();

      GCamera *pRenderCam = MainScene->GetDefaultCamera();

	  if (MyCT != NULL)
	    MyCT->Draw(MainScene->GetWorldMatrix(), pRenderCam);
	  if (MyNPCMgr != NULL)
        MyNPCMgr->Draw();

      MainScene->Render();

	  MyUI->DrawUI(MainScene->GetWorldMatrix(), pRenderCam);

      myGDevice->SwapBuffer();
	}
  }
  closeapp = true;
}

//-----------------------------------------------

void MainGame::Input_CubeEditor(g_bool HaveEvent)
{
  g_int8 KeyMods = myGInput->CheckKeyMods();
  
  g_uint16 obj_index = GetSelectedCubeIndex();

  //Undo
  if (myGInput->CheckAction(GM_ACT_UNDO))
  {
	G_UNI_CUBE_OBJECT loaded_undo;
	if (MyCT->SaveMgr.LoadUndo("undo", loaded_undo))
	{
	  PauseRender();
	  MyCT->SaveMgr.SetObjectToMap(loaded_undo, 0, true, false, false);
      MyCT->UpdateSectorsMesh();
      ContinueRender();	  
	}
  }
  
  //Включение/выключение мыши
  if (myGInput->CheckAction(GM_ACT_MAPROTENABLE))
  {
	MyUI->MouseVisible = false;
    MyPlayer->MouseEnabled = true;
  }
  else
  {
    MyUI->MouseVisible = true;
    MyPlayer->MouseEnabled = false;
  }

  //Wireframe
  if (myGInput->CheckAction(GM_ACT_WIREFRAME))
  {
	SetWireframe(!_settings.use_wireframe);
  }

  if ((!sel_cubealign) && (!MyUI->MouseVisible))
  {
    g_ivec3 abs_cpos;
    g_vec3 cube_vec = MyPlayer->GetPosition() + MyPlayer->GetDirection()*15;
    abs_cpos = GetAbsCubePosByVec3(cube_vec);

    g_bool CubeFound = (MyCT->IsValidCube(abs_cpos) >= 0);

    if (!CubeFound)
      return;

    g_vec3 respos = AbsCubePosToRealPos(abs_cpos);

	GMeshInstance *Sel_Mesh_Inst = myResMgr->GetModelByUID(UID_MESH_SEL)->GetInstance(0);
    if (Sel_Mesh_Inst != NULL)
    {
      Sel_Mesh_Inst->SetScaling(csize/2);
      Sel_Mesh_Inst->SetPosition(respos);
    }
  }

  if (MyUI->MouseVisible)
  {
    g_ivec3 abs_cpos, prev_abs_cpos, real_abs_cpos, prev_real_abs_cpos;
    g_vec3 c_pos,c_dir;
      
    MyPlayer->GetCamera()->GetRayFromScreenPos(g_ivec2(myGInput->AX,myGInput->AY),
                                               MainScene->GetWorldMatrix(),&c_pos,&c_dir);
    g_bool CubeFound = false, RealCubeFound = false;
    if (sel_cubealign)
    {
      CubeFound = MyCT->GetTargetCube(c_pos,c_dir, abs_cpos, prev_abs_cpos);
	  RealCubeFound = MyCT->GetTargetCube(c_pos,c_dir, real_abs_cpos, prev_real_abs_cpos, true);
    }
    else
    {
      g_vec3 cube_vec = MyPlayer->GetPosition() + MyPlayer->GetDirection()*15;
      abs_cpos = GetAbsCubePosByVec3(cube_vec);
      prev_abs_cpos = abs_cpos;
      CubeFound = (MyCT->IsValidCube(abs_cpos) >= 0);
    }

    if (!CubeFound)
      return;
   
    g_vec3 respos = AbsCubePosToRealPos(abs_cpos);
        
    GMeshInstance *Sel_Mesh_Inst = myResMgr->GetModelByUID(UID_MESH_SEL)->GetInstance(0);
       
    switch (tool_type)
    {
      case 4:
          if (Sel_Mesh_Inst != NULL)
          {
            g_ivec3 losz = loaded_object._size + 1;
            g_uint8 rot  = MyCT->GetCubeRotFromDir(MyPlayer->GetDirection());
            g_vec3  pos  = AbsCubePosToRealPos(prev_abs_cpos);
            
            if ((rot == 1) || (rot == 3))
            {
              Sel_Mesh_Inst->SetScaling(g_vec3((losz.z/2.0f)*csize.x,
                                          (losz.y/2.0f)*csize.y,
                                          (losz.x/2.0f)*csize.z));
              if (losz.z % 2 == 0) pos.x += csize.x/2.0f;            
              if (losz.x % 2 == 0) pos.z += csize.z/2.0f;
            }
            else
            {
              Sel_Mesh_Inst->SetScaling(g_vec3((losz.x/2.0f)*csize.x,
                                          (losz.y/2.0f)*csize.y,
                                          (losz.z/2.0f)*csize.z)); 
              if (losz.x % 2 == 0) pos.x += csize.x/2.0f;           
              if (losz.z % 2 == 0) pos.z += csize.z/2.0f;
            }

            pos.y += (losz.y*csize.z/2.0f) - csize.y/2.0f;

            Sel_Mesh_Inst->SetPosition(pos);
          }
          break;
      default:
          if (Sel_Mesh_Inst != NULL)
          {
            Sel_Mesh_Inst->SetScaling(csize/2);
            Sel_Mesh_Inst->SetPosition(respos);
          }
          break;
    }

    g_ivec3 spos,cpos; 
	    
    if (!HaveEvent)
    {
        //Событие на нажатие клавиши
        if (myGInput->CheckAction(GM_ACT_LMTOOL_DOWN))
        {
          switch (tool_type)
          {
            case 0:
              {
				PauseRender();
                GetSectorAndCubePosByAbsCubePos(prev_abs_cpos,spos,cpos);				
                MyCT->SetCube(spos,cpos,obj_index+1,MyCT->GetCubeRotFromDir(MyPlayer->GetDirection()));
                ContinueRender();
				if (_settings.use_sound) sVoice->Play(); // Test
              }
              break;
            case 1:
            case 2:
            case 3:
            case 5:
			case 7:
              {
				sel_started = true;
                sel_start = AbsCubePosToRealPos(prev_abs_cpos);
                sel_start_acpos = prev_abs_cpos;
              }
              break;
			case 9:
              {
				sel_started = true;
                sel_start = AbsCubePosToRealPos(abs_cpos);
                sel_start_acpos = abs_cpos;
              }
              break;
			case 6:
            case 15:
			  {
				sel_started = true;
			  }
			  break;
            case 4:
              {
                if ((CubeObjLB != NULL) && (CubeObjLB->SelectedItem != -1))
                {
				  PauseRender();
				  loaded_object._pos = prev_abs_cpos;
                  MyCT->SaveMgr.SetObjectToMap(loaded_object, MyCT->GetCubeRotFromDir(MyPlayer->GetDirection()), false, true, true);
                  MyCT->UpdateSectorsMesh();
                  ContinueRender();
                }
              }
			  break;
			case 8:
			  {
                PauseRender();
                GetSectorAndCubePosByAbsCubePos(abs_cpos, spos, cpos);
                g_uint16 curr_cube = MyCT->SectorsMgr.GetSector(spos)->Cube(cpos);
				ContinueRender();
				if ((curr_cube > 0) && (curr_cube < CubesLB->Items.size()))
				  CubesLB->OnSelectItem(curr_cube-1);
			  }
              break;
          }
        }
        
        //Событие на удержание клавиши
        if ((myGInput->CheckAction(GM_ACT_LMTOOL_PRS)) && (sel_started))
        {
          switch (tool_type)
          {
            case 1:
            case 2:
            case 3:
			case 7:
              {
                g_vec3 sel_cent, sel_scale = g_vec3(0,0,0);
                
                if ((KeyMods & GMOD_ALT) != GMOD_ALT)
                {
                  sel_end = AbsCubePosToRealPos(sel_start_acpos);
                  sel_end_acpos = sel_start_acpos;
                }

                if (KeyMods == GMOD_ALT)
                {
                  MyCT->GetTargetCube(c_pos, c_dir, abs_cpos, prev_abs_cpos, false, 1, sel_start_acpos.x);
                  sel_end.y = AbsCubePosToRealPos(abs_cpos).y;
                  sel_end_acpos.y = abs_cpos.y;
                }

                if (KeyMods == GMOD_ALT + GMOD_SHIFT)
                {
                  MyCT->GetTargetCube(c_pos, c_dir, abs_cpos, prev_abs_cpos, false, 1, sel_start_acpos.y);
                  sel_end.x = AbsCubePosToRealPos(abs_cpos).x;
                  sel_end_acpos.x = abs_cpos.x; 
                }

                if (KeyMods == GMOD_ALT + GMOD_CTRL)
                {
                  MyCT->GetTargetCube(c_pos, c_dir, abs_cpos, prev_abs_cpos, false, 1, sel_start_acpos.y);
                  sel_end.z = AbsCubePosToRealPos(abs_cpos).z;
                  sel_end_acpos.z = abs_cpos.z;
                }

                if (KeyMods == GMOD_SHIFT)
                {
                  MyCT->GetTargetCube(c_pos, c_dir, abs_cpos, prev_abs_cpos, false, 1, sel_start_acpos.x);
                  sel_end = AbsCubePosToRealPos(abs_cpos);
                  sel_end_acpos = abs_cpos;
                }

                if (KeyMods == GMOD_CTRL)
                {
                  MyCT->GetTargetCube(c_pos, c_dir, abs_cpos, prev_abs_cpos, false, 3, sel_start_acpos.z);
                  sel_end = AbsCubePosToRealPos(abs_cpos);
                  sel_end_acpos = abs_cpos;
                }

                if (KeyMods == GMOD_NONE)
                {
                  MyCT->GetTargetCube(c_pos, c_dir, abs_cpos, prev_abs_cpos, false, 2, sel_start_acpos.y);
                  sel_end = AbsCubePosToRealPos(abs_cpos);
                  sel_end_acpos = abs_cpos;
                }

				g_int32 maxlen = G_MAX_SEL_LENGTH;
				if (tool_type == 7) maxlen = G_MAX_SEL_LENGTH_COPY;

				g_int32 sel_len = sel_end_acpos.x - sel_start_acpos.x;
				if (abs(sel_len) > maxlen)
				  sel_end_acpos.x = sel_start_acpos.x + (sel_len/abs(sel_len))*maxlen;

				sel_len = sel_end_acpos.y - sel_start_acpos.y;
				if (abs(sel_len) > maxlen)
				  sel_end_acpos.y = sel_start_acpos.y + (sel_len/abs(sel_len))*maxlen;

				sel_len = sel_end_acpos.z - sel_start_acpos.z;
				if (abs(sel_len) > maxlen)
				  sel_end_acpos.z = sel_start_acpos.z + (sel_len/abs(sel_len))*maxlen;
				
				sel_end = AbsCubePosToRealPos(sel_end_acpos);

                sel_scale.y = sel_end.y - sel_start.y; 
                sel_scale.x = sel_end.x - sel_start.x; 
                sel_scale.z = sel_end.z - sel_start.z;

                sel_cent.x = sel_start.x + sel_scale.x/2.0f;
                sel_cent.y = sel_start.y + sel_scale.y/2.0f;
                sel_cent.z = sel_start.z + sel_scale.z/2.0f;
                AbsVec(&sel_scale);

                Sel_Mesh_Inst->SetScaling((sel_scale + csize)/2.0f);
                Sel_Mesh_Inst->SetPosition(sel_cent); 

                g_char Text[200];
                sprintf_s(Text,200,"%d  %d  %d",
                          abs(sel_start_acpos.x - sel_end_acpos.x) + 1,
                          abs(sel_start_acpos.y - sel_end_acpos.y) + 1,
                          abs(sel_start_acpos.z - sel_end_acpos.z) + 1);
                MouseLabel->SetCaption(Text);
              }
              break;
            case 5:
              {
                g_vec3 sel_cent, sel_scale = g_vec3(0,0,0);
                
                if (KeyMods == GMOD_SHIFT)
                {
                  MyCT->GetTargetCube(c_pos, c_dir,abs_cpos, prev_abs_cpos, false, 1, sel_start_acpos.x);
                  AlignSizeToMin(sel_start_acpos.y, sel_start_acpos.z, abs_cpos.y, abs_cpos.z);
                  sel_end = AbsCubePosToRealPos(abs_cpos);
                  sel_end_acpos = abs_cpos;
                }

                if (KeyMods == GMOD_CTRL)
                {
                  MyCT->GetTargetCube(c_pos, c_dir,abs_cpos, prev_abs_cpos, false, 3, sel_start_acpos.z);
                  AlignSizeToMin(sel_start_acpos.x, sel_start_acpos.y, abs_cpos.x, abs_cpos.y);
                  sel_end = AbsCubePosToRealPos(abs_cpos);
                  sel_end_acpos = abs_cpos;
                }

                if (KeyMods == GMOD_NONE)
                {
                  MyCT->GetTargetCube(c_pos, c_dir,abs_cpos, prev_abs_cpos, false, 2, sel_start_acpos.y);
                  AlignSizeToMin(sel_start_acpos.x, sel_start_acpos.z, abs_cpos.x, abs_cpos.z);
                  sel_end = AbsCubePosToRealPos(abs_cpos);
                  sel_end_acpos = abs_cpos;
                }

				g_int32 sel_len = sel_end_acpos.x - sel_start_acpos.x;
				if (abs(sel_len) > G_MAX_SEL_LENGTH)
				  sel_end_acpos.x = sel_start_acpos.x + (sel_len/abs(sel_len))*G_MAX_SEL_LENGTH;

				sel_len = sel_end_acpos.y - sel_start_acpos.y;
				if (abs(sel_len) > G_MAX_SEL_LENGTH)
				  sel_end_acpos.y = sel_start_acpos.y + (sel_len/abs(sel_len))*G_MAX_SEL_LENGTH;

				sel_len = sel_end_acpos.z - sel_start_acpos.z;
				if (abs(sel_len) > G_MAX_SEL_LENGTH)
				  sel_end_acpos.z = sel_start_acpos.z + (sel_len/abs(sel_len))*G_MAX_SEL_LENGTH;

				sel_end = AbsCubePosToRealPos(sel_end_acpos);

                sel_scale.y = sel_end.y - sel_start.y; 
                sel_scale.x = sel_end.x - sel_start.x; 
                sel_scale.z = sel_end.z - sel_start.z;

                sel_cent.x = g_float(sel_start.x) + sel_scale.x/2.0f;
                sel_cent.y = g_float(sel_start.y) + sel_scale.y/2.0f;
                sel_cent.z = g_float(sel_start.z) + sel_scale.z/2.0f;
                AbsVec(&sel_scale);

                Sel_Mesh_Inst->SetScaling((sel_scale + csize)/2.0f);
                Sel_Mesh_Inst->SetPosition(sel_cent); 

                g_char Text[200];
                sprintf_s(Text,200,"%d  %d  %d",
                          abs(sel_start_acpos.x - sel_end_acpos.x) + 1,
                          abs(sel_start_acpos.y - sel_end_acpos.y) + 1,
                          abs(sel_start_acpos.z - sel_end_acpos.z) + 1);
                MouseLabel->SetCaption(Text);
              }
              break;
            case 6:
              {
				PauseRender();
                GetSectorAndCubePosByAbsCubePos(abs_cpos,spos,cpos);
                MyCT->SetCube(spos,cpos,obj_index+1,MyCT->GetCubeRotFromDir(MyPlayer->GetDirection()),false);
                if (GetTicks() - upd_time > 50)
                {
                  upd_time = GetTicks();
                  MyCT->UpdateSectorsMesh();
                }
				ContinueRender();
              }
              break;
			case 9:
			  {
				g_vec3 sel_cent, sel_scale = g_vec3(0,0,0);

                MyCT->GetTargetCube(c_pos,c_dir,abs_cpos, prev_abs_cpos,false,2,sel_start_acpos.y);
                sel_end_acpos = abs_cpos;

				g_int32 x_len = sel_end_acpos.x - sel_start_acpos.x;
				g_int32 z_len = sel_end_acpos.z - sel_start_acpos.z;
				
				sel_scale.y = 6*csize.x;
				sel_cent.y = g_float(sel_start.y) + sel_scale.y/2.0f;

				if (abs(x_len) > abs(z_len))
				{
				  sel_end_acpos.z = sel_start_acpos.z;
				  if (abs(x_len) > G_MAX_SEL_LENGTH)
				  sel_end_acpos.x = sel_start_acpos.x + (x_len/abs(x_len))*G_MAX_SEL_LENGTH;

				  sel_end = AbsCubePosToRealPos(sel_end_acpos);

				  sel_scale.x = sel_end.x - sel_start.x; 
                  sel_scale.z = 6*csize.z;

				  sel_cent.x = g_float(sel_start.x) + sel_scale.x/2.0f;         
                  sel_cent.z = g_float(sel_start.z);
				}
				else
				{
				  sel_end_acpos.x = sel_start_acpos.x;
				  if (abs(z_len) > G_MAX_SEL_LENGTH)
				  sel_end_acpos.z = sel_start_acpos.z + (z_len/abs(z_len))*G_MAX_SEL_LENGTH;

				  sel_end = AbsCubePosToRealPos(sel_end_acpos);

				  sel_scale.x = 6*csize.x; 
                  sel_scale.z = sel_end.z - sel_start.z;

				  sel_cent.x = g_float(sel_start.x);         
                  sel_cent.z = g_float(sel_start.z) + sel_scale.z/2.0f;
				}

                AbsVec(&sel_scale);

                Sel_Mesh_Inst->SetScaling((sel_scale + csize)/2.0f);
                Sel_Mesh_Inst->SetPosition(sel_cent); 
			  }
			  break;
			case 15:
              {
                if (GetTicks() - upd_time > 50)
                {
                  upd_time = GetTicks();
				  PauseRender();
                  MyCT->FillAreaInvisible(real_abs_cpos - g_ivec3(19,19,19), real_abs_cpos + g_ivec3(19,19,19));
                  ContinueRender();
				}            
              }
              break;
          }
        }

        //Событие на отжатие клавиши
        if ((myGInput->CheckAction(GM_ACT_LMTOOL_UP)) && (sel_started))
        {
          MouseLabel->SetCaption("");
		  sel_started = false;
          switch (tool_type)
          {
            case 1:
              {
				PauseRender();
                MyCT->FillArea(sel_start_acpos, sel_end_acpos, obj_index+1,
                               MyCT->GetCubeRotFromDir(MyPlayer->GetDirection()));
				ContinueRender();
              }
              break;
            case 2:
              {
				PauseRender();
                MyCT->FillArea(sel_start_acpos, sel_end_acpos, obj_index+1,
                               MyCT->GetCubeRotFromDir(MyPlayer->GetDirection()), true, true, 
                               G_CT_AREATYPE_SPHERE);
				ContinueRender();
              }
              break;
            case 3:
              {     
				PauseRender();
                if ((KeyMods == GMOD_ALT) || (KeyMods == GMOD_NONE))
                  MyCT->FillArea(sel_start_acpos, sel_end_acpos, obj_index+1,
                                MyCT->GetCubeRotFromDir(MyPlayer->GetDirection()), true, true, 
                                G_CT_AREATYPE_CYLINDER_Y);

                if ((KeyMods == GMOD_ALT + GMOD_SHIFT) || (KeyMods == GMOD_SHIFT))
                  MyCT->FillArea(sel_start_acpos, sel_end_acpos, obj_index+1,
                                MyCT->GetCubeRotFromDir(MyPlayer->GetDirection()), true, true, 
                                G_CT_AREATYPE_CYLINDER_X);                

                if ((KeyMods == GMOD_ALT + GMOD_CTRL) || (KeyMods == GMOD_CTRL))
                  MyCT->FillArea(sel_start_acpos, sel_end_acpos, obj_index+1,
                                MyCT->GetCubeRotFromDir(MyPlayer->GetDirection()), true, true, 
                                G_CT_AREATYPE_CYLINDER_Z); 
				ContinueRender();
              }
              break;
			case 5:
              {
				PauseRender();
                MyCT->FillLine(sel_start_acpos, sel_end_acpos, obj_index+1,
                               MyCT->GetCubeRotFromDir(MyPlayer->GetDirection()), true, true);
				ContinueRender();
              }
              break;
            case 6:
              {
                PauseRender();
                MyCT->UpdateSectorsMesh();
				ContinueRender();
              }
			  break;
			case 7:
              {
                G_UNI_CUBE_OBJECT uni_obj;
				PauseRender();
				if (MyCT->SaveMgr.GetObjectFromMap(uni_obj, sel_start_acpos, sel_end_acpos))
				{
				  g_string fn = MyCT->SaveMgr.MakeObjectFileName("!Copy");
				  MyCT->SaveMgr.SaveObject(fn, uni_obj);

				  g_char FindMask[300];
                  sprintf_s(FindMask,300,SAVE_OBJ_PATH_TEMPLATE,"*","pobj");
                  CubeObjLB->FillFromDirectory(FindMask,false,false,5);
				}
				ContinueRender();
              }
			  break;
			case 9:
              {
				PauseRender();
                MyCT->MakeRoadV1(sel_start_acpos, sel_end_acpos, obj_index+1, obj_index+1, true);
				ContinueRender();
              }
              break;
			case 15:
              {
				PauseRender();
                MyCT->FillAreaInvisible(g_ivec3(0,0,0), g_ivec3(0,0,0));
				ContinueRender();
              }
              break;
		  break;
          }
        }

        // Удаление кубика
	    if (myGInput->CheckAction(GM_ACT_RMTOOL_DOWN))
        {
          switch (tool_type)
          {
            case 1:
            case 2:
            case 3:
              {
                sel_start = AbsCubePosToRealPos(abs_cpos);
                sel_start_acpos = abs_cpos;
              }
              break;
            default:
              {
				PauseRender();
                GetSectorAndCubePosByAbsCubePos(abs_cpos, spos, cpos);
                MyCT->RemoveCube(spos, cpos);
				ContinueRender();
                if (_settings.use_sound) rVoice->Play(); // Test
              }
              break;
          }        
        }    

        if (myGInput->CheckAction(GM_ACT_RMTOOL_PRS))
        {
          switch (tool_type)
          {
            case 1:
            case 2:
            case 3:
              {
                g_vec3 sel_cent, sel_scale = g_vec3(0,0,0);
                
                if ((KeyMods & GMOD_ALT) != GMOD_ALT)
                {
                  sel_end = AbsCubePosToRealPos(sel_start_acpos);
                  sel_end_acpos = sel_start_acpos;
                }

                if (KeyMods == GMOD_ALT)
                {
                  MyCT->GetTargetCube(c_pos,c_dir,abs_cpos, prev_abs_cpos,false,1,sel_start_acpos.x);
                  sel_end.y = AbsCubePosToRealPos(abs_cpos).y;
                  sel_end_acpos.y = abs_cpos.y;
                }

                if (KeyMods == GMOD_ALT + GMOD_SHIFT)
                {
                  MyCT->GetTargetCube(c_pos,c_dir,abs_cpos, prev_abs_cpos,false,1,sel_start_acpos.y);
                  sel_end.x = AbsCubePosToRealPos(abs_cpos).x;
                  sel_end_acpos.x = abs_cpos.x;
                }

                if (KeyMods == GMOD_ALT + GMOD_CTRL)
                {
                  MyCT->GetTargetCube(c_pos,c_dir,abs_cpos, prev_abs_cpos,false,1,sel_start_acpos.y);
                  sel_end.z = AbsCubePosToRealPos(abs_cpos).z;
                  sel_end_acpos.z = abs_cpos.z; 
                }

                if (KeyMods == GMOD_SHIFT)
                {
                  MyCT->GetTargetCube(c_pos,c_dir,abs_cpos, prev_abs_cpos,false,1,sel_start_acpos.x);
                  sel_end = AbsCubePosToRealPos(abs_cpos);
                  sel_end_acpos = abs_cpos; 
                }

                if (KeyMods == GMOD_CTRL)
                {
                  MyCT->GetTargetCube(c_pos,c_dir,abs_cpos, prev_abs_cpos,false,3,sel_start_acpos.z);
                  sel_end = AbsCubePosToRealPos(abs_cpos);
                  sel_end_acpos = abs_cpos; 
                }

                if (KeyMods == GMOD_NONE)
                {
                  MyCT->GetTargetCube(c_pos,c_dir,abs_cpos, prev_abs_cpos,false,2,sel_start_acpos.y);
                  sel_end = AbsCubePosToRealPos(abs_cpos);
                  sel_end_acpos = abs_cpos;
                }

				g_int32 sel_len = sel_end_acpos.x - sel_start_acpos.x;
				if (abs(sel_len) > G_MAX_SEL_LENGTH)
				  sel_end_acpos.x = sel_start_acpos.x + (sel_len/abs(sel_len))*G_MAX_SEL_LENGTH;

				sel_len = sel_end_acpos.y - sel_start_acpos.y;
				if (abs(sel_len) > G_MAX_SEL_LENGTH)
				  sel_end_acpos.y = sel_start_acpos.y + (sel_len/abs(sel_len))*G_MAX_SEL_LENGTH;

				sel_len = sel_end_acpos.z - sel_start_acpos.z;
				if (abs(sel_len) > G_MAX_SEL_LENGTH)
				  sel_end_acpos.z = sel_start_acpos.z + (sel_len/abs(sel_len))*G_MAX_SEL_LENGTH;
				
				sel_end = AbsCubePosToRealPos(sel_end_acpos);

                sel_scale.y = sel_end.y - sel_start.y; 
                sel_scale.x = sel_end.x - sel_start.x; 
                sel_scale.z = sel_end.z - sel_start.z;

                sel_cent.x = g_float(sel_start.x) + sel_scale.x/2.0f;
                sel_cent.y = g_float(sel_start.y) + sel_scale.y/2.0f;
                sel_cent.z = g_float(sel_start.z) + sel_scale.z/2.0f;
                AbsVec(&sel_scale);

                Sel_Mesh_Inst->SetScaling((sel_scale + csize)/2.0f);
                Sel_Mesh_Inst->SetPosition(sel_cent); 

                g_char Text[200];
                sprintf_s(Text,200,"%d  %d  %d",
                          abs(sel_start_acpos.x - sel_end_acpos.x) + 1,
                          abs(sel_start_acpos.y - sel_end_acpos.y) + 1,
                          abs(sel_start_acpos.z - sel_end_acpos.z) + 1);
                MouseLabel->SetCaption(Text);
              }
              break;
          }        
        }    

        if (myGInput->CheckAction(GM_ACT_RMTOOL_UP))
        {
          MouseLabel->SetCaption("");
          switch (tool_type)
          {
            case 1:
              {
                PauseRender();
				MyCT->FillArea(sel_start_acpos, sel_end_acpos, 0);
				ContinueRender();
              }
              break;
            case 2:
              {
                PauseRender();
                MyCT->FillArea(sel_start_acpos, sel_end_acpos, 0, 0, true, true, G_CT_AREATYPE_SPHERE);
				ContinueRender();
              }
              break;
            case 3:
              {
                PauseRender();
                if ((KeyMods == GMOD_ALT) || (KeyMods == GMOD_NONE))
                  MyCT->FillArea(sel_start_acpos, sel_end_acpos, 0, 0, true, true, G_CT_AREATYPE_CYLINDER_Y);

                if ((KeyMods == GMOD_ALT + GMOD_SHIFT) || (KeyMods == GMOD_SHIFT))
                  MyCT->FillArea(sel_start_acpos, sel_end_acpos, 0, 0, true, true, G_CT_AREATYPE_CYLINDER_X);

                if ((KeyMods == GMOD_ALT + GMOD_CTRL) || (KeyMods == GMOD_CTRL))
                  MyCT->FillArea(sel_start_acpos, sel_end_acpos, 0, 0, true, true, G_CT_AREATYPE_CYLINDER_Z);
 				ContinueRender();
			  }
              break;
          }        
        }    

        //---------------------------------------
    }
  }
}

//-----------------------------------------------

void MainGame::Input_NPCEditor(g_bool HaveEvent)
{
  //Включение/выключение мыши
  if (myGInput->CheckAction(GM_ACT_MAPROTENABLE))
  {
	MyUI->MouseVisible = false;
    MyPlayer->MouseEnabled = true;
  }
  else
  {
    MyUI->MouseVisible = true;
    MyPlayer->MouseEnabled = false;
  }

  if (MyUI->MouseVisible)
  {
    g_ivec3 abs_cpos, prev_abs_cpos;
    g_vec3 c_pos,c_dir;
      
    MyPlayer->GetCamera()->GetRayFromScreenPos(g_ivec2(myGInput->AX,myGInput->AY),MainScene->GetWorldMatrix(),&c_pos,&c_dir);
    if (MyCT->GetTargetCube(c_pos,c_dir,abs_cpos, prev_abs_cpos))
    {    
      g_vec3 respos;
      respos.x = g_float(abs_cpos.x)*csize.x + csize.x/2;
	  respos.y = g_float(abs_cpos.y)*csize.y + csize.y/2;
	  respos.z = g_float(abs_cpos.z)*csize.z + csize.z/2;
        
      GMeshInstance *Sel_Mesh_Inst = myResMgr->GetModelByUID(UID_MESH_SEL)->GetInstance(0);
      if (Sel_Mesh_Inst != NULL)
        Sel_Mesh_Inst->SetPosition(respos);

      g_ivec3 spos,cpos; 
	    
      if (!HaveEvent)
      {
        g_int32 sel_npc = NPCLB->SelectedItem;
        if (sel_npc >= 0)
        {
          if (myGInput->CheckAction(GM_ACT_SETNPC))
          {
			G_GM_Unit *Curr_NPC = MyNPCMgr->GetNPCbyIndex(NPCLB->SelectedItem);
            Curr_NPC->SetAbsCubePos(prev_abs_cpos);
			Curr_NPC->SetVisible(true);
          }

	      if (myGInput->CheckAction(GM_ACT_MOVENPC))
          {         
			GetSectorAndCubePosByAbsCubePos(prev_abs_cpos,spos,cpos);

			GPathFinder PF(MyCT);
			G_PATH_INIT iprm;
			iprm.jump_power = 3;
			iprm.search_alg = paAstarMod3D;
			iprm.search_radius = g_ivec3(100,50,100);
			PF.Init(iprm);
			g_ivec3 start_pos = MyNPCMgr->GetNPCbyIndex(NPCLB->SelectedItem)->GetAbsCubePos();
			 
			G_PATH_RESULT CalcPath = PF.FindPath(start_pos, prev_abs_cpos);

			if (CalcPath.state == psFound)
              MyNPCMgr->GetNPCbyIndex(NPCLB->SelectedItem)->MoveTo(CalcPath.path);

			PF.Release();
          }
        }        
      }
    }
  }
}

//-----------------------------------------------

void MainGame::Input_Game(g_bool HaveEvent)
{

}

//-----------------------------------------------

void MainGame::MainCycle()
{
  if (closeapp) SendMessage(MainWindow->GetHandle(),WM_CLOSE,0,0);
  
  if (DeviceCreated)
  {
	MainScene->CheckFPS(1);
	//Получение данных с устройств ввода
	myGInput->GetDeviceState();
	
    //Проверка срабатывания событий для UI
	g_bool HaveMouseEvent = MyUI->CheckMouseEvents(myGInput);
	g_bool HaveKeyEvent = MyUI->CheckKeyEvents();
    
    //Проверка событий игрока
	if (MyPlayer != NULL)
	{
	  MyPlayer->InputEnabled = !HaveKeyEvent;
	  MyPlayer->CheckInput();
	
      //Тест движений камеры 
      if (cam_heightalign)
        MyPlayer->MoveToHeight(MyCT->GetMapHeightInPos(MyPlayer->GetPosition()),true);
	}
    //Перемещение текстового поля мыши
    MouseLabel->SetPosition(g_ivec2(myGInput->AX + 32, myGInput->AY + 32));

    switch (game_mode)
    {
      case G_GAMEMODE_EDITOR_OBJECT:
      case G_GAMEMODE_EDITOR_CUBE:{
		                            if ((MyCT != NULL) && (MyPlayer != NULL))
                                      Input_CubeEditor(HaveMouseEvent);
                                    break;
                                  }
      case G_GAMEMODE_EDITOR_NPC: {
		                            if ((MyCT != NULL) && (MyPlayer != NULL))
                                      Input_NPCEditor(HaveMouseEvent);
                                    break;
                                  }
    }

    /*Test Physics*/
    /*
    if (Physics->DynamicsWorld)
    {
      Physics->DynamicsWorld->stepSimulation(0.001f);
      
          const g_int32 nPhysObjects = Physics->DynamicsWorld->getNumCollisionObjects();
          btScalar m[16];

          for (g_int32 i = 0; i<nPhysObjects; i++)
          {
            btCollisionObject*	colObj = Physics->DynamicsWorld->getCollisionObjectArray()[i];
		    btRigidBody*		body = btRigidBody::upcast(colObj);
		
            if(body && body->getMotionState())
		    {
		      btDefaultMotionState* myMotionState = (btDefaultMotionState*)body->getMotionState();
		      myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(m);
            
              if (i==(nPhysObjects-1))
              {
                btTransform WTB;
                g_vec3 ppos = MyPlayer->GetPosition();
                WTB.setIdentity();
                WTB.setOrigin(btVector3(ppos.x,ppos.y,ppos.z));
                myMotionState->setWorldTransform(WTB);
              }
		    }
		    else
		    {
		      colObj->getWorldTransform().getOpenGLMatrix(m);
		    }

            MainScene->List_3DMeshes[2+i]->SetLocalRotationMatrix((g_mat4x4*)m);
          }      
    }
    */

	//Отображение Hint
    const g_string& curr_hint = MyUI->GetCurrentHint();
	MouseLabel->SetVisible(curr_hint.size() > 0);
	MouseLabel->SetCaption(curr_hint);

	//Подсчет треугольников
	g_uint32 TriCount = 0;
	if (MyCT != NULL)
	  for (g_uint32 i = 0; i < MyCT->SectorsMgr.Sectors.size(); i++)
      {
        GCubeSector *Sector = MyCT->SectorsMgr.Sectors[i];
        TriCount += Sector->icount;
      }
	TriCount /= 3;

    //Вывод Инфы	
	char theText[256];
	g_vec3 pos = g_vec3(0,0,0);
	if (MyPlayer != NULL)
	  pos = MyPlayer->GetPosition();
    g_float fps = MainScene->GetFPS(0);
    if (fps > maxfps) maxfps = fps;
    if (fps < minfps) minfps = fps;
	sprintf_s(theText,"FPS: %.4g  CPS: %.6g  Tris: %d Pos: %.4g, %.4g, %.4g MaxFPS: %.4g MinFPS %.4g",
              fps,MainScene->GetFPS(1),TriCount,pos.x,pos.y,pos.z,
              maxfps,minfps);
	FPSLabel->SetCaption(theText);
  }
}

//-----------------------------------------------

void MainGame::UpdateClipMesh(g_ivec2 MapSize)
{
  g_vec3 m_center;
  g_vec3 m_scaling;
  
  m_center.x = g_float(center_sector_xy.x*CUBE_SECTOR_SIZE);
  m_center.y = (-MAPDEEP+2)*CUBE_SECTOR_SIZE/2.0f;
  m_center.z = g_float(center_sector_xy.y*CUBE_SECTOR_SIZE);
  if (MapSize.x % 2 != 0) m_center.x += CUBE_SECTOR_SIZE/2.0f;
  if (MapSize.y % 2 != 0) m_center.z += CUBE_SECTOR_SIZE/2.0f;

  m_scaling.x = (MapSize.x/2.0f)*CUBE_SECTOR_SIZE;
  m_scaling.y = (MAPDEEP/2.0f)*CUBE_SECTOR_SIZE;
  m_scaling.z = (MapSize.y/2.0f)*CUBE_SECTOR_SIZE; 

  ClipBox.SetCR(m_center, m_scaling);
  
  GMeshInstance *_clip_inst = ClipModel->GetInstance(0);
  if (_clip_inst != NULL)
  {
    _clip_inst->SetPosition(m_center);
    _clip_inst->SetScaling(m_scaling);
  }
}

//-----------------------------------------------

void MainGame::Test_Add_Meshes()
{
  GModel *NewModel;

  myGDevice->ILayoutList->AddInputLayout(UID_IL_SEL_CUBE, mtrSelect->GetTech("Render")->GetDXTech(), 
	                                     def_vertex_layout,def_layout_elements);

  myGDevice->ILayoutList->AddInputLayout(UID_IL_SKY_SPHERE, mtrSky->GetTech("Render")->GetDXTech(), 
	                                     def_vertex_layout, def_layout_elements);

  NewModel = myResMgr->AddModel(UID_MESH_SEL, "SelectionCube", mtrSelect, 0,
	                             sizeof(G_VERTEX), UID_IL_SEL_CUBE);
  if (NewModel != NULL)
  {
	GMeshInstance *minst = NewModel->AddInstance();
    CreateCubeForModel(NewModel,true);
    NewModel->CreateVIBuffers();
    minst->SetPosition(g_vec3(2,-1,2));
    minst->SetScaling(csize/2);
    NewModel->SetPrimitiveTopology(G_PT_LINE_LIST);
    MainScene->AddToDrawList(NewModel);
  }

  NewModel = myResMgr->AddModel(UID_MESH_SKYSPHERE, "SkySphere", mtrSky, 0,
	                             sizeof(G_VERTEX), UID_IL_SKY_SPHERE);
  if (NewModel != NULL)
  {
	GMeshInstance *minst = NewModel->AddInstance();
	GObjectLoader objl;
	objl.Load3DSFromFile("..\\Data\\Objects3DS\\wSphere.3ds");
	objl.SetToModel(NewModel, 0);
    NewModel->CreateVIBuffers();	
    minst->SetPosition(world_center);
    minst->SetScaling(g_vec3(200,120,200));
    MainScene->AddToDrawList(NewModel);
  }
  
  ClipModel = myResMgr->AddModel(UID_MESH_ALLMAP, "AllMapCube", mtrSelect, 0,
	                              sizeof(G_VERTEX), UID_IL_SEL_CUBE);
  if (ClipModel != NULL)
  {
	ClipModel->AddInstance();
    CreateCubeForModel(ClipModel, true);
    ClipModel->CreateVIBuffers();
    UpdateClipMesh(g_ivec2(MAPRADIUS*2-1,MAPRADIUS*2-1));
    ClipModel->SetPrimitiveTopology(G_PT_LINE_LIST);
    MainScene->AddToDrawList(ClipModel);
  }

  NewModel = myResMgr->AddModel(UID_MESH_PATH, "PathMesh", mtrSelect, 0,
	                             sizeof(G_VERTEX), UID_IL_SEL_CUBE);
  if (NewModel != NULL)
  {
	NewModel->AddInstance();
    NewModel->SetPrimitiveTopology(G_PT_LINE_LIST);
    MainScene->AddToDrawList(NewModel);
  }
}

//-----------------------------------------------

void MainGame::Test_Add_UI_Elements()
{
  GUI_Desktop *CurrDesktop;
  GUI_Panel *TempPanel;
  GUI_CheckBox *TempChkBox;
  GUI_TrackBar *TempTrkBar;
  GUI_RadioButton* TempRadBtn;

  /* Test Load UI */
  MyUI->LoadUIFromFile("..\\Settings\\testui.gui");

  /* Editor Desktop */
  CurrDesktop = (GUI_Desktop*)MyUI->FindUIObjectByUID(NULL, UID_DESKTOP_EDITOR);

  /* Save/Load Window */
  SaveLoadWindow = (GUI_Window*)MyUI->FindUIObjectByUID(CurrDesktop,UID_WINDOW_SAVELOAD);
  SaveLoadList = (GUI_ListBox*)MyUI->FindUIObjectByUID(SaveLoadWindow,UID_LISTBOX_LISTOBJ);
  SaveLoadLabel = (GUI_Label*)MyUI->FindUIObjectByUID(SaveLoadWindow,UID_LABEL_SLINFO);
  SaveLoadEdit = (GUI_Edit*)MyUI->FindUIObjectByUID(SaveLoadWindow,UID_EDIT_OBJNAME);

  /* Cubes Window */
  TempPanel = (GUI_Panel*)MyUI->FindUIObjectByUID(CurrDesktop,UID_WINDOW_OBJECTS);
  CubesLB = (GUI_ListBox*)MyUI->FindUIObjectByUID(TempPanel,UID_LISTBOX_TEXTURE);
  PreviewImg = (GUI_Image*)MyUI->FindUIObjectByUID(TempPanel,UID_IMAGE_OBJECT);
  GroupsList = (GUI_ScrollList*)MyUI->FindUIObjectByUID(TempPanel,UID_SCROLLLIST_GROUPS);

  /* NPC Window */
  TempPanel = (GUI_Panel*)MyUI->FindUIObjectByUID(CurrDesktop,UID_WINDOW_NPC);
  NPCLB = (GUI_ListBox*)MyUI->FindUIObjectByUID(TempPanel,UID_LISTBOX_NPC);
  if (TempPanel != NULL) TempPanel->SetVisible(false);

  /* Settings Window */
  TempPanel = (GUI_Panel*)MyUI->FindUIObjectByUID(CurrDesktop,UID_WINDOW_SETTINGS);
  TempTrkBar = (GUI_TrackBar*)MyUI->FindUIObjectByUID(TempPanel,UID_TRACKBAR_SPEED);
  if (TempTrkBar != NULL) TempTrkBar->SetRange(1,40);
  if (TempTrkBar != NULL) TempTrkBar->SetBarPosition(40);
  TempChkBox = (GUI_CheckBox*)MyUI->FindUIObjectByUID(TempPanel,UID_CHECKBOX_HEIGHT);
  if (TempChkBox != NULL) TempChkBox->SetCheck(true);
  TempChkBox = (GUI_CheckBox*)MyUI->FindUIObjectByUID(TempPanel,UID_CHECKBOX_CUBEALIGN);
  if (TempChkBox != NULL) TempChkBox->SetCheck(true);
  TempChkBox = (GUI_CheckBox*)MyUI->FindUIObjectByUID(TempPanel,UID_CHECKBOX_SHDMAP);
  if (TempChkBox != NULL) TempChkBox->SetCheck(true);
  TempChkBox = (GUI_CheckBox*)MyUI->FindUIObjectByUID(TempPanel,UID_CHECKBOX_FOG);
  if (TempChkBox != NULL) TempChkBox->SetCheck(true);
  TempTrkBar = (GUI_TrackBar*)MyUI->FindUIObjectByUID(TempPanel,UID_TRACKBAR_FOV);
  if (TempTrkBar != NULL) TempTrkBar->SetRange(40,120);
  if (TempTrkBar != NULL) TempTrkBar->SetBarPosition(65);
  TempTrkBar = (GUI_TrackBar*)MyUI->FindUIObjectByUID(TempPanel,UID_TRACKBAR_DIST);
  if (TempTrkBar != NULL) TempTrkBar->SetRange(5,30);
  if (TempTrkBar != NULL) TempTrkBar->SetBarPosition(9);
  if (TempPanel != NULL) TempPanel->SetVisible(false);
  ResolutList = (GUI_ScrollList*)MyUI->FindUIObjectByUID(TempPanel,UID_SCROLLLIST_RESOLUT);

  /* Other Settings Window */
  TempPanel = (GUI_Panel*)MyUI->FindUIObjectByUID(CurrDesktop,UID_WINDOW_OTHERSET);
  if (TempPanel != NULL) TempPanel->SetVisible(false);

  /* CubeObj Window */
  TempPanel = (GUI_Panel*)MyUI->FindUIObjectByUID(CurrDesktop,UID_WINDOW_CUBEOBJ);
  CubeObjLB = (GUI_ListBox*)MyUI->FindUIObjectByUID(TempPanel,UID_LISTBOX_CUBEOBJ);

  /* Light Cam Window */
  TempPanel = (GUI_Panel*)MyUI->FindUIObjectByUID(CurrDesktop, UID_WINDOW_LIGHTCAM);
  LightCamImg = (GUI_Image*)MyUI->FindUIObjectByUID(TempPanel, UID_IMAGE_LIGHTCAM);
  TempTrkBar = (GUI_TrackBar*)MyUI->FindUIObjectByUID(TempPanel, UID_TRACKBAR_LCAMX);
  if (TempTrkBar != NULL) TempTrkBar->SetRange(20,160);
  if (TempTrkBar != NULL) TempTrkBar->SetBarPosition(50);
  TempTrkBar = (GUI_TrackBar*)MyUI->FindUIObjectByUID(TempPanel, UID_TRACKBAR_LCAMZ);
  if (TempTrkBar != NULL) TempTrkBar->SetRange(20,160);
  if (TempTrkBar != NULL) TempTrkBar->SetBarPosition(50);
  if (TempPanel != NULL) TempPanel->SetVisible(false);

  /* Log Window */
  WinLog = (GUI_Window*)MyUI->FindUIObjectByUID(CurrDesktop,UID_WINDOW_LOG);
  LogLB = (GUI_ListBox*)MyUI->FindUIObjectByUID(WinLog, UID_LISTBOX_LOG);
  if (WinLog != NULL) WinLog->SetVisible(false);
  ConsoleEdit = (GUI_Edit*)MyUI->FindUIObjectByUID(WinLog, UID_EDIT_CONSOLE);
  if (ConsoleEdit != NULL) ConsoleEdit->SetMaxLength(100);

  /* Tool Window */
  TempPanel = (GUI_Panel*)MyUI->FindUIObjectByUID(CurrDesktop,UID_WINDOW_TOOLS);
  CEditorBGrid = (GUI_ButtonGrid*)MyUI->FindUIObjectByUID(TempPanel,UID_BUTTON_TOOLGRID);
  if (CEditorBGrid != NULL) CEditorBGrid->AddButtons(10);

  /* Editor Mode Window */
  TempPanel = (GUI_Panel*)MyUI->FindUIObjectByUID(CurrDesktop,UID_WINDOW_EDITORMODE);
  TempRadBtn = (GUI_RadioButton*)MyUI->FindUIObjectByUID(TempPanel, UID_RADIOBTN_EDITMAP);
  if (TempRadBtn != NULL) TempRadBtn->SetCheck(true);

  /* Sidebar Panel */
  TempPanel = (GUI_Panel*)MyUI->FindUIObjectByUID(CurrDesktop, UID_PANEL_SIDEBAR);
  TempPanel->SetPosition(g_ivec2(0,0));
  TempPanel->SetSize(g_ivec2(240, myGDevice->GetHeight()));
  TempPanel->SetAnchors(g_anchors_set(g_anst_low, g_anst_both));
  MenuBGrid = (GUI_ButtonGrid*)MyUI->FindUIObjectByUID(TempPanel,UID_BUTTON_MENUGRID);
  if (MenuBGrid != NULL) MenuBGrid->AddButtons(3);
  GUI_Button *HideButton = (GUI_Button*)MyUI->FindUIObjectByUID(CurrDesktop,UID_BUTTON_HIDESIDEBAR);
  HideButton->SetPosition(g_ivec2(TempPanel->GetSize().x, myGDevice->GetHeight() - 50)); 
  HideButton->SetAnchors(g_anchors_set(g_anst_low, g_anst_high));
  FPSLabel = MyUI->AddLabel(CurrDesktop, TempPanel->GetSize().x + 2, 2, myGDevice->GetWidth(), 20, "FPS", g_anchors_set(g_anst_low, g_anst_low));
  /* Main Menu Desktop */
  CurrDesktop = (GUI_Desktop*)MyUI->FindUIObjectByUID(NULL, UID_DESKTOP_MAINMENU);

  /* Main Menu Controls */
  GUI_Image *TempImg = (GUI_Image*)MyUI->FindUIObjectByUID(NULL, UID_IMAGE_MM_BACKGR);
  TempImg->SetSize(g_ivec2(myGDevice->GetWidth(), myGDevice->GetHeight()));
  TempImg->SetAnchors(g_anchors_set(g_anst_both, g_anst_both));

  /* Other */
  MouseLabel = MyUI->AddLabel(CurrDesktop,2,30,200,25,"", g_anchors_set(g_anst_low, g_anst_low));
}

//-----------------------------------------------

void MainGame::RenderCubeObjectPreview()
{
  if ((MainScene == NULL) || (myGDevice == NULL)) return;

  GModel* _model = myResMgr->GetModelByUID(UID_MESH_OBJPREV);

  if (_model == NULL)
  {
    g_uint32 op_layout_elements = 3;

    D3D10_INPUT_ELEMENT_DESC op_vertex_layout[] =
    {
      { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
      { "COLOR",     0, DXGI_FORMAT_R32G32_FLOAT,       0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
      { "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 20, D3D10_INPUT_PER_VERTEX_DATA, 0 }
    };

	myGDevice->ILayoutList->AddInputLayout(UID_IL_OBJ_PREV, mtrObjPreview->GetTech("Render")->GetDXTech(), 
		                                    op_vertex_layout, op_layout_elements);

	_model = myResMgr->AddModel(UID_MESH_OBJPREV, "PreviewMesh", mtrObjPreview, 0,
	                            sizeof(G_OP_VERTEX), UID_IL_OBJ_PREV);
    _model->AddInstance();

	vector <GTexture*> _objprev_tex_array;
    MyCT->GetMaterial()->GetTextureArray(0, _objprev_tex_array);
    mtrObjPreview->AddTextureArray(_objprev_tex_array, 0);
    mtrObjPreview->ApplyAllTextures();
  }

  if (_model != NULL)
  { 
    g_uint32    *_ind;
    G_OP_VERTEX *_vert;

    //Make Mesh
	g_uint16 obj_index = GetSelectedCubeIndex();
    g_uint8 ot = MyCT->Objects.Objects[obj_index].ObjectType;
    g_int32 nFace = MyCT->Objects.ObjTypes[ot].nIndices/3;
    g_int32 nVert = MyCT->Objects.ObjTypes[ot].nIndices;

    _vert = new G_OP_VERTEX[nVert];
    _ind = new g_uint32[nVert];
  
    for (g_int32 i = 0; i < nFace; i++)
    {
        g_int32 dface = (i-i%2)/2;
        g_uint16  main_tex = MyCT->Objects.Objects[obj_index].Properties.FaceMainTex[dface];
        g_uint16  mod_tex = MyCT->Objects.Objects[obj_index].Properties.FaceModTex[dface];
        g_uint32 dir = g_uint32(floor(MyCT->Objects.ObjTypes[ot]._dirs[0][i]/262144.0f));
        G_CUBE_VERTEX v[3];
        for (g_int32 j = 0; j < 3; j++)
        {
          v[j] = MyCT->Objects.ObjTypes[ot]._vertices[0][MyCT->Objects.ObjTypes[ot]._indices[i*3+j]];
          
          _vert[i*3+j].Pos = v[j].Pos;     
          _vert[i*3+j].Col.x = main_tex;
          _vert[i*3+j].Col.y = mod_tex;

		  if (dir == 1)
            _vert[i*3+j].Tex = g_vec2(v[j].Tex.z,  -v[j].Tex.y);
          if (dir == 2)
            _vert[i*3+j].Tex = g_vec2(v[j].Tex.x,  v[j].Tex.z);
          if (dir == 0)
            _vert[i*3+j].Tex = g_vec2(-v[j].Tex.x, -v[j].Tex.y);

          _ind[i*3+j] = i*3+j;
        }  
    }

    _model->DeleteVIBuffers();
    _model->ClearVIArrays();

    _model->AddVertexArray(_vert, nVert);
    _model->AddIndexArray(_ind, nVert);
    _model->CreateVIBuffers();
    _model->GetInstance(0)->SetScaling(g_vec3(2.0f,2.0f,2.0f));

	GCamera *Cam = myResMgr->GetCameraByUID(UID_CAM_OBJPREV);
    
	GRenderTarget *_rt_co_prev = myGDevice->RTList->GetRenderTarget("rt_cobj_prev");
	GRenderTarget *_rt_main = myGDevice->RTList->GetRenderTarget("rt_main");

	if ((_rt_co_prev != NULL) && (_rt_main != NULL))
	{
      PauseRender();
	  _rt_co_prev->Set();
	  _rt_co_prev->Clear(true, true, false, g_color(0.0f,0.0f,0.0f,0.0f));
   
      if (Cam != NULL) Cam->SetViewport();
      _model->SetWorldMatrix(MainScene->GetWorldMatrix());
	  _model->SetViewProjMatrix(Cam->GetViewProjMatrix());  
	  _model->Draw(false);

	  _rt_main->Set();
	  MainScene->GetDefaultCamera()->SetViewport();	
      ContinueRender();
	}

    delete[]_vert;
    delete[]_ind;
  }
}

//-----------------------------------------------

void MainGame::PauseRender()
{
  pause_render = true;
  while (!is_paused_render)
  {
    Sleep(1);
  }
}

//-----------------------------------------------

void MainGame::ContinueRender()
{
  pause_render = false;
}

//-----------------------------------------------

void MainGame::RenderLightCam()
{ 
  g_float MinLen = 5;
  g_float Len[G_SHDMAP_CASC] = {250, 850, 2500};
  g_vec3  frp[(G_SHDMAP_CASC + 1)*4];
  g_vec3  *points_fr;

  MainScene->GetDefaultCamera()->GetFrustumPoints(&points_fr);
  
  g_vec3 dirs[4];
  for (g_uint32 dir_n = 0; dir_n < 4; dir_n++)
  {
	D3DXVec3Normalize(&dirs[dir_n], &(points_fr[dir_n + 4] - points_fr[dir_n]));
	frp[dir_n] = points_fr[dir_n] + dirs[dir_n]*MinLen;

	for (g_uint32 casc_n = 0; casc_n < G_SHDMAP_CASC; casc_n++)
      frp[dir_n + 4 + casc_n*4] = points_fr[dir_n] + dirs[dir_n]*Len[casc_n];
  }

  //
  GCamera *Cam = myResMgr->GetCameraByUID(UID_CAM_LIGHTCAM);
  Cam->InitViewport(G_SHDMAP_RES, G_SHDMAP_RES, 0, 1, 0, 0);
  Cam->InitProjection(true);
  g_mat4x4 mP_light = Cam->GetProjMatrix();
  g_mat4x4 mV_light = Cam->GetViewMatrix();
  //g_vec3 min, max;
  g_mat4x4 m_crop[G_SHDMAP_CASC];
  g_vec3 _outcoord[8];

  //Calc Crop Proj Matrix
  for (g_uint32 j = 0; j<G_SHDMAP_CASC; j++)
  {
    for (g_uint32 i = 0; i < 8; i++)
    {
      D3DXVec3Project(&_outcoord[i], &frp[i+j*4], &Cam->GetViewport(), 
	                  &mP_light, &mV_light, &MainScene->GetWorldMatrix());
    }

    g_vec3 min = _outcoord[0];
    g_vec3 max = _outcoord[0];

    for (g_uint32 i = 1; i < 8; i++)
    {
	  if (_outcoord[i].x < min.x) min.x = _outcoord[i].x;
	  if (_outcoord[i].x > max.x) max.x = _outcoord[i].x;
	  if (_outcoord[i].y < min.y) min.y = _outcoord[i].y;
	  if (_outcoord[i].y > max.y) max.y = _outcoord[i].y;
    }
  
    D3DXMatrixOrthoOffCenterLH(&m_crop[j], min.x, max.x, min.y, max.y, 0.0f, 10000.0f);
  }
 
  GRenderTarget *_rt_light_cam = myGDevice->RTList->GetRenderTarget("rt_light_cam");
  GRenderTarget *_rt_main = myGDevice->RTList->GetRenderTarget("rt_main");
 
  if ((_rt_light_cam != NULL) && (_rt_main != NULL))
  {
	_rt_light_cam->Set();
	_rt_light_cam->Clear(false, true, false, g_color(0.0f, 0.0f, 0.0f, 1.0f));
 
	GEffect *_ct_eff = MyCT->GetMaterial()->Effect;
	_ct_eff->GetVar(G_SVT_CUSTOM, 4)->SetRawData(Cam->GetDirection());
	_ct_eff->GetVar(G_SVT_CUSTOM, 5)->SetMatrixArray(m_crop, 3);
	_ct_eff->GetVar(G_SVT_CUSTOM, 2)->SetMatrix(mV_light);
 
	//Draw
	for (g_uint32 j = 0; j<G_SHDMAP_CASC; j++)
	{
	  Cam->InitViewport(G_SHDMAP_RES, G_SHDMAP_RES, 0, 1, 0, G_SHDMAP_RES*j);
	  Cam->SetViewport();
	  Cam->SetCustomProjMatrix(m_crop[j]);	
      MyCT->DrawSM(MainScene->GetWorldMatrix(), Cam);
	  MyNPCMgr->DrawSM(Cam);
	}
 
	_rt_main->Set();
 
	MainScene->GetDefaultCamera()->SetViewport();				   
  }
}

//-----------------------------------------------

g_uint16 MainGame::GetSelectedCubeIndex()
{
  g_uint16 sel_grp = GroupsList->GetSelected();
  g_int32 sel_cub = CubesLB->SelectedItem;
  if (sel_grp > GroupsList->Items.size()) sel_grp = 0;
  if ((sel_cub < 0) && (sel_cub > g_int32(CubesLB->Items.size()))) sel_cub = 0;
	return MyCT->Objects.ObjGroups[sel_grp].ObjIDList[sel_cub];
}

//-----------------------------------------------

void MainGame::SetScreen(DXGI_MODE_DESC mode_desc, g_bool Windowed)
{
  if (myGDevice->SetResolution(mode_desc, Windowed))
  {
    _settings.scr_height = mode_desc.Height;
    _settings.scr_width = mode_desc.Width;

	//Resize Window
    MainWindow->SetSize(mode_desc.Width, mode_desc.Height);

	//Resize Cameras
	GCamera *TempCam; 
    TempCam = myResMgr->GetCameraByUID(UID_CAM_MENU);
	if (TempCam != NULL) TempCam->ResizeViewport(mode_desc.Width, mode_desc.Height);

	TempCam = myResMgr->GetCameraByUID(UID_CAM_PLAYER);
	if (TempCam != NULL) TempCam->ResizeViewport(mode_desc.Width, mode_desc.Height);

	//Update UI
	MyUI->UpdateAllUIObject();

	//Resize Input (переделать)
	g_rect mrect;
	myGInput->GetMouseRect(mrect);
	ResizeRect(mrect, mode_desc.Width, mode_desc.Height);
	myGInput->SetMouseRect(mrect.left, mrect.right, mrect.top, mrect.bottom);
  }  
}

//-----------------------------------------------

void MainGame::SetShadowmap(g_bool Enable)
{
  _settings.use_shadowmap = Enable;

  //Сделать лучше
  if ((MyCT != NULL) && (MyCT->GetMaterial() != NULL) && (MyCT->GetMaterial()->Effect != NULL))
	MyCT->GetMaterial()->Effect->GetVar(G_SVT_CUSTOM, 3)->SetRawData(_settings.use_shadowmap);
}

//-----------------------------------------------

void MainGame::SetFog(g_bool Enable)
{
  _settings.use_fog = Enable;

  //Сделать лучше
  if ((MyCT != NULL) && (MyCT->GetMaterial() != NULL) && (MyCT->GetMaterial()->Effect != NULL))
	MyCT->GetMaterial()->Effect->GetVar(G_SVT_CUSTOM, 7)->SetRawData(_settings.use_fog);
}

//-----------------------------------------------

void MainGame::SetWireframe(g_bool Enable)
{
  _settings.use_wireframe = Enable;
  myGDevice->SetWireframe(_settings.use_wireframe);
}

//-----------------------------------------------

void MainGame::SetSound(g_bool Enable)
{
  _settings.use_sound = Enable;
}

//-----------------------------------------------

void MainGame::SetFOV(g_int32 FOV)
{
  CheckRange(&FOV, 30, 180);
  _settings.FOV = FOV;

  if ((MyPlayer != NULL) && (MyPlayer->GetCamera() != NULL))
    MyPlayer->GetCamera()->SetFOV(g_float(FOV));
}

//-----------------------------------------------

void MainGame::MainMenu_Init()
{
  
}

//-----------------------------------------------

void MainGame::MainMenu_Release()
{
  
}

//-----------------------------------------------

void MainGame::Editor_Init()
{
  PauseRender();
  /* Add Effect Pools */
  fxpDef = myResMgr->AddEffectPool(UID_EP_DEFAULT, "DefPool","..\\Shaders\\DefPool.gfxp");
  //fxpCT = myResMgr->AddEffectPool(UID_EP_CUBETERR, "CTPool", "", false);

  /* Load Effects */
  fxDef = myResMgr->AddEffect(UID_FX_DEFAULT, "Effect1", "..\\Shaders\\Def.fx", fxpDef);
  fxObjPrev = myResMgr->AddEffect(UID_FX_OBJPREV, "ObjPrevFX","..\\Shaders\\ObjPreview.fx", fxpDef);
  fxSky = myResMgr->AddEffect(UID_FX_SKY, "SkyEffect", "..\\Shaders\\Sky.fx", fxpDef);

  fxCT = myResMgr->AddEffect(UID_FX_CUBETERR, "CubeTerrainFX", "..\\Shaders\\CubeTerrain.fx");

  /* Create Materials */
  mtrSelect = myResMgr->AddMaterial(UID_MAT_SEL, "mSelect", fxDef);
  mtrSky = myResMgr->AddMaterial(UID_MAT_SKY_SPHERE, "mSkySphere", fxSky);
  mtrObjPreview = myResMgr->AddMaterial(UID_MAT_OBJPREV, "ObjPreview", fxObjPrev);
  
  GMaterial *matCT = myResMgr->AddMaterial(UID_MAT_CUBETERR, "CubeTerrain", fxCT);

  /* Load textures */
  GTexture *Rad_Texture = myResMgr->AddTexture(UID_TEX_RAD, "Rad","..\\Textures\\rad.dds");
  mtrSelect->AddTexture(Rad_Texture,0);

  GTexture *Sky_Texture = myResMgr->AddTexture(UID_TEX_SKY, "Sky", "..\\Textures\\sky.dds");
  mtrSky->AddTexture(Sky_Texture, 0);

  /*Cube Obj Preview Texture*/
  GTexture *COP_Texture = myResMgr->AddTexture(UID_TEX_OBJPREV, "CubeObjPreview");
  COP_Texture->CreateCustomTexture2D(DESC_CO_PREV, true);

  /*Light Cam Texture*/
  GTexture *LC_Texture = myResMgr->AddTexture(UID_TEX_LIGHTCAM, "LightCamTexture");
  LC_Texture->CreateCustomTexture2D(DESC_LIGHT_CAM, true, DXGI_FORMAT_R32_FLOAT);
  fxCT->GetVar(G_SVT_RESOURCE, 2)->SetTexture(LC_Texture);

  /*SSAO Textures*/
  GTexture *SSAO_DS_MAP = myResMgr->AddTexture(UID_TEX_SSAO_DS, "SSAO_DS_MAP");
  GTexture *SSAO_NORM_MAP = myResMgr->AddTexture(UID_TEX_SSAO_NORM, "SSAO_NORM_MAP");
  //SSAO_DS_MAP->CreateCustomTexture2D(DESC_SSAO_DS, true, DXGI_FORMAT_R32_FLOAT);
  //SSAO_NORM_MAP->CreateCustomTexture2D(DESC_SSAO_NORM, true);

  /* Add Cameras */
  GCamera *camPlayer; 
  camPlayer = myResMgr->AddCamera(UID_CAM_PLAYER, "Player Cam", myGDevice->GetWidth(), myGDevice->GetHeight(), false, 0.1f, 500000.0f);
  camPlayer->SetView(g_vec3(-10.0f,60.0f,-110.0f), g_vec3(-1, -1, -1));
  MainScene->SetDefaultCamera(camPlayer);    

  GCamera *CamPreview = myResMgr->AddCamera(UID_CAM_OBJPREV, "CubeObj Preview", 96, 96, false);
  CamPreview->SetView(g_vec3(3.1f,3.1f,3.1f), g_vec3(-2.6f,-2.9f,-2.6f));
  CamPreview->SetFOV(50);
  CamPreview->Enabled = false;

  GCamera *camGlobLight;
  camGlobLight = myResMgr->AddCamera(UID_CAM_LIGHTCAM, "Light Cam", G_SHDMAP_RES, G_SHDMAP_RES, true);
  g_vec3 sun_pos = g_vec3(world_center.x+600.01f, world_center.y + sun_dist/2.0f, world_center.z+600.01f); 
  camGlobLight->SetViewEx(sun_pos, world_center);
  camGlobLight->Enabled = false;

  /* Load And Create Meshes */
  Test_Add_Meshes();

  /* Добавляем рендер таргет для превью объектов */
  GRenderTarget *_rt_co_prev = myGDevice->RTList->AddRenderTarget("rt_cobj_prev"); // CubeObj Preview RT
  _rt_co_prev->CreateDepthStensilView(myGDevice->pDepthStencil, DXGI_FORMAT_D32_FLOAT);
  _rt_co_prev->SetRenderToTexture(96, 96, (ID3D10Texture2D*)COP_Texture->TextureRes);
  PreviewImg->SetFromTexture(&COP_Texture);

  GRenderTarget *_rt_light_cam = myGDevice->RTList->AddRenderTarget("rt_light_cam"); // Light Cam RT
  _rt_light_cam->CreateDepthStensilView(LC_Texture->TextureRes, DXGI_FORMAT_D32_FLOAT);
  LightCamImg->SetFromTexture(&LC_Texture);

  GRenderTarget *_rt_ssao = myGDevice->RTList->AddRenderTarget("rt_ssao"); // SSAO RT
  //_rt_ssao->CreateDepthStensilView(SSAO_DS_MAP->TextureRes, DXGI_FORMAT_D32_FLOAT);
  //_rt_ssao->SetRenderToTexture(scr_w, scr_h, (ID3D10Texture2D*)SSAO_NORM_MAP->TextureRes);

  /* Создаем кубический террэйн */
  MyCT = new GCubeTerrain(myGDevice, matCT);

  /* Загружаем персонажей  */
  MyNPCMgr = new G_GM_NPCMgr(myGDevice, MyCT, MainScene);
  MyNPCMgr->LoadNPCs("..\\Data\\NPC.wnpc");

  NPCLB->ClearItems();
  for (g_uint16 i = 0; i<MyNPCMgr->GetNPCCount(); i++) 
  {
    g_char itemname[G_MAXUNITNAMELEN];
    sprintf_s(itemname, G_MAXUNITNAMELEN, "%d.%s", i+1, MyNPCMgr->GetNPCbyIndex(i)->GetName().c_str());
    NPCLB->Items.push_back(itemname);
  }
  NPCLB->SelectedItem = 0;

  /* Создаем список групп */
  GroupsList->ClearItems();
  for (g_uint16 i = 0; i<MyCT->Objects.ObjGroups.size(); i++) 
	GroupsList->Items.push_back(MyCT->Objects.ObjGroups[i].Name);
  GroupsList->SetSelected(0);

  /* Создаем список разрешений */
  ResolutList->ClearItems();
  myGDevice->Info->GetShortResolutionList(0, 0, _resolut_list);

  for (g_uint16 i = 0; i<_resolut_list.size(); i++) 
  {
	g_char ResolutOut[64];
	sprintf_s(ResolutOut, 64, "%dx%d", _resolut_list[i].Width, _resolut_list[i].Height);	
	ResolutList->Items.push_back(ResolutOut);
    if ((_resolut_list[i].Width == myGDevice->GetWidth()) &&
		(_resolut_list[i].Height == myGDevice->GetHeight()))
	  ResolutList->SetSelected(i);
  } 

  /* Создаем список объектов нулевой группы */
  CubesLB->ClearItems();
  for (g_uint16 i = 0; i<MyCT->Objects.ObjGroups[0].nObjects; i++) 
  {
    g_char itemname[200];
	sprintf_s(itemname, 200, "%d.%s", i+1, MyCT->Objects.Objects[MyCT->Objects.ObjGroups[0].ObjIDList[i]].Name);
    CubesLB->Items.push_back(itemname);
  }
  CubesLB->SelectedItem = 0;	

  /* Создаем список объектов из кубиков */
  g_char FindMask[300];
  sprintf_s(FindMask, 300, SAVE_OBJ_PATH_TEMPLATE, "*", "pobj");
  CubeObjLB->FillFromDirectory(FindMask,false,false,5);
  CubeObjLB->SelectedItem = 0;

  /* Панель инструментов */
  CEditorBGrid->SetImagesTexture(myResMgr->AddTexture(UID_TEX_TOOLS, "TestBGridPics", "..\\Textures\\PicsEd.dds"),
                                 g_ivec2(24,24), g_ivec2(4,4));

  /* Панель меню */
  MenuBGrid->SetImagesTexture(myResMgr->AddTexture(UID_TEX_MENU, "MenuBGridPics", "..\\Textures\\MenuEd.dds"),
                                 g_ivec2(24,24), g_ivec2(5,1));
  MenuBGrid->SetPressedButton(0);
  /* */
  GUI_RadioButton *MapEdRadBtn = (GUI_RadioButton*)MyUI->FindUIObjectByUID(NULL, UID_RADIOBTN_EDITMAP);
  MapEdRadBtn->SetCheck(true);

  /* Инициализация игрока */
  MyPlayer = new G_GM_Player(myGInput, camPlayer, "Gibe");
  MyPlayer->SetPosition(g_vec3(100.0f, -200.0f, 100.0f));
  MyPlayer->SetTiltAngle(60);
  MyPlayer->SetSpeed(300);
  MyPlayer->MoveInYCoord = false;
  MyPlayer->MoveClipping = false;//true;
  MyPlayer->SetMoveClip(ClipBox); 

  game_mode = G_GAMEMODE_EDITOR_CUBE;

  /* Применение настроек */
  SetFOV(_settings.FOV);
  SetFog(_settings.use_fog);
  SetShadowmap(_settings.use_shadowmap);

  ContinueRender();
}

//-----------------------------------------------

void MainGame::Editor_Release()
{
  PauseRender();
  FREENULL_VAR(MyPlayer);
  FREENULLRELEASE(MyNPCMgr);
  FREENULLRELEASE(MyCT);

  myGDevice->RTList->RemoveRenderTarget("rt_cobj_prev");
  myGDevice->RTList->RemoveRenderTarget("rt_light_cam");
  myGDevice->RTList->RemoveRenderTarget("rt_ssao");

  MainScene->SetDefaultCamera(UID_CAM_MENU);
  MainScene->ClearDrawList();

  myResMgr->RemoveResourceByUID(UID_CAM_LIGHTCAM);
  myResMgr->RemoveResourceByUID(UID_CAM_OBJPREV);
  myResMgr->RemoveResourceByUID(UID_CAM_PLAYER);

  myResMgr->RemoveResourcesByType(G_OT_MODEL);
  myResMgr->RemoveResourcesByType(G_OT_TEXTURE);
  myResMgr->RemoveResourcesByType(G_OT_MATERIAL);
  myResMgr->RemoveResourcesByType(G_OT_EFFECTPOOL);

  myGDevice->ILayoutList->RemoveInputLayout(UID_IL_SEL_CUBE);
  myGDevice->ILayoutList->RemoveInputLayout(UID_IL_SKY_SPHERE);
  ContinueRender();
  
  game_mode = G_GAMEMODE_MAIN_MENU;
}

//-----------------------------------------------

void MainGame::Game_Init()
{
  PauseRender();
  /* Add Effect Pools */
  fxpDef = myResMgr->AddEffectPool(UID_EP_DEFAULT, "DefPool","..\\Shaders\\DefPool.gfxp");

  /* Load Effects */
  fxDef = myResMgr->AddEffect(UID_FX_DEFAULT, "Effect1", "..\\Shaders\\Def.fx", fxpDef);
  fxCT = myResMgr->AddEffect(UID_FX_CUBETERR, "CubeTerrainFX", "..\\Shaders\\CubeTerrain.fx");

  /* Create Materials */
  GMaterial *matCT = myResMgr->AddMaterial(UID_MAT_CUBETERR, "CubeTerrain", fxCT);

  /* Load textures */
  /*Light Cam Texture*/
  GTexture *LC_Texture = myResMgr->AddTexture(UID_TEX_LIGHTCAM, "LightCamTexture");
  LC_Texture->CreateCustomTexture2D(DESC_LIGHT_CAM, true, DXGI_FORMAT_R32_FLOAT);
  fxCT->GetVar(G_SVT_RESOURCE, 2)->SetTexture(LC_Texture);

  /*SSAO Textures*/
  GTexture *SSAO_DS_MAP = myResMgr->AddTexture(UID_TEX_SSAO_DS, "SSAO_DS_MAP");
  GTexture *SSAO_NORM_MAP = myResMgr->AddTexture(UID_TEX_SSAO_NORM, "SSAO_NORM_MAP");
  //SSAO_DS_MAP->CreateCustomTexture2D(DESC_SSAO_DS, true, DXGI_FORMAT_R32_FLOAT);
  //SSAO_NORM_MAP->CreateCustomTexture2D(DESC_SSAO_NORM, true);

  /* Add Cameras */
  GCamera *camPlayer; 
  camPlayer = myResMgr->AddCamera(UID_CAM_PLAYER, "Player Cam", myGDevice->GetWidth(), myGDevice->GetHeight(), false);
  camPlayer->SetView(g_vec3(-10.0f,60.0f,-110.0f), g_vec3(-1, -1, -1));
  MainScene->SetDefaultCamera(camPlayer);    

  GCamera *camGlobLight;
  camGlobLight = myResMgr->AddCamera(UID_CAM_LIGHTCAM, "Light Cam", G_SHDMAP_RES, G_SHDMAP_RES, true);
  g_vec3 sun_pos = g_vec3(world_center.x+600.01f, world_center.y + sun_dist/2.0f, world_center.z+600.01f); 
  camGlobLight->SetViewEx(sun_pos, world_center);
  camGlobLight->Enabled = false;

  /* Добавляем рендер таргет для превью объектов */
  GRenderTarget *_rt_light_cam = myGDevice->RTList->AddRenderTarget("rt_light_cam"); // Light Cam RT
  _rt_light_cam->CreateDepthStensilView(LC_Texture->TextureRes, DXGI_FORMAT_D32_FLOAT);

  GRenderTarget *_rt_ssao = myGDevice->RTList->AddRenderTarget("rt_ssao"); // SSAO RT
  //_rt_ssao->CreateDepthStensilView(SSAO_DS_MAP->TextureRes, DXGI_FORMAT_D32_FLOAT);
  //_rt_ssao->SetRenderToTexture(scr_w, scr_h, (ID3D10Texture2D*)SSAO_NORM_MAP->TextureRes);

  /* Создаем кубический террэйн */
  MyCT = new GCubeTerrain(myGDevice, matCT);

  /* Загружаем персонажей  */
  MyNPCMgr = new G_GM_NPCMgr(myGDevice, MyCT, MainScene);
  MyNPCMgr->LoadNPCs("..\\Data\\NPC.wnpc");

  /* Инициализация игрока */
  MyPlayer = new G_GM_Player(myGInput, camPlayer, "Gibe");
  MyPlayer->SetPosition(g_vec3(100.0f,-200.0f,100.0f));
  MyPlayer->SetTiltAngle(60);
  MyPlayer->SetSpeed(200);
  MyPlayer->MoveInYCoord = false;
  MyPlayer->MoveClipping = true;
  MyPlayer->SetMoveClip(ClipBox); 

  /* Тестовая загрузка карты */
  G_MAP_INFO map_info;
  if (MyCT->LoadMap("7",map_info))
  {
    MyCT->UpdateSectorsMesh();
    UpdateClipMesh(map_info.map_size);
    MyPlayer->SetMoveClip(ClipBox);
  }

  game_mode = G_GAMEMODE_GAME_PLAY;

  /* Применение настроек */
  SetFOV(_settings.FOV);
  SetFog(_settings.use_fog);
  SetShadowmap(_settings.use_shadowmap);

  ContinueRender();
}

//-----------------------------------------------

void MainGame::Game_Release()
{
  PauseRender();
  FREENULL_VAR(MyPlayer);
  FREENULLRELEASE(MyNPCMgr);
  FREENULLRELEASE(MyCT);

  myGDevice->RTList->RemoveRenderTarget("rt_light_cam");
  myGDevice->RTList->RemoveRenderTarget("rt_ssao");

  MainScene->SetDefaultCamera(UID_CAM_MENU);
  MainScene->ClearDrawList();

  myResMgr->RemoveResourceByUID(UID_CAM_LIGHTCAM);
  myResMgr->RemoveResourceByUID(UID_CAM_PLAYER);

  myResMgr->RemoveResourcesByType(G_OT_MODEL);
  myResMgr->RemoveResourcesByType(G_OT_TEXTURE);
  myResMgr->RemoveResourcesByType(G_OT_MATERIAL);
  myResMgr->RemoveResourcesByType(G_OT_EFFECTPOOL);
  ContinueRender();
  
  game_mode = G_GAMEMODE_MAIN_MENU;
}

//-----------------------------------------------

g_bool MainGame::SaveSettings()
{
  G_CONFIG *set_conf = myGDevice->ConfMgr->GetConfig(G_CUID_GAMECONFIG);

  if (set_conf == NULL) return false;

  //Video Settings
  GC_SECTION *vid_sect = set_conf->GetSectionByName("Video", true);

  if (vid_sect != NULL) 
  {
	GC_KEY *curr_key = vid_sect->GetKeyByName("ScreenWidth", true);
	if (curr_key != NULL) curr_key->FromInt(_settings.scr_width);

	curr_key = vid_sect->GetKeyByName("ScreenHeight", true);
	if (curr_key != NULL) curr_key->FromInt(_settings.scr_height);

	curr_key = vid_sect->GetKeyByName("Windowed", true);
	if (curr_key != NULL) curr_key->FromBool(!_settings.fullscreen);
	  
	curr_key = vid_sect->GetKeyByName("FOV", true);
	if (curr_key != NULL) curr_key->FromInt(_settings.FOV);

	curr_key = vid_sect->GetKeyByName("UseShdMap", true);
	if (curr_key != NULL) curr_key->FromBool(_settings.use_shadowmap);

	curr_key = vid_sect->GetKeyByName("UseFog", true);
	if (curr_key != NULL) curr_key->FromBool(_settings.use_fog);

	curr_key = vid_sect->GetKeyByName("Wireframe", true);
	if (curr_key != NULL) curr_key->FromBool(_settings.use_wireframe);
  }

  //Audio Settings
  GC_SECTION *aud_sect = set_conf->GetSectionByName("Audio", true);

  if (aud_sect != NULL) 
  {
    GC_KEY *curr_key = aud_sect->GetKeyByName("UseSound", true);
	if (curr_key != NULL) curr_key->FromBool(_settings.use_sound);
  }

  //Save
  if (!myGDevice->ConfMgr->SaveConfig(G_CUID_GAMECONFIG))
	return false;

  return true;
}

//-----------------------------------------------

g_bool MainGame::LoadSettings()
{
  G_CONFIG *set_conf = myGDevice->ConfMgr->GetConfig(G_CUID_GAMECONFIG);

  if (set_conf == NULL) return false;

  //Load Settings

  GC_SECTION *vid_sect = set_conf->GetSectionByName("Video", true);

  if (vid_sect != NULL) 
  {
	GC_KEY *curr_key = vid_sect->GetKeyByName("ScreenWidth", true, "800");
	if (curr_key != NULL) _settings.scr_width = curr_key->AsInt();

	curr_key = vid_sect->GetKeyByName("ScreenHeight", true, "600");
	if (curr_key != NULL) _settings.scr_height = curr_key->AsInt();

	curr_key = vid_sect->GetKeyByName("Windowed", true, "True");
	if (curr_key != NULL) _settings.fullscreen = !curr_key->AsBool();
	  
	curr_key = vid_sect->GetKeyByName("FOV", true, "65");
	if (curr_key != NULL) _settings.FOV = curr_key->AsInt();

	curr_key = vid_sect->GetKeyByName("UseShdMap", true, "False");
	if (curr_key != NULL) _settings.use_shadowmap = curr_key->AsBool();

	curr_key = vid_sect->GetKeyByName("UseFog", true, "False");
	if (curr_key != NULL) _settings.use_fog = curr_key->AsBool();

	curr_key = vid_sect->GetKeyByName("Wireframe", true, "False");
	if (curr_key != NULL) _settings.use_wireframe = curr_key->AsBool();
  }

  GC_SECTION *aud_sect = set_conf->GetSectionByName("Audio", true);

  if (aud_sect != NULL) 
  {
    GC_KEY *curr_key = aud_sect->GetKeyByName("UseSound", true, "True");
	if (curr_key != NULL) _settings.use_sound = curr_key->AsBool();
  }

  return true;
}

//-----------------------------------------------
