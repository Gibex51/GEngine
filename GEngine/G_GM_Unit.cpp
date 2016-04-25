#include "G_GM_Unit.h"

//-----------------------------------------------

G_GM_Unit::G_GM_Unit(const g_string& Name, g_uint32 Health)
{
  _init_unit(Name, Health);
}

//-----------------------------------------------

G_GM_Unit::~G_GM_Unit(void)
{

}

//-----------------------------------------------

void G_GM_Unit::_init_unit(const g_string& Name, g_uint32 Health)
{
  SetHealth(Health);
  SetMeshInstance(NULL);
  SetPos(g_vec3(0,0,0));
  SetDirection(g_vec3(-1,0,0));
  SetName(Name);

  _p_npc_data = NULL;

  _ustate = g_us_idle;
  _init_cust_anims();
}

//-----------------------------------------------

void G_GM_Unit::SetName(const g_string& Name) 
{
  _name = Name;
  if (_name.length() > G_MAXUNITNAMELEN)
    _name.resize(G_MAXUNITNAMELEN);  
}

//-----------------------------------------------

void G_GM_Unit::SetPos(g_vec3 Pos) 
{
  _abs_position = Pos;
  if (_mesh != NULL)
    _mesh->SetPosition(_abs_position);
  _run_idle_anim(); //Test
}

//-----------------------------------------------

void G_GM_Unit::SetAbsCubePos(g_ivec3 abs_cpos)
{
  _abs_position = AbsCubePosToRealPos(abs_cpos);
  if (_mesh != NULL)
    _mesh->SetPosition(_abs_position);
  _run_idle_anim(); //Test
}

//-----------------------------------------------

g_ivec3 G_GM_Unit::GetAbsCubePos()
{
  return GetAbsCubePosByVec3(_abs_position);
}

//-----------------------------------------------

void G_GM_Unit::UpdateState()
{
  if ((_ustate == g_us_move) && (_mesh->GetAnimationState(UNIT_CUST_ANIM_MOVE) == g_as_inactive))
    MoveContinue();

  if ((_ustate == g_us_test) && (_mesh->GetActiveAnimCount() == 0))
	MoveStop();
}

//-----------------------------------------------

void G_GM_Unit::MoveContinue()
{
  if ((_next_way_point >= 0) && (_next_way_point < g_int32(_curr_path.way_points.size())-1))
  {
	_run_move_anim(_next_way_point);
	_next_way_point++;
  }
  else
	MoveStop();
}

//-----------------------------------------------

void G_GM_Unit::MoveStop()
{
  _ustate = g_us_idle;
  _run_idle_anim();
}

//-----------------------------------------------

void G_GM_Unit::MoveTo(G_WP_PATH &path)
{
  _curr_path = path;
  _run_move_anim(0);
  _next_way_point = 1;
  _ustate = g_us_move;
}

//-----------------------------------------------

void G_GM_Unit::SetMeshInstance(GMeshInstance *Mesh) 
{
  _mesh = Mesh;
}

//-----------------------------------------------

void G_GM_Unit::RunAnim(g_uint32 index)
{
  if (_mesh != NULL)
  {
    _mesh->ClearActiveAnimList();
	_mesh->RunAnimation(index, 1, 0);
	_ustate = g_us_test;
  }
}

//-----------------------------------------------

void G_GM_Unit::_init_cust_anims()
{
  //Init Idle Animation
  {
    G_MATRIX_FRAME FrameArray[nframes_idle_anim] = {
                             G_MATRIX_FRAME(0   , g_vec3(0,0,0), g_vec3(0,0,0), g_vec3(0,0,0), true, false, false),
                             G_MATRIX_FRAME(300 , g_vec3(0,0,0), g_vec3(0,0,0), g_vec3(0,0,0), true, false, false),
                             G_MATRIX_FRAME(1200, g_vec3(0,0,0), g_vec3(0,0,0), g_vec3(0,0,0), true, false, false),
                             G_MATRIX_FRAME(1500, g_vec3(0,0,0), g_vec3(0,0,0), g_vec3(0,0,0), true, false, false),
                             G_MATRIX_FRAME(1800, g_vec3(0,0,0), g_vec3(0,0,0), g_vec3(0,0,0), true, false, false),
                             G_MATRIX_FRAME(2700, g_vec3(0,0,0), g_vec3(0,0,0), g_vec3(0,0,0), true, false, false),
                             G_MATRIX_FRAME(3000, g_vec3(0,0,0), g_vec3(0,0,0), g_vec3(0,0,0), true, false, false)
                           };

	_cust_anim_stay.SetUID(UNIT_CUST_ANIM_STAY);
    _cust_anim_stay.Release();
    _cust_anim_stay.SetLength(3000);
    _cust_anim_stay.AddKeyFrameArray(FrameArray,nframes_idle_anim);
  }
  //Init Move Animation
  {
    G_MATRIX_FRAME FrameArray[nframes_move_anim] = {
                             G_MATRIX_FRAME(0   , g_vec3(0,0,0), g_vec3(0,0,0), g_vec3(0,0,0), true, false, true),
							 G_MATRIX_FRAME(50  , g_vec3(0,0,0), g_vec3(0,0,0), g_vec3(0,0,0), true, false, true),
                             G_MATRIX_FRAME(300 , g_vec3(0,0,0), g_vec3(0,0,0), g_vec3(0,0,0), true, false, false),
                           };

	_cust_anim_move.SetUID(UNIT_CUST_ANIM_MOVE);
    _cust_anim_move.Release();
    _cust_anim_move.SetLength(300);
    _cust_anim_move.AddKeyFrameArray(FrameArray,nframes_move_anim);
  }
}

