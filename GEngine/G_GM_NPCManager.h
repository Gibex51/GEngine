#pragma once

#ifndef GGM_NPCMGR_H
#define GGM_NPCMGR_H

#include "funcs.h"
#include "GCubeTerrain.h"
#include "G_GM_Hero.h"

//-----------------------------------------------
// GMP Nodes ID

#define NODE_NPC_HEADER         0xF000
#define NODE_NPC_VER            0xF001
#define NODE_NPC_NEXTID         0xF002

#define NODE_NPC_DATA           0xF100
#define NODE_NPC_NPCNODE        0xF110
#define NODE_NPC_NAME           0xF111
#define NODE_NPC_ID             0xF112
#define NODE_NPC_SINGLE         0xF113
#define NODE_NPC_PRMBUF         0xF114
#define NODE_NPC_MODELIND       0xF115
#define NODE_NPC_TEXIND         0xF116

#define NODE_MODEL_DATA         0xF200
#define NODE_MODEL_MODELNODE    0xF210
#define NODE_MODEL_NAME         0xF211
#define NODE_MODEL_FILENAME     0xF212
#define NODE_MODEL_TEXPACKNODE  0xF213
#define NODE_MODEL_TEXTYPE      0xF214
#define NODE_MODEL_TEXRESID     0xF215
#define NODE_MODEL_TEXNODE      0xF216
#define NODE_MODEL_TEXFILENAME  0xF217

#define WNPC_VERSION            0x01

//-----------------------------------------------

struct G_NPC_TEX
{
  g_bool    IsLoaded() {};

  GTexture* GetTexture() {};

  void      Load() {};
  void      UnLoad() {};
private:
  g_int32   _tex_uid;
  g_string  _path; 

  GResourceManager  *_resmgr;
};

//-----------------------------------------------

struct G_NPC_MODEL
{
  g_bool    IsLoaded() {};

  GModel*   GetModel() {};

  void      Load() {};
  void      UnLoad() {};
private:
  g_int32   _model_uid;
  g_string  _path;
  vector <G_NPC_TEX> _textures;

  GResourceManager  *_resmgr;
};

//-----------------------------------------------

class G_GM_NPCManager
{
public:

  void Release();
  //Конструктор и деструктор
  G_GM_NPCManager(GD3DDevice *Device, GCubeTerrain *CubeTerrain);
  ~G_GM_NPCManager(void);
private:
  vector <G_NPC_DATA*>   _npc_data;
  vector <GModel*>       _models;

  //
  GCubeTerrain         *_cubeterr;
  GD3DDevice           *_device;

  void   _read_npc_chunk(G_MYCHUNKNODE *npc_chunk);
  void   _read_model_chunk(G_MYCHUNKNODE *model_chunk);
  void   _read_texpack_chunk(G_MYCHUNKNODE *texpack_chunk, GModel *model);
  void   _read_tex_chunk(G_MYCHUNKNODE *tex_chunk, vector <GTexture*> &texpack); 

  g_bool _load_data(const g_string &filename);
};

#endif //GGM_NPCMGR_H