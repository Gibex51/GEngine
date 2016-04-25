#include "GUI_Desktop.h"

//-----------------------------------------------

GUI_Desktop::GUI_Desktop(const g_string &Name, GD3DDevice *Device):GUI_BaseObject(Name, Device)
{
  _init_desk();
}

//-----------------------------------------------

GUI_Desktop::~GUI_Desktop(void)
{
  
}

//-----------------------------------------------

void GUI_Desktop::_init_desk()
{
  SetType(G_OT_UI_DESKTOP);
}

//-----------------------------------------------
/*
void GUI_Desktop::GetRawSettings(void **pBuffer, g_uint32 &BufSize)
{
  
}

//-----------------------------------------------

void GUI_Desktop::SetRawSettings(void *Buffer, g_uint32 BufSize)
{
  
}
*/
//-----------------------------------------------