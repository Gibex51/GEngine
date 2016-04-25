#include "GCamera.h"

//-----------------------------------------------

GCamera::GCamera(const g_string &Name, GD3DDevice *Device):GBaseObject(Name,Device)
{
  Init();
  _fr_points = new g_vec3[8];
  SetType(G_OT_CAMERA);
}

//-----------------------------------------------

GCamera::~GCamera(void)
{
  if (_fr_points != NULL) delete[]_fr_points;
}

//-----------------------------------------------

void GCamera::Init()
{
  _is_ortho = false;
  Enabled = true;

  _fov = G_PI_180 * 65.0f;

  D3DXMatrixIdentity(&_mproj);
  D3DXMatrixIdentity(&_mview);
  D3DXMatrixIdentity(&_mviewproj);
  D3DXMatrixIdentity(&_mproj2d);
  D3DXMatrixIdentity(&_mview2d);
  D3DXMatrixIdentity(&_mviewproj2d);
}

//-----------------------------------------------

void GCamera::MakeViewProjMatrix()
{
  D3DXMatrixLookAtLH(&_mview,&_eye,&_at,&_up);
  D3DXMatrixMultiply(&_mviewproj,&_mview,&_mproj);
  CalcFrustum();
}

//-----------------------------------------------

void GCamera::CalcFrustum()
{
  // Calculate near plane of frustum.
  _fr_planes[0].a = _mviewproj._14 + _mviewproj._13;
  _fr_planes[0].b = _mviewproj._24 + _mviewproj._23;
  _fr_planes[0].c = _mviewproj._34 + _mviewproj._33;
  _fr_planes[0].d = _mviewproj._44 + _mviewproj._43;
  D3DXPlaneNormalize(&_fr_planes[0], &_fr_planes[0]);

  // Calculate far plane of frustum.
  _fr_planes[1].a = _mviewproj._14 - _mviewproj._13; 
  _fr_planes[1].b = _mviewproj._24 - _mviewproj._23;
  _fr_planes[1].c = _mviewproj._34 - _mviewproj._33;
  _fr_planes[1].d = _mviewproj._44 - _mviewproj._43;
  D3DXPlaneNormalize(&_fr_planes[1], &_fr_planes[1]);

  // Calculate left plane of frustum.
  _fr_planes[2].a = _mviewproj._14 + _mviewproj._11; 
  _fr_planes[2].b = _mviewproj._24 + _mviewproj._21;
  _fr_planes[2].c = _mviewproj._34 + _mviewproj._31;
  _fr_planes[2].d = _mviewproj._44 + _mviewproj._41;
  D3DXPlaneNormalize(&_fr_planes[2], &_fr_planes[2]);

  // Calculate right plane of frustum.
  _fr_planes[3].a = _mviewproj._14 - _mviewproj._11; 
  _fr_planes[3].b = _mviewproj._24 - _mviewproj._21;
  _fr_planes[3].c = _mviewproj._34 - _mviewproj._31;
  _fr_planes[3].d = _mviewproj._44 - _mviewproj._41;
  D3DXPlaneNormalize(&_fr_planes[3], &_fr_planes[3]);

  // Calculate top plane of frustum.
  _fr_planes[4].a = _mviewproj._14 - _mviewproj._12; 
  _fr_planes[4].b = _mviewproj._24 - _mviewproj._22;
  _fr_planes[4].c = _mviewproj._34 - _mviewproj._32;
  _fr_planes[4].d = _mviewproj._44 - _mviewproj._42;
  D3DXPlaneNormalize(&_fr_planes[4], &_fr_planes[4]);

  // Calculate bottom plane of frustum.
  _fr_planes[5].a = _mviewproj._14 + _mviewproj._12;
  _fr_planes[5].b = _mviewproj._24 + _mviewproj._22;
  _fr_planes[5].c = _mviewproj._34 + _mviewproj._32;
  _fr_planes[5].d = _mviewproj._44 + _mviewproj._42;
  D3DXPlaneNormalize(&_fr_planes[5], &_fr_planes[5]);

  g_rect vpRect = {0,0,_viewport.Width,_viewport.Height};
  GetFrustumPoints(vpRect, _viewport.MinDepth, _viewport.MaxDepth, &_fr_points);

  _fr_aabb.SetFromPoints(&_fr_points, 8);
}

//-----------------------------------------------

