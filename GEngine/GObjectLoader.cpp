#include "GObjectLoader.h"

//-----------------------------------------------

GObjectLoader::GObjectLoader()
{
  _loaded = false;
}

//-----------------------------------------------

GObjectLoader::~GObjectLoader()
{
  ClearBuffers();
}

//-----------------------------------------------

void GObjectLoader::ClearBuffers()
{
  _loaded = false;

  for (g_uint32 i = 0; i<_objects.size(); i++)
    delete _objects[i];
  _objects.clear();

  GMP_ClearTree();
}

//-----------------------------------------------

g_bool GObjectLoader::G3DS_ReadChunk(g_char *chk_buffer, g_uint32 chk_buf_sz, g_uint32 chk_offset, G_CHUNK *chunk)
{
  if (chk_buffer == NULL)
    return false;

  if (chk_buf_sz < chk_offset + 6)
    return false;

  memcpy(&chunk->chunk_id, &chk_buffer[chk_offset], 2);
  memcpy(&chunk->chunk_sz, &chk_buffer[chk_offset + 2], 4);

  g_int32 offset = 6;

  if (chunk->chunk_id == CHK_EDIT_OBJECT)
  {
    chunk->chunk_name = &chk_buffer[chk_offset + 6];
    g_int32 namelen = strlen(chunk->chunk_name) + 1;    
    offset += namelen;
  }
  else
    chunk->chunk_name = NULL;

  if (chk_buf_sz < chk_offset + chunk->chunk_sz)
    return false;

  chunk->chunk_sz -= offset;

  chunk->chunk_data = &chk_buffer[chk_offset + offset]; //Передается только указатель на данные

  return true;
}

//-----------------------------------------------

g_bool GObjectLoader::G3DS_ReadSubChunks(G_CHUNK *chunk)
{
  g_uint32 processed = 0;

  while (processed < chunk->chunk_sz)
  {
    G_CHUNK sub_chunk;

    if (!G3DS_ReadChunk(chunk->chunk_data,chunk->chunk_sz,processed,&sub_chunk))
      return false;
              
    processed += sub_chunk.chunk_sz + 6;

    if (sub_chunk.chunk_name != NULL) 
      processed += strlen(sub_chunk.chunk_name) + 1;
              
    if (!G3DS_ProcessChunk(&sub_chunk))
      return false;             
  }

  return true;
}

//-----------------------------------------------

g_bool GObjectLoader::G3DS_ProcessVertices(G_CHUNK *chunk)
{
  g_uint16 vert_count = 0;
  G_3D_OBJ *cobj = _objects[_objects.size() - 1];
  memcpy(&vert_count,&chunk->chunk_data[0],sizeof(vert_count));

  cobj->_nvertices = vert_count;

  if (g_uint32(vert_count*12+2) > chunk->chunk_sz)
    return false;

  if (cobj->_vertices == NULL)
    cobj->_vertices = new G_VERTEX[vert_count];

  for (g_uint32 i = 0; i<vert_count; i++)
  {
    memcpy(&((G_VERTEX*)cobj->_vertices)[i].Pos,&chunk->chunk_data[i*12+2],12); 
	_xzy_to_xyz(((G_VERTEX*)cobj->_vertices)[i].Pos); 
    ((G_VERTEX*)cobj->_vertices)[i].Color = g_vec4(1,1,1,1); // may be later ...
  }

  return true;
}

//-----------------------------------------------

g_bool GObjectLoader::G3DS_ProcessMapping(G_CHUNK *chunk)
{
  g_uint16 vert_count = 0;
  G_3D_OBJ *cobj = _objects[_objects.size() - 1];
  memcpy(&vert_count,&chunk->chunk_data[0],sizeof(vert_count));

  if (g_uint32(vert_count*8+2) > chunk->chunk_sz)
    return false;

  if (cobj->_vertices == NULL)
    cobj->_vertices = new G_VERTEX[vert_count];

  for (g_uint32 i = 0; i<vert_count; i++)
  {
    memcpy(&((G_VERTEX*)cobj->_vertices)[i].Tex,&chunk->chunk_data[i*8+2],8); 

    ((G_VERTEX*)cobj->_vertices)[i].Tex.y = - ((G_VERTEX*)cobj->_vertices)[i].Tex.y; /* ХЗ почему */
  }

  return true;
}

