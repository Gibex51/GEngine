#pragma once

#ifndef GCONFMGR_H
#define GCONFMGR_H

#include "GFileStream.h"

//---Sub Structs---//

struct GC_KEY
{
  g_char Key[256];
  g_char Value[256];

  g_int32  AsInt();
  g_float  AsFloat();
  g_bool   AsBool();
  g_uint32 AsHexInt();

  void FromInt(g_int32 Val);
  void FromFloat(g_float Val);
  void FromBool(g_bool Val);
};

struct GC_SECTION
{
  g_char Name[128];
  std::vector<GC_KEY*> Keys;

  GC_KEY* AddKey(g_char *Key, g_char *Value);
  void RemoveKey(g_uint32 Key_Index);
  void RemoveKey(g_char *Key_Name);
  void RemoveAllKeys();

  GC_KEY*  GetKeyByName(g_char *Key_Name, g_bool CreateIfNoExists = false, g_char *CreateValue = NULL);
  g_int32 GetKeyIndex(g_char *Key_Name);
};

//---GConfig---//

struct G_CONFIG
{
  g_uint8 UID;
  g_char FileName[G_MAX_NAME_LEN];
  std::vector<GC_SECTION*> Sections;

  GC_SECTION* AddSection(g_char *Sect_Name);
  void RemoveSection(g_uint32 Sect_Index);
  void RemoveSection(g_char *Sect_Name);
  void RemoveAllSections();
  
  GC_SECTION* GetSectionByName(g_char *Sect_Name, g_bool CreateIfNoExists = false);
  g_int32 GetSectionIndex(g_char *Sect_Name);
};

//---GConfigMgr---//

class GConfigMgr
{
public:
  std::vector<G_CONFIG*> Configs;
  
  g_bool   LoadConfig(const g_string &FileName, g_uint8 UID);
  g_bool   CreateConfig(const g_string &FileName, g_uint8 UID);
  g_bool   SaveConfig(g_uint8 UID);
  g_bool   CloseConfig(g_uint8 UID); //Without saving
  void     CloseAllConfigs(); //Without saving
  void     SaveAllConfigs();

  G_CONFIG* GetConfig(g_uint8 UID);
  g_int32 GetConfigIndex(g_uint8 UID);

  //Конструктор и деструктор
  GConfigMgr(void);
  ~GConfigMgr(void);
};

#endif //GCONFMGR_H