//-----------------------------------------------

void G_GM_Unit::_run_idle_anim()
{
  if (_mesh != NULL)
  {
	_mesh->ClearActiveAnimList();

	g_vec3 st_pos = _abs_position;
	
	if (_cust_anim_stay.KeyFrames_List.size() == nframes_idle_anim)
	{
	  g_vec3 move_offset[nframes_idle_anim] = {g_vec3(0,0,0), g_vec3(0,0.1f,0), g_vec3(0,0.9f,0), 
		                                       g_vec3(0,1,0), g_vec3(0,0.9f,0), g_vec3(0,0.1f,0), 
											   g_vec3(0,0,0)};

	  for (g_uint32 i = 0; i<nframes_idle_anim; i++)
        _cust_anim_stay.KeyFrames_List[i]->_position = st_pos + move_offset[i];
	}
    
	_mesh->RunCustomAnimation(&_cust_anim_stay, /*UNIT_CUST_ANIM_STAY,*/ 0, 0);
	_mesh->RunAnimation(AID_NPC_STAY_DEF, 0, 0);
  }
}

//-----------------------------------------------

void G_GM_Unit::_run_move_anim(g_uint32 curr_wp)
{
  if (_mesh != NULL)
  {
    _mesh->ClearActiveAnimList();

	g_uint32 nFrames = _curr_path.way_points.size();
	g_uint32 next_wp = curr_wp + 1;

	if (next_wp >= nFrames) 
	  return;

    g_vec3  curr_pos = AbsCubePosToRealPos(_curr_path.way_points[next_wp].move_pos);
	g_vec3  prev_pos = AbsCubePosToRealPos(_curr_path.way_points[curr_wp].move_pos);

	g_vec3 new_dir = curr_pos - prev_pos;
	g_float curr_angle = CalcVec3Angle(&g_vec3(0,0,1), &_direction, &g_vec3(0,1,0)) + 180;
	g_float next_angle = curr_angle;

	if ((abs(new_dir.x) > 0.01f) || (abs(new_dir.z) > 0.01f))
	{
      next_angle = CalcVec3Angle(&g_vec3(0,0,1), &new_dir, &g_vec3(0,1,0)) + 180;
	  _direction = new_dir;
	}

	if (next_angle - curr_angle > 180)
	  next_angle -= 360;
	
	if (curr_angle - next_angle > 180)
	  curr_angle -= 360;

	g_int32 rot_time = g_int32(abs(next_angle - curr_angle));
    g_int32 curr_time = g_int32(_curr_path.way_points[next_wp].length*300.0f) + rot_time;

	if (_cust_anim_move.KeyFrames_List.size() == nframes_move_anim)
	{
	  g_vec3 move_pos[nframes_move_anim] = {_abs_position, _abs_position, curr_pos};
	  g_int32 move_time[nframes_move_anim] = {0, rot_time, curr_time};
      g_vec3 move_rot[nframes_move_anim] = {g_vec3(0,curr_angle,0), g_vec3(0,next_angle,0), g_vec3(0,0,0)};

	  for (g_uint32 i = 0; i<nframes_move_anim; i++)
	  {
        _cust_anim_move.KeyFrames_List[i]->_start_time = move_time[i];
        _cust_anim_move.KeyFrames_List[i]->_position = move_pos[i];
		_cust_anim_move.KeyFrames_List[i]->_rotation = move_rot[i];
	  }

	  _cust_anim_move.SetLength(curr_time);
	}

	_abs_position = curr_pos;

    //_mesh->RunAnimation(AID_NPC_STAY_DEF, 0, anim_len);
    _mesh->RunCustomAnimation(&_cust_anim_move, /*UNIT_CUST_ANIM_MOVE,*/ 1, 0 /*curr_time*/);
  }
}

//-----------------------------------------------