//-----------------------------------------------

g_bool GObjectLoader::G3DS_ProcessIndices(G_CHUNK *chunk)
{
  g_uint16 face_count = 0;
  G_3D_OBJ *cobj = _objects[_objects.size() - 1]; 
  memcpy(&face_count,&chunk->chunk_data[0],sizeof(face_count));
  
  cobj->_nindices = face_count*3;

  if (g_uint32(face_count*8+2) > chunk->chunk_sz)
    return false;

  if (cobj->_indices == NULL)
    cobj->_indices = new g_uint32[face_count*3];

  for (g_uint32 i = 0; i<face_count; i++)
  {
    g_uint16 tempind = 0;
    memcpy(&tempind,&chunk->chunk_data[i*8+2],2); 
    cobj->_indices[i*3+0] = tempind;
    memcpy(&tempind,&chunk->chunk_data[i*8+4],2); 
    cobj->_indices[i*3+1] = tempind;
    memcpy(&tempind,&chunk->chunk_data[i*8+6],2); 
    cobj->_indices[i*3+2] = tempind;
  }

  return true;
}

//-----------------------------------------------

g_bool GObjectLoader::G3DS_ProcessChunk(G_CHUNK *chunk)
{
  if (chunk != NULL)
  {
    switch (chunk->chunk_id)
    {
      case CHK_MAIN:
      case CHK_EDIT3DS:
      case CHK_OBJ_TRIMESH:
          {
            if (!G3DS_ReadSubChunks(chunk))
              return false;
          }
          break; 
      case CHK_EDIT_OBJECT:
          {
            //Add New Object
            G_3D_OBJ *NewObject = new G_3D_OBJ;
            _objects.push_back(NewObject);

            if (!G3DS_ReadSubChunks(chunk))
              return false;
          }
          break;
      case CHK_TRI_VERTEXL:
          {
            if (!G3DS_ProcessVertices(chunk))
              return false;
          }
          break;
      case CHK_TRI_MAPPINGCOORS:
          {
            if (!G3DS_ProcessMapping(chunk))
              return false;
          }
          break;
      case CHK_TRI_FACEL1:
          {
            if (!G3DS_ProcessIndices(chunk))
              return false;
          }
          break;
      default:
          break;
    }
  }
  else
    return false;

  return true;
}

//-----------------------------------------------

g_int32 GObjectLoader::Load3DSFromFile(g_char *FileName)
{
  FILE     *file_3ds = NULL;
  g_char   *chk_buf = NULL;
  G_CHUNK  main_chunk;

  try
  {
    if (fopen_s(&file_3ds, FileName, "rb") == 0)
    {
      fseek(file_3ds, 0, SEEK_END);
      g_long f_size = ftell(file_3ds);
      fseek(file_3ds, 0, SEEK_SET);

      if (f_size > MAX_3DS_FILE_SIZE)
        throw E3DS_FILE_TOO_LARGE;

      chk_buf = new g_char[f_size];

      fread(chk_buf, 1, f_size, file_3ds);

      ClearBuffers();

      //Чтение основного чанка
      if (G3DS_ReadChunk(chk_buf, f_size, 0, &main_chunk))
      {
        if (main_chunk.chunk_id != CHK_MAIN)
          throw E3DS_NOT_3DS;

        //Чтение остальных чанков
        if (!G3DS_ProcessChunk(&main_chunk))
          throw E3DS_INVALID_CHUNK;

        _loaded = true;
      }
      else
        throw E3DS_INVALID_CHUNK;
    }
    else
      throw E3DS_FILE_OPEN_ERROR;

    throw E3DS_OK;
  }
  catch(g_int32 e)
  {
    //Освобождение памяти
    if (file_3ds != NULL) fclose(file_3ds);
    if (chk_buf != NULL) delete[]chk_buf;
    return e;
  }

  return E3DS_UNKNOWN;
}

//-----------------------------------------------

