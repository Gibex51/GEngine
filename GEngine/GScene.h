#pragma once

#include "GObjectLoader.h"
#include "GEffectPool.h"
#include "GCamera.h"
#include "GModel.h"
#include "GResourceManager.h"

#ifndef GSCENE_H
#define GSCENE_H

struct FPS_INFO
{
  g_int16    _frc;
  g_float    _fps;
  g_double   _lastft;
};

class GScene
{
public:   
	void   AddToDrawList(GModel *Model);
    void   RemoveFromDrawList(GModel *Model);
	void   ClearDrawList();

	g_bool Render();

	g_float GetFPS(g_uint8 fpsid);
	void CheckFPS(g_uint8 fpsid);
	g_uint8 AddFPSInfo();
	void RemoveFPSInfo(g_uint8 fpsid);

	void SetDefaultCamera(GCamera *DefCamera) {_defcamera = DefCamera;};
	void SetDefaultCamera(g_uint32 UID);
    GCamera* GetDefaultCamera() {return _defcamera;};

	void ResetWorldMatrix();
	g_mat4x4& GetWorldMatrix() {return _mworld;};

	void Release();

	//Конструктор и деструктор
	GScene(GD3DDevice *Device);
	~GScene(void);
private:
	std::vector <GModel*> _draw_list;

	g_mat4x4   _mworld;
    g_uint32   _next_uid;

	GD3DDevice *_device;

	GCamera    *_defcamera;

	vector <FPS_INFO> _fpsinfo;
};

#endif /* GSCENE_H */