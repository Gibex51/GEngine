#include "GMeshObject.h"

//-----------------------------------------------

GMeshObject::GMeshObject(const g_string &Name, GD3DDevice *Device):GBaseObject(Name,Device)
{
  _vertices = NULL;
  _indices = NULL;
  _nindices = 0;
  _nvertices = 0;
  _vertsize = 0;

  _ib = NULL;
  _vb = NULL;
  _il = NULL;
}

//-----------------------------------------------

GMeshObject::~GMeshObject(void)
{
  ReleaseMesh();
}

//-----------------------------------------------

g_bool GMeshObject::AddIndexArray(g_uint32 *IndexArray, g_uint32 IndexCount)
{
  if ((IndexCount == 0) || (IndexArray == NULL)) 
	return false;

  g_uint32 *new_buff = new g_uint32[_nindices+IndexCount];

  if (_indices != NULL)
  {
    memcpy(new_buff, _indices, _nindices*4);
    delete[]_indices;
  }

  memcpy(&new_buff[_nindices], IndexArray, IndexCount*4);
  _nindices += IndexCount;
  _indices = new_buff;

  return true;
}

//-----------------------------------------------

g_bool GMeshObject::AddVertexArray(void *VertexArray, g_uint32 VertexCount)
{
  if ((VertexCount == 0) || (VertexArray == NULL)) 
	return false;
  
  g_uint8 *new_buff = new g_uint8[(_nvertices+VertexCount)*_vertsize];

  if (_vertices != NULL)
  {
    memcpy(new_buff, _vertices, _nvertices*_vertsize);
    delete[]_vertices;
  }

  memcpy(&new_buff[_nvertices*_vertsize], VertexArray, VertexCount*_vertsize);
  _nvertices += VertexCount;
  _vertices = new_buff;

  return true;
}

//-----------------------------------------------

void GMeshObject::ClearVIArrays()
{
  FREENULL_ARR(_vertices);
  FREENULL_ARR(_indices);
  _nvertices = 0;
  _nindices = 0;
}

//-----------------------------------------------

void GMeshObject::ReleaseMesh()
{
  _vertsize = 0;
  ClearVIArrays();
  DeleteVIBuffers();
}

//-----------------------------------------------

void GMeshObject::InitMesh(g_uint16 VertexSize, g_uint8 PrimitiveTopology, g_uint32 InputLayoutUID)
{
  _vertsize = VertexSize;
  _prim_topology = PrimitiveTopology;
  _il = _device->ILayoutList->GetInputLayout(InputLayoutUID);
}

//-----------------------------------------------

void GMeshObject::InitMesh(g_uint16 VertexSize, g_uint8 PrimitiveTopology, GInputLayout *InputLayout)
{
  _vertsize = VertexSize;
  _prim_topology = PrimitiveTopology;
  _il = InputLayout;
}

//-----------------------------------------------

g_bool GMeshObject::CreateVIBuffers()
{  
  D3D10_BUFFER_DESC bd;
  bd.Usage = D3D10_USAGE_DEFAULT;
  bd.ByteWidth = _vertsize * _nvertices;
  bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
  bd.CPUAccessFlags = 0;
  bd.MiscFlags = 0;
  
  D3D10_SUBRESOURCE_DATA InitData;
  InitData.pSysMem = _vertices;

  DeleteVIBuffers();

  if((_nvertices == 0) || FAILED(_device->D3DDevice->CreateBuffer(&bd,&InitData,&_vb)))
    return false;

  bd.ByteWidth = sizeof(g_uint32) * _nindices;
  bd.BindFlags = D3D10_BIND_INDEX_BUFFER;

  InitData.pSysMem = _indices;

  if((_nindices == 0) || FAILED(_device->D3DDevice->CreateBuffer(&bd,&InitData,&_ib)))
    return false;

  return true;
}

//-----------------------------------------------

void GMeshObject::DeleteVIBuffers()
{
  NULLRELEASE(_vb);
  NULLRELEASE(_ib);
}

//-----------------------------------------------

void GMeshObject::SetVIBuffers()
{
  g_uint32 stride = _vertsize;
  g_uint32 offset = 0;
  _device->D3DDevice->IASetVertexBuffers(0, 1, &_vb, &stride, &offset);
  _device->D3DDevice->IASetIndexBuffer(_ib, DXGI_FORMAT_R32_UINT, 0);
}

//-----------------------------------------------

g_bool GMeshObject::IsVIBuffersCreated()
{
  return ((_vb != NULL) && (_ib != NULL));
}

//-----------------------------------------------

void GMeshObject::GetVertexArray(void **pVertexArray, g_uint32 &VertexCount)
{
  *pVertexArray = _vertices;
  VertexCount = _nvertices;
}

//-----------------------------------------------

void GMeshObject::GetIndexArray(g_uint32 **pIndexArray, g_uint32 &IndexCount)
{
  *pIndexArray = _indices;
  IndexCount = _nindices;
}

//-----------------------------------------------

void GMeshObject::UpdateVB(g_int32 offset, g_int32 num)
{
  //l,t,f,r,bt,bk
  D3D10_BOX b = {_vertsize*offset,0,0,_vertsize*(offset+num),1,1};
  _device->D3DDevice->UpdateSubresource(_vb, 0, &b, &_vertices[offset*_vertsize], _vertsize, 0);
}

//-----------------------------------------------

void GMeshObject::UpdateIB(g_int32 offset, g_int32 num)
{
  D3D10_BOX b = {sizeof(g_uint32)*offset,0,0,sizeof(g_uint32)*(offset+num),1,1};
  _device->D3DDevice->UpdateSubresource(_ib, 0, &b, &_indices[offset], sizeof(g_uint32), 0);
}

//-----------------------------------------------