void GObjectLoader::_xzy_to_xyz(g_vec3 &vec)
{
  swap_var(vec.y, vec.z);
}

//-----------------------------------------------

G_3D_OBJ* GObjectLoader::GetUniObject(g_uint32 obj_index)
{
  if (!_loaded) 
    return false;

  if (obj_index < _objects.size())
    return _objects[obj_index];

  return NULL;
}

//-----------------------------------------------
/*
g_bool GObjectLoader::SetToMesh(GMesh *Mesh, g_uint32 obj_index)
{
  if (!_loaded) 
    return false;

  if (obj_index >= _objects.size())
    return false;
  
  if ((_objects[obj_index]->_nvertices == 0) || (_objects[obj_index]->_nindices == 0))
    return false;

  if (!Mesh->AddVertexArray(_objects[obj_index]->_vertices,_objects[obj_index]->_nvertices))
    return false;

  if (!Mesh->AddIndexArray(_objects[obj_index]->_indices,_objects[obj_index]->_nindices))
    return false;

  if (_objects[obj_index]->_nanimations > 0)
  {
	for (g_uint32 i = 0; i < _objects[obj_index]->_nanimations; i++)
	{
      GBoneAnimation* NewAnim = Mesh->AddBoneAnimation(false, 0);
	  NewAnim->SetLength(_objects[obj_index]->_animations[i].GetLength());
	  NewAnim->AssignFrames(_objects[obj_index]->_animations[i].KeyFrames_List);
	}
  }

  return true;
}
*/
//-----------------------------------------------

g_bool GObjectLoader::SetToModel(GModel *Model, g_uint32 obj_index)
{
  if (!_loaded) 
    return false;

  if (obj_index >= _objects.size())
    return false;
  
  if ((_objects[obj_index]->_nvertices == 0) || (_objects[obj_index]->_nindices == 0))
    return false;

  if (!Model->AddVertexArray(_objects[obj_index]->_vertices,_objects[obj_index]->_nvertices))
    return false;

  if (!Model->AddIndexArray(_objects[obj_index]->_indices,_objects[obj_index]->_nindices))
    return false;
  
  Model->SetBonesCount(_objects[obj_index]->_nbones);

  if (_objects[obj_index]->_nanimations > 0)
  {
	for (g_uint32 i = 0; i < _objects[obj_index]->_nanimations; i++)
	{
      /*GBoneAnimation* NewAnim = Model->AddBoneAnimation(false, 0);
	  NewAnim->SetLength(_objects[obj_index]->_animations[i].GetLength());
	  NewAnim->AssignFrames(_objects[obj_index]->_animations[i].KeyFrames_List);*/
		
	  //Test Convert Bone Frames
	  vector <G_BONE_FRAME*> NewBoneFrames;
	  for (g_uint32 j = 0; j < _objects[obj_index]->_animations[i].KeyFrames_List.size(); j++)
	  {
        G_BONE_FRAME* bf = new G_BONE_FRAME();
		bf->_time = _objects[obj_index]->_animations[i].KeyFrames_List[j]->_time;
		for (g_uint32 k = 0; k < _objects[obj_index]->_animations[i].KeyFrames_List[j]->Bones.size(); k++)
		{		  
		  G_BONE_SET bs;
		  bs.Orientation = _objects[obj_index]->_animations[i].KeyFrames_List[j]->Bones[k].Orientation;
		  bs.Position = _objects[obj_index]->_animations[i].KeyFrames_List[j]->Bones[k].Position;
		  bf->Bones.push_back(bs);
		}
		NewBoneFrames.push_back(bf);
	  }

	  GBoneAnimation* NewAnim = (GBoneAnimation*)Model->AddAnimation(i, G_AT_BONE);
	  NewAnim->SetLength(_objects[obj_index]->_animations[i].GetLength());
	  NewAnim->AssignFrames(NewBoneFrames);

	  //Test
	  for (g_uint32 j = 0; j < NewBoneFrames.size(); j++)
		delete NewBoneFrames[j];
	  NewBoneFrames.clear();
	}
  }

  return true;
}

//-----------------------------------------------

