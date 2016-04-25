#pragma once

#ifndef GCTSAVEMGR_H
#define GCTSAVEMGR_H

#include "G_CT_SectorsMgr.h"

struct G_REG_HEADER
{
  g_bool IsEmpty[MAPDEEP];
};

struct G_MAP_INFO
{
  g_ivec2 map_size;
};

struct G_LOADED_CUBE // 4b
{
  g_uint8 info;
  g_uint16 cube;
};

struct G_UNI_CUBE_OBJECT
{
  g_ivec3        _pos;
  g_ivec3        _size;
  G_LOADED_CUBE *_data;

  G_UNI_CUBE_OBJECT() {_data = NULL; _size = g_ivec3(0,0,0); _pos = g_ivec3(0,0,0);};
  ~G_UNI_CUBE_OBJECT() {Release();};

  void Init(g_ivec3 &area_size, g_ivec3 &pos)
  {
    g_uint32 datasize = (area_size.x+1)*(area_size.y+1)*(area_size.z+1);
	_data = new G_LOADED_CUBE[datasize];
	memset(_data, 0, datasize);
	_size = area_size; 
	_pos = pos;  
  };

  void Release()
  {
	if (_data!=NULL) delete[]_data;
  };
};

class G_CT_SaveMgr
{
public:
  void SetObjectToMap(G_UNI_CUBE_OBJECT &uni_object, g_uint8 rot, g_bool replace_empty, g_bool centering, g_bool make_undo);  
  g_bool GetObjectFromMap(G_UNI_CUBE_OBJECT &uni_object, g_ivec3 min_abs_cpos, g_ivec3 max_abs_cpos);

  g_bool LoadObject(const g_string &filename, G_UNI_CUBE_OBJECT &uni_object);
  g_bool SaveObject(const g_string &filename, const G_UNI_CUBE_OBJECT &uni_object);

  g_bool SaveUndo(const g_string &objname, g_ivec3 min_abs_cpos, g_ivec3 max_abs_cpos);
  g_bool LoadUndo(const g_string &objname, G_UNI_CUBE_OBJECT &loaded_undo);
  void   ClearUndo(const g_string &objname);
  void   SetUndoEnabled(g_bool Enabled) {_undo_enabled = Enabled;};
  g_bool GetUndoEnabled() {return _undo_enabled;};

  g_bool LoadMap(const g_string &mapname);
  g_bool SaveMap(const g_string &mapname);

  g_bool LoadMapInfo(const g_string &mapname, G_MAP_INFO &mapinfo);
  g_bool SaveMapInfo(const g_string &mapname, G_MAP_INFO &mapinfo);

  g_string MakeObjectFileName(const g_string &objname);
  g_string MakeUndoFileName(const g_string &objname, g_int32 undo_ind);

  void Init(G_CT_SectorsMgr *pSectMgr);
  void Release();

  //Конструктор и деструктор
  G_CT_SaveMgr(void);
  ~G_CT_SaveMgr(void);
private:
  G_CT_SectorsMgr  *_psectmgr;

  G_PK_ZIP         _pk_zip;

  g_bool           _undo_enabled;
  vector <g_uint8> _undo_list;

  g_bool LoadMapRegion(GMapRegion &MapRegion, const g_string &mapname);
  g_bool SaveMapRegion(GMapRegion &MapRegion, const g_string &mapname);
};

#endif //GCTSAVEMGR_H