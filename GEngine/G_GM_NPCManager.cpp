#include "G_GM_NPCManager.h"

//-----------------------------------------------

G_GM_NPCManager::G_GM_NPCManager(GD3DDevice *Device, GCubeTerrain *CubeTerrain)
{
  _cubeterr = CubeTerrain;
  _device = Device;
}

//-----------------------------------------------

G_GM_NPCManager::~G_GM_NPCManager(void)
{
  Release();
}

//-----------------------------------------------

void G_GM_NPCManager::Release()
{
  clear_del_vector(_npc_data);
  clear_del_vector(_models);
}

//-----------------------------------------------

void G_GM_NPCManager::_read_npc_chunk(G_MYCHUNKNODE *npc_chunk)
{
  if (npc_chunk->Chunk.chunk_id != NODE_NPC_NPCNODE) 
	return;

  G_NPC_DATA *new_npc_data = new G_NPC_DATA();

  for (g_uint32 i = 0; i<npc_chunk->Childs.size(); i++)
    switch (npc_chunk->Childs[i]->Chunk.chunk_id)
    {
      case NODE_NPC_NAME: {
            g_char npcName[G_MAXUNITNAMELEN];
			npc_chunk->Childs[i]->Chunk.GetDataAsChar(npcName, G_MAXUNITNAMELEN);
			new_npc_data->Name = npcName;
            } break;
      case NODE_NPC_ID:{ 
		    memcpy(&new_npc_data->ID, &npc_chunk->Childs[i]->Chunk.chunk_data[0], 4);
			} break;
      default: break;
    }

  _npc_data.push_back(new_npc_data);
}

//-----------------------------------------------

void G_GM_NPCManager::_read_tex_chunk(G_MYCHUNKNODE *tex_chunk, vector <GTexture*> &texpack)
{
  if (tex_chunk->Chunk.chunk_id != NODE_MODEL_TEXNODE) 
	return;

  g_char  tex_filename[G_MAX_NAME_LEN] = "/0";

  for (g_uint32 i = 0; i<tex_chunk->Childs.size(); i++)
    switch (tex_chunk->Childs[i]->Chunk.chunk_id)
    {
      case NODE_MODEL_TEXFILENAME: {
		    tex_chunk->Childs[i]->Chunk.GetDataAsChar(tex_filename, G_MAX_NAME_LEN); 
			} break;
      default: break;
    }

  //texpack.push_back(_device->ResMgr->AddTexture(TEX_UID, "npc_tex", tex_filename));
}

//-----------------------------------------------

void G_GM_NPCManager::_read_texpack_chunk(G_MYCHUNKNODE *texpack_chunk, GModel *model)
{
  if (texpack_chunk->Chunk.chunk_id != NODE_MODEL_TEXPACKNODE) 
	return;

  g_int32 res_id = -1;
  g_uint8 tex_type = 0;
  vector <GTexture*> new_texpack;

  for (g_uint32 i = 0; i<texpack_chunk->Childs.size(); i++)
    switch (texpack_chunk->Childs[i]->Chunk.chunk_id)
    {
      case NODE_MODEL_TEXTYPE: {
		    tex_type = texpack_chunk->Childs[i]->Chunk.chunk_data[0]; 
			} break;
      case NODE_MODEL_TEXRESID: {
		    res_id = *(g_int32*)texpack_chunk->Childs[i]->Chunk.chunk_data; 
			} break;
      case NODE_MODEL_TEXNODE: {
		    for (g_uint32 j = 0; j<texpack_chunk->Childs.size(); j++)
              _read_tex_chunk(texpack_chunk->Childs[j], new_texpack);
			} break;
      default: break;
    }

  if (new_texpack.size() == 0) 
	return;

  switch (tex_type)
  {
    case 0: model->GetMaterial()->AddTexture(new_texpack[0], res_id); break;
    case 1: model->GetMaterial()->AddTextureArray(new_texpack, res_id); break;
  }
}

//-----------------------------------------------

void G_GM_NPCManager::_read_model_chunk(G_MYCHUNKNODE *model_chunk)
{
  if (model_chunk->Chunk.chunk_id != NODE_MODEL_MODELNODE) 
	return;

  GModel *new_model = new GModel("npc_model", _device);

  for (g_uint32 i = 0; i<model_chunk->Childs.size(); i++)
    switch (model_chunk->Childs[i]->Chunk.chunk_id)
    {
	  case NODE_MODEL_NAME: {
		    g_char model_name[G_MAXMODELNAMELEN];
			model_chunk->Childs[i]->Chunk.GetDataAsChar(model_name, G_MAXMODELNAMELEN);            
		    new_model->SetName(model_name);
			} break;
	  case NODE_MODEL_FILENAME: {
		    g_char model_filename[G_MAX_NAME_LEN];
            model_chunk->Childs[i]->Chunk.GetDataAsChar(model_filename, G_MAX_NAME_LEN);
			// Загрузка модели из файла
			} break;
	  case NODE_MODEL_TEXPACKNODE: {
		    for (g_uint32 j = 0; j<model_chunk->Childs.size(); j++)
              _read_texpack_chunk(model_chunk->Childs[j], new_model);
			} break;
      default: break;
    }

  _models.push_back(new_model);
}

//-----------------------------------------------

g_bool G_GM_NPCManager::_load_data(const g_string &filename)
{
  Release();

  GObjectLoader ObjLoader;
  G_MYCHUNKNODE *GMPTree = NULL;

  if (ObjLoader.LoadGMPFile(filename.c_str(), &GMPTree) != EGMP_OK)
	return false;
  
  if (GMPTree->Chunk.chunk_id == GMP_NODE_ROOT)
  {
    g_uint32 ch_len = GMPTree->Childs.size();
    g_uint8  ver = 0;
    //Read Header
    for (g_uint32 i = 0; i<ch_len; i++)
    {
      G_MYCHUNKNODE *SubChunk = GMPTree->Childs[i];
      if (SubChunk->Chunk.chunk_id != NODE_NPC_HEADER) continue;

      for (g_uint32 j = 0; j<SubChunk->Childs.size(); j++)
        switch (SubChunk->Childs[j]->Chunk.chunk_id)
        {
          case NODE_NPC_VER: ver = SubChunk->Childs[j]->Chunk.chunk_data[0];
                             break;
          case NODE_NPC_NEXTID: /* Не нужно? */ break;
          default: break;
        }
    }
    //Read Data
    if (ver == WNPC_VERSION) 
      for (g_uint32 i = 0; i<ch_len; i++)
      {
        G_MYCHUNKNODE *SubChunk = GMPTree->Childs[i];
		switch (SubChunk->Chunk.chunk_id)
		{
		  case NODE_NPC_DATA:{     
		                       for (g_uint32 j = 0; j<SubChunk->Childs.size(); j++)
                                 _read_npc_chunk(SubChunk->Childs[j]);
							 }
							 break;
          case NODE_MODEL_DATA:{     
		                       for (g_uint32 j = 0; j<SubChunk->Childs.size(); j++)
                                 _read_model_chunk(SubChunk->Childs[j]);
							 }
							 break;
		}
      }
  }

  return true;
}

//-----------------------------------------------
