#include "GScene.h"

//-----------------------------------------------

GScene::GScene(GD3DDevice *Device)
{
  _device = Device;
  _next_uid = 0;
  _defcamera = NULL;

  D3DXMatrixIdentity(&_mworld);
}

//-----------------------------------------------

GScene::~GScene(void)
{

}

//-----------------------------------------------

void GScene::SetDefaultCamera(g_uint32 UID)
{
  GCamera* _cam = _device->ResMgr->GetCameraByUID(UID);
  if (_cam != NULL)
	_defcamera = _cam;
}

//-----------------------------------------------

void GScene::AddToDrawList(GModel *Model)
{
  if (Model == NULL) return;
  for (g_uint32 i = 0; i<_draw_list.size(); i++)
    if (_draw_list[i] == Model)
      return;
  _draw_list.push_back(Model);
}

//-----------------------------------------------

void GScene::RemoveFromDrawList(GModel *Model)
{
  for (g_uint32 i = 0; i<_draw_list.size(); i++)
    if (_draw_list[i] == Model)
      _draw_list.erase(_draw_list.begin() + i);
}

//-----------------------------------------------

void GScene::ClearDrawList()
{
  _draw_list.clear();
}

//-----------------------------------------------

g_bool GScene::Render()
{
  if ((_defcamera == NULL) || (_defcamera->Enabled == false)) return false;
  _defcamera->SetViewport();

  //_device->D3DDevice->ClearDepthStencilView(_device->pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0 );//???
  
  for (g_uint32 i=0;i<_draw_list.size();i++)
  {
	_draw_list[i]->SetWorldMatrix(GetWorldMatrix());
	_draw_list[i]->SetViewProjMatrix(_defcamera->GetViewProjMatrix());
	_draw_list[i]->Draw();
  }

  return true;
}

//-----------------------------------------------

void GScene::ResetWorldMatrix()
{
  D3DXMatrixIdentity(&_mworld);
}

//-----------------------------------------------

void GScene::CheckFPS(g_uint8 fpsid)
{
  if (fpsid < _fpsinfo.size())
  {
    _fpsinfo[fpsid]._frc++;
    if (_fpsinfo[fpsid]._frc >= 25 )
    {
      _fpsinfo[fpsid]._frc = 0;
      double ft = _fpsinfo[fpsid]._lastft;
      _fpsinfo[fpsid]._lastft = GetTicks();
      _fpsinfo[fpsid]._fps = g_float(25000/(_fpsinfo[fpsid]._lastft-ft));
    }
  }
}

//-----------------------------------------------

g_float GScene::GetFPS(g_uint8 fpsid)
{
  if (fpsid < _fpsinfo.size())
    return _fpsinfo[fpsid]._fps;
  else
	return 0;
}

//-----------------------------------------------

g_uint8 GScene::AddFPSInfo()
{
  FPS_INFO fi;
  fi._frc = 0;
  _fpsinfo.push_back(fi);
  return _fpsinfo.size()-1;
}

//-----------------------------------------------

void GScene::RemoveFPSInfo(g_uint8 fpsid)
{
  if (fpsid < _fpsinfo.size())
    _fpsinfo.erase(_fpsinfo.begin()+fpsid);
}

//-----------------------------------------------

void GScene::Release()
{ 
  ClearDrawList();
}

//-----------------------------------------------
