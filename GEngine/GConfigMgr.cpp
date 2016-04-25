#include "GConfigMgr.h"

//-----------------------------------------------
//---GC_KEY---//
//-----------------------------------------------

g_int32 GC_KEY::AsInt()
{ 
  if (this == NULL) return 0;
  return atoi(Value);
}

//-----------------------------------------------

g_float GC_KEY::AsFloat()
{
  if (this == NULL) return 0;
  return g_float(atof(Value));
}

//-----------------------------------------------

g_bool GC_KEY::AsBool()
{
  if (this == NULL) return false;

  string ValueStr = Value;
  std::transform(ValueStr.begin(), ValueStr.end(), ValueStr.begin(), ::toupper);
  return ((ValueStr == "TRUE") || (ValueStr == "1") || (ValueStr == "YES"));
}

//-----------------------------------------------

g_uint32 GC_KEY::AsHexInt()
{
  if (this == NULL) return 0;

  return 0;
}

//-----------------------------------------------

void GC_KEY::FromInt(g_int32 Val)
{ 
  if (this == NULL) return;
  _itoa_s(Val, Value, 255, 10);
}

//-----------------------------------------------

void GC_KEY::FromFloat(g_float Val)
{
  if (this == NULL) return;
  sprintf_s(Value, 255, "%.5f", Val);
}

//-----------------------------------------------

void GC_KEY::FromBool(g_bool Val)
{
  if (this == NULL) return;
  if (Val) 
	memcpy(Value, "True", 5);
  else
	memcpy(Value, "False", 6);
}

//-----------------------------------------------
//---GC_SECTION---//
//-----------------------------------------------

GC_KEY* GC_SECTION::GetKeyByName(g_char *Key_Name, g_bool CreateIfNoExists, g_char *CreateValue)
{
  if (this == NULL) return NULL;

  for (g_uint32 i = 0; i<Keys.size(); i++)
  {
	if (strcmp(Keys[i]->Key,Key_Name)==0)
	  return Keys[i];
  }

  if (CreateIfNoExists)
  {
	if (CreateValue == NULL)
      return AddKey(Key_Name, "\0\0");
	else
	  return AddKey(Key_Name, CreateValue);
  }

  return NULL;
}

//-----------------------------------------------

g_int32 GC_SECTION::GetKeyIndex(g_char *Key_Name)
{
  for (g_uint32 i = 0; i<Keys.size(); i++)
  {
	if (strcmp(Keys[i]->Key,Key_Name)==0)
	  return i;
  }
  return -1;
}

//-----------------------------------------------

GC_KEY* GC_SECTION::AddKey(g_char *Key, g_char *Value)
{
  GC_KEY *NewKey = new GC_KEY;
  memset(NewKey,0,sizeof(GC_KEY));

  if (Key!=NULL)
    strncpy_s(&NewKey->Key[0],256,&Key[0],strlen(Key));
  if (Value!=NULL)
    strncpy_s(&NewKey->Value[0],256,&Value[0],strlen(Value));

  Keys.push_back(NewKey);

  return NewKey;
}

//-----------------------------------------------

void GC_SECTION::RemoveKey(g_uint32 Key_Index)
{
  if (Key_Index < Keys.size())
  {
	delete Keys[Key_Index];
	Keys.erase(Keys.begin()+Key_Index);
  }
}

//-----------------------------------------------

void GC_SECTION::RemoveAllKeys()
{
  for (g_uint32 i = 0; i<Keys.size(); i++)
	delete Keys[i];

  Keys.clear();
}

//-----------------------------------------------

void GC_SECTION::RemoveKey(g_char *Key_Name)
{
  g_int32 KInd = GetKeyIndex(Key_Name);
  if (KInd > 0)
	RemoveKey(KInd);
}

//-----------------------------------------------
//---G_CONFIG---//
//-----------------------------------------------

