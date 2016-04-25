#include "G_GM_Player.h"

//-----------------------------------------------

G_GM_Player::G_GM_Player(GInput *pGInput, GCamera *pCamera, string PlayerName)
{
  _position = g_vec3(0,0,0);
  _prevpos = g_vec3(0,0,0);
  _direction = g_vec3(1,0,0);
  _speed = 20.0f;
  _camdist = 40.0f;
  _min_ang_y = -89;
  _max_ang_y = 89;
  _min_camdist = 0.5f;
  _max_camdist = 600;
  _prevtime = GetTickCount();

  _ginput = pGInput;
  _camera = pCamera;

  _aabb_radius = g_vec3(1,1,1);

  _heading = 0;
  _tilt = 0;

  Name = PlayerName;
  InputEnabled = true;
  MouseEnabled = false;
  MouseScrollMode = true;
  MoveInYCoord = false;
  MoveClipping = false;

  UpdateCameraView();
}

//-----------------------------------------------

G_GM_Player::~G_GM_Player(void)
{

}

//-----------------------------------------------

void G_GM_Player::UpdateCameraView()
{ 
  if (_camera != NULL)
  {
    _camera->RotateAround(_position,_camdist,_tilt * G_PI_180,_heading * G_PI_180);
	_direction = _camera->GetDirection();
  }
}

//-----------------------------------------------

void G_GM_Player::SetSpeed(g_float Speed)
{
  if (Speed < 0) Speed = 0;
  if (Speed > G_GM_MAX_SPEED) Speed = G_GM_MAX_SPEED;
  _speed = Speed;
}

//-----------------------------------------------

void G_GM_Player::SetPosition(g_vec3 Position) 
{
  _position = Position;

  if (MoveClipping)
    CheckRangeVec(&_position,_move_clip.GetPoint(0),_move_clip.GetPoint(7));

  UpdateCameraView();
}

//-----------------------------------------------

void G_GM_Player::MoveToHeight(g_float Height, g_bool Smoothly)
{
  if (Smoothly)
  {
    g_float dist = Height - _position.y;

    _position.y += dist/200.0f;
  }
  else
    _position.y = Height;
}

//-----------------------------------------------

void G_GM_Player::SetDirection(g_vec3 Direction) 
{
  D3DXVec3Normalize(&_direction,&Direction);
}

//-----------------------------------------------

void G_GM_Player::SetTiltRange(g_float min, g_float max)
{
  _min_ang_y = min;
  _max_ang_y = max;
}

//-----------------------------------------------

void G_GM_Player::SetCamDistRange(g_float min, g_float max)
{
  _min_camdist = min;
  _max_camdist = max;
}

//-----------------------------------------------

void G_GM_Player::CheckInput()
{
  if (_ginput != NULL)
  {
	//Для перемещения камеры мышкой
	g_bool mv_l = false, mv_r = false, mv_t = false, mv_b = false;

    if (MouseEnabled) //Обработка движений мышью
    {	  
      if ((_ginput->RX != 0) || (_ginput->RY != 0))
      {
	    _heading += _ginput->RX * 0.17f;
	    _tilt += _ginput->RY * 0.1f;
	    CheckRange(&_tilt,_min_ang_y,_max_ang_y);
      }
    }
	else if (MouseScrollMode) 
	{
      if (_ginput->AX < G_GM_SCROLL_FRAME_WIDTH) mv_l = true;
	  if (_ginput->AY < G_GM_SCROLL_FRAME_WIDTH) mv_t = true;  
	  if (_ginput->AX >= g_int32(_camera->GetViewport().Width)-G_GM_SCROLL_FRAME_WIDTH) mv_r = true;
	  if (_ginput->AY >= g_int32(_camera->GetViewport().Height)-G_GM_SCROLL_FRAME_WIDTH) mv_b = true;
	}
    
    if (InputEnabled) //Обработка нажатий клавиш 
    {	
      if (_ginput->RZ != 0)
	  {
	    _camdist -= _ginput->RZ / 40.0f;
	    CheckRange(&_camdist,_min_camdist,_max_camdist);
	  }

      g_double _togtime = GetTicks();
	  g_double _timeleft = _togtime - _prevtime;

	  if (_timeleft > 2000)
	  {
	    _prevtime = _togtime;
	    _timeleft = 0;
	  }

	  if (_timeleft > 0)
	  {
	    g_bool   _moving = false;
	    g_vec3   _movedirection = g_vec3(0,0,0);

	    if ((_ginput->CheckAction(GM_ACT_MAPMOVEUP)) || (mv_t == true))
	    {
	      _moving = true;
		  _movedirection += _direction;
	    }

	    if ((_ginput->CheckAction(GM_ACT_MAPMOVELEFT)) || (mv_l == true))
	    {
	      _moving = true;
		  _movedirection.x -= _direction.z;
		  _movedirection.z += _direction.x;
	    }

	    if ((_ginput->CheckAction(GM_ACT_MAPMOVEDOWN)) || (mv_b == true))
	    {
	      _moving = true;
		  _movedirection -= _direction;
	    }

	    if ((_ginput->CheckAction(GM_ACT_MAPMOVERIGHT)) || (mv_r == true))
	    {
	      _moving = true;
		  _movedirection.x += _direction.z;
		  _movedirection.z -= _direction.x;
	    }

	    if (_moving)
	    {
	  	  if (!MoveInYCoord)
            _movedirection.y = 0;
		  D3DXVec3Normalize(&_movedirection,&_movedirection);
          _prevpos = _position;
		  _position += _movedirection*g_float((_speed*_timeleft)/1000.0f);
  
          if (MoveClipping)
            CheckRangeVec(&_position,_move_clip.GetPoint(0),_move_clip.GetPoint(7));
	    }

	    _prevtime = _togtime;
	  }
    }
	//Rotate Camera
	UpdateCameraView();
  }
}

//-----------------------------------------------

void G_GM_Player::SetHeadingAngle(g_float angle)
{
  _heading = angle;
  UpdateCameraView();
}

//-----------------------------------------------

void G_GM_Player::SetTiltAngle(g_float angle)
{
  _tilt = angle;
  CheckRange(&_tilt,_min_ang_y,_max_ang_y);
  UpdateCameraView();
}

//-----------------------------------------------
