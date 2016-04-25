#pragma once

#ifndef GMESHOBJ_H
#define GMESHOBJ_H

#include "GBaseObject.h"

class GMeshObject :
	public GBaseObject
{
public:
	void InitMesh(g_uint16 VertexSize, g_uint8 PrimitiveTopology, g_uint32 InputLayoutUID);
	void InitMesh(g_uint16 VertexSize, g_uint8 PrimitiveTopology, GInputLayout *InputLayout);
	void ReleaseMesh();
    
	g_bool AddIndexArray(g_uint32 *IndexArray, g_uint32 IndexCount);
	g_bool AddVertexArray(void *VertexArray, g_uint32 VertexCount);
    void ClearVIArrays();

	g_uint32 GetVertexCount() {return _nvertices;};
    g_uint32 GetIndexCount()  {return _nindices;};

	g_bool CreateVIBuffers();
	g_bool IsVIBuffersCreated();
    void   DeleteVIBuffers();
    void   SetVIBuffers();	

	void   GetVertexArray(void **pVertexArray, g_uint32 &VertexCount);
	void   GetIndexArray(g_uint32 **pIndexArray, g_uint32 &IndexCount);

	GInputLayout* GetInputLayout() {return _il;};

	void   SetPrimitiveTopology(g_uint8 PrimTopology) {_prim_topology = PrimTopology;};

	void UpdateVB(g_int32 offset, g_int32 num);
	void UpdateIB(g_int32 offset, g_int32 num);

	//Конструктор и деструктор
	GMeshObject(const g_string &Name, GD3DDevice *Device);
	~GMeshObject(void);
protected:
	g_uint16   _vertsize;
	g_uint8    _prim_topology;

	g_uint32   _nvertices, _nindices;

    g_uint8    *_vertices;
    g_uint32   *_indices;

	GInputLayout *_il;

	ID3D10Buffer *_ib, *_vb;
};

#endif //GMESHOBJ_H