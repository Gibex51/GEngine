#include "GObjectTransform.h"

//-----------------------------------------------

GObjectTransform::GObjectTransform(void)
{
  _position     = g_vec3(0,0,0);
  _rotation     = g_vec3(0,0,0);
  _scale        = g_vec3(1,1,1);
  _localcenter  = g_vec3(0,0,0);
  
  D3DXMatrixIdentity(&_mtrans);
  D3DXMatrixIdentity(&_mscale);
  D3DXMatrixIdentity(&_mrotate);
  D3DXMatrixIdentity(&_mrotatelocal);
}

//-----------------------------------------------

GObjectTransform::~GObjectTransform(void)
{

}

//-----------------------------------------------

void GObjectTransform::SetPosition(g_vec3 Pos)
{
  _position = Pos;
  D3DXMatrixTranslation(&_mtrans,Pos.x,Pos.y,Pos.z);
}

//-----------------------------------------------

void GObjectTransform::SetScaling(g_vec3 Scale)
{
  _scale = Scale;
  D3DXMatrixScaling(&_mscale,Scale.x,Scale.y,Scale.z);
}

//-----------------------------------------------

void GObjectTransform::RotateX(g_vec3 Center, float Angle)
{
  g_mat4x4 TMP;
  D3DXMatrixTranslation(&TMP,-Center.x,-Center.y,-Center.z);
  D3DXMatrixMultiply(&_mrotate,&_mrotate,&TMP);
  D3DXMatrixRotationX(&TMP,Angle*G_PI_180);
  D3DXMatrixMultiply(&_mrotate,&_mrotate,&TMP);
  D3DXMatrixTranslation(&TMP,Center.x,Center.y,Center.z);
  D3DXMatrixMultiply(&_mrotate,&_mrotate,&TMP);
}

//-----------------------------------------------

void GObjectTransform::RotateY(g_vec3 Center, float Angle)
{
  g_mat4x4 TMP;
  D3DXMatrixTranslation(&TMP,-Center.x,-Center.y,-Center.z);
  D3DXMatrixMultiply(&_mrotate,&_mrotate,&TMP);
  D3DXMatrixRotationY(&TMP,Angle*G_PI_180);
  D3DXMatrixMultiply(&_mrotate,&_mrotate,&TMP);
  D3DXMatrixTranslation(&TMP,Center.x,Center.y,Center.z);
  D3DXMatrixMultiply(&_mrotate,&_mrotate,&TMP);
}

//-----------------------------------------------

void GObjectTransform::RotateZ(g_vec3 Center, float Angle)
{
  g_mat4x4 TMP;
  D3DXMatrixTranslation(&TMP,-Center.x,-Center.y,-Center.z);
  D3DXMatrixMultiply(&_mrotate,&_mrotate,&TMP);
  D3DXMatrixRotationZ(&TMP,Angle*G_PI_180);
  D3DXMatrixMultiply(&_mrotate,&_mrotate,&TMP);
  D3DXMatrixTranslation(&TMP,Center.x,Center.y,Center.z);
  D3DXMatrixMultiply(&_mrotate,&_mrotate,&TMP);
}

//-----------------------------------------------

void GObjectTransform::SetLocalCenter(g_vec3 Center)
{
   _localcenter = Center;
}

//-----------------------------------------------

void GObjectTransform::RotateLocal(g_vec3 Angle)
{
  _rotation = Angle;
  g_mat4x4 TMP;
  D3DXMatrixIdentity(&_mrotatelocal);

  D3DXMatrixTranslation(&TMP,-_localcenter.x,-_localcenter.y,-_localcenter.z);
  D3DXMatrixMultiply(&_mrotatelocal,&_mrotatelocal,&TMP);

  D3DXMatrixRotationX(&TMP,Angle.x*G_PI_180);
  D3DXMatrixMultiply(&_mrotatelocal,&_mrotatelocal,&TMP);
  D3DXMatrixRotationY(&TMP,Angle.y*G_PI_180);
  D3DXMatrixMultiply(&_mrotatelocal,&_mrotatelocal,&TMP);
  D3DXMatrixRotationZ(&TMP,Angle.z*G_PI_180);
  D3DXMatrixMultiply(&_mrotatelocal,&_mrotatelocal,&TMP);

  D3DXMatrixTranslation(&TMP,_localcenter.x,_localcenter.y,_localcenter.z);
  D3DXMatrixMultiply(&_mrotatelocal,&_mrotatelocal,&TMP);
}

//-----------------------------------------------

void GObjectTransform::GetTransform(g_mat4x4 &TransformMatrix)
{
  D3DXMatrixMultiply(&TransformMatrix, &_mscale, &_mrotatelocal);
  D3DXMatrixMultiply(&TransformMatrix, &TransformMatrix, &_mrotate);
  D3DXMatrixMultiply(&TransformMatrix, &TransformMatrix, &_mtrans);
}

//-----------------------------------------------