g_bool GCamera::Frustum_CheckAABB(G_AABB AABB)
{
  G_AABB_P AABB_Pnts;
  AABB.GetPoints(AABB_Pnts);

  for(g_int32 i = 0; i < 6; i++) 
  {
    if (D3DXPlaneDotCoord(&_fr_planes[i], &AABB_Pnts.points[0]) >= 0.0f) continue;
    if (D3DXPlaneDotCoord(&_fr_planes[i], &AABB_Pnts.points[1]) >= 0.0f) continue;
	if (D3DXPlaneDotCoord(&_fr_planes[i], &AABB_Pnts.points[2]) >= 0.0f) continue;
	if (D3DXPlaneDotCoord(&_fr_planes[i], &AABB_Pnts.points[3]) >= 0.0f) continue;
	if (D3DXPlaneDotCoord(&_fr_planes[i], &AABB_Pnts.points[4]) >= 0.0f) continue;
	if (D3DXPlaneDotCoord(&_fr_planes[i], &AABB_Pnts.points[5]) >= 0.0f) continue;
	if (D3DXPlaneDotCoord(&_fr_planes[i], &AABB_Pnts.points[6]) >= 0.0f) continue;
	if (D3DXPlaneDotCoord(&_fr_planes[i], &AABB_Pnts.points[7]) >= 0.0f) continue;
    return false;
  }

  return true;
}

//-----------------------------------------------

void GCamera::InitViewport(g_int32 Width, g_int32 Height, g_float MinDepth, g_float MaxDepth, 
							g_int32 TopLeftX, g_int32 TopLeftY )
{
  _viewport.Width = Width;
  _viewport.Height = Height;
  _viewport.MinDepth = MinDepth;
  _viewport.MaxDepth = MaxDepth;
  _viewport.TopLeftX = TopLeftX;
  _viewport.TopLeftY = TopLeftY;

  SetRect(&_scissrect,TopLeftX,TopLeftY,TopLeftX + Width,TopLeftY + Height);
}

//----------------------------------------------

void GCamera::SetViewport()
{
  _device->D3DDevice->RSSetViewports(1, &_viewport);
  _device->D3DDevice->RSSetScissorRects(1, &_scissrect);
}

//-----------------------------------------------

void GCamera::ResizeViewport(g_int32 Width, g_int32 Height)
{
  InitViewport(Width, Height, _viewport.MinDepth, _viewport.MaxDepth, _viewport.TopLeftX, _viewport.TopLeftY);
  InitProjection(_is_ortho, _near, _far);
}

//-----------------------------------------------

void GCamera::InitProjection(g_bool Ortho, g_float Near, g_float Far)
{
  _aspect = _viewport.Width/(g_float)_viewport.Height;
  _near = Near;
  _far = Far;
  _is_ortho = Ortho;

  //Инициализация 2D
  D3DXMatrixOrthoOffCenterLH(&_mproj2d,g_float(_viewport.TopLeftX),
	                                   g_float(_viewport.TopLeftX + _viewport.Width),
									   g_float(_viewport.TopLeftY + _viewport.Height),
									   g_float(_viewport.TopLeftY),0,100);
  g_vec3 _eye_2d = D3DXVECTOR3(0.0f,0.0f,-0.1f);
  g_vec3 _at_2d  = D3DXVECTOR3(0.0f,0.0f,0.0f);
  g_vec3 _up_2d  = D3DXVECTOR3(0.0f,1.0f,0.0f);
  D3DXMatrixLookAtLH(&_mview2d,&_eye_2d,&_at_2d,&_up_2d);
  D3DXMatrixMultiply(&_mviewproj2d,&_mview2d,&_mproj2d);

  //Инициализация 3D
  if (_is_ortho)
  {
	D3DXMatrixOrthoOffCenterLH(&_mproj,g_float(_viewport.TopLeftX),
	                                   g_float(_viewport.TopLeftX + _viewport.Width),
									   g_float(_viewport.TopLeftY + _viewport.Height),
									   g_float(_viewport.TopLeftY), _near, _far);
  }
  else
    D3DXMatrixPerspectiveFovLH(&_mproj, _fov, _aspect, _near, _far);
  D3DXMatrixMultiply(&_mviewproj,&_mview,&_mproj);
  CalcFrustum();
}

//-----------------------------------------------

void GCamera::SetFOV(g_float FOV_Value)
{
  if (_is_ortho) return;
  _fov = G_PI_180 * FOV_Value;
  D3DXMatrixPerspectiveFovLH(&_mproj, _fov, _aspect, _near, _far);
  D3DXMatrixMultiply(&_mviewproj,&_mview,&_mproj);
  CalcFrustum();
}

//-----------------------------------------------

void GCamera::RotateAround(g_vec3 Center, g_float Distance, g_float AngX, g_float AngY)
{
  g_mat4x4 _finalmat, _tmp;
  D3DXMatrixTranslation(&_finalmat,Distance,0,0);
  D3DXMatrixRotationZ(&_tmp,AngX);
  D3DXMatrixMultiply(&_finalmat,&_finalmat,&_tmp);
  D3DXMatrixRotationY(&_tmp,AngY);
  D3DXMatrixMultiply(&_finalmat,&_finalmat,&_tmp);
  D3DXMatrixTranslation(&_tmp,Center.x,Center.y,Center.z);
  D3DXMatrixMultiply(&_finalmat,&_finalmat,&_tmp);

  g_vec4 _dir;
  g_vec3 _src = g_vec3(0,0,0);
  D3DXVec3Transform(&_dir,&_src,&_finalmat);
  _eye = g_vec3(_dir.x,_dir.y,_dir.z);
  _at = Center;

  MakeViewProjMatrix();
}