g_int32 GObjectLoader::LoadGMPFile(const g_char *FileName, G_MYCHUNKNODE **out_GMP_Tree)
{
  if (!GMPFS.Create(FileName, G_FSMODE_READ))
    return EGMP_FILE_OPEN_ERROR;

  g_int32 result = EGMP_OK;

  GMP_ClearTree();

  if (GMP_ReadChunk(&_gmptree) == 0)
    result = EGMP_INVALID_CHUNK;

  if (_gmptree.Chunk.chunk_id != GMP_NODE_ROOT)
  {
    GMP_ClearTree();
    result = EGMP_NOT_GMP;
  }

  *out_GMP_Tree = &_gmptree;

  GMPFS.Release();
  return result;
}

//-----------------------------------------------

g_int32 GObjectLoader::SaveGMPFile(const g_char *FileName)
{
  if (!GMPFS.Create(FileName, G_FSMODE_WRITE))
    return EGMP_FILE_OPEN_ERROR;

  g_int32 result = EGMP_OK;
  GMP_CheckNodeSize(&_gmptree);

  if (_gmptree.Chunk.chunk_id != GMP_NODE_ROOT)
    result = EGMP_NOT_GMP;
  else
    GMP_WriteChunk(&_gmptree);

  GMPFS.Release();
  return result;
}

//-----------------------------------------------

g_int32 GObjectLoader::GMP_ReadChunk(G_MYCHUNKNODE *node)
{
  GMPFS.Read(&node->Chunk.chunk_id,2);
  GMPFS.Read(&node->Chunk.chunk_type,1);
  g_uint32 result = 3;

  switch (node->Chunk.chunk_type)
  {
    case GMP_CT_NODE:
    case GMP_CT_BUF:{
                      GMPFS.Read(&node->Chunk.chunk_size,4);
                      result += 4;
                    }
                    break;
    case GMP_CT_BYTE:
    case GMP_CT_BOOL:  node->Chunk.chunk_size = 1;
                       break;
    case GMP_CT_WORD:  node->Chunk.chunk_size = 2;
                       break;
    case GMP_CT_DWORD: node->Chunk.chunk_size = 4;
                       break;
    case GMP_CT_QWORD: node->Chunk.chunk_size = 8;
                       break;
    default: return 0;
  }

  result += node->Chunk.chunk_size;

  if (node->Chunk.chunk_type == GMP_CT_NODE) 
  {
    g_uint32 dataleft = node->Chunk.chunk_size;
    node->Childs.clear();

    while (dataleft > 0)
    {
      G_MYCHUNKNODE *newnode = new G_MYCHUNKNODE;
      g_uint32 read = GMP_ReadChunk(newnode);
      if (read > 0)
      {
        if (read > dataleft)
          dataleft = 0;
        else
          dataleft -= read;
      }
      else
      {
        delete newnode;
        return 0;
      }
      node->Childs.push_back(newnode);
    }
  }
  else
  {
    node->Chunk.chunk_data = new g_uint8[node->Chunk.chunk_size];
    GMPFS.Read(&node->Chunk.chunk_data[0],node->Chunk.chunk_size);
  }

  return result;
}

//-----------------------------------------------

void GObjectLoader::GMP_WriteChunk(G_MYCHUNKNODE *node)
{
  GMPFS.Write(&node->Chunk.chunk_id,2);
  GMPFS.Write(&node->Chunk.chunk_type,1);
  
  if ((node->Chunk.chunk_type == GMP_CT_NODE) ||
     (node->Chunk.chunk_type == GMP_CT_BUF))
    GMPFS.Write(&node->Chunk.chunk_size,4);
 
  if (node->Chunk.chunk_type == GMP_CT_NODE)
  {
    for (g_uint32 i = 0; i<node->Childs.size(); i++)
      GMP_WriteChunk(node->Childs[i]);
  }
  else
    GMPFS.Write(&node->Chunk.chunk_data[0],node->Chunk.chunk_size);
}

//-----------------------------------------------

