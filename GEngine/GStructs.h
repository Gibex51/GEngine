#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include <windows.h>
#include <vector>
#include <map>
#include <queue>
#include <list>
#include <string>
#include <dinput.h>
//#include <dinputd.h>
#include <vld.h>
#include <usp10.h>
#include <assert.h>
#include <xaudio2.h>

#include "funcs.h"
#include "GConstants.h"
#include "G_PK_ZIP.h"

#ifndef GSTRUCTS_H
#define GSTRUCTS_H

#pragma comment (lib, "d3d10.lib")
#pragma comment (lib, "d3dx10.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "usp10.lib")

//Class Defines
class GResourceManager;

// Int Vectors
struct g_ivec4
{
    g_int32 x,y,z,w;

    g_ivec4()
    {
	  x=0; y=0; z=0; w=0;
    }

    g_ivec4(g_int32 _x, g_int32 _y, g_int32 _z, g_int32 _w)
    {
	  x=_x; y=_y; z=_z; w=_w;
    }

    g_ivec4 operator * (g_int32 value) const
    {
	  return g_ivec4(x*value,y*value,z*value,w*value);
    }
};

struct g_ivec3
{
    g_int32 x,y,z;

    g_ivec3()
    {
	  x=0; y=0; z=0;
    }

    g_ivec3(g_int32 _x, g_int32 _y, g_int32 _z)
    {
	  x=_x; y=_y; z=_z;
    }

    g_ivec3(g_vec3 &vec3)
    {
	  x=g_int32(vec3.x); y=g_int32(vec3.y); z=g_int32(vec3.z);
    }

    g_vec3 AsVec3()
    {
	  return g_vec3(g_float(x),g_float(y),g_float(z));
    }

	g_double Length()
	{
	  return sqrt(x*x+y*y+z*z);
	}

    void FloorVec3(const g_vec3 &in_vec3)
    {
      x = g_int32(floor(in_vec3.x));
      y = g_int32(floor(in_vec3.y));
      z = g_int32(floor(in_vec3.z));
    }

    void CeilVec3(const g_vec3 &in_vec3)
    {
      x = g_int32(ceil(in_vec3.x));
      y = g_int32(ceil(in_vec3.y));
      z = g_int32(ceil(in_vec3.z));
    }

	void RoundVec3(const g_vec3 &in_vec3)
    {
      x = g_round(in_vec3.x);
      y = g_round(in_vec3.y);
      z = g_round(in_vec3.z);
    }

	void SetIfMin(const g_ivec3 &cmp_vec)
	{
	  if (cmp_vec.x < x) x = cmp_vec.x;
	  if (cmp_vec.y < y) y = cmp_vec.y;
	  if (cmp_vec.z < z) z = cmp_vec.z;
	}

	void SetIfMin(const g_int32 &cmp_val)
	{
	  if (cmp_val < x) x = cmp_val;
	  if (cmp_val < y) y = cmp_val;
	  if (cmp_val < z) z = cmp_val;
	}

	void SetIfMax(const g_ivec3 &cmp_vec)
	{
	  if (cmp_vec.x > x) x = cmp_vec.x;
	  if (cmp_vec.y > y) y = cmp_vec.y;
	  if (cmp_vec.z > z) z = cmp_vec.z;
	}

	void SetIfMax(const g_int32 &cmp_val)
	{
	  if (cmp_val > x) x = cmp_val;
	  if (cmp_val > y) y = cmp_val;
	  if (cmp_val > z) z = cmp_val;
	}

    g_ivec3& Abs()
    {
      x = abs(x);
      y = abs(y);
      z = abs(z);
	  return *this;
    }

    g_ivec3 operator % (const g_int32 &value) const
    {
	  return g_ivec3(x%value,y%value,z%value);
    }

    g_ivec3 operator % (const g_ivec3 &vec) const
    {
	  return g_ivec3(x%vec.x,y%vec.y,z%vec.z);
    }

    g_ivec3 operator * (const g_int32 &value) const
    {
	  return g_ivec3(x*value,y*value,z*value);
    }

