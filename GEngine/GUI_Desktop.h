#pragma once

#include "GUI_BaseObject.h"

#ifndef GUI_DESKTOP_H
#define GUI_DESKTOP_H

class GUI_Desktop :
	public GUI_BaseObject
{
public:
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

	virtual void UpdateObj(g_mat4x4 &mWorld, g_mat4x4 &mViewProj, GSprite *Sprite) {};
	
	//������ � �������
    //virtual void GetRawSettings(void **pBuffer, g_uint32 &BufSize);
    //virtual void SetRawSettings(void *Buffer, g_uint32 BufSize);

	//����������� � ����������
	GUI_Desktop(const g_string &Name, GD3DDevice *Device);
	~GUI_Desktop(void);
private:
	void _init_desk();
};

#endif //GUI_DESKTOP_H