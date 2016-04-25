#include "G_BulletPhysics.h"

//-----------------------------------------------

G_BulletPhysics::G_BulletPhysics(void)
{
  _collision_config = NULL;
  _collision_dispatcher = NULL;
  _broadphase = NULL;
  _solver = NULL;
  DynamicsWorld = NULL;
  _initialized = false;
}

//-----------------------------------------------

G_BulletPhysics::~G_BulletPhysics(void)
{

}

//-----------------------------------------------

void G_BulletPhysics::Init()
{
  if (!_initialized)
  {
    //collision configuration contains default setup for memory, collision setup
    _collision_config = new btDefaultCollisionConfiguration();
  
    //use the default collision dispatcher. For parallel processing you can use 
    //a diffent dispatcher (see Extras/BulletMultiThreaded)
    _collision_dispatcher = new btCollisionDispatcher(_collision_config);

    _broadphase = new btDbvtBroadphase();

    //the default constraint solver. For parallel processing you can use 
    //a different solver (see Extras/BulletMultiThreaded)
    _solver = new btSequentialImpulseConstraintSolver();

    DynamicsWorld = new btDiscreteDynamicsWorld(_collision_dispatcher,_broadphase,_solver,_collision_config);

    DynamicsWorld->setGravity(btVector3(0,-80,0));

    _initialized = true;
  }
}

//-----------------------------------------------

void G_BulletPhysics::Release()
{
  if (_initialized)
  {
	for (g_int32 i = DynamicsWorld->getNumCollisionObjects()-1; i>=0 ; i--)
	{
	  btCollisionObject* obj = DynamicsWorld->getCollisionObjectArray()[i];
	  btRigidBody* body = btRigidBody::upcast(obj);
	  if (body && body->getMotionState())
	  {
		delete body->getMotionState();
	  }
	  DynamicsWorld->removeCollisionObject( obj );
	  delete obj;
	}
   
	for (g_int32 j=0; j<CollisionShapes.size(); j++)
	{
	  btCollisionShape* shape = CollisionShapes[j];
	  delete shape;
	}
	CollisionShapes.clear();

    for (g_uint32 j=0; j<IndexVertexArrays.size(); j++)
	{
	  delete IndexVertexArrays[j];
	}
	IndexVertexArrays.clear();

	delete DynamicsWorld; DynamicsWorld = NULL;
	delete _solver;	_solver = NULL;
	delete _broadphase; _broadphase = NULL;
	delete _collision_dispatcher; _collision_dispatcher = NULL;
	delete _collision_config; _collision_config = NULL;

    _initialized = false;
  }
}

//-----------------------------------------------

btCollisionShape* G_BulletPhysics::AddBoxShape(g_vec3 &HalfSize)
{
  btCollisionShape* NewShape = new btBoxShape(btVector3(btScalar(HalfSize.x),btScalar(HalfSize.y),btScalar(HalfSize.z)));
  CollisionShapes.push_back(NewShape);
  return NewShape;
}

//-----------------------------------------------

btCollisionShape* G_BulletPhysics::AddSphereShape(g_float &Radius)
{
  btCollisionShape* NewShape = new btSphereShape(btScalar(Radius));
  CollisionShapes.push_back(NewShape);
  return NewShape;
}

//-----------------------------------------------

btCollisionShape* G_BulletPhysics::AddTriangleMeshShape(G_IndexVertexArray* G_IVArray)
{
  btCollisionShape* NewShape = new btBvhTriangleMeshShape(G_IVArray->IV_Array,true);
  CollisionShapes.push_back(NewShape);
  return NewShape;
}

//-----------------------------------------------

G_IndexVertexArray* G_BulletPhysics::AddIndexVertexArray(g_int32 uid)
{
  G_IndexVertexArray *ind_vert_array = new G_IndexVertexArray();
  IndexVertexArrays.push_back(ind_vert_array);
  return ind_vert_array;
}

