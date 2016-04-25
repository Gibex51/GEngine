#pragma once

#ifndef GWINDOW_H
#define GWINDOW_H

#include "GStructs.h"

class GWindow
{
public:
  g_bool Create(const g_char *ClassName, const g_char* Title, g_int32 Width, g_int32 Height, 
	            HINSTANCE hInst, WNDPROC WinProc);

  void Show(g_int32 cmdshow); 
  void SetSize(g_uint32 width, g_uint32 height);

  const HWND GetHandle() {return _handle;};
  const HINSTANCE GetInstance() {return _hinst;};

  void SetCaption(const g_char *Caption);
  //Конструктор и деструктор
  GWindow();
  ~GWindow();
private:
  HWND _handle;
  HINSTANCE _hinst;
};

#endif //GWINDOW_H