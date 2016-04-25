#pragma once

#ifndef GGMUNIT_H
#define GGMUNIT_H

#include "FG_GM_Funcs.h"

//-----------------------------------------------

struct G_NPC_DATA
{
  //NPC Data
  g_string Name;
  g_int32  ID;

  //Model
  g_uint32 ModelID;
  g_uint32 TexID;

  //Base Characteristics
  //...

  G_NPC_DATA()
  {
    Name = "";
	ID = 0; ModelID = 0; TexID = 0;
  };
};

//-----------------------------------------------

#define UNIT_CUST_ANIM_MOVE   AID_CUSTOM_DEF
#define UNIT_CUST_ANIM_STAY   AID_CUSTOM_DEF + 1

const g_int32 nframes_idle_anim = 7;
const g_int32 nframes_move_anim = 3;

enum g_unit_state {g_us_idle, g_us_move, g_us_shoot, g_us_cast, 
	               g_us_attacked, g_us_dying, g_us_die, g_us_test};

//-----------------------------------------------

class G_GM_Unit:
  public GBaseMapObject
{
  //friend class G_GM_NPCMgr;
public:
  void SetName(const g_string& Name);
  const g_string& GetName() {return _name;};
  
  void SetPos(g_vec3 Pos);
  g_vec3 GetPos() {return _abs_position;};
  void SetAbsCubePos(g_ivec3 abs_cpos);
  g_ivec3 GetAbsCubePos();
  
  void SetDirection(g_vec3 Direction) {_direction = Direction;};
  g_vec3 GetDirection() {return _direction;};
  
  void SetVisible(g_bool Visible) {_mesh->Visible = Visible;};
  g_bool GetVisible() {return _mesh->Visible;};
  
  void SetHealth(g_uint32 Health) {_health = Health;};
  g_uint32 GetHealth() {return _health;}; 

  void UpdateState();

  void MoveTo(G_WP_PATH &path);
  void MoveContinue();
  void MoveStop();

  void RunAnim(g_uint32 index);

  void SetMeshInstance(GMeshInstance *Mesh);

  //Конструктор и деструктор
  G_GM_Unit(const g_string& Name, g_uint32 Health);
  ~G_GM_Unit(void);
private:
  g_string       _name;            //sgi-

  G_NPC_DATA     *_p_npc_data;     //--i-
  GMeshInstance  *_mesh;           //s-i-

  g_vec3         _abs_position;    //sgi-
  g_vec3         _direction;       //sgi-

  g_uint32       _health;          //sgi-

  g_unit_state   _ustate;          //--i-

  G_WP_PATH      _curr_path;       //--i-?
  g_int32        _next_way_point;  //--i-?
   
  //
  GMatrixAnimation  _cust_anim_stay; //--i-?
  GMatrixAnimation  _cust_anim_move; //--i-?
  
  void _init_cust_anims();
  void _run_idle_anim();
  void _run_move_anim(g_uint32 curr_wp);
  
  void _init_unit(const g_string& Name, g_uint32 Health);
};

#endif //GGMUNIT_H