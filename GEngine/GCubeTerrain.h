#pragma once

#include "GFaceArrayBuilder.h"
#include "G_CT_SaveMgr.h"
#include "G_CT_MapGenMgr.h"
#include "GResourceManager.h"
#include "math.h"

#ifndef GCUBETERRAIN_H
#define GCUBETERRAIN_H

//-----------------------------------------------

//const g_ivec3 collideradius = g_ivec3(1,2,1);

//-----------------------------------------------

class GCubeTerrain 
{
public:
  G_CT_SaveMgr     SaveMgr;
  G_CT_SectorsMgr  SectorsMgr;
  G_CT_Objects     Objects;

  // Save/Load Functions
  g_bool SaveCroppedObject(g_string objname);
  g_bool LoadCubeObject(g_string objname, g_ivec3 position, g_uint8 rot = 0);

  g_bool SaveMap(g_string mapname);
  g_bool LoadMap(g_string mapname, G_MAP_INFO &mapinfo);

  // Update Sector Functions
  void UpdateSectorsMesh();
  void UpdateSectorsMesh(g_uint16 index, g_uint16 num);
  void UpdateSectorMesh(g_uint16 index);
  //void UpdateSectorsColumn(g_ivec2 sc_pos);

  // ChangeCube Functions
  void RemoveCube(g_ivec3 &spos, g_ivec3 &cpos);
  void SetCube(g_ivec3 &spos, g_ivec3 &cpos, g_uint16 cType, g_uint8 cRot, g_bool UpdSector = true);
  
  void FillAreaInvisible(g_ivec3 min_abs_cpos, g_ivec3 max_abs_cpos);
  void FillArea(g_ivec3 min_abs_cpos, g_ivec3 max_abs_cpos, g_uint16 cType, g_uint8 cRot = 0,
                  g_bool overwrite = true, g_bool UpdSector = true, g_uint8 AreaType = G_CT_AREATYPE_CUBE);
  void FillLine(g_ivec3 min_abs_cpos, g_ivec3 max_abs_cpos, g_uint16 cType, g_uint8 cRot = 0,
                  g_bool overwrite = true, g_bool UpdSector = true);

  void MakeRoadV1(g_ivec3 st_pos, g_ivec3 end_pos, g_uint16 main_ctype, g_uint16 bord_ctype, g_bool UpdSector = true);

  g_uint8 GetCubeRotFromDir(g_vec3 &dir_vec);
  
  g_bool GetTargetCube(g_vec3 &pos, g_vec3 &dir, g_ivec3 &out_abs_cpos, g_ivec3 &prev_abs_cpos, 
	                   g_bool Real = false, g_uint8 PlaneBound = 0, g_int32 PlaneValue = 0);

  g_int32 IsValidCube(g_ivec3 &abs_cpos, g_bool Real = false);

  // Init/Release Functions
  void InitMap_Test(g_uint8 type, g_ivec2 size);  
  void Release();

  // HeightMap
  g_float GetMapHeightInPos(g_vec3 &pos);
  g_ivec2 GetMapSize() {return MapGenMgr.GetMapSize();};
  
  // Draw
  void Draw(g_mat4x4 &mWorld, GCamera *Camera, g_bool CheckDistance = true);
  void DrawSM(g_mat4x4 &mWorld, GCamera *Camera);
  void SetDrawDist(g_uint32 distance) {_draw_dist = distance;};

  GMaterial *GetMaterial() {return _ct_material;};

  // Physics
  //g_bool CheckCollisions(G_AABB PlayerAABB, g_vec3 PrevPlayerPosition, g_vec3 *NewPosition);
  //void SetCheckCollisions(g_bool check) {checkcollisions = check;};
  //void SetPhysicsPlugin(G_BulletPhysics *Physics) {physics_plugin = Physics;}; 

  //Конструктор и деструктор
  GCubeTerrain(GD3DDevice *Device, GMaterial *UsedMaterial);
  ~GCubeTerrain(void);
private: 
  //g_bool            checkcollisions;
  g_char            WorldName[64];
  g_uint32          _draw_dist;

  G_CT_MapGenMgr    MapGenMgr;

  //Material & Effects
  GD3DDevice            *_device;
  GMaterial             *_ct_material;
  GInputLayout          *_ct_il;
  GEffectTech           *_usedtech;
  GEffectTech           *_usedtech_sm;  

  // Update Sectors Mesh
  G_CUBE_VERTEX     *s_vert;
  g_uint32          *s_ind;
  G_OPTFACE_INFO    *s_face_inf;

  // FaceArrayBuilder
  GFaceArrayBuilder *fa_builder;
  GOffset           **SectorToOffset;

  //Invisible Area
  g_ivec3 ia_min_acpos, ia_max_acpos;

  // Physics
  //G_BulletPhysics   *physics_plugin;
  //g_vec3            *phys_vertices;
  //g_uint32          *phys_indices;

  //Render Control
  g_bool pause_render, is_paused_render;
  g_double pause_waittime;
};

#endif //GCUBETERRAIN_H