	g_ivec3 operator * (const g_ivec3 &vec) const
    {
	  return g_ivec3(x*vec.x,y*vec.y,z*vec.z);
    }

    g_ivec3 operator / (const g_int32 &value) const
    {
	  return g_ivec3(x/value,y/value,z/value);
    }

    g_ivec3 operator / (const g_ivec3 &vec) const
    {
	  return g_ivec3(x/vec.x,y/vec.y,z/vec.z);
    }

    g_ivec3 operator + (const g_int32 &value) const
    {
	  return g_ivec3(x+value,y+value,z+value);
    }

    g_ivec3 operator + (const g_ivec3 &vec) const
    {
	  return g_ivec3(x+vec.x,y+vec.y,z+vec.z);
    }

    g_ivec3 operator - (const g_int32 &value) const
    {
	  return g_ivec3(x-value,y-value,z-value);
    }

    g_ivec3 operator - (const g_ivec3 &vec) const
    {
  	  return g_ivec3(x-vec.x,y-vec.y,z-vec.z);
    }

    g_bool operator == (const g_ivec3 &vec) const
    {
	  return ((vec.x == x) && (vec.y == y) && (vec.z == z));
    }

    g_bool operator != (const g_ivec3 &vec) const
    {
	  return ((vec.x != x) || (vec.y != y) || (vec.z != z));
    }
};

struct g_ivec2
{
    g_int32 x,y;

    g_ivec2()
    {
	  x=0; y=0;
    }

    g_ivec2(g_int32 _x, g_int32 _y)
    {
	  x=_x; y=_y;
    }

    g_ivec2(const g_ivec2& vec)
    {
	  x = vec.x;
	  y = vec.y;
    }

    g_ivec2 operator * (g_int32 value) const
    {
	  return g_ivec2(x*value,y*value);
    }

    g_ivec2 operator + (g_int32 value) const
    {
	  return g_ivec2(x+value,y+value);
    }

    g_ivec2 operator + (g_ivec2 vec) const
    {
	  return g_ivec2(x+vec.x,y+vec.y);
    }

    g_ivec2 operator - (g_int32 value) const
    {
	  return g_ivec2(x-value,y-value);
    }

    g_ivec2 operator - (g_ivec2 vec) const
    {
	  return g_ivec2(x-vec.x,y-vec.y);
    }

    g_bool operator == (const g_ivec2& vec) const
    {
 	  return ((vec.x == x) && (vec.y == y));
    }

	g_bool operator != (const g_ivec2& vec) const
    {
 	  return ((vec.x != x) || (vec.y != y));
    }
};

//Byte Vectors

struct g_bvec3
{
    g_int8 x,y,z;
   
    g_bvec3() {x=0;y=0;z=0;};
    g_bvec3(g_int8 _x,g_int8 _y,g_int8 _z) {x=_x;y=_y;z=_z;};
};


struct g_bvec2
{
    g_int8 x,y;
   
    g_bvec2() {x=0;y=0;};
    g_bvec2(g_int8 _x,g_int8 _y) {x=_x;y=_y;};
};

//Точка
struct G_POINT:POINT  
{
    G_POINT() {x = 0; y = 0;};
    G_POINT(int _x, int _y)
    {
	  x = _x; 
	  y = _y; 
    };
};

//Anchors

struct g_anchors_set
{
  g_anchors_state x;
  g_anchors_state y;

  g_anchors_set()
  {
	x = g_anst_center; y = g_anst_center;
  }

  g_anchors_set(g_anchors_state ax, g_anchors_state ay)
  {
	x = ax; y = ay;
  }
};