void GObjectLoader::GMP_ClearSubNode(G_MYCHUNKNODE *node)
{
  if (node->Chunk.chunk_data != NULL)
    delete[]node->Chunk.chunk_data;

  for (g_uint32 i = 0; i<node->Childs.size(); i++)
  {
    GMP_ClearSubNode(node->Childs[i]);
    delete node->Childs[i];
  }
  node->Childs.clear();
}

//-----------------------------------------------

void GObjectLoader::GMP_ClearTree()
{
  if (_gmptree.Chunk.chunk_data != NULL)
    delete[]_gmptree.Chunk.chunk_data;
  _gmptree.Chunk.chunk_data = NULL;

  for (g_uint32 i = 0; i<_gmptree.Childs.size(); i++)
  {
    GMP_ClearSubNode(_gmptree.Childs[i]);
    delete _gmptree.Childs[i];
  }
  _gmptree.Childs.clear();
}

//-----------------------------------------------

g_int32 GObjectLoader::GMP_CheckNodeSize(G_MYCHUNKNODE *node)
{
  g_int32 result = 3;
  if ((node->Chunk.chunk_type == GMP_CT_NODE) ||
      (node->Chunk.chunk_type == GMP_CT_BUF))
    result += 4;

  if (node->Chunk.chunk_type == GMP_CT_NODE)
  {
    node->Chunk.chunk_size = 0;
    for (g_uint32 i = 0; i<node->Childs.size(); i++)
      node->Chunk.chunk_size += GMP_CheckNodeSize(node->Childs[i]);
  }

  result += node->Chunk.chunk_size;
  return result;
}

//-----------------------------------------------

g_int32 GObjectLoader::LoadBM1File(const g_char *FileName)
{
  G_MYCHUNKNODE *BM1_GMP_Tree = NULL;
  G_3D_OBJ *NewObject = new G_3D_OBJ;
  
  _objects.push_back(NewObject);

  if (LoadGMPFile(FileName, &BM1_GMP_Tree) == EGMP_OK)
  {
    if (BM1_GMP_Tree->Chunk.chunk_id == GMP_NODE_ROOT)
    {
      g_uint32 ch_len = BM1_GMP_Tree->Childs.size();
      g_uint8  ver = 0;
      //Read Header
      for (g_uint32 i = 0; i<ch_len; i++)
      {
        G_MYCHUNKNODE *SubChunk = BM1_GMP_Tree->Childs[i];
        if (SubChunk->Chunk.chunk_id == NODE_BM_HEADER)
          for (g_uint32 j = 0; j<SubChunk->Childs.size(); j++)
            switch (SubChunk->Childs[j]->Chunk.chunk_id)
            {
              case NODE_BM_VER: ver = SubChunk->Childs[j]->Chunk.chunk_data[0];
                                break;
              case NODE_BM_INFO: NewObject->_nvertices = *(g_uint32*)&SubChunk->Childs[j]->Chunk.chunk_data[0];
								 NewObject->_nindices = *(g_uint32*)&SubChunk->Childs[j]->Chunk.chunk_data[4];
								 NewObject->_nbones = *(g_uint32*)&SubChunk->Childs[j]->Chunk.chunk_data[8];
								 NewObject->_nanimations = *(g_uint32*)&SubChunk->Childs[j]->Chunk.chunk_data[12];
				                 NewObject->_vertices = NULL;
                                 NewObject->_indices = new g_uint32[NewObject->_nindices];
								 NewObject->_animations = new GBoneAnimation[NewObject->_nanimations];
								 break;
              default: break;
            }
      }
      //Read Data
	  g_int32 anim_cnt = 0;
      if (ver == BM1_VERSION) 
        for (g_uint32 i = 0; i<ch_len; i++)
        {
          G_MYCHUNKNODE *SubChunk = BM1_GMP_Tree->Childs[i];
		  switch (SubChunk->Chunk.chunk_id)
		  {
		    case NODE_BM_VERTICES:
                    for (g_uint32 j = 0; j<SubChunk->Childs.size(); j++)
                      BM1_ProcessVertices(SubChunk->Childs[j]);
					break;
            case NODE_BM_INDICES:
                    for (g_uint32 j = 0; j<SubChunk->Childs.size(); j++)
                      BM1_ProcessIndices(SubChunk->Childs[j]);
					break;
            case NODE_BM_ANIM:
                    for (g_uint32 j = 0; j<SubChunk->Childs.size(); j++)
					  switch (SubChunk->Childs[j]->Chunk.chunk_id) 
                      {
                        case NODE_BM_ANIM_NAME:
								NewObject->_animations[anim_cnt].SetName((g_char*)SubChunk->Childs[j]->Chunk.chunk_data);
		                        break;
                        case NODE_BM_ANIM_TOTALTIME:
								NewObject->_animations[anim_cnt].SetLength(*(g_uint32*)SubChunk->Childs[j]->Chunk.chunk_data);
		                        break;
                        case NODE_BM_ANIM_FRAME:
                                if (SubChunk->Childs[j]->Childs.size() >= 2)
								{
								  //GBoneFrame *NewFrame = NewObject->_animations[anim_cnt].AddKeyFrame(0);
								  G_BONE_FRAME *NewFrame = NewObject->_animations[anim_cnt].AddKeyFrame(0);
								  BM1_ProcessFrames(SubChunk->Childs[j]->Childs[0], &NewFrame);
								  BM1_ProcessFrames(SubChunk->Childs[j]->Childs[1], &NewFrame);
								}
		                        break;
                      }
					anim_cnt++;
					break;
		  } // switch
        }
    }
  }
  _loaded = true;
  return true;
}

