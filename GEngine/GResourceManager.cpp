#include "GResourceManager.h"

//-----------------------------------------------

GResourceManager::GResourceManager(GD3DDevice *Device)
{
  _device = Device;
}

//-----------------------------------------------

GResourceManager::~GResourceManager(void)
{
  Release();
}

//-----------------------------------------------

void GResourceManager::_del_res(GBaseObject *res) const
{
  if (res != NULL)
    switch (res->GetType())
	{
	  case G_OT_SPRITE:     delete dynamic_cast<GSprite*>(res); break;
	  case G_OT_MATERIAL:   delete dynamic_cast<GMaterial*>(res); break;
	  case G_OT_EFFECT:     delete dynamic_cast<GEffect*>(res); break;
	  case G_OT_TEXTURE:    delete dynamic_cast<GTexture*>(res); break;
	  case G_OT_CAMERA:     delete dynamic_cast<GCamera*>(res); break;
	  case G_OT_EFFECTPOOL: delete dynamic_cast<GEffectPool*>(res); break;
      case G_OT_MODEL:      delete dynamic_cast<GModel*>(res); break;
      case G_OT_FONT:       delete dynamic_cast<GFont*>(res); break;
	}
}

//-----------------------------------------------

void GResourceManager::Release()
{
  map<g_uint32, GBaseObject*>::iterator it;
  for (it = _resources.begin(); it != _resources.end(); it++)
    _del_res((*it).second);
  _resources.clear();
}

//-----------------------------------------------

GModel* GResourceManager::AddModel(g_uint32 UID, const g_string &Name, GMaterial *Material, g_uint32 TechID, 
		                 g_uint32 VertexSize, g_int32 InputLayoutID)
{
  GModel *NewModel = new GModel(Name, _device);
  NewModel->SetMaterial(Material, TechID);
  NewModel->SetInstanceVariables(3);
  NewModel->InitMesh(VertexSize, G_PT_TRIANGLE_LIST, InputLayoutID);
  NewModel->SetUID(UID);
  _del_res(_resources[UID]);
  _resources[UID] = NewModel;
  return NewModel;
}

//-----------------------------------------------

GMaterial* GResourceManager::AddMaterial(g_uint32 UID, const g_string &Name, GEffect *Effect)
{
  GMaterial *NewMaterial = new GMaterial(Name,_device); 
  NewMaterial->SetUID(UID);
  NewMaterial->Effect = Effect;
  _del_res(_resources[UID]);
  _resources[UID] = NewMaterial;
  return NewMaterial;
}

//-----------------------------------------------

GTexture* GResourceManager::AddTexture(g_uint32 UID, const g_string &Name, const g_string &Path, g_uint8 Type)
{
  GTexture *NewTexture;
  if (Path != "")
    NewTexture = new GTexture(Name,Path,Type,true,_device);
  else
    NewTexture = new GTexture(Name,_device);
  NewTexture->SetUID(UID);
  _del_res(_resources[UID]);
  _resources[UID] = NewTexture;
  return NewTexture;
}

//-----------------------------------------------

GCamera* GResourceManager::AddCamera(g_uint32 UID, const g_string &Name, g_int32 Width, g_int32 Height, g_bool Ortho, g_float Near, g_float Far)
{
  GCamera *NewCamera = new GCamera(Name,_device);
  NewCamera->SetUID(UID);
  NewCamera->InitViewport(Width, Height);
  NewCamera->InitProjection(Ortho, Near, Far);
  _del_res(_resources[UID]);
  _resources[UID] = NewCamera;
  return NewCamera;
}

//-----------------------------------------------

GEffectPool* GResourceManager::AddEffectPool(g_uint32 UID, const g_string &Name, const g_string &fxPath)
{
  GEffectPool *NewEP = new GEffectPool(fxPath, Name, _device);
  NewEP->SetUID(UID);
  
  _del_res(_resources[UID]);
  _resources[UID] = NewEP;
  return NewEP;
}

//-----------------------------------------------

GEffect* GResourceManager::AddEffect(g_uint32 UID, const g_string &Name, const g_string &fxPath, GEffectPool* EffectPool)
{
  GEffect *NewEffect = new GEffect(fxPath, EffectPool, Name, _device);
  NewEffect->SetUID(UID);
  
  _del_res(_resources[UID]);
  _resources[UID] = NewEffect;
  return NewEffect;
}

