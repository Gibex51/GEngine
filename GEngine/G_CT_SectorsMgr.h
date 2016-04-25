#pragma once

#ifndef GCTSECTMGR_H
#define GCTSECTMGR_H

#include "G_CT_Objects.h"

using namespace std;

//-----------------------------------------------

const g_uint8 cAdjTypes[6] = {1,2,4,8,16,32}; // new 0,1 : x+- ; 2,3 : y+- ; 4,5 : z+-
                                              // old 0,1 : z-+ ; 2,3 : y+- ; 4,5 : x+-                                             
const g_uint8 cAdjAxis[6] = {1,0,3,2,5,4};    //0,1 : x+- ; 2,3 : y+- ; 4,5 : z+-

//-----------------------------------------------

class G_CT_SectorsMgr
{
public:
  vector <GCubeSector*> Sectors;

  GCubeSector* GetSector(g_ivec3 spos);
  g_int32 GetSectorIndex(g_ivec3 spos);

  g_uint16& GetAbsCube(const g_ivec3 &abs_cpos, g_bool real = false); 
  
  void GenerateSectorsHASHTable();
  
  void Init(G_CT_Objects *pObjects);
  void Release();

  void CheckCubeAdj(GCubeSector *Sector, g_ivec3 cInd, g_bool Real);

  void CheckSectorsAdj();
  void CheckAllMapCubeAdj();

  //Конструктор и деструктор
  G_CT_SectorsMgr(void);
  ~G_CT_SectorsMgr(void);
private:
  G_CT_Objects      *_p_objects;

  g_uint32          *SectCoordHASHToIndex;
  g_ivec3           sect_coord_offset, sect_in_axis, smin, smax; 

  g_uint16          _empty_info;

  GCubeInfo& GetAdjCube(GCubeSector *Sector, g_ivec3 curr_cpos, g_uint8 axis, g_uint16 &adj_cube, g_bool SetUpdateFlag = false, g_bool Real = false);
};

#endif //GCTSECTMGR_H