#include "GPathFinder.h"

//-----------------------------------------------

GPathFinder::GPathFinder(GCubeTerrain *pCT)
{
  _create(pCT);
}

//-----------------------------------------------

GPathFinder::~GPathFinder(void)
{
  Release();
}

//-----------------------------------------------

void GPathFinder::Init(const G_PATH_INIT &init_prm)
{
  if (_is_init) Release();

  _init_params = init_prm;
  _init_params.search_radius.Abs();
  _init_params.search_radius.SetIfMin(g_max_radius);

  _cache_area_size = _init_params.search_radius*2;
  _cache_buff_size = (_cache_area_size.x+1)*(_cache_area_size.y+1)*(_cache_area_size.z+1);

  if (_cache_buff_size > 0)
  {
    _cached_cells = new G_CELL_INFO[_cache_buff_size];
    _is_init = true;
  }
}

//-----------------------------------------------

void GPathFinder::Release()
{
  if (!_is_init) return;
  _clear_path();
  if (_cached_cells != NULL) delete[]_cached_cells;
  _cached_cells = NULL;
}

//-----------------------------------------------

const G_PATH_RESULT& GPathFinder::FindPath(const g_ivec3 &st_pos, const g_ivec3 &end_pos)
{
  _clear_path();

  if (!_is_init) 
	return _last_path;
 
  if ((st_pos == end_pos) || (!_is_valid_pos(st_pos)) || (!_is_valid_pos(end_pos))) 
  {
	_last_path.state = psIvalidInput;
	return _last_path;
  }

  g_ivec3 curr_range = (end_pos - st_pos).Abs();
  if ((curr_range.y > _init_params.search_radius.y) ||
	  (curr_range.x > _init_params.search_radius.x) ||
	  (curr_range.z > _init_params.search_radius.z))
  {
	_last_path.state = psOutOfRange;
	return _last_path;
  }

  switch (_init_params.search_alg)
  {
    case paAstarMod3D: 
	  {
		_alg_astarmod_3d(st_pos, end_pos); 
		break;
	  }
	default: 
	  {
		_last_path.state = psIvalidInput;
	    break;
	  }
  }

  return _last_path;
}

//-----------------------------------------------

const G_PATH_RESULT& GPathFinder::GetLastPath() const
{
  return _last_path;
}

//-----------------------------------------------

void GPathFinder::MakeMeshOfPath(const G_PATH_RESULT &path, G_LVERTEX **out_vertices, g_int32 &n_vert, 
								 g_uint32 **out_indices, g_int32 &n_ind) const
{
  g_uint32 n_wp = path.path.way_points.size();
  *out_vertices = new G_LVERTEX[n_wp];
  *out_indices = new g_uint32[n_wp];

  for (g_uint32 i = 0; i < n_wp; i++)
  {
	(*out_vertices)[i].Pos = AbsCubePosToRealPos(path.path.way_points[i].move_pos);
	(*out_vertices)[i].Tex = g_vec2(0,0);
	(*out_indices)[i] = i;
  }

  n_ind = n_wp;
  n_vert = n_wp;
}

//-----------------------------------------------

void GPathFinder::_create(GCubeTerrain *pCT)
{
  _ct = pCT;
  _is_init = false;
  _init_params.jump_power = 0;
  _init_params.search_radius = g_ivec3(0,0,0);

  _cached_cells = NULL;

  _clear_path();
}

//-----------------------------------------------

void GPathFinder::_clear_path()
{
  _last_path.state = psNotInit;
  _last_path.path.total_length = 0;
  _last_path.path.way_points.clear();
}

//-----------------------------------------------

inline g_bool GPathFinder::_is_valid_pos(const g_ivec3 &abs_pos) const
{
  G_CELL_INFO pos_info;
  pos_info.state = csNotChecked;
  _get_info_at_pos(abs_pos + g_ivec3(0,-1,0), pos_info);
  if (pos_info.move_resist < 255) 
    return false;
  pos_info.state = csNotChecked;
  _get_info_at_pos(abs_pos + g_ivec3(0,1,0), pos_info);
  return pos_info.move_resist < 255;
}

//-----------------------------------------------

inline g_int32 GPathFinder::_pos_to_index(const g_ivec3 &rel_pos, const g_ivec3 &area_size) const
{
  return rel_pos.x + rel_pos.y*area_size.x + rel_pos.z*area_size.x*area_size.y;
}

//-----------------------------------------------

inline g_bool GPathFinder::_pos_in_range(const g_ivec3 &min, const g_ivec3 &max, const g_ivec3 &pos) const
{
  return ((pos.x <= max.x) && (pos.x >= min.x) &&
	      (pos.y <= max.y) && (pos.y >= min.y) &&
	      (pos.z <= max.z) && (pos.z >= min.z));
} 