//-----------------------------------------------

GFont* GResourceManager::AddFont(g_int32 UID, const g_string &FontName, g_int32 FontSize, g_color FontColor,
						         g_uint32 FontWeight, g_bool FontItalic)
{
  GFont *NewFont = new GFont(FontName,FontSize,FontColor,FontWeight,FontItalic,"Font",_device);
  NewFont->SetUID(UID);

  _del_res(_resources[UID]);
  _resources[UID] = NewFont;
  return NewFont;
}

//-----------------------------------------------

GSprite* GResourceManager::AddSprite(g_uint32 UID, const g_string &Name)
{
  GSprite *NewSprite = new GSprite(Name,_device); 
  NewSprite->SetUID(UID);

  _del_res(_resources[UID]);
  _resources[UID] = NewSprite;
  return NewSprite;
}

//-----------------------------------------------

GEffectPool* GResourceManager::GetEffectPoolByUID(g_uint32 UID)
{
  return dynamic_cast<GEffectPool*>(_resources[UID]);
}

//-----------------------------------------------

GEffect* GResourceManager::GetEffectByUID(g_uint32 UID)
{
  return dynamic_cast<GEffect*>(_resources[UID]);
}

//-----------------------------------------------

GCamera* GResourceManager::GetCameraByUID(g_uint32 UID)
{
  return dynamic_cast<GCamera*>(_resources[UID]);
}

//-----------------------------------------------

GTexture* GResourceManager::GetTextureByUID(g_uint32 UID)
{
  return dynamic_cast<GTexture*>(_resources[UID]);
}

//-----------------------------------------------

GMaterial* GResourceManager::GetMaterialByUID(g_uint32 UID)
{
  return dynamic_cast<GMaterial*>(_resources[UID]);
}

//----------------------------------------------

GModel* GResourceManager::GetModelByUID(g_uint32 UID)
{
  return dynamic_cast<GModel*>(_resources[UID]);
}

//-----------------------------------------------

GSprite* GResourceManager::GetSpriteByUID(g_uint32 UID)
{
  return dynamic_cast<GSprite*>(_resources[UID]);
}

//-----------------------------------------------

GFont* GResourceManager::GetFontByUID(g_uint32 UID)
{
  return dynamic_cast<GFont*>(_resources[UID]);
}

//-----------------------------------------------

void GResourceManager::RemoveResourceByUID(g_uint32 UID)
{
  GBaseObject *CurrRes = _resources[UID];
  _del_res(CurrRes);
  _resources.erase(UID);
}

//-----------------------------------------------

void GResourceManager::RemoveResourcesByUIDRange(g_uint32 MinUID, g_uint32 MaxUID)
{
  vector <g_uint32> ObjectsToRemoveList;

  map<g_uint32, GBaseObject*>::iterator it;
  for (it = _resources.begin(); it != _resources.end(); it++)
  {
    GBaseObject *CurrRes = (*it).second;
    if ((CurrRes != NULL) && (CurrRes->GetUID() >= MinUID) && (CurrRes->GetUID() <= MaxUID))
	  ObjectsToRemoveList.push_back(CurrRes->GetUID());
  }

  for (g_uint32 i = 0; i < ObjectsToRemoveList.size(); i++)
    RemoveResourceByUID(ObjectsToRemoveList[i]);
}

//-----------------------------------------------

void GResourceManager::RemoveResourcesByType(g_int32 ResType, g_bool OnlyUserDef)
{
  vector <g_uint32> ObjectsToRemoveList;

  map<g_uint32, GBaseObject*>::iterator it;
  for (it = _resources.begin(); it != _resources.end(); it++)
  {
    GBaseObject *CurrRes = (*it).second;
    if ((CurrRes != NULL) && (CurrRes->GetType() == ResType))
	  ObjectsToRemoveList.push_back(CurrRes->GetUID());
  }

  for (g_uint32 i = 0; i < ObjectsToRemoveList.size(); i++)
  {
	if ((ObjectsToRemoveList[i] > RES_UID_USERDEFINE) || (!OnlyUserDef))
      RemoveResourceByUID(ObjectsToRemoveList[i]);
  }
}

//-----------------------------------------------