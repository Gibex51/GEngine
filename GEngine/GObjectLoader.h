#pragma once

//#include "GMesh.h"
#include "GModel.h"

#ifndef GOBJLOADER_H
#define GOBJLOADER_H

//Max File Size
#define MAX_3DS_FILE_SIZE         0x0FA00000 // 256Mb

//Messages
#define E3DS_OK                   0
#define E3DS_FILE_OPEN_ERROR      1
#define E3DS_INVALID_CHUNK        2
#define E3DS_NOT_3DS              3
#define E3DS_FILE_TOO_LARGE       4
#define E3DS_UNKNOWN              5

#define EGMP_OK                   0
#define EGMP_FILE_OPEN_ERROR      1
#define EGMP_INVALID_CHUNK        2
#define EGMP_NOT_GMP              3

//3DS Chunk ID's
#define CHK_MAIN                  0x4D4D
#define CHK_EDIT3DS               0x3D3D
  #define CHK_EDIT_OBJECT           0x4000
    #define CHK_OBJ_TRIMESH           0x4100
      #define CHK_TRI_VERTEXL           0x4110 
        /* 
           0-1   - total vertices (g_uint16)
           2-13  - vector3d (g_vec3)
        */
      #define CHK_TRI_MAPPINGCOORS      0x4140
        /*
           0-1   - total vertices
           2-9   - uv-coords (g_vec2)
        */
      #define CHK_TRI_FACEL1            0x4120
        /* 
           0-1   - total faces (g_uint16)
           2-3   - vertex index 1 (g_uint16) 0-based
           4-5   - vertex index 2 (g_uint16)
           6-7   - vertex index 3 (g_uint16)
           8-9   - face info (binary 16 bit)
        */

//3DS Chunk Struct
struct G_CHUNK
{
  g_uint16 chunk_id;
  g_uint32 chunk_sz;
  g_char *chunk_data;
  g_char *chunk_name;

  G_CHUNK() {chunk_data = NULL; chunk_name = NULL; chunk_id = 0; chunk_sz = 0;};
};

struct G_3D_OBJ
{
  g_uint32        _nvertices;
  g_uint32        _nindices;
  g_uint32        _nbones;
  g_uint32        _nanimations;
  void            *_vertices;
  g_uint32        *_indices;
  GBoneAnimation  *_animations;

  void Release()
  {
    _nvertices = 0;
    _nindices = 0;
	_nbones = 0;
	_nanimations = 0;
    if (_vertices != NULL) 
      delete[]_vertices; 
    _vertices = NULL;
    if (_indices != NULL) 
      delete[]_indices; 
    _indices = NULL;
	if (_animations != NULL)
	{
	  for (g_uint32 i = 0; i < _nanimations; i++)
        _animations[i].Release();
	  delete[]_animations;
	}
	_animations = NULL;
  };

  G_3D_OBJ() 
  {
	_vertices = NULL; _indices = NULL; _animations = NULL;
	_nvertices = 0;_nindices = 0; _nbones = 0; _nanimations = 0;
  };
  ~G_3D_OBJ() {Release();};
};

//GMP Chunk IDs

#define GMP_NODE_ROOT 0x0DF0

//GMP Chunk Types

#define GMP_CT_NODE   0x01
#define GMP_CT_BYTE   0x02
#define GMP_CT_WORD   0x03
#define GMP_CT_DWORD  0x04
#define GMP_CT_QWORD  0x05
#define GMP_CT_BUF    0x06
#define GMP_CT_BOOL   0x07

//GMP Chunk Struct
struct G_MYCHUNK
{
  g_uint16 chunk_id;
  g_uint8  chunk_type;
  g_uint32 chunk_size;
  g_uint8  *chunk_data;

  void GetDataAsChar(g_char *out_buffer, g_int32 buf_size)
  {
    memset(out_buffer, 0, buf_size);
    memcpy_s(&out_buffer[0], buf_size, &chunk_data[0], chunk_size);              
  }

  G_MYCHUNK() {chunk_data = NULL; chunk_type = 0; chunk_id = 0; chunk_size = 0;};
};

struct G_MYCHUNKNODE
{
  G_MYCHUNK Chunk;
  vector <G_MYCHUNKNODE*> Childs;
};

//BM1 Chunk ID's
#define NODE_BM_HEADER          0x4D42
#define NODE_BM_VER             0xE000
#define NODE_BM_INFO            0xE001
#define NODE_BM_VERTICES        0xE100
#define NODE_BM_VERTICESARR     0xE101
#define NODE_BM_INDICES         0xE200
#define NODE_BM_INDICESARR      0xE201
#define NODE_BM_ANIM            0xE300
#define NODE_BM_ANIM_NAME       0xE301
#define NODE_BM_ANIM_NFRAMES    0xE302
#define NODE_BM_ANIM_TOTALTIME  0xE303
#define NODE_BM_ANIM_FRAME      0xE310
#define NODE_BM_ANIM_FRAMETIME  0xE311
#define NODE_BM_ANIM_FRAMEBONES 0xE312

#define BM1_VERSION             0x01

//-----------------------------------------------

class GObjectLoader
{
public:  
  g_int32 Load3DSFromFile(g_char *FileName);
  g_int32 LoadBM1File(const g_char *FileName);

  g_int32 LoadGMPFile(const g_char *FileName, G_MYCHUNKNODE **out_GMP_Tree);
  g_int32 SaveGMPFile(const g_char *FileName);

  G_3D_OBJ* GetUniObject(g_uint32 obj_index); 
  //g_bool SetToMesh(GMesh *Mesh, g_uint32 obj_index);
  g_bool SetToModel(GModel *Model, g_uint32 obj_index);

  void ClearBuffers();
  //Конструктор и деструктор
  GObjectLoader();
  ~GObjectLoader();
private:
  g_bool   _loaded;

  vector <G_3D_OBJ*> _objects;

  //3DS
  g_bool G3DS_ReadChunk(g_char *chk_buffer, g_uint32 chk_buf_sz, g_uint32 chk_offset, G_CHUNK *chunk);
  g_bool G3DS_ProcessChunk(G_CHUNK *chunk);

  g_bool G3DS_ReadSubChunks(G_CHUNK *chunk);
  g_bool G3DS_ProcessVertices(G_CHUNK *chunk);
  g_bool G3DS_ProcessIndices(G_CHUNK *chunk);
  g_bool G3DS_ProcessMapping(G_CHUNK *chunk);

  void _xzy_to_xyz(g_vec3 &vec);

  //GMP
  G_MYCHUNKNODE _gmptree;
  GFileStream GMPFS;

  g_int32 GMP_ReadChunk(G_MYCHUNKNODE *node);
  void    GMP_WriteChunk(G_MYCHUNKNODE *node);
  void    GMP_ClearSubNode(G_MYCHUNKNODE *node);
  void    GMP_ClearTree();
  g_int32 GMP_CheckNodeSize(G_MYCHUNKNODE *node);

  //BM1
  g_bool BM1_ProcessVertices(G_MYCHUNKNODE *chunk);
  g_bool BM1_ProcessIndices(G_MYCHUNKNODE *chunk);
  //g_bool BM1_ProcessFrames(G_MYCHUNKNODE *chunk, GBoneFrame **out_frame);
  g_bool BM1_ProcessFrames(G_MYCHUNKNODE *chunk, G_BONE_FRAME **out_frame);
};

#endif // GOBJLOADER_H