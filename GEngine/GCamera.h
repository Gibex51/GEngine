#pragma once

#include "GBaseObject.h"

#ifndef GCAMERA_H
#define GCAMERA_H

class GCamera :
	public GBaseObject
{
public:
	g_bool Enabled;
	
	//Viewport
	void InitViewport(g_int32 Width, g_int32 Height, g_float MinDepth = 0, g_float MaxDepth = 1, 
		              g_int32 TopLeftX = 0, g_int32 TopLeftY = 0);
	void SetViewport();
	const D3D10_VIEWPORT& GetViewport() {return _viewport;};
	void ResizeViewport(g_int32 Width, g_int32 Height);

	//View Projection
	void InitProjection(g_bool Ortho, g_float Near = 0.1f, g_float Far = 10000.0f);
	void SetFOV(g_float FOV_Value);

	void RotateAround(g_vec3 Center, g_float Distance, g_float AngX, g_float AngY);
	void SetView(const g_vec3 &vPos, const g_vec3 &vDir, const g_vec3 &vUp = g_vec3(0,1,0));
	void SetViewEx(const g_vec3 &vEye, const g_vec3 &vAt, const g_vec3 &vUp = g_vec3(0,1,0));

	void SetPosition(const g_vec3 &vPos);
	void SetDirection(const g_vec3 &vDir);
	void SetEye(const g_vec3 &vEye);
	void SetAt(const g_vec3 &vAt);
	void SetUp(const g_vec3 &vUp);

	g_vec3 GetDirection();
	inline g_vec3 GetPosition() {return _at;};

	void GetNearFar(g_vec2 &NearFar) {NearFar.x = _near; NearFar.y = _far;};

	void GetRayFromScreenPos(g_ivec2 &pos, g_mat4x4 &mWorld, g_vec3 *out_ray_pos, g_vec3 *out_ray_dir);
	
	g_bool Frustum_CheckAABB(G_AABB AABB);

	//Matrix
	inline g_mat4x4& GetViewMatrix() {return _mview;};
	inline g_mat4x4& GetProjMatrix() {return _mproj;};
	inline g_mat4x4& GetViewProjMatrix() {return _mviewproj;};
	inline g_mat4x4& GetView2DMatrix() {return _mview2d;};
	inline g_mat4x4& GetProj2DMatrix() {return _mproj2d;};
	inline g_mat4x4& GetViewProj2DMatrix() {return _mviewproj2d;};

	void SetCustomViewMatrix(g_mat4x4 &mView);
	void SetCustomProjMatrix(g_mat4x4 &mProj);

	void GetFrustumPoints(g_vec3 **fr_points) {*fr_points = _fr_points;};
	void GetFrustumPoints(g_rect _Rect, g_float Near, g_float Far, g_vec3 **fr_points);

	G_AABB& GetFrustumAABB() {return _fr_aabb;};

	//Конструктор и деструктор
	GCamera(const g_string &Name, GD3DDevice *Device);
	~GCamera(void);
private:
	//3D 
	g_mat4x4 _mproj;
	g_mat4x4 _mview;
	g_mat4x4 _mviewproj;
    //2D для интерфейса
	g_mat4x4 _mproj2d;
	g_mat4x4 _mview2d;
	g_mat4x4 _mviewproj2d;
	//Frustum planes
	g_plane  _fr_planes[6];
	g_vec3   *_fr_points;
	//Frustum AABB
	G_AABB   _fr_aabb;
	//Параметры
    g_bool   _is_ortho;

	D3D10_VIEWPORT _viewport;
	D3D10_RECT     _scissrect;

	g_float  _fov, _aspect, _near, _far;
	g_vec3   _eye, _at, _up;
	//Инициализация
	void Init();
	inline void MakeViewProjMatrix();
	void CalcFrustum();
};

#endif //GCAMERA_H