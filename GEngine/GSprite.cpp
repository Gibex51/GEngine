#include "GSprite.h"

//-----------------------------------------------

GSprite::GSprite(const g_string &Name, GD3DDevice *Device):GBaseObject(Name,Device)
{
  SetType(G_OT_SPRITE);
  if (D3DX10CreateSprite(Device->D3DDevice,0, &_sprite) != S_OK)
    _sprite = NULL;
}

//-----------------------------------------------

GSprite::~GSprite(void)
{
  _sprite->Release();
}

//-----------------------------------------------

g_bool GSprite::Begin(g_uint32 Flags)
{
  return (_sprite->Begin(Flags) == S_OK);
}

//-----------------------------------------------

g_bool GSprite::DrawBuffered(D3DX10_SPRITE *Sprites, g_uint32 nSprites)
{
  return (_sprite->DrawSpritesBuffered(Sprites,nSprites) == S_OK);
}

//-----------------------------------------------

g_bool GSprite::End()
{
  return (_sprite->End() == S_OK);
}

//-----------------------------------------------
