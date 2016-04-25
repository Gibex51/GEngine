#include <windows.h>
#include <time.h>
// Game Headers//
#include "MainGame.h"

//-----------------------------------------------

LRESULT CALLBACK WindowProcedure (HWND, g_uint32, WPARAM, LPARAM);

/* Global Variables */

MainGame MyGame;

//-----------------------------------------------

DWORD RenderProc(LPVOID Param)
{
  MyGame.StartRenderCycle();
  ExitThread(0);
  return 0;
}

//-----------------------------------------------

void EventProc(GUI_EVENTPROC_DESC* eDesc)
{
  MyGame.UIEventProc(eDesc);
}

//-----------------------------------------------

void MyLogProc(const string &Msg)
{
  MyGame.LogProc(Msg);
}

//-----------------------------------------------

g_int32 WINAPI WinMain (HINSTANCE hThisInstance,
                        HINSTANCE hPrevInstance,
                        LPSTR pCmdLine,
                        g_int32 nShowCmd)
{
  SetLogFunction(MyLogProc);
  MyGame.InitWindow(hThisInstance,WindowProcedure,nShowCmd);

  if (MyGame.Init3DDevice())
  {
    MyGame.InitUI(EventProc);

    if (!MyGame.InitEngineSubSystems())
      MyGame.CloseApp();
  }
  else
    MyGame.CloseApp();

  // Create Render Process
  g_ulong rthid;
  CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&RenderProc, NULL, 0, &rthid);

  // Message loop
  MSG messages = {0};

  while ((WM_QUIT != messages.message))
  {
    while (PeekMessage(&messages, NULL, 0, 0, PM_REMOVE) == TRUE)
    {
      TranslateMessage(&messages);
      DispatchMessage(&messages);
    }
	Sleep(1); // Тормоз
    MyGame.MainCycle();
  }

  // Exit
  return messages.wParam;
	return 0;
}


//-----------------------------------------------

LRESULT CALLBACK WindowProcedure (HWND hwnd, g_uint32 message, WPARAM wParam, LPARAM lParam)
{
  MyGame.MsgProc(hwnd,message,wParam,lParam);

  switch (message)                 
  {
    case WM_SYSKEYDOWN:
		             if (wParam == VK_MENU) return FALSE;
                     break;
    case WM_SYSKEYUP:
                     if (wParam == VK_MENU) return FALSE;
                     break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break; 
    default:
        return DefWindowProc (hwnd, message, wParam, lParam);
  }

  return 0;
}

//-----------------------------------------------