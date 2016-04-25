#pragma once

#ifndef GGMPLAYER_H
#define GGMPLAYER_H

#include "FG_GM_Funcs.h"

using namespace std;

class G_GM_Player
{
public:
  string Name;
  g_bool InputEnabled;
  g_bool MouseEnabled;
  g_bool MouseScrollMode;
  g_bool MoveInYCoord;
  g_bool MoveClipping;

  void CheckInput();

  void SetPosition(g_vec3 Position);
  g_vec3 GetPosition() {return _position;};
  g_vec3 GetPrevPosition() {return _prevpos;};
  void MoveToHeight(g_float Height, g_bool Smoothly);

  void SetDirection(g_vec3 Direction);
  g_vec3 GetDirection() {return _direction;};

  void SetSpeed(g_float Speed);
  g_float GetSpeed() {return _speed;};

  void SetAABBRadius(g_vec3 r) {_aabb_radius = r;};
  G_AABB GetAABB() {G_AABB aabb; aabb.center = _position; aabb.radius = _aabb_radius; return aabb;};

  void SetMoveClip(G_AABB &_clip_box) {_move_clip = _clip_box;};
  G_AABB& GetMoveClip() {return _move_clip;};

  g_float GetHeadingAngle() {return _heading;};
  g_float GetTiltAngle() {return _tilt;};
  void SetHeadingAngle(g_float angle);
  void SetTiltAngle(g_float angle);
  g_float GetCamDist() {return _camdist;};

  void SetInput(GInput *pGInput) {_ginput = pGInput;};
  void SetCamera(GCamera *pCamera) {_camera = pCamera;};
  GCamera* GetCamera() {return _camera;};

  void SetTiltRange(g_float min, g_float max);
  void SetCamDistRange(g_float min, g_float max);

  //Конструктор и деструктор
  G_GM_Player(GInput *pGInput, GCamera *pCamera, string Name);
  ~G_GM_Player(void);
private:
  g_vec3   _position;
  g_vec3   _direction;
  g_vec3   _aabb_radius;
  g_vec3   _prevpos;
  g_float  _speed;
  g_float  _camdist;
  g_float  _heading;
  g_float  _tilt;
  g_float  _min_ang_y;
  g_float  _max_ang_y;
  g_float  _min_camdist;
  g_float  _max_camdist;
  G_AABB   _move_clip;

  g_double _prevtime;

  GInput   *_ginput;
  GCamera  *_camera;

  void UpdateCameraView();
};

#endif //GGMPLAYER_H
