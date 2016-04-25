#pragma once

#ifndef GOBJTRANS_H
#define GOBJTRANS_H

#include "GDevice.h"

class GObjectTransform
{
public:
  void SetPosition(g_vec3 Pos);
  g_vec3 GetPosition() {return _position;};

  void SetScaling(g_vec3 Scale);
  g_vec3 GetScaling() {return _scale;};

  void RotateX(g_vec3 Center, float Angle);	
  void RotateY(g_vec3 Center, float Angle);
  void RotateZ(g_vec3 Center, float Angle);

  void RotateLocal(g_vec3 Angle);
  void SetLocalCenter(g_vec3 Center);
  g_vec3 GetLocalAngle() {return _rotation;};	
  g_vec3 GetLocalCenter() {return _localcenter;};
    
  /* ????Доделать!!!! */
  void SetTranslateMatrix(const g_mat4x4 &tmat) {_mtrans = tmat;};
  void SetScaleMatrix(const g_mat4x4 &smat) {_mscale = smat;};
  void SetRotationMatrix(const g_mat4x4 &rmat) {_mrotate = rmat;};
  void SetLocalRotationMatrix(const g_mat4x4 &lrmat) {_mrotatelocal = lrmat;};

  void GetTransform(g_mat4x4 &TransformMatrix);
  //Конструктор и деструктор
  GObjectTransform(void);
  ~GObjectTransform(void);
protected:

  g_vec3 _position;
  g_vec3 _rotation;
  g_vec3 _scale;
  g_vec3 _localcenter;

  g_mat4x4 _mtrans;
  g_mat4x4 _mscale;
  g_mat4x4 _mrotate;
  g_mat4x4 _mrotatelocal;
};

#endif //GOBJTRANS_H