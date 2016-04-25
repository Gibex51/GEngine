#pragma once

#include "FG_GM_Funcs.h"

#ifndef GFABUILDER_H
#define GFABUILDER_H

using namespace std;

#define FB_ID 11000

// Element Size Defines //
#define fa_elemtype g_uint32
const g_int32 bufelemsize = sizeof(fa_elemtype);
const DXGI_FORMAT elemformat = DXGI_FORMAT_R32_UINT;
// -------------------- //

struct GOffset
{
  g_uint16 index;
  g_uint32 offset;
  g_uint32 size;
  fa_elemtype *facearray;
};

class GFaceArrayBuilder
{
public:
  fa_elemtype *BuildedFaceArray;
  vector <GOffset*> Offsets;

  GTexture             *fa_buffer;
  ID3D10EffectVariable *fa_offset;

  GOffset* AddOffset(fa_elemtype *facebuffer, g_uint32 fbsize);
  void     RemoveOffset(g_uint16 index);
  void     MoveEndOffset(g_uint16 index);
  void     UpdateOffset(g_uint16 index, fa_elemtype *facebuffer, g_uint32 fbsize);
  void     ClearOffsets();
  void     Build();
  void     BuildChanges();
  g_uint32 GetFaceArraySize();

  void     SetEffectVariable(ID3D10EffectVariable *EV) {fa_offset = EV;};
  void     SetOffsetToShader(g_uint16 oindex);
  void     UpdateOffsetInShader(g_uint32 min, g_uint32 max);

  //Конструктор и деструктор
  GFaceArrayBuilder(g_int32 FaceArraySize, GD3DDevice *Device);
  ~GFaceArrayBuilder();
private:
  GD3DDevice *_device;
  ID3D10Resource *fa_res;

  g_uint32 _max_fa_size;
  g_uint16 _min_chg_index;
  g_uint16 _max_chg_index;
  g_bool   _changed;
};

#endif //GFABUILDER_H