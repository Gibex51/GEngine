#include "G_GM_NPCMgr.h"

//-----------------------------------------------

g_uint32 defnpc_layout_elements = 5;

/*
D3D10_INPUT_ELEMENT_DESC defnpc_vertex_layout[] =
{
  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 28, D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 36, D3D10_INPUT_PER_VERTEX_DATA, 0 }
};*/

D3D10_INPUT_ELEMENT_DESC defnpc_vertex_layout[] =
{
  { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,       0, 24,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "WEIGHTS",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "BONES",      0, DXGI_FORMAT_R32G32B32A32_UINT,  0, 48,  D3D10_INPUT_PER_VERTEX_DATA, 0 }
};

//-----------------------------------------------

G_GM_NPCMgr::G_GM_NPCMgr(GD3DDevice *Device, GCubeTerrain *pCubeTerrain, GScene *pScene)
{
  busy = false;

  _pCT = pCubeTerrain;
  _pScene = pScene;
  _pDevice = Device;
  _hero = NULL;
  _nullnpc = new G_GM_Unit("NullNPC",1);

  CurrUID = UID_MESH_BASENPC;

  GEffectPool *_pDefPool = _pDevice->ResMgr->GetEffectPoolByUID(UID_EP_DEFAULT);

  GEffect *_pEffect = _pDevice->ResMgr->AddEffect(UID_FX_BASENPC, "Def_NPC_Effect", "..\\Shaders\\TestModel.fx", _pDefPool);
  _pDefMater = _pDevice->ResMgr->AddMaterial(UID_MAT_DEFNPC,"Def_NPC_Material",_pEffect);

  _pTexture = new GTexture("1234", "..\\Textures\\NakMan.dds", Device);
  _pNormTexture = new GTexture("2234", "..\\Textures\\NakManNorm.dds", Device);

  _pDevice->ILayoutList->AddInputLayout(UID_IL_NPC1, _pEffect->GetDXEffect()->GetTechniqueByName("Render"),
	                                    defnpc_vertex_layout, defnpc_layout_elements);

  //_pDefMater->TexList.push_back(new GTexItem(_pScene->AddTexture(UID_TEX_DEFNPC1, "Def_NPC_Tex1","..\\Textures\\TF.jpg"),0));
  //_pDefMater->TexList.push_back(new GTexItem(_pScene->AddTexture(UID_TEX_DEFNPC2, "Def_NPC_Tex2","..\\Textures\\TFC.jpg"),2));
}

//-----------------------------------------------

G_GM_NPCMgr::~G_GM_NPCMgr(void)
{
  delete _nullnpc;
  delete _pTexture;
  delete _pNormTexture;
}

//-----------------------------------------------

void G_GM_NPCMgr::Read_NPC_Chunk(G_MYCHUNKNODE *NPCChunk, g_bool IsHero)
{
  if (NPCChunk->Chunk.chunk_id == NODE_NPC_NPCNODE)
  {
    G_GM_Unit* NewNPC = NULL;
    if (IsHero)
    {
      _hero = new G_GM_Hero("HeroNPC",100);
      NewNPC = _hero;
    }
    else
      NewNPC = new G_GM_Unit("DefaultNPC",100);

    ////////////////////
    /* Test Mesh */
    g_char MeshName[G_MAX_NAME_LEN];
    sprintf_s(MeshName,G_MAX_NAME_LEN,"NPC_Mesh_%d/0",_allunits.size());

	GObjectLoader OLo;
    OLo.LoadBM1File("..\\Data\\ObjectsBM1\\new.bm1");

	GModel *npc_model = _pDevice->ResMgr->AddModel(CurrUID, MeshName, _pDefMater, 0, sizeof(G_BONE_LVERTEX), UID_IL_NPC1);
	_models.push_back(npc_model);

	OLo.SetToModel(npc_model, 0);
    npc_model->CreateVIBuffers();

	GMeshInstance* curr_inst = npc_model->AddInstance();
    curr_inst->SetPosition(g_vec3(0,-1000,0));
    curr_inst->SetScaling(csize);
	curr_inst->Visible = false;
	curr_inst->SetTexIndex(_allunits.size());

	NewNPC->SetMeshInstance(curr_inst);
	CurrUID++;

    /* Test Mesh */
    ////////////////////

    for (g_uint32 i = 0; i<NPCChunk->Childs.size(); i++)
      switch (NPCChunk->Childs[i]->Chunk.chunk_id)
      {
        case NODE_NPC_NAME: {
                              g_char npcName[G_MAXUNITNAMELEN];
                              memset(npcName,0,G_MAXUNITNAMELEN);
                              memcpy_s(&npcName[0],G_MAXUNITNAMELEN,
                                       &NPCChunk->Childs[i]->Chunk.chunk_data[0],
                                       NPCChunk->Childs[i]->Chunk.chunk_size);                              
                              NewNPC->SetName(npcName);
                            }
                            break;
        case NODE_NPC_ID:   //memcpy(&NewNPC->_id,&NPCChunk->Childs[i]->Chunk.chunk_data[0],4);
                            break;
        default: break;
      }
    _allunits.push_back(NewNPC);
  }
}

