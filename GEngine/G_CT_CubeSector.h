#pragma once

#include "G_GM_Constants.h"

struct GCubeSector;

struct GAdjSectors
{
  GCubeSector* nxs;
  GCubeSector* pxs;
  GCubeSector* nys;
  GCubeSector* pys;
  GCubeSector* nzs;
  GCubeSector* pzs;

  GAdjSectors() {nxs = NULL;pxs = NULL;nys = NULL;pys = NULL;nzs = NULL;pzs = NULL;}
};

struct GCubeInfo
{
  g_uint8 info_adj      : 6;
  g_uint8 info_rot      : 2;
  g_bool  info_vis      : 1;
  g_uint8 info_reserved : 7;
};

struct GCubeSector
{
  g_bool       Empty;
  g_bool       Changed;
  g_uint32     icount;
  g_ivec3      Position;
  G_AABB       BoundBox;
  GAdjSectors  AdjSect;

  void InitCubes();
  void ReleaseCubes();

  void FillCubes(g_uint16 value);
  void FillInfo(g_uint16 value);

  g_uint16& Cube(g_int32 x, g_int32 y, g_int32 z, g_bool real = false);
  g_uint16& Cube(g_ivec3 &cpos, g_bool real = false);
  g_uint16& Cube(g_uint32 ind, g_bool real = false);

  GCubeInfo& Info(g_int32 x, g_int32 y, g_int32 z, g_bool real = false);
  GCubeInfo& Info(g_ivec3 &cpos, g_bool real = false);
  GCubeInfo& Info(g_uint32 ind, g_bool real = false);

  g_uint8 GetAdjInfo(g_int32 x, g_int32 y, g_int32 z);
  g_uint8 GetAdjInfo(g_uint32 ind);

  g_uint8 GetRotInfo(g_int32 x, g_int32 y, g_int32 z);
  g_uint8 GetRotInfo(g_uint32 ind);

  void SetAdjInfo(g_int32 x, g_int32 y, g_int32 z, g_uint8 value);
  void SetAdjInfo(g_ivec3 &cpos, g_uint8 value);
  void SetAdjInfo(g_uint32 ind, g_uint8 value);

  void SetRotInfo(g_int32 x, g_int32 y, g_int32 z, g_uint8 value);
  void SetRotInfo(g_ivec3 &cpos, g_uint8 value);
  void SetRotInfo(g_uint32 ind, g_uint8 value);

  void SetVisInfo(g_uint32 ind, g_bool value);
  void SetVisInfo(g_ivec3 &cpos, g_bool value);
  void SetVisInfo(g_int32 x, g_int32 y, g_int32 z, g_bool value);

  g_ivec3 IndToPos(g_uint32 ind);

  GCubeSector(g_ivec3 pos) 
  {
    icount = 0;
    _inited = false;
    Changed = true;
    Position = pos;
    Empty = true;
    _cubes_array = NULL; 
    _info_array = NULL;
  };

  ~GCubeSector() {ReleaseCubes();};
private:
  g_bool    _inited;
  g_uint16  *_cubes_array;
  GCubeInfo *_info_array;
  g_uint16  empty_cell;
};

//-----------------------------------------------

inline void GCubeSector::InitCubes()
{
  if (!_inited)
  {
    _cubes_array = new g_uint16[cubescount];
    _info_array = new GCubeInfo[cubescount];
    fill_word(_cubes_array, cubescount, 0);
    fill_word(_info_array, cubescount, 0);
    _inited = true;
  }
}

//-----------------------------------------------

inline void GCubeSector::ReleaseCubes()
{        
  _inited = false;
  if (_cubes_array != NULL) delete[]_cubes_array; 
  _cubes_array = NULL;
  if (_info_array != NULL) delete[]_info_array; 
  _info_array = NULL;   
}

//-----------------------------------------------

inline void GCubeSector::FillCubes(g_uint16 value)
{
  if (_inited)
    fill_word(_cubes_array, cubescount, value);
}

//-----------------------------------------------

inline void GCubeSector::FillInfo(g_uint16 value)
{
  if (_inited)
    fill_word(_info_array, cubescount, value);
}

//-----------------------------------------------

inline g_uint16& GCubeSector::Cube(g_uint32 ind, g_bool real)
{
  if ((!_inited) || ((real == false) && (_info_array[ind].info_vis == true)))
  {
    empty_cell = 0;
    return empty_cell;
  }

  return _cubes_array[ind];
}

//-----------------------------------------------

inline GCubeInfo& GCubeSector::Info(g_uint32 ind, g_bool real)
{
  if ((!_inited) || ((real == false) && (_info_array[ind].info_vis == true)))
  {
    empty_cell = 0;
    return *(GCubeInfo*)&empty_cell;
  }
  return _info_array[ind];
}

//-----------------------------------------------

inline g_uint8 GCubeSector::GetAdjInfo(g_uint32 ind)
{
  if ((!_inited) || (_info_array[ind].info_vis == true))
    return 0;
  return _info_array[ind].info_adj;
}

//-----------------------------------------------

