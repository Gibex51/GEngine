#pragma once

#include "G_GM_Constants.h"
#include "G_CT_CubeSector.h"

//-----------------------------------------------

//struct GCubeSector;

class G_GM_NPCMgr;

//-----------------------------------------------

class GBaseMapObject
{
public:
  g_uint32 type;
  g_uint32 move_mod; //В процентах
  g_ivec2  pos;

  GBaseMapObject() {};
  virtual ~GBaseMapObject() {};
private:

};

struct GMapRegion
{
  g_ivec2 RegionPos;
  vector <GCubeSector*> SectorsList;
};

//-----------------------------------------------

struct G_WAYPOINT
{
  g_ivec3 move_pos;
  g_float length;

  G_WAYPOINT() 
  {
	move_pos = g_vec3(0,0,0); length = 0;
  };

  G_WAYPOINT(g_ivec3 _move_pos, g_float _length) 
  {
	move_pos = _move_pos; length = _length;
  };
};

class G_WP_PATH
{
public:
  g_float total_length;  
  vector <G_WAYPOINT> way_points;

  G_WP_PATH()
  {
	total_length = 0;
  }

  G_WP_PATH(G_WP_PATH *copy_path)
  {
	assign_path(copy_path);
  }

  G_WP_PATH(const G_WP_PATH &copy_path)
  {
	assign_path(copy_path);
  }

  void add_wp(G_WAYPOINT &wp)
  {
    way_points.push_back(wp);
	total_length += wp.length;
  };

  void add_front_wp(G_WAYPOINT &wp)
  {
    way_points.insert(way_points.begin(), wp);
	total_length += wp.length;
  };

  void assign_path(G_WP_PATH *copy_path)
  {
    total_length = copy_path->total_length;
	way_points = copy_path->way_points;
  }

  void assign_path(const G_WP_PATH &copy_path)
  {
    total_length = copy_path.total_length;
	way_points = copy_path.way_points;
  }

  //Сравнение длинны для сортировки
  g_bool operator()(G_WP_PATH* path1, G_WP_PATH* path2)
  {
	return path1->total_length > path2->total_length;
  }; 
};

//-----------------------------------------------

/*struct GPathCell
{
  g_bool    checked;
  g_uint16  weight;  
  g_int16   prev_x;
  g_int16   prev_y;
};*/

struct GMapRect
{
  g_uint32 offset_x; 
  g_uint32 offset_y; 
  g_uint32 size_x; 
  g_uint32 size_y;
};

struct GMapCell
{
  std::vector <GBaseMapObject*> objects;
  g_uint8   height;
  g_uint8   areatype;

  GMapCell() {height = 0; areatype = 0;};
};

struct GAreaSet
{
  g_uint16 object_id;
  g_uint8  deep_min;
  g_uint8  deep_max;
};

struct GMapSect
{
  GMapCell *heightmap[NCUBESXZ];
};


struct GSectorPreGen
{
  g_ivec2  pos;
  g_uint16 **heightmap;
  g_uint8  status;
};

struct GCubeRotateInfo
{
  g_bool Rot90;
  g_bool Rot180;
};

struct GAxisInfo
{
  g_uint8  ftype; //0:custom; 1:full; 2:part:center; 3:part:custom
  g_uint8  fsize;
  g_uint64 fhash64;
};

struct GFaceInfo
{
  g_uint8  faxis; //0:xyz; 1,2:x+-; 3,4:y+-; 5,6:z+-
  g_uint8  ftype; //0:custom; 1:full; 2:part:center; 3:part:custom
  g_uint8  fsize;
  union
  {
    g_uint16 fhash[4];
	g_uint64 _fhash;
  };
  g_uint32  findices[6];
};

//-----------------------------------------------

struct G_CUBE_VERTEX_PACK //20b
{
  g_vec3    Pos;
  g_uint32  Color;
  g_uint32  Tex;
};

struct G_CUBE_VERTEX //28b
{
  g_vec3    Pos;
  g_uint32  Color;
  //g_uint32  Tex;
  g_vec3    Tex;
};

struct G_OBJPROP
{
  g_bool  Colliding;
  g_bool  EnableModTex;
  g_bool  Transparency;
  g_bool  TwoSidedPoly;
  g_bool  RemSameFaces;
  g_uint8 PhysicType;
  g_uint8 Strength; 
  g_uint8 LightPower;
  g_uint16 FaceMainTex[42];  
  g_uint16 FaceModTex[42];  
};

enum g_en_cuttypes {en_ct_Misc, en_ct_Cube};

struct G_OBJECTTYPE
{
  g_uint8       nIndices;
  g_uint8       nVertices;
  g_uint32      *_indices;
  G_CUBE_VERTEX *_vertices[4];
  g_vec3        *_facenormals[4];
  g_uint32      *_packednormals[4];
  g_uint32      *_dirs[4];

  //Extra Info New
  GFaceInfo     *_faceinfo[4];
  GAxisInfo     _dockaxis[4][6];
  g_bool        _fullvisible;
};

struct G_OBJECT
{
  g_uint8     ObjectType;
  g_char      Name[64];  
  G_OBJPROP   Properties;
};

struct G_OBJECTGROUP
{
  g_char      Name[64];
  g_uint16    nObjects;
  g_uint16    *ObjIDList;
};

struct G_OPTFACE_INFO
{
  g_uint16 f_type;
  g_uint8  f_dir;
  g_bool   used;
};

//-----------------------------------------------

struct S_USED
{
  g_bool used;
  g_int32 offset;
};

//-----------------------------------------------
