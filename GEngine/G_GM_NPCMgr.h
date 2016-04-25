#pragma once

#ifndef GGMNPCMGR_H
#define GGMNPCMGR_H

#include "G_GM_Hero.h"
#include "GCubeTerrain.h"
#include "GScene.h"

#define NODE_NPC_HEADER   0xF000
#define NODE_NPC_VER      0xF001
#define NODE_NPC_NEXTID   0xF002

#define NODE_NPC_DATA     0xF100
#define NODE_NPC_NPCNODE  0xF110
#define NODE_NPC_NAME     0xF111
#define NODE_NPC_ID       0xF112
#define NODE_NPC_PRMBUF   0xF113

#define WNPC_VERSION      0x01

class G_GM_NPCMgr
{
public:
  g_bool LoadNPCs(const g_string &filename);
  //SaveNPCs

  //AddNPC
  //RemoveNPC

  //GetNPCbyID
  G_GM_Unit* GetNPCbyIndex(g_uint32 index);
  //GetHero
  //GetHeroGroupByIndex

  void Draw();
  void DrawSM(GCamera *Camera);

  g_uint32 GetNPCCount() {return _allunits.size();};

  void Release();

  //Конструктор и деструктор
  G_GM_NPCMgr(GD3DDevice *Device, GCubeTerrain *pCubeTerrain, GScene *pScene);
  ~G_GM_NPCMgr(void);
private: 
  //vector <G_NPC_DATA*> _npc_data;
  vector <G_GM_Unit*>  _allunits;
  vector <G_GM_Unit*>  _herogroup;
  vector <GModel*>     _models;

  G_GM_Hero*           _hero;
  G_GM_Unit*           _nullnpc;

  GCubeTerrain         *_pCT;
  GScene               *_pScene;
  GD3DDevice           *_pDevice;
  
  GMaterial            *_pDefMater;
  GTexture             *_pTexture;
  GTexture             *_pNormTexture;

  g_bool               busy;

  g_uint32             CurrUID;

  void Read_NPC_Chunk(G_MYCHUNKNODE *NPCChunk, g_bool IsHero);
};

#endif //GGMNPCMGR_H