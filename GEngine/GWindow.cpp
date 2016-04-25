#include "GWindow.h"

//-----------------------------------------------

GWindow::GWindow()
{
  _handle = 0;
}

//-----------------------------------------------

GWindow::~GWindow(void)
{

}

//-----------------------------------------------

g_bool GWindow::Create(const g_char *ClassName, const g_char* Title, g_int32 Width, g_int32 Height, 
	                   HINSTANCE hInst, WNDPROC WinProc)
{
  _hinst = hInst;
  
  WNDCLASSEX wincl;

  wincl.hInstance = hInst;
  wincl.lpszClassName = ClassName;
  wincl.lpfnWndProc = WinProc;     
  wincl.style = CS_DBLCLKS;               
  wincl.cbSize = sizeof (WNDCLASSEX);

  wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
  wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
  wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
  wincl.lpszMenuName = NULL;                
  wincl.cbClsExtra = 0;                    
  wincl.cbWndExtra = 0;   
  wincl.hbrBackground = NULL;

  if (!RegisterClassEx (&wincl))
	return false;

  RECT WRect = {0,0,Width,Height};
  AdjustWindowRect(&WRect,WS_OVERLAPPEDWINDOW,false);

  _handle = CreateWindowEx (
            0,                      /* Extended possibilites for variation */
            ClassName,              /* Classname */
            Title,                  /* Title Text */
            WS_OVERLAPPEDWINDOW,    /* Window Style */
            0,                      /* X position */
            0,                      /* Y position */
            WRect.right-WRect.left, /* Width */
            WRect.bottom-WRect.top, /* Height */
            HWND_DESKTOP,           /* Parent Handle */
            NULL,                   /* No menu */
            hInst,                  /* Program Instance handler */
            NULL                    /* No Window Creation data */
            );

  return true;
}

//-----------------------------------------------

void GWindow::Show(g_int32 cmdshow)
{
  ShowWindow(_handle, cmdshow);
}

//-----------------------------------------------

void GWindow::SetCaption(const g_char *Caption)
{
  SetWindowText(_handle, Caption);
}

//-----------------------------------------------

void GWindow::SetSize(g_uint32 width, g_uint32 height)
{
  WINDOWPLACEMENT wndplace;
  GetWindowPlacement(_handle, &wndplace);

  ResizeRect(wndplace.rcNormalPosition, width, height);
  AdjustWindowRect(&wndplace.rcNormalPosition, WS_OVERLAPPEDWINDOW, false);
  MoveRect(wndplace.rcNormalPosition, 0, 0);
  
  SetWindowPlacement(_handle, &wndplace);
}

//-----------------------------------------------