inline g_uint8 GCubeSector::GetRotInfo(g_uint32 ind)
{
  if ((!_inited) || (_info_array[ind].info_vis == true))
    return 0;
  return _info_array[ind].info_rot;
}

//-----------------------------------------------

inline void GCubeSector::SetAdjInfo(g_uint32 ind, g_uint8 value)
{
  if ((!_inited) || (_info_array[ind].info_vis == true))
    return;
  _info_array[ind].info_adj = value;
}

//-----------------------------------------------

inline void GCubeSector::SetRotInfo(g_uint32 ind, g_uint8 value)
{
  if ((!_inited) || (_info_array[ind].info_vis == true))
    return;
  _info_array[ind].info_rot = value;
}

//-----------------------------------------------

inline void GCubeSector::SetVisInfo(g_uint32 ind, g_bool value)
{
  if (!_inited)
    return;
  _info_array[ind].info_vis = value;
}

//-----------------------------------------------

inline g_ivec3 GCubeSector::IndToPos(g_uint32 ind)
{
  g_ivec3 pos;

  __asm
  {
    mov eax, ind;
    xor edx,edx;
    div cubes_in_sec.z;
    mov pos.z, edx
    xor edx,edx;
    div cubes_in_sec.y;
    mov pos.y, edx
    mov pos.x, eax
  };

  return pos;
}

/*
inline g_ivec3 GCubeSector::IndToPos(g_uint32 ind)
{
  g_ivec3 pos;

  pos.z = ind % NCUBESXZ;
  pos.y = ((ind - pos.z)/NCUBESXZ) % NCUBESY;
  pos.x = ((ind - pos.z - pos.y*NCUBESXZ)/(NCUBESXZ*NCUBESY)) % NCUBESXZ;
  return pos;
}
*/

//-----------------------------------------------
//-----------------------------------------------

inline g_uint16& GCubeSector::Cube(g_int32 x, g_int32 y, g_int32 z, g_bool real)
{
  return Cube(x*NCUBESXZ*NCUBESY + y*NCUBESXZ + z, real);
}

//-----------------------------------------------

inline g_uint16& GCubeSector::Cube(g_ivec3 &cpos, g_bool real)
{
  return Cube(cpos.x*NCUBESXZ*NCUBESY + cpos.y*NCUBESXZ + cpos.z, real);
}

//-----------------------------------------------

inline GCubeInfo& GCubeSector::Info(g_int32 x, g_int32 y, g_int32 z, g_bool real)
{
  return Info(x*NCUBESXZ*NCUBESY + y*NCUBESXZ + z, real);
}

//-----------------------------------------------

inline GCubeInfo& GCubeSector::Info(g_ivec3 &cpos, g_bool real)
{
  return Info(cpos.x*NCUBESXZ*NCUBESY + cpos.y*NCUBESXZ + cpos.z, real);
}

//-----------------------------------------------

inline g_uint8 GCubeSector::GetAdjInfo(g_int32 x, g_int32 y, g_int32 z)
{
  return GetRotInfo(x*NCUBESXZ*NCUBESY + y*NCUBESXZ + z);
}

//-----------------------------------------------

inline g_uint8 GCubeSector::GetRotInfo(g_int32 x, g_int32 y, g_int32 z)
{
  return GetRotInfo(x*NCUBESXZ*NCUBESY + y*NCUBESXZ + z);
}

//-----------------------------------------------

inline void GCubeSector::SetAdjInfo(g_int32 x, g_int32 y, g_int32 z, g_uint8 value)
{
  SetAdjInfo(x*NCUBESXZ*NCUBESY + y*NCUBESXZ + z, value);
}

//-----------------------------------------------

inline void GCubeSector::SetAdjInfo(g_ivec3 &cpos, g_uint8 value)
{
  SetAdjInfo(cpos.x*NCUBESXZ*NCUBESY + cpos.y*NCUBESXZ + cpos.z, value);
}

//-----------------------------------------------

inline void GCubeSector::SetRotInfo(g_int32 x, g_int32 y, g_int32 z, g_uint8 value)
{
  SetRotInfo(x*NCUBESXZ*NCUBESY + y*NCUBESXZ + z, value);
}

//-----------------------------------------------

inline void GCubeSector::SetRotInfo(g_ivec3 &cpos, g_uint8 value)
{
  SetRotInfo(cpos.x*NCUBESXZ*NCUBESY + cpos.y*NCUBESXZ + cpos.z, value);
}

//-----------------------------------------------

inline void GCubeSector::SetVisInfo(g_int32 x, g_int32 y, g_int32 z, g_bool value)
{
  SetVisInfo(x*NCUBESXZ*NCUBESY + y*NCUBESXZ + z, value);
}

//-----------------------------------------------

inline void GCubeSector::SetVisInfo(g_ivec3 &cpos, g_bool value)
{
  SetVisInfo(cpos.x*NCUBESXZ*NCUBESY + cpos.y*NCUBESXZ + cpos.z, value);
}

//-----------------------------------------------
//----------------------------------------------
