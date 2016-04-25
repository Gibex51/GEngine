#pragma once

#ifndef MAINGAME_H
#define MAINGAME_H

#include "GResourceManager.h"
#include "GScene.h"
#include "G_UI_MainArea.h"
#include "GAudioMgr.h"

#include "GWindow.h"
#include "G_GM_Player.h"
#include "G_GM_NPCMgr.h"
#include "GPathFinder.h"
#include "GCommandParser.h"

#include "GCubeTerrain.h"

//#include "G_VideoLib.h"
//#include "G_BulletPhysics.h"

#pragma comment (lib, "winmm.lib")

struct G_GAME_SETTINGS
{
  g_bool  fullscreen;
  g_bool  use_shadowmap;
  g_bool  use_fog;
  g_bool  use_wireframe;

  g_bool  use_sound;

  g_int32 scr_width;
  g_int32 scr_height;

  g_int32 FOV; 
};

class MainGame
{
public: 
  g_bool   closeapp;
  g_bool   closethread;

  g_bool   DeviceCreated;

  GWindow          *MainWindow;
  GD3DDevice       *myGDevice;
  GInput           *myGInput;
  GResourceManager *myResMgr;
  GAudioMgr        *myAudioMgr;
  GScene           *MainScene;
  G_UI_MainArea    *MyUI;

  G_GM_Player     *MyPlayer;
  G_GM_NPCMgr     *MyNPCMgr;

  GCubeTerrain    *MyCT;

  //G_BulletPhysics *Physics;
  //G_VideoLib      *FFVideo;

  /* Variables For Test */
  g_vec2           LightAng;
  g_float          maxfps,minfps;

  g_uint8          game_mode;
  g_bool           cam_heightalign;
  g_bool           sel_cubealign;

  g_int8           tool_type;
  g_vec3           sel_start, sel_end;
  g_bool           sel_started;
  g_ivec3          sel_start_acpos, sel_end_acpos;
  g_double         upd_time;

  vector <DXGI_MODE_DESC> _resolut_list;

  GUI_Label       *FPSLabel, *MouseLabel;
  GUI_ListBox     *CubesLB, *NPCLB, *CubeObjLB, *LogLB;
  GUI_Image       *PreviewImg, *LightCamImg;
  GUI_ScrollList  *GroupsList, *ResolutList;

  GUI_Window      *SaveLoadWindow;
  GUI_Edit        *SaveLoadEdit;
  GUI_Label       *SaveLoadLabel;
  GUI_ListBox     *SaveLoadList;

  GUI_Edit        *ConsoleEdit;
 
  GUI_ButtonGrid  *CEditorBGrid, *MenuBGrid;

  GUI_Window      *WinLog;

  GMaterial        *mtrSelect, *mtrObjPreview, *mtrSky;
  GEffect          *fxCT, *fxDef, *fxObjPrev, *fxSky;
  GEffectPool      *fxpCT, *fxpDef;

  GSourceVoice     *sVoice, *rVoice;

  G_AABB           ClipBox;
  GModel           *ClipModel;

  G_UNI_CUBE_OBJECT loaded_object;

  D3D10_TEXTURE2D_DESC DESC_CO_PREV;
  D3D10_TEXTURE2D_DESC DESC_LIGHT_CAM;

  //Functions
  void ConsoleInput(const string &CmdLine);
  void UIEventProc(GUI_EVENTPROC_DESC *eDesc);
  void LogProc(const string &Message);
  void StartRenderCycle();
  void MainCycle();

  //Input
  void Input_CubeEditor(g_bool HaveEvent);
  void Input_NPCEditor(g_bool HaveEven);
  void Input_Game(g_bool HaveEvent);

  void MsgProc(HWND hwnd, g_uint32 message, WPARAM wParam, LPARAM lParam);

  //Init / Release
  g_bool InitWindow(HINSTANCE _hInst, WNDPROC _wndProc, g_int32 _cmdShow);
  g_bool Init3DDevice();
  g_bool InitEngineSubSystems();

  g_bool InitInput();
  void InitSound();
  void InitUI(void (*_uieventproc)(GUI_EVENTPROC_DESC*));

  void CloseApp() {closeapp = true;};

  //Settings
  void SetScreen(DXGI_MODE_DESC mode_desc, g_bool Windowed);
  void SetShadowmap(g_bool Enable);
  void SetFog(g_bool Enable);
  void SetWireframe(g_bool Enable);
  void SetSound(g_bool Enable);
  void SetFOV(g_int32 FOV);
  
  g_bool SaveSettings();
  g_bool LoadSettings();

  //Конструктор и деструктор
  MainGame(void);
  ~MainGame(void);
private:
  //Render
  g_bool pause_render, is_paused_render;

  void RenderCubeObjectPreview();
  void RenderLightCam();

  void PauseRender();
  void ContinueRender();

  //Console
  vector <string> console_history;
  g_uint32 console_history_index;

  void OutputAdaptersInfo();

  //Settings
  G_GAME_SETTINGS _settings;

  //Events
  void (*_eventcallback)(GUI_EVENTPROC_DESC*);

  //Init
  void InitVariables();

  //Main Menu
  void MainMenu_Init();
  void MainMenu_Release();

  //Editor
  void Editor_Init();
  void Editor_Release();

  //Game
  void Game_Init();
  void Game_Release();

  void UpdateClipMesh(g_ivec2 MapSize);

  g_uint16 GetSelectedCubeIndex();

  //Test Functions
  void Test_Add_Meshes();
  void Test_Add_UI_Elements();
};

#endif //MAINGAME_H