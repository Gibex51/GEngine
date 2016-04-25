#pragma once

#ifndef GPATHFINDER_H
#define GPATHFINDER_H

#include "GCubeTerrain.h"

using namespace std;

//-----------------------------------------------

enum g_path_state {psNotInit, psNotFound, psFound, psOutOfRange, psIvalidInput, psTooLong};
enum g_path_alg   {paAstarMod3D};
enum g_cell_state {csNotChecked, csChecked, csAllowMove, csDenyMove};
enum g_wp_res     {wpNormal, wpEnd};

const g_int32 g_max_radius = 150;

const g_ivec3 move_dir[8] = {g_ivec3(0,0,-1),g_ivec3(-1,0,-1),g_ivec3(-1,0,0), g_ivec3(-1,0,1),
                             g_ivec3(0,0,1), g_ivec3(1,0,1),g_ivec3(1,0,0),g_ivec3(1,0,-1)};

const g_float move_weight[4] = {1.0f, 1.414f, 1.3f, 1.0f}; //0 - Прямо , 1 - Диагональ , 2 - Подъем, 3 - Спуск

struct G_WP
{
  g_float total_length;
  g_float dist;
  G_WP    *prev_wp;
  g_ivec3 pos;

  G_WP(){prev_wp = NULL;};
  G_WP(const g_ivec3 &_pos, const g_float _total_length, const g_float _dist, G_WP *_prev_wp)
  {
	pos = _pos;
	prev_wp = _prev_wp;
	total_length = _total_length;
	dist = _dist;
  };

  //Сравнение длинны для сортировки
  g_bool operator()(G_WP* wp1, G_WP* wp2)
  {
	if (wp1->total_length > wp2->total_length)
	  return true;
    if (wp1->total_length < wp2->total_length)
	  return false;
	return wp1->dist > wp2->dist;
  }; 
};

struct G_WP_TREE
{
  vector <G_WP*> way_points;

  void clear()
  {
	for (g_uint32 i = 0; i < way_points.size(); i++)
	  delete way_points[i];
  };

  G_WP* add_wp(const g_ivec3 &pos, const g_float length, const g_float dist, G_WP *prev_wp)
  {
	G_WP *new_wp = NULL;
	if (prev_wp != NULL)
	  new_wp = new G_WP(pos, prev_wp->total_length + length, dist, prev_wp);
	else
	  new_wp = new G_WP(pos, length, dist, NULL);
    way_points.push_back(new_wp);
	return new_wp;
  };
};

struct G_CELL_INFO
{
  g_cell_state state;
  g_uint8      move_resist; //0-полная проходимость; 255-непроходимо
};

struct G_PATH_RESULT
{
  g_path_state state;
  G_WP_PATH  path;
};

struct G_PATH_INIT
{
  g_path_alg search_alg;
  g_ivec3    search_radius;   // radius for each axis {~ xz:80-100, y:30-50}
  g_uint32   jump_power;      // in cells
};

//-----------------------------------------------

class GPathFinder
{
public:
  const G_PATH_RESULT& FindPath(const g_ivec3 &st_pos, const g_ivec3 &end_pos);
  const G_PATH_RESULT& GetLastPath() const; 
  
  void MakeMeshOfPath(const G_PATH_RESULT &path, G_LVERTEX **out_vertices, g_int32 &n_vert, 
					  g_uint32 **out_indices, g_int32 &n_ind) const;

  void Init(const G_PATH_INIT &init_prm);
  void Release();
  //Конструктор и деструктор
  GPathFinder(GCubeTerrain *pCT);
  ~GPathFinder(void);
private: 
  GCubeTerrain  *_ct;

  g_bool        _is_init;
  G_PATH_RESULT _last_path;
  G_PATH_INIT   _init_params;

  G_CELL_INFO   *_cached_cells;
  g_ivec3       _cache_area_size;
  g_int32       _cache_buff_size;

  void _alg_astarmod_3d(const g_ivec3 &st_pos, const g_ivec3 &end_pos);

  void _create(GCubeTerrain *pCT);
  void _clear_path();

  inline void _get_info_at_pos(const g_ivec3 &abs_cpos, G_CELL_INFO &out_info) const;
  inline g_bool _is_valid_pos(const g_ivec3 &abs_pos) const;
  inline g_int32 _pos_to_index(const g_ivec3 &rel_pos, const g_ivec3 &area_size) const;
  inline g_bool _pos_in_range(const g_ivec3 &min, const g_ivec3 &max, const g_ivec3 &pos) const;
};

//-----------------------------------------------
// 2D Path Finder
//-----------------------------------------------

/*
struct G_PATH_RESULT
{
  vector <g_ivec2> OutPath;
  g_uint16 Steps;
};

g_ivec2 PntSet[8] = {g_ivec2(0,-1),g_ivec2(-1,0),g_ivec2(0,1),g_ivec2(1,0),
                     g_ivec2(1,1),g_ivec2(1,-1),g_ivec2(-1,1),g_ivec2(-1,-1)};

class GPathFinder
{
public:
  void Init(GMapCell **heightmap, g_ivec2 map_size);
  void Release();

  g_bool FindPath(g_ivec2 StartPos, g_ivec2 EndPos, G_PATH_RESULT *PathResult);
  Конструктор и деструктор
  GPathFinder(void);
  ~GPathFinder(void);
private:
  g_bool     _inited;
  g_ivec2    _map_size;
  GPathCell  **_pathmap;
  GMapCell   **_heightmap;

  vector <g_ivec2> _pnt_buffer;

  g_bool OutOfRange(g_ivec2 &value);
};
*/

#endif //GPATHFINDER_H