//-----------------------------------------------

void GCamera::SetView(const g_vec3 &vPos, const g_vec3 &vDir, const g_vec3 &vUp)
{
  _eye = vPos;
  _at = vPos + vDir;
  _up = vUp;
  
  MakeViewProjMatrix();
}

//-----------------------------------------------

void GCamera::SetViewEx(const g_vec3 &vEye, const g_vec3 &vAt, const g_vec3 &vUp)
{
  _eye = vEye;
  _at = vAt;
  _up = vUp;
  
  MakeViewProjMatrix();
}

//-----------------------------------------------

void GCamera::SetPosition(const g_vec3 &vPos)
{
  _at = (_at - _eye) + vPos;
  _eye = vPos;

  MakeViewProjMatrix();
}

//-----------------------------------------------

void GCamera::SetDirection(const g_vec3 &vDir)
{
  _at = _eye + vDir;
  
  MakeViewProjMatrix();
}

//-----------------------------------------------

void GCamera::SetEye(const g_vec3 &vEye)
{
  _eye = vEye;

  MakeViewProjMatrix();
}

//-----------------------------------------------

void GCamera::SetAt(const g_vec3 &vAt)
{
  _at = vAt;
  
  MakeViewProjMatrix();
}

//-----------------------------------------------

void GCamera::SetUp(const g_vec3 &vUp)
{
  _up = vUp;
  
  MakeViewProjMatrix();
}

//-----------------------------------------------

g_vec3 GCamera::GetDirection()
{
  g_vec3 ResultDirection = _at-_eye;
  D3DXVec3Normalize(&ResultDirection,&ResultDirection);
  return ResultDirection;
}

//-----------------------------------------------

void GCamera::GetRayFromScreenPos(g_ivec2 &pos, g_mat4x4 &mWorld, g_vec3 *out_ray_pos, g_vec3 *out_ray_dir)
{
  if (this == NULL) return;
  g_vec3 p1,p2;

  D3DXVec3Unproject(&p1, &g_vec3(g_float(pos.x), g_float(pos.y), _viewport.MinDepth), &_viewport, &_mproj, &_mview, &mWorld);
  D3DXVec3Unproject(&p2, &g_vec3(g_float(pos.x), g_float(pos.y), _viewport.MaxDepth), &_viewport, &_mproj, &_mview, &mWorld);

  *out_ray_pos = p1;
  *out_ray_dir = p2 - p1;
  D3DXVec3Normalize(out_ray_dir,out_ray_dir);
}

//-----------------------------------------------

void GCamera::SetCustomViewMatrix(g_mat4x4 &mView) 
{
  _mview = mView;
  D3DXMatrixMultiply(&_mviewproj,&_mview,&_mproj); 
  CalcFrustum();
}

//-----------------------------------------------

void GCamera::SetCustomProjMatrix(g_mat4x4 &mProj) 
{
  _mproj = mProj; 
  D3DXMatrixMultiply(&_mviewproj,&_mview,&_mproj); 
  CalcFrustum();
}

//-----------------------------------------------

void GCamera::GetFrustumPoints(g_rect _Rect, g_float Near, g_float Far, g_vec3 **fr_points)
{
  g_float vpl = g_float(_Rect.left);
  g_float vpt = g_float(_Rect.top);
  g_float vpr = g_float(_Rect.right);
  g_float vpb = g_float(_Rect.bottom);

  //Near Points
  D3DXVec3Unproject(&(*fr_points)[0], &g_vec3(vpl, vpt, Near), &_viewport, &_mproj, &_mview, NULL);
  D3DXVec3Unproject(&(*fr_points)[1], &g_vec3(vpr, vpt, Near), &_viewport, &_mproj, &_mview, NULL);
  D3DXVec3Unproject(&(*fr_points)[2], &g_vec3(vpl, vpb, Near), &_viewport, &_mproj, &_mview, NULL);
  D3DXVec3Unproject(&(*fr_points)[3], &g_vec3(vpr, vpb, Near), &_viewport, &_mproj, &_mview, NULL);
  
  //Far Points
  D3DXVec3Unproject(&(*fr_points)[4], &g_vec3(vpl, vpt, Far), &_viewport, &_mproj, &_mview, NULL);
  D3DXVec3Unproject(&(*fr_points)[5], &g_vec3(vpr, vpt, Far), &_viewport, &_mproj, &_mview, NULL);
  D3DXVec3Unproject(&(*fr_points)[6], &g_vec3(vpl, vpb, Far), &_viewport, &_mproj, &_mview, NULL);
  D3DXVec3Unproject(&(*fr_points)[7], &g_vec3(vpr, vpb, Far), &_viewport, &_mproj, &_mview, NULL);
}

//-----------------------------------------------