//-----------------------------------------------

inline void GPathFinder::_get_info_at_pos(const g_ivec3 &abs_cpos, G_CELL_INFO &out_info) const
{
  if (out_info.state == csNotChecked)
  {
	out_info.state = csChecked;

	g_ivec3 cpos, spos;
	GetSectorAndCubePosByAbsCubePos(abs_cpos, spos, cpos);

	GCubeSector *Sector = _ct->SectorsMgr.GetSector(spos);
	
	if ((Sector != NULL) && (Sector->Cube(cpos, true) == 0))
	  out_info.move_resist = 0;
	else
	  out_info.move_resist = 255;
  }
}

//-----------------------------------------------

void GPathFinder::_alg_astarmod_3d(const g_ivec3 &st_pos, const g_ivec3 &end_pos)
{
  g_double ticks = GetTicks();

  priority_queue <G_WP*, vector<G_WP*>, G_WP> WPQueue;
  G_WP_TREE WPTree;
  
  WPQueue.push(WPTree.add_wp(st_pos, 0, 0, NULL));

  g_ivec3 min = st_pos - _init_params.search_radius;
  g_ivec3 max = st_pos + _init_params.search_radius;
  
  g_float end_dist = g_float((end_pos - st_pos).Length());
  g_float max_dist = end_dist*1.3f;
  if (max_dist < 25) max_dist += 10.0f;

  memset(_cached_cells, 0, _cache_buff_size*sizeof(G_CELL_INFO));

  G_WP *curr_wp;

  _last_path.state = psNotFound;

  while (WPQueue.size() > 0)
  {
	curr_wp = WPQueue.top();
	WPQueue.pop();
	//Проверяем является ли точка конечной
	g_ivec3 curr_pos = curr_wp->pos;
	if (curr_pos == end_pos)
	{
	  _last_path.state = psFound;
	  break;
	}
	//Проверяем максимальную длинну пути
	/*if (curr_wp->total_length > g_max_length)
	{
      _last_path.state = psTooLong;
	  break;
	}*/
	//Проверяем возможность прыжка
	g_int32 curr_jump = -2;
	for (g_uint32 i = 0; i < _init_params.jump_power + 2; i++)
	{
      g_ivec3 new_pos = curr_pos + g_ivec3(0,i,0);
	  if (!_pos_in_range(min,max,new_pos)) break;
	  g_int32 index = _pos_to_index(new_pos - min, _cache_area_size);
	  _get_info_at_pos(new_pos, _cached_cells[index]);
	  _cached_cells[index].state = csDenyMove;
      if (_cached_cells[index].move_resist == 255) break;
	  curr_jump++;
	}
	//Проверяем точки по осям
	for (g_int32 curr_dir = 0; curr_dir < 8; curr_dir++)
	{
	  g_int32 diag = curr_dir%2;
	  g_ivec3 targ_pos = curr_pos + move_dir[curr_dir];
	  if (!_pos_in_range(min,max,targ_pos)) continue;

	  G_CELL_INFO *up_cell_info[3], *down_cell_info[3];
	  
	  //Получаем инфу о соседних ячейках (сверху и снизу)
	  for (g_int32 i = 0; i < 3; i++)
	  {
		g_ivec3 new_pos = targ_pos + g_ivec3(0,i-1,0);
		g_int32 index = _pos_to_index(new_pos - min, _cache_area_size);
		_get_info_at_pos(new_pos, _cached_cells[index]);
		up_cell_info[i] = down_cell_info[i] = &_cached_cells[index];	
	  }
	  g_bool check_way_down = true;
	  // Проверяем перемещение вверх
	  for (g_int32 i = 0; i < curr_jump + 1; i++)
	  {
		if (up_cell_info[0]->move_resist == 255)
		{
		  g_bool no_wall = true;
		  if (i < 3) check_way_down = false;
		  if (diag == 1)
		  {
		    g_ivec3 new_pos[2] = {curr_pos + g_ivec3(0,i,0), curr_pos + g_ivec3(0,i,0)}; 
		    new_pos[0].x += move_dir[curr_dir].x;
		    new_pos[1].z += move_dir[curr_dir].z;
		  
            for (g_int32 j = 0; j < 4; j++)
		    {
		      g_int32 index = _pos_to_index(new_pos[j%2] - min, _cache_area_size);
			  _get_info_at_pos(new_pos[j%2], _cached_cells[index]);
			  if (_cached_cells[index].move_resist == 255)
			  {
			    no_wall = false; 
			    break;
			  }
			  new_pos[j%2].y++;
		    }
		  }
		  if (no_wall)
		    if ((up_cell_info[1]->state != csDenyMove) && (up_cell_info[2]->state != csDenyMove) &&
		  	    (up_cell_info[1]->move_resist < 255) && (up_cell_info[2]->move_resist < 255))
		    {
		      g_ivec3 new_pos = targ_pos + g_ivec3(0,i,0);
			  if (i < 2) up_cell_info[1]->state = csDenyMove;
			  g_float dist = g_float((end_pos - new_pos).Length());
			  if (dist < max_dist)
			  {
				G_WP *temp_wp = curr_wp;
				if (i > 0) //Если движемся вверх
				  temp_wp = WPTree.add_wp(curr_pos + g_ivec3(0,i,0), move_weight[2]*i, dist, curr_wp);
				WPQueue.push(WPTree.add_wp(new_pos, move_weight[diag], dist, temp_wp));
			  }
		    }
		}
		up_cell_info[0] = up_cell_info[1];
		up_cell_info[1] = up_cell_info[2];
		g_ivec3 new_pos = targ_pos + g_ivec3(0,2+i,0);
		g_int32 index = _pos_to_index(new_pos - min, _cache_area_size);
		_get_info_at_pos(new_pos, _cached_cells[index]);
		up_cell_info[2] = &_cached_cells[index];
	  }
	  // Проверяем перемещение вниз
	  if (check_way_down)
	    for (g_int32 i = 0; i <= g_int32(_init_params.jump_power); i++)
	    {
		  if ((down_cell_info[0]->move_resist == 255) ||
		  	  (down_cell_info[1]->move_resist == 255) ||
			  (down_cell_info[2]->move_resist == 255))
		  {
            if ((down_cell_info[1]->state != csDenyMove) && (down_cell_info[2]->state != csDenyMove) &&
			    (down_cell_info[1]->move_resist < 255) && (down_cell_info[2]->move_resist < 255))
		    {
              g_ivec3 new_pos = targ_pos + g_ivec3(0,-i,0);
			  if (i < 2) down_cell_info[1]->state = csDenyMove;
			  g_float dist = g_float((end_pos - new_pos).Length());
			  if (dist < max_dist)
			  {
				if (i > 0) //Если движемся вниз
				{
				  G_WP *temp_wp = WPTree.add_wp(targ_pos, move_weight[diag], dist, curr_wp);
			      WPQueue.push(WPTree.add_wp(new_pos, move_weight[3]*i, dist, temp_wp));
				}
				else
				  WPQueue.push(WPTree.add_wp(new_pos, move_weight[diag], dist, curr_wp));
			  }
		    }
		    break;
		  }
		  down_cell_info[2] = down_cell_info[1];
	      down_cell_info[1] = down_cell_info[0];
		  g_ivec3 new_pos = targ_pos + g_ivec3(0,-2-i,0);
		  g_int32 index = _pos_to_index(new_pos - min, _cache_area_size);
		  _get_info_at_pos(new_pos, _cached_cells[index]);
		  down_cell_info[0] = &_cached_cells[index];
	    }
	}
  }

  if (_last_path.state == psFound)
  {
    _last_path.path.total_length = 0;
	G_WAYPOINT new_wp;
	while (curr_wp->prev_wp != NULL)
	{  
	  new_wp.move_pos = curr_wp->pos;
	  new_wp.length = curr_wp->total_length - curr_wp->prev_wp->total_length;
	  _last_path.path.add_front_wp(new_wp);
	  curr_wp = curr_wp->prev_wp;
	}
	new_wp.move_pos = curr_wp->pos;
	new_wp.length = 0;
	_last_path.path.add_front_wp(new_wp);
  }
  WPTree.clear();

  g_char res_str[100];
  sprintf_s(res_str,100,"wp: %d st: %d dist: %.3f", WPTree.way_points.size(), _last_path.state, max_dist);
  CheckTimeAndWriteToFile(ticks, res_str);
}