//Bounding Box
/*struct G_BOX
{
    g_float max_x,min_x,max_y,min_y,max_z,min_z;

    G_BOX()
    {
	  min_x = 0; min_y = 0; min_z = 0;
	  max_x = 0; max_y = 0; max_z = 0;
    }

    G_BOX(g_vec3 min, g_vec3 max)
    {
	  Set(min,max);
    }

    void Set(g_vec3 min, g_vec3 max)
    {
	  min_x = min.x;
	  min_y = min.y;
	  min_z = min.z;
	  max_x = max.x;
	  max_y = max.y;
	  max_z = max.z;
    }

    g_vec3 GetVector(g_uint8 point_num)
    {
	  switch (point_num)
	  {
	    case 0: return g_vec3(min_x,min_y,min_z); break;  //Min
	    case 1: return g_vec3(max_x,min_y,min_z); break;
	    case 2: return g_vec3(min_x,max_y,min_z); break;
	    case 3: return g_vec3(max_x,max_y,min_z); break;
	    case 4: return g_vec3(min_x,min_y,max_z); break;
	    case 5: return g_vec3(max_x,min_y,max_z); break;
	    case 6: return g_vec3(min_x,max_y,max_z); break;
	    case 7: return g_vec3(max_x,max_y,max_z); break;  //Max
	    default: return g_vec3(0,0,0); 
	  }
    }
};*/

//AABB
struct G_AABB_P
{
  g_vec3 points[8];
};

struct G_AABB
{
  g_vec3 center;
  g_vec3 radius;

  void SetCR(g_vec3 &_center, g_vec3 &_radius)
  {
    center = _center;
	radius = _radius;
  }

  void SetMinMax(g_vec3 &min, g_vec3 &max)
  {
    radius = (max - min)/2.0f;
	center = min + radius;
  }

  g_bool TestAABB(G_AABB &_aabb)
  {
    if (abs(center.x - _aabb.center.x) > (radius.x + _aabb.radius.x)) return false;
    if (abs(center.y - _aabb.center.y) > (radius.y + _aabb.radius.y)) return false;
    if (abs(center.z - _aabb.center.z) > (radius.z + _aabb.radius.z)) return false;
    return true;
  }

  //-----------------------------------------------

  void SetFromPoints(g_vec3 **Points, g_uint32 nPoints)
  {
    if ((nPoints == 0) || (Points == NULL)) return;

    g_vec3 min = (*Points)[0];
    g_vec3 max = (*Points)[0];

    for (g_uint32 i = 1; i < nPoints; i++)
    {
	  if ((*Points)[i].x < min.x) min.x = (*Points)[i].x;
	  if ((*Points)[i].y < min.y) min.y = (*Points)[i].y;
	  if ((*Points)[i].z < min.z) min.z = (*Points)[i].z;
	  if ((*Points)[i].x > max.x) max.x = (*Points)[i].x;
	  if ((*Points)[i].y > max.y) max.y = (*Points)[i].y;
	  if ((*Points)[i].z > max.z) max.z = (*Points)[i].z;
    }

    SetMinMax(min, max);
  }

  //-----------------------------------------------

  void GetPoints(G_AABB_P &AABB_P)
  {
    AABB_P.points[0] = g_vec3((center.x - radius.x), 
		                      (center.y - radius.y), 
							  (center.z - radius.z));
    AABB_P.points[1] = g_vec3((center.x + radius.x), 
		                      (center.y - radius.y), 
							  (center.z - radius.z));
    AABB_P.points[2] = g_vec3((center.x - radius.x), 
		                      (center.y + radius.y), 
							  (center.z - radius.z));
    AABB_P.points[3] = g_vec3((center.x + radius.x), 
		                      (center.y + radius.y), 
							  (center.z - radius.z));
    AABB_P.points[4] = g_vec3((center.x - radius.x), 
		                      (center.y - radius.y), 
							  (center.z + radius.z));
    AABB_P.points[5] = g_vec3((center.x + radius.x), 
		                      (center.y - radius.y), 
							  (center.z + radius.z));
    AABB_P.points[6] = g_vec3((center.x - radius.x), 
		                      (center.y + radius.y), 
							  (center.z + radius.z));
    AABB_P.points[7] = g_vec3((center.x + radius.x), 
		                      (center.y + radius.y), 
							  (center.z + radius.z));
  }