GC_SECTION* G_CONFIG::GetSectionByName(g_char *Sect_Name, g_bool CreateIfNoExists)
{
  if (this == NULL) return NULL;

  for (g_uint32 i = 0; i<Sections.size(); i++)
  {
	if (strcmp(Sections[i]->Name,Sect_Name)==0)
	  return Sections[i];
  }

  if (CreateIfNoExists)
    return AddSection(Sect_Name);

  return NULL;
}

//-----------------------------------------------

g_int32 G_CONFIG::GetSectionIndex(g_char *Sect_Name)
{
  for (g_uint32 i = 0; i<Sections.size(); i++)
  {
	if (strcmp(Sections[i]->Name,Sect_Name)==0)
	  return i;
  }
  return -1;
}

//-----------------------------------------------

GC_SECTION* G_CONFIG::AddSection(g_char *Sect_Name)
{
  GC_SECTION *NewSection = new GC_SECTION;
  memset(NewSection->Name,0,128);

  if (Sect_Name!=NULL)
    strncpy_s(&NewSection->Name[0],128,&Sect_Name[0],strlen(Sect_Name));

  Sections.push_back(NewSection);

  return NewSection;
}

//-----------------------------------------------

void G_CONFIG::RemoveSection(g_uint32 Sect_Index)
{
  if (Sect_Index < Sections.size())
  {
	Sections[Sect_Index]->RemoveAllKeys();
	delete Sections[Sect_Index];
	Sections.erase(Sections.begin()+Sect_Index);
  }
}

//-----------------------------------------------

void G_CONFIG::RemoveSection(g_char *Sect_Name)
{
  g_int32 SInd = GetSectionIndex(Sect_Name);
  if (SInd > 0)
	RemoveSection(SInd);
}

//-----------------------------------------------

void G_CONFIG::RemoveAllSections()
{
  for (g_uint32 i = 0; i<Sections.size(); i++)
  {
	Sections[i]->RemoveAllKeys();
	delete Sections[i];
  }
  Sections.clear();
}

//-----------------------------------------------
//---GConfigMgr---//
//-----------------------------------------------

GConfigMgr::GConfigMgr(void)
{

}

//-----------------------------------------------

GConfigMgr::~GConfigMgr(void)
{
  CloseAllConfigs();
}

//-----------------------------------------------

g_bool GConfigMgr::CreateConfig(const g_string &FileName, g_uint8 UID)
{
  g_bool Result = true;

  if (!FileExists(FileName.c_str()))
  {
	G_CONFIG *NewConfig = new G_CONFIG;
	NewConfig->UID = UID;
	memset(NewConfig->FileName, 0, G_MAX_NAME_LEN);
	strcpy_s(&NewConfig->FileName[0], G_MAX_NAME_LEN, FileName.c_str());
	Configs.push_back(NewConfig);
  }
  else
	Result = false;

  return Result;
}

//-----------------------------------------------

g_bool GConfigMgr::LoadConfig(const g_string &FileName, g_uint8 UID)
{
  if (FileExists(FileName.c_str()))
  { 
	//Creating New Config
	G_CONFIG *NewConfig = new G_CONFIG;
	NewConfig->UID = UID;
	memset(NewConfig->FileName, 0, G_MAX_NAME_LEN);
	strcpy_s(&NewConfig->FileName[0], G_MAX_NAME_LEN, FileName.c_str());
	Configs.push_back(NewConfig);
	
	//Reading Data From File
    GFileStream ConfFS;
    if (!ConfFS.Create(FileName, G_FSMODE_READ))
      return false;

    g_char *buffer;
    g_uint32 fSize = ConfFS.ReadEntireFile((void**)&buffer);
    ConfFS.Release();

    if (fSize == 0)
      return false;

	//Parsing Data
	g_uint32 prevpos = 0;
	g_char TextLine[515];
	GC_SECTION *CurrSection = NULL;

	for (g_uint32 i = 0; i < fSize; i++)
	{
	  if ((buffer[i] == 10) || (buffer[i] == 13))
	  {
		g_uint32 LineLen = i-prevpos;
		if (LineLen > 4)
		{
		  memset(&TextLine[0],0,515);
		  strncpy_s(&TextLine[0],515,&buffer[prevpos],LineLen);

		  if (TextLine[0] == '[') //Next Section
		  {
			g_char sect_name[128];
			memset(sect_name,0,128);
			for (g_uint16 j = 1; j < LineLen; j++)
			  if (TextLine[j] == ']')
			  {
				strncpy_s(&sect_name[0],128,&TextLine[1],j-1);
				break;
			  }
            CurrSection = NewConfig->AddSection(sect_name);
		  }
		  else  //Next Key
		  {
			g_char key[256];
			g_char value[256];
			memset(key,0,256);
			memset(value,0,256);

			for (g_uint16 j = 0; j < LineLen; j++)
			  if (TextLine[j] == '=')
			  {
				strncpy_s(&key[0],256,&TextLine[0],j);
				strncpy_s(&value[0],256,&TextLine[j+1],LineLen-j-1);
				break;
			  }

			if (CurrSection != NULL)
			  if (key[0] != 0)
				CurrSection->AddKey(key,value);
		  }
		}
		prevpos = i+1;
	  }
	}

	//Clear
	delete[]buffer;
  }
  else
	return false;

  return true;
}

