#include "GFaceArrayBuilder.h"

//-----------------------------------------------

GFaceArrayBuilder::GFaceArrayBuilder(g_int32 MaxFaceArraySize, GD3DDevice *Device)
{
  BuildedFaceArray = new fa_elemtype[MaxFaceArraySize];
  memset(BuildedFaceArray,0,MaxFaceArraySize*bufelemsize);
  _max_fa_size = MaxFaceArraySize;
  _min_chg_index = 0;
  _max_chg_index = 0;
  _changed = false;
  _device = Device;

  G_BUFFER *fbuffer;
  fbuffer = _device->BufferList->AddBuffer(G_BT_SHADERRES,G_BF_DEFAULT,bufelemsize,
                                           _max_fa_size,BuildedFaceArray,FB_ID);

  if (fbuffer != NULL)
  {
    fa_buffer = new GTexture("fabuffer",_device);
    if (fa_buffer->CreateFromBuffer(&fbuffer->Buffer, _max_fa_size, elemformat))
    {
      fa_buffer->Texture->GetResource(&fa_res);
    }
    else
    {
      delete fa_buffer;
      fa_buffer = NULL;
      fa_res = NULL;
    }
  }
  else
  {
    fa_buffer = NULL;
    fa_res = NULL;
  }

  fa_offset = NULL;
}

//-----------------------------------------------

GFaceArrayBuilder::~GFaceArrayBuilder()
{
  delete[]BuildedFaceArray;
  if (fa_buffer != NULL)
    delete fa_buffer;
  _device->BufferList->DeleteBufferByID(FB_ID);
  ClearOffsets();
}

//-----------------------------------------------

GOffset* GFaceArrayBuilder::AddOffset(fa_elemtype *facebuffer, g_uint32 fbsize)
{
  GOffset *NewOffset = new GOffset;

  g_uint32 n_offs = Offsets.size();

  if (n_offs > 0)
    NewOffset->offset = Offsets[n_offs-1]->offset + Offsets[n_offs-1]->size;
  else
    NewOffset->offset = 0;

  NewOffset->size = fbsize;

  if ((NewOffset->offset + NewOffset->size) > _max_fa_size)
  {
    delete NewOffset;
    return NULL;
  }
  
  if (fbsize > 0)
  {
    NewOffset->facearray = new fa_elemtype[fbsize];
    memcpy(NewOffset->facearray,facebuffer,fbsize*bufelemsize);
  }
  else
    NewOffset->facearray = new fa_elemtype[1];

  NewOffset->index = n_offs;
  Offsets.push_back(NewOffset);

  if (_changed)
  {
    _max_chg_index = n_offs;
  }
  else
  {
    _min_chg_index = n_offs;
    _max_chg_index = _min_chg_index;
    _changed = true;
  }

  return NewOffset;
}

//-----------------------------------------------

void GFaceArrayBuilder::RemoveOffset(g_uint16 index)
{
  if (index < Offsets.size())
  {
    delete[]Offsets[index]->facearray;
    delete Offsets[index];
    Offsets.erase(Offsets.begin()+index);
    for (g_uint32 i = index; i<Offsets.size(); i++)
    {
      if (i > 0)
        Offsets[i]->offset = Offsets[i-1]->offset + Offsets[i-1]->size; 
      else
        Offsets[i]->offset = 0;
      Offsets[i]->index = i;
    }

    if (_changed)
    {
      _max_chg_index = Offsets.size()-1;
      if (index<_min_chg_index) _min_chg_index = index;
    }
    else
    {
      _min_chg_index = index;
      _max_chg_index = Offsets.size()-1;
      _changed = true;
    }
  }
}

//-----------------------------------------------

void GFaceArrayBuilder::MoveEndOffset(g_uint16 index)
{
  if (index < Offsets.size())
  {
    GOffset *MovedOffset = Offsets[index];
    Offsets.erase(Offsets.begin()+index);
    Offsets.push_back(MovedOffset);

    for (g_uint32 i = index; i<Offsets.size(); i++)
    {
      if (i > 0)
        Offsets[i]->offset = Offsets[i-1]->offset + Offsets[i-1]->size; 
      else
        Offsets[i]->offset = 0;
      Offsets[i]->index = i;
    }

    if (_changed)
    {
      _max_chg_index = Offsets.size()-1;
      if (index<_min_chg_index) _min_chg_index = index;
    }
    else
    {
      _min_chg_index = index;
      _max_chg_index = Offsets.size()-1;
      _changed = true;
    }
  } 
}

//-----------------------------------------------

void GFaceArrayBuilder::UpdateOffset(g_uint16 index, fa_elemtype *facebuffer, g_uint32 fbsize)
{
  if (index < Offsets.size())
  {   
    if (Offsets[index]->size > fbsize)
    {
      Offsets[index]->size = fbsize;

      if (fbsize > 0)
        memcpy(Offsets[index]->facearray,facebuffer,fbsize*bufelemsize);

      if (_changed)
      {
        if (index < _min_chg_index) _min_chg_index = index;
        if (index > _max_chg_index) _max_chg_index = index;
      }
      else
      {
        _min_chg_index = index;
        _max_chg_index = index;
        _changed = true;
      }
    }
    else
    {
      Offsets[index]->size = fbsize;

      delete[]Offsets[index]->facearray;
      Offsets[index]->facearray = new fa_elemtype[fbsize];

      memcpy(Offsets[index]->facearray,facebuffer,fbsize*bufelemsize);

      MoveEndOffset(index);
    }
  }
}

//-----------------------------------------------

void GFaceArrayBuilder::ClearOffsets()
{
  for (g_uint32 i = 0; i<Offsets.size(); i++)
  {
    delete[]Offsets[i]->facearray;
    delete Offsets[i];
  }
  Offsets.clear();
  _changed = false;
}

//-----------------------------------------------

void GFaceArrayBuilder::Build()
{
  for (g_uint32 i = 0; i<Offsets.size(); i++)
    memcpy(&BuildedFaceArray[Offsets[i]->offset],Offsets[i]->facearray,Offsets[i]->size*bufelemsize);

  UpdateOffsetInShader(0,GetFaceArraySize());
  _changed = false;
}

//-----------------------------------------------

void GFaceArrayBuilder::BuildChanges()
{
  if (_changed)
  {
    for (g_uint32 i = _min_chg_index; i<=_max_chg_index; i++)
      memcpy(&BuildedFaceArray[Offsets[i]->offset],Offsets[i]->facearray,Offsets[i]->size*bufelemsize);

    g_uint32 min = Offsets[_min_chg_index]->offset;
    g_uint32 num = Offsets[_max_chg_index]->offset+Offsets[_max_chg_index]->size; 

    UpdateOffsetInShader(min,num);
    _changed = false;
  }
}

//-----------------------------------------------

g_uint32 GFaceArrayBuilder::GetFaceArraySize()
{
  return (Offsets[Offsets.size()-1]->offset + Offsets[Offsets.size()-1]->size);
}

//-----------------------------------------------

void GFaceArrayBuilder::SetOffsetToShader(g_uint16 oindex)
{
  if (fa_offset != NULL)
  {
    fa_offset->AsScalar()->SetInt(Offsets[oindex]->offset);
  }
}

//-----------------------------------------------

void GFaceArrayBuilder::UpdateOffsetInShader(g_uint32 min, g_uint32 max)
{
  if (fa_res != NULL)
  {
    D3D10_BOX box = {bufelemsize*min,0,0,bufelemsize*max,1,1};
    _device->D3DDevice->UpdateSubresource(fa_res,0,&box,&BuildedFaceArray[min],bufelemsize,0);
  }
}