//-----------------------------------------------

G_IndexVertexArray* G_BulletPhysics::GetIndexVertexArray(g_int32 uid)
{
  for (g_int32 i = 0; i<g_int32(IndexVertexArrays.size()); i++)
    if (IndexVertexArrays[i]->_id == uid)
      return IndexVertexArrays[i];

  return NULL;
}

//-----------------------------------------------

void G_BulletPhysics::AddBody(btCollisionShape* shape, g_vec3 &position, g_float mass, g_int32 flag)
{
  btTransform BodyTransform;
  BodyTransform.setIdentity();
  BodyTransform.setOrigin(btVector3(btScalar(position.x),btScalar(position.y),btScalar(position.z)));

  //rigidbody is dynamic if and only if mass is non zero, otherwise static
  g_bool isDynamic = (mass != 0.f);

  btVector3 localInertia(0,0,0);
  if (isDynamic)
	shape->calculateLocalInertia(mass,localInertia);

  //using motionstate is recommended, it provides interpolation capabilities, 
  //and only synchronizes 'active' objects
  btDefaultMotionState* myMotionState = new btDefaultMotionState(BodyTransform);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,shape,localInertia);
  btRigidBody* body = new btRigidBody(rbInfo);

  if ((flag & G_BF_KINEMATIC) == G_BF_KINEMATIC)
    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

  if ((flag & G_BF_DISABLE_DEACTIVATION) == G_BF_DISABLE_DEACTIVATION)
    body->setActivationState(DISABLE_DEACTIVATION);

  //add the body to the dynamics world
  DynamicsWorld->addRigidBody(body);
}

//-----------------------------------------------
// G_IndexVertexArray
//-----------------------------------------------

G_IndexVertexArray::G_IndexVertexArray()
{
  IV_Array = new btTriangleIndexVertexArray();
  _id = 0;
}

//-----------------------------------------------

G_IndexVertexArray::~G_IndexVertexArray()
{
  delete IV_Array;
  _meshesid.clear();
}

//-----------------------------------------------

g_int32 G_IndexVertexArray::GetMeshIndex(g_int32 uid)
{
  for (g_int32 i = 0; i<g_int32(_meshesid.size()); i++)
    if (_meshesid[i] == uid)
      return i;

  return -1;
}

//-----------------------------------------------

void G_IndexVertexArray::AddMesh(g_int32 uid, g_uint32 *IndicesList, g_int32 nIndices, g_vec3 *VerticesPosList, g_int32 nVertices)
{
  btIndexedMesh IMesh;

  IMesh.m_numTriangles = nIndices/3;
  IMesh.m_triangleIndexBase = (const unsigned char*)IndicesList;
  IMesh.m_triangleIndexStride = sizeof(g_uint32);
  IMesh.m_numVertices = nVertices;
  IMesh.m_vertexBase = (const unsigned char*)VerticesPosList;
  IMesh.m_vertexStride = sizeof(g_vec3);

  IV_Array->addIndexedMesh(IMesh);

  _meshesid.push_back(uid);
}

//-----------------------------------------------

void G_IndexVertexArray::ChangeMesh(g_int32 uid, g_uint32 *IndicesList, g_int32 nIndices, g_vec3 *VerticesPosList, g_int32 nVertices)
{
  g_int32 ind = GetMeshIndex(uid);
  if (ind < 0)
    return;

  btIndexedMesh IMesh;

  IMesh.m_numTriangles = nIndices/3;
  IMesh.m_triangleIndexBase = (const unsigned char*)IndicesList;
  IMesh.m_triangleIndexStride = sizeof(g_uint32);
  IMesh.m_numVertices = nVertices;
  IMesh.m_vertexBase = (const unsigned char*)VerticesPosList;
  IMesh.m_vertexStride = sizeof(g_vec3);

  IV_Array->getIndexedMeshArray()[ind] = IMesh;
}

//-----------------------------------------------