//-----------------------------------------------
// 2D Path Finder
//-----------------------------------------------
/*
GPathFinder::GPathFinder(void)
{
  _pathmap = NULL;
  _heightmap = NULL;
  _map_size = g_ivec2(0,0);
  _inited = false;
}

//-----------------------------------------------

GPathFinder::~GPathFinder(void)
{
  Release();
}

//-----------------------------------------------

void GPathFinder::Init(GMapCell **heightmap, g_ivec2 map_size)
{
  if (heightmap == NULL) return;
  if (((map_size.x <= 0) || (map_size.x > max_fullmap_size)) ||
      ((map_size.y <= 0) || (map_size.y > max_fullmap_size))) return;

  _heightmap = heightmap;
  _map_size = map_size;
  
  _pathmap = new GPathCell*[_map_size.x];
  for (g_int32 i = 0; i<_map_size.x; i++)
    _pathmap[i] = new GPathCell[_map_size.y];

  _inited = true;
}

//-----------------------------------------------

void GPathFinder::Release()
{
  _inited = false;

  if (_pathmap != NULL)
  {
    for (g_int32 i = 0; i<_map_size.x; i++)
      delete[]_pathmap[i];

    delete[]_pathmap;
    _pathmap = NULL;
  }

  _heightmap = NULL;
  _map_size = g_ivec2(0,0);
}

//-----------------------------------------------

g_bool GPathFinder::FindPath(g_ivec2 StartPos, g_ivec2 EndPos, G_PATH_RESULT *PathResult)
{
  if (!_inited) return false;
  if (OutOfRange(StartPos) || OutOfRange(EndPos) || PathResult == NULL) return false;

  //Инициализация
  memset(_pathmap,0,sizeof(GPathCell)*_map_size.x*_map_size.y);

  _pnt_buffer.clear();

  //Заносим первую точку
  _pathmap[StartPos.x][StartPos.y].checked = true;
  _pathmap[StartPos.x][StartPos.y].weight = 0;
  _pathmap[StartPos.x][StartPos.y].prev_x = -1;
  _pnt_buffer.push_back(StartPos);
  
  //В цикле проверяем ближайшие точки
  while (_pnt_buffer.size() > 0)
  {
    g_int32 mini = 0, minw = G_MAX_INT;

    for (g_uint32 i = 0; i<_pnt_buffer.size(); i++)
      if (_pathmap[_pnt_buffer[i].x][_pnt_buffer[i].y].weight < minw)
      {
        mini = i;
        minw = _pathmap[_pnt_buffer[i].x][_pnt_buffer[i].y].weight;
      }

    g_ivec2 PrevPnt = _pnt_buffer[mini];
    for (g_int32 i = 0; i < 8; i++)
    {     
      g_ivec2 CurrPnt = PrevPnt + PntSet[i];
      if (OutOfRange(CurrPnt)) continue;

      if (_pathmap[CurrPnt.x][CurrPnt.y].checked) continue;
      
      g_uint32 mm = 0;
      for (g_uint32 obj_i = 0; obj_i<_heightmap[CurrPnt.x][CurrPnt.y].objects.size(); obj_i++)
      {
        if (mm < _heightmap[CurrPnt.x][CurrPnt.y].objects[obj_i]->move_mod)
          mm = _heightmap[CurrPnt.x][CurrPnt.y].objects[obj_i]->move_mod;
      }
      if (mm == 0) continue;

      g_int32 currw = abs(_heightmap[CurrPnt.x][CurrPnt.y].height - 
                          _heightmap[PrevPnt.x][PrevPnt.y].height) + 1;

      if (currw > 3) continue;
      if (i > 3) currw++;

      _pathmap[CurrPnt.x][CurrPnt.y].checked = true;
      _pathmap[CurrPnt.x][CurrPnt.y].weight = minw + currw;
      _pathmap[CurrPnt.x][CurrPnt.y].prev_x = PrevPnt.x;
      _pathmap[CurrPnt.x][CurrPnt.y].prev_y = PrevPnt.y;

      if ((CurrPnt.x == EndPos.x) && (CurrPnt.y == EndPos.y))
      {
        //Если дошли до конца создаем список точек и выходим
        PathResult->OutPath.clear();
        PathResult->OutPath.push_back(EndPos);
        g_ivec2 cp = EndPos;
        PathResult->Steps = _pathmap[cp.x][cp.y].weight;

        while (_pathmap[cp.x][cp.y].prev_x >= 0)
        {
          cp = g_ivec2(_pathmap[cp.x][cp.y].prev_x,_pathmap[cp.x][cp.y].prev_y);
          PathResult->OutPath.insert(PathResult->OutPath.begin(),cp);
        }

        _pnt_buffer.clear();
        break;
      }

      _pnt_buffer.push_back(CurrPnt);
    }

    _pnt_buffer.erase(_pnt_buffer.begin() + mini);
  }

  return true;
}

//-----------------------------------------------

g_bool GPathFinder::OutOfRange(g_ivec2 &value)
{
  return ((value.x < 0) || (value.x >= _map_size.x) ||
          (value.y < 0) || (value.y >= _map_size.y));
}
*/
//-----------------------------------------------
