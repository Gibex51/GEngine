#pragma once

#include "GEffect.h"
#include "GCamera.h"
#include "GModel.h"
#include "GFont.h"

#ifndef GRESMGR_H
#define GRESMGR_H

class GResourceManager
{
public:
  GModel*      AddModel(g_uint32 UID, const g_string &Name, GMaterial *Material, g_uint32 TechID, 
		                g_uint32 VertexSize, g_int32 InputLayoutID);

  GCamera*     AddCamera(g_uint32 UID, const g_string &Name, g_int32 Width, g_int32 Height, g_bool Ortho, 
	                     g_float Near = 0.1f, g_float Far = 10000.0f);
  
  GFont*       AddFont(g_int32 UID, const g_string &FontName, g_int32 FontSize, g_color FontColor,
				       g_uint32 FontWeight = FW_NORMAL, g_bool FontItalic = false);

  GMaterial*   AddMaterial(g_uint32 UID, const g_string &Name, GEffect *Effect);
  GTexture*    AddTexture(g_uint32 UID, const g_string &Name, const g_string &Path = "", g_uint8 Type = 0);
  GEffectPool* AddEffectPool(g_uint32 UID, const g_string &Name, const g_string &fxPath = "");
  GEffect*     AddEffect(g_uint32 UID, const g_string &Name, const g_string &fxPath = "", GEffectPool* EffectPool = NULL);
  GSprite*     AddSprite(g_uint32 UID, const g_string &Name);  

  GEffectPool* GetEffectPoolByUID(g_uint32 UID);
  GEffect*     GetEffectByUID(g_uint32 UID);
  GCamera*     GetCameraByUID(g_uint32 UID);
  GTexture*    GetTextureByUID(g_uint32 UID);
  GMaterial*   GetMaterialByUID(g_uint32 UID);
  GModel*      GetModelByUID(g_uint32 UID); 
  GSprite*     GetSpriteByUID(g_uint32 UID);
  GFont*       GetFontByUID(g_uint32 UID);


  void RemoveResourceByUID(g_uint32 UID); 
  void RemoveResourcesByUIDRange(g_uint32 MinUID, g_uint32 MaxUID);
  void RemoveResourcesByType(g_int32 ResType, g_bool OnlyUserDef = true);

  void Release();

  GResourceManager(GD3DDevice *Device);
  ~GResourceManager(void);
private:
  std::map <g_uint32, GBaseObject*>  _resources;

  GD3DDevice *_device;

  void _del_res(GBaseObject *res) const;
};

#endif //GRESMGR_H