//-----------------------------------------------

g_bool GObjectLoader::BM1_ProcessVertices(G_MYCHUNKNODE *chunk)
{
  if (chunk->Chunk.chunk_id != NODE_BM_VERTICESARR) 
	return false;

  G_3D_OBJ* curr_obj = _objects.back();
  curr_obj->_vertices = new g_uint8[chunk->Chunk.chunk_size];
  memcpy(curr_obj->_vertices, chunk->Chunk.chunk_data, chunk->Chunk.chunk_size);
  return true;
}

//-----------------------------------------------

g_bool GObjectLoader::BM1_ProcessIndices(G_MYCHUNKNODE *chunk)
{
  if (chunk->Chunk.chunk_id != NODE_BM_INDICESARR) 
	return false;

  G_3D_OBJ* curr_obj = _objects.back();
  memcpy(curr_obj->_indices, chunk->Chunk.chunk_data, curr_obj->_nindices*sizeof(g_uint32));
  return true;
}

//-----------------------------------------------
/*
g_bool GObjectLoader::BM1_ProcessFrames(G_MYCHUNKNODE *chunk, GBoneFrame **out_frame)
{
  G_3D_OBJ* curr_obj = _objects.back();
  switch (chunk->Chunk.chunk_id) 
  {
    case NODE_BM_ANIM_FRAMETIME:
		{
          (*out_frame)->_time = *(g_uint32*)chunk->Chunk.chunk_data;  
		}
		break;
	case NODE_BM_ANIM_FRAMEBONES:
		{
		  GBoneParams* BonesArray = (GBoneParams*)chunk->Chunk.chunk_data;
		  for (g_uint32 i = 0; i < curr_obj->_nbones; i++)
		    (*out_frame)->Bones.push_back(BonesArray[i]);
		}
		break;
  }
  return true;
}
*/
//-----------------------------------------------

g_bool GObjectLoader::BM1_ProcessFrames(G_MYCHUNKNODE *chunk, G_BONE_FRAME **out_frame)
{
  G_3D_OBJ* curr_obj = _objects.back();
  switch (chunk->Chunk.chunk_id) 
  {
    case NODE_BM_ANIM_FRAMETIME:
		{
          (*out_frame)->_time = *(g_uint32*)chunk->Chunk.chunk_data;  
		}
		break;
	case NODE_BM_ANIM_FRAMEBONES:
		{
		  G_BONE_SET* BonesArray = (G_BONE_SET*)chunk->Chunk.chunk_data;
		  for (g_uint32 i = 0; i < curr_obj->_nbones; i++)
		    (*out_frame)->Bones.push_back(BonesArray[i]);
		}
		break;
  }
  return true;
}

//-----------------------------------------------