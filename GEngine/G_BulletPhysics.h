#pragma once

#ifndef GBULLPHYS_H
#define GBULLPHYS_H

#include "GStructs.h"
#include "btBulletDynamicsCommon.h"

#ifdef _DEBUG
  #pragma comment (lib, "LinearMath_vs2010_debug.lib")
  #pragma comment (lib, "BulletCollision_vs2010_debug.lib")
  #pragma comment (lib, "BulletDynamics_vs2010_debug.lib")
#else
  #pragma comment (lib, "LinearMath_vs2010.lib")
  #pragma comment (lib, "BulletCollision_vs2010.lib")
  #pragma comment (lib, "BulletDynamics_vs2010.lib")
#endif


#define G_BF_KINEMATIC             1
#define G_BF_DISABLE_DEACTIVATION  2

class G_BulletPhysics;

class G_IndexVertexArray
{
  friend class G_BulletPhysics;
public:  
  btTriangleIndexVertexArray* IV_Array;

  void AddMesh(g_int32 uid, g_uint32 *IndicesList, g_int32 nIndices, g_vec3 *VerticesPosList, g_int32 nVertices);
  void ChangeMesh(g_int32 uid, g_uint32 *IndicesList, g_int32 nIndices, g_vec3 *VerticesPosList, g_int32 nVertices);
  //void RemoveMesh(g_int32 uid);
  g_int32 GetMeshIndex(g_int32 uid);

  G_IndexVertexArray();
  ~G_IndexVertexArray();
private:
  g_int32 _id;
  std::vector <g_int32> _meshesid;
};

class G_BulletPhysics
{
public:
  btAlignedObjectArray <btCollisionShape*>   CollisionShapes;
  std::vector <G_IndexVertexArray*>          IndexVertexArrays;

  btDynamicsWorld                            *DynamicsWorld;

  G_IndexVertexArray* AddIndexVertexArray(g_int32 uid);
  G_IndexVertexArray* GetIndexVertexArray(g_int32 uid);
  
  btCollisionShape* AddBoxShape(g_vec3 &HalfSize);
  btCollisionShape* AddSphereShape(g_float &Radius);
  btCollisionShape* AddTriangleMeshShape(G_IndexVertexArray* G_IVArray);

  void AddBody(btCollisionShape* shape, g_vec3 &position, g_float mass, g_int32 flag);

  void Init();
  void Release();

  //Конструктор и деструктор
  G_BulletPhysics(void);
  ~G_BulletPhysics(void);
private:
  g_bool                                    _initialized;
  
  btDefaultCollisionConfiguration           *_collision_config;
  btCollisionDispatcher                     *_collision_dispatcher;
  btBroadphaseInterface                     *_broadphase;
  btConstraintSolver                        *_solver;
};

#endif //GBULLPHYS_H