  g_vec3 GetPoint(g_uint8 point_num)
  {
	switch (point_num)
	{
	    case 0: return g_vec3((center.x - radius.x),(center.y - radius.y),(center.z - radius.z)); break;  //Min
	    case 1: return g_vec3((center.x + radius.x),(center.y - radius.y),(center.z - radius.z)); break;
	    case 2: return g_vec3((center.x - radius.x),(center.y + radius.y),(center.z - radius.z)); break;
	    case 3: return g_vec3((center.x + radius.x),(center.y + radius.y),(center.z - radius.z)); break;
	    case 4: return g_vec3((center.x - radius.x),(center.y - radius.y),(center.z + radius.z)); break;
	    case 5: return g_vec3((center.x + radius.x),(center.y - radius.y),(center.z + radius.z)); break;
	    case 6: return g_vec3((center.x - radius.x),(center.y + radius.y),(center.z + radius.z)); break;
	    case 7: return g_vec3((center.x + radius.x),(center.y + radius.y),(center.z + radius.z)); break;  //Max
	    default: return g_vec3(0,0,0); 
	}
  }
};



//Параметры вертекса

struct G_OP_VERTEX
{
    g_vec3  Pos;
    g_vec2  Col;
    g_vec2  Tex;    
};

struct G_VERTEX  
{
    g_vec3 Pos;
    g_vec4 Color;
    g_vec2 Tex;   
    g_vec3 Norm;

	void Set(g_vec3 _pos, g_vec4 _color, g_vec2 _tex, g_vec3 _norm)
	{
	  Pos = _pos;
	  Color = _color;
	  Tex = _tex;
	  Norm = _norm;
	}
};

struct G_BONE_VERTEX
{
    g_vec3  Pos;
	g_vec3  Norm;
	g_vec2  Tex;
	g_vec4  Color;
	g_vec4  Weights;
	g_ivec4 Bones;

	void SetVert(g_vec3 _pos, g_vec4 _color, g_vec2 _tex, g_vec3 _norm)
	{
	  Pos = _pos;
	  Color = _color;
	  Tex = _tex;
	  Norm = _norm;
	}

	void SetBones(g_vec4 _Weights, g_ivec4 _Bones)
	{
	  Weights = _Weights;
	  Bones = _Bones;
	}
};

struct G_LVERTEX  
{
    g_vec3  Pos;
	g_vec2  Tex;

	G_LVERTEX() {Pos = g_vec3(0,0,0); Tex = g_vec2(0,0);};
	G_LVERTEX(g_vec3 _Pos, g_vec2 _Tex) {Pos = _Pos; Tex = _Tex;};
};

struct G_BONE_LVERTEX
{
    g_vec3  Pos;
	g_vec3  Norm;
	g_vec2  Tex;
	g_vec4  Weights;
	g_ivec4 Bones;

	G_BONE_LVERTEX()
	{
	  Pos = g_vec3(0,0,0); 
	  Norm = g_vec3(0,0,0); 
	  Tex = g_vec2(0,0);
	  Weights = g_vec4(0,0,0,0);
	  Bones = g_ivec4(-1,-1,-1,-1);
	}

	G_BONE_LVERTEX(g_vec3 _Pos, g_vec3 _Norm, g_vec2 _Tex, g_vec4 _Weights, g_ivec4 _Bones)
	{
	  Pos = _Pos;
	  Tex = _Tex;
	  Norm = _Norm;
	  Weights = _Weights;
	  Bones = _Bones;
	}
};

//Параметры источника света
struct G_LIGHT_SET  
{
    g_float Enabled;            //0-off, !0-on
	g_vec3  Position;
    g_vec3  Direction;
    g_float Type;               //0-Ambient, 1-Direct, 2-Omni, 3-Spot
    g_vec4  Diffuse;
    g_vec4  Ambient;
    g_vec4  Specular;
};

//UID ???
struct G_UID
{
    g_uint32 uid;
    void *object;
};

#endif /* GSTRUCTS_H */