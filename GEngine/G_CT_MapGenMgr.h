#pragma once

#ifndef GCTMAPGENMGR_H
#define GCTMAPGENMGR_H

#include "G_CT_SectorsMgr.h"
#include "FG_GM_Funcs.h"

using namespace std;

//-----------------------------------------------

struct G_HM_Header      // 8b
{
  g_uint16 xsize,ysize; // 4b
  g_uint16 reserved1;   // 2b
  g_bool   areamap;     // 1b
  g_uint8  reserved2;   // 1b
};

//-----------------------------------------------

class G_CT_MapGenMgr
{
public:
  //Общее
  void Init(G_CT_SectorsMgr *pSectorsMgr);
  void Release();
  
  void SaveHeightMap(string filename);
  void LoadHeightMap(string filename, g_ivec2 &mapsize);

  g_ivec2 GetMapSize() {return _map_size;};

  //Неограниченая генерация (GT1)
  void InitMap();     
  void InitMapClear();
  
  //Неограниченая генерация (GT2)
  void InitMap2(g_ivec2 &map_size, g_bool UseWater);
  void InitMap2Clear(g_ivec2 &map_size);
  void InitMapAutoSize(g_bool UseWater);

  g_float GetMapHeightInPos(g_vec3 &pos);
  //void UpdateMapHeightInPos(g_ivec2 &abs_cpos);
  //void UpdateMapHeightInArea(g_ivec2 min_abs_cpos, g_vec2 max_abs_cpos);

  //Конструктор и деструктор
  G_CT_MapGenMgr(void);
  ~G_CT_MapGenMgr(void);
private:
  g_uint8 _gentype;
  //Общее
  G_CT_SectorsMgr *pSectMgr;
 
  void GenTree(GCubeSector *Sector, g_ivec3 cpos, g_int8 height, g_uint8 type);

  //Неограниченая генерация (GT1)
  vector <GSectorPreGen*> PGSectors;

  GSectorPreGen* GT1_AddPGSector(g_ivec2 pos, g_uint16 initvalue);
  g_bool GT1_LoadPGSector(g_ivec2 pos); // <-- empty
  void GT1_SavePGSector(g_ivec2 pos); // <-- empty
  void GT1_RemovePGSector(g_ivec2 pos);
  void GT1_RemoveAllPGSectors();
  GSectorPreGen* GT1_GetPGSector(g_ivec2 pos);
  g_int32 GT1_GetPGSectorIndex(g_ivec2 pos);
  void GT1_GenerateSectorPreview(g_ivec2 pos, g_bool fullgen);
  
  void GT1_GenerateSectorRelease(g_ivec3 pos);
  void GT1_GenerateSectorClear(g_ivec3 pos);

  g_uint16 GT1_GetCubeFromPreview(g_ivec3 sectorpos, g_ivec3 cubepos);

  void GT1_PrevCoordToSect(g_ivec2 incoord, g_int16 *secind, g_ivec2 *mappos);
  void GT1_SmoothHeightMap(g_uint16 **heightmap);

  g_float GT1_GetMapHeightInPos(g_vec3 &pos); 

  //Ограниченая генерация (GT2)
  GMapCell **_heightmap;
  GMapSect **_sect_heightmap;
  GAreaSet _areas[256];

  g_ivec2  _full_map_size;
  g_ivec2  _map_size;

  void   GT2_CreateHeightMap(g_ivec2 &size);
  void   GT2_GenerateHeightMap(g_uint8 flag);
  g_bool GT2_LoadHeightMap(string filename);
  g_bool GT2_SaveHeightMap(string filename, g_bool save_areamap);
  void   GT2_ReleaseHeightMap();
  
  g_float GT2_GetMapHeightInPos(g_vec3 &pos);

  void     GT2_SetHeightMapToSectors(g_bool UseWater);
  void     GT2_CreateClearSectors();
  g_uint16 GT2_GetCubeFromMapCell(GMapCell *mapcell, g_int32 abs_y_cpos, g_bool UseWater);
  //  
  // void GT2_UpdateHeightMapFromSectors(GMapRect &MapRect);
  // void GT2_GenCubeObjectsOnMap(GMapRect &MapRect, g_uint8 obj_type, string obj_name);
  //
};

#endif //GCTMAPGENMGR_H
