#pragma once

#include "FG_GM_Funcs.h"

#ifndef GCTOBJECT_H
#define GCTOBJECT_H

class G_CT_Objects
{
public:
  vector <G_OBJECT>      Objects;
  vector <G_OBJECTTYPE>  ObjTypes;
  vector <G_OBJECTGROUP> ObjGroups;

  g_bool LoadCubeObjects(g_char *FileName);
  void   ClearCubeObjects();

  void Release();

  //Конструктор и деструктор
  G_CT_Objects(void);
  ~G_CT_Objects(void);
private:
  void CalcPointHASH(const g_ivec3 &fvert, g_uint8 axis, g_uint16 &result_hash);
  void SortHASH(g_uint16 *fhash);
  void AnalyseFace(G_CUBE_VERTEX *fvert, g_uint32 *find, g_vec3 *fnorm, GFaceInfo &finfo);
};

#endif //GCTOBJECT_H