//-----------------------------------------------

g_bool GConfigMgr::SaveConfig(g_uint8 UID)
{
  G_CONFIG *CurrConfig = GetConfig(UID);

  if (CurrConfig != NULL)
  {
	//Build Data
    g_char *WriteBuffer = new g_char[1048576];
	memset(&WriteBuffer[0],0,1048576);
	g_uint32 writepos = 0;
	g_int32  written = 0;
	for (g_uint32 i=0; i<CurrConfig->Sections.size();i++)
	{
	  written = sprintf_s(&WriteBuffer[writepos],515,"[%s]\n",CurrConfig->Sections[i]->Name);
	  writepos += written;

	  for (g_uint32 j=0; j<CurrConfig->Sections[i]->Keys.size();j++)
	  {
		written = sprintf_s(&WriteBuffer[writepos],515,"%s=%s\n",CurrConfig->Sections[i]->Keys[j]->Key,
		                    CurrConfig->Sections[i]->Keys[j]->Value);
	    writepos += written;
	  }

	  written = sprintf_s(&WriteBuffer[writepos],515,"\n");
	  writepos += written;
	}
    
    //Write To File
    GFileStream ConfFS;
    if (ConfFS.Create(CurrConfig->FileName, G_FSMODE_WRITE))
    {   
      ConfFS.Write(&WriteBuffer[0], writepos);
      ConfFS.Release();
    }

    //Clear
	delete[]WriteBuffer;
  }
  else
	return false;

  return true;
}

//-----------------------------------------------

g_bool GConfigMgr::CloseConfig(g_uint8 UID)
{
  g_int32 ConfInd = GetConfigIndex(UID);
  
  if (ConfInd > -1)
  {
	Configs[ConfInd]->RemoveAllSections();
	delete Configs[ConfInd];
	Configs.erase(Configs.begin()+ConfInd);
  }
  else
	return false;

  return true;
}

//-----------------------------------------------

G_CONFIG* GConfigMgr::GetConfig(g_uint8 UID)
{
  if (this == NULL) return NULL;

  for (g_uint32 i = 0; i<Configs.size(); i++)
  {
	if (Configs[i]->UID == UID)
	  return Configs[i];
  }
  return NULL;
}

//-----------------------------------------------

g_int32 GConfigMgr::GetConfigIndex(g_uint8 UID)
{
  for (g_uint32 i = 0; i<Configs.size(); i++)
  {
	if (Configs[i]->UID == UID)
	  return i;
  }
  return -1;
}

//-----------------------------------------------

void GConfigMgr::CloseAllConfigs()
{
  for (g_uint32 i = 0; i<Configs.size(); i++)
  {
	Configs[i]->RemoveAllSections();
	delete Configs[i];
  }
  Configs.clear();
}

//-----------------------------------------------

void GConfigMgr::SaveAllConfigs()
{

}

//-----------------------------------------------