//-----------------------------------------------

g_bool G_GM_NPCMgr::LoadNPCs(const g_string &filename)
{
  Release();
  
  GObjectLoader ObjLoader;
  G_MYCHUNKNODE *GMPTree = NULL;

  if (ObjLoader.LoadGMPFile(filename.c_str(), &GMPTree) == EGMP_OK)
  {
    if (GMPTree->Chunk.chunk_id == GMP_NODE_ROOT)
    {
      g_uint32 ch_len = GMPTree->Childs.size();
      g_uint8  ver = 0;
      //Read Header
      for (g_uint32 i = 0; i<ch_len; i++)
      {
        G_MYCHUNKNODE *SubChunk = GMPTree->Childs[i];
        if (SubChunk->Chunk.chunk_id == NODE_NPC_HEADER)
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
          if (SubChunk->Chunk.chunk_id == NODE_NPC_DATA)
            for (g_uint32 j = 0; j<SubChunk->Childs.size(); j++)
              Read_NPC_Chunk(SubChunk->Childs[j], (j==0));
        }
    }
  }
  else
    return false;

  ObjLoader.ClearBuffers();
  return true;
}

//-----------------------------------------------

G_GM_Unit* G_GM_NPCMgr::GetNPCbyIndex(g_uint32 index)
{
  if (index < _allunits.size())
  {
    return _allunits[index];
  }
  else
    return _nullnpc;
}

//-----------------------------------------------

void G_GM_NPCMgr::Draw()
{ 
  if (!busy)
  {
    for (g_uint32 i=0;i<_allunits.size();i++)
      _allunits[i]->UpdateState();

    GCamera *Camera = _pScene->GetDefaultCamera();
    Camera->SetViewport();

	_pDefMater->ApplyTexture(_pTexture, 0);
	_pDefMater->ApplyTexture(_pNormTexture, 1);
	GCamera *Light_Cam = _pDevice->ResMgr->GetCameraByUID(UID_CAM_LIGHTCAM);
	g_vec3 dir = Light_Cam->GetDirection();

	_pDefMater->Effect->GetVar(G_SVT_CUSTOM, 10)->SetRawData(dir);

    for (g_uint32 i=0;i<_models.size();i++)
    {
	  _models[i]->SetWorldMatrix(_pScene->GetWorldMatrix());
      _models[i]->SetViewProjMatrix(Camera->GetViewProjMatrix());
	  _models[i]->Draw();
    }
  }
}

//-----------------------------------------------

void G_GM_NPCMgr::DrawSM(GCamera *Camera)
{ 
  if (!busy)
  {
    for (g_uint32 i=0;i<_allunits.size();i++)
      _allunits[i]->UpdateState();

    Camera->SetViewport();

    for (g_uint32 i=0;i<_models.size();i++)
    {
	  _models[i]->SetWorldMatrix(_pScene->GetWorldMatrix());
      _models[i]->SetViewProjMatrix(Camera->GetViewProjMatrix());
	  _models[i]->Draw();
    }
  }
}

//-----------------------------------------------

void G_GM_NPCMgr::Release()
{
  busy = true;
  clear_del_vector(_allunits);

  _hero = NULL;
  _herogroup.clear();
  busy = false;
}

//-----------------------------------------------
