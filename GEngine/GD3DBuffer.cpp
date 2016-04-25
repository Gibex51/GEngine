#include "GD3DBuffer.h"

//-----------------------------------------------

GD3DBuffer::GD3DBuffer(void)
{
  _device = NULL;
}

//-----------------------------------------------

GD3DBuffer::~GD3DBuffer(void)
{
  ReleaseAll();
}

//-----------------------------------------------

G_BUFFER* GD3DBuffer::AddBuffer(g_uint8 bType, g_uint8 bFlag, g_uint32 elementsize, 
                                g_uint32 elementscount, void *elembuffer, g_uint16 BufID)
{
  if ((_device != NULL) && (elementscount != 0) && (elementsize != 0))
  {
	D3D10_BUFFER_DESC bd;
    bd.MiscFlags = 0;
    bd.ByteWidth = elementsize * elementscount;
    bd.BindFlags = bType;

	switch (bFlag)
	{
	  case G_BF_DEFAULT:
	           bd.Usage = D3D10_USAGE_DEFAULT;
               bd.CPUAccessFlags = 0;
			   break;
	  case G_BF_DYNAMIC:
	           bd.Usage = D3D10_USAGE_DYNAMIC;
               bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
			   break;   
      case G_BF_IMMUTABLE:
               bd.Usage = D3D10_USAGE_IMMUTABLE;
               bd.CPUAccessFlags = 0;
			   break;
	  default: return NULL;
	}	
  
    D3D10_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = elembuffer;

	G_BUFFER *NewBuffer = new G_BUFFER();
	NewBuffer->ID = BufID;
    if(FAILED(_device->CreateBuffer(&bd,&InitData,&NewBuffer->Buffer)))
	{
	  delete NewBuffer;
      return NULL;
	}

	Buffers[BufID] = NewBuffer;

    return NewBuffer;
  }
  return NULL;
}

//-----------------------------------------------

void GD3DBuffer::SetBuffer(G_BUFFER* Buffer)
{
  if ((Buffer != NULL) && (Buffer->Buffer != NULL))
  {
    G_BUFFER* OldBuffer = Buffers[Buffer->ID];
	Buffers[Buffer->ID] = Buffer;
	if (OldBuffer != NULL)
      delete OldBuffer;
  }
}

//-----------------------------------------------

G_BUFFER* GD3DBuffer::CreateBuffer(g_uint8 bType, g_uint8 bFlag, g_uint32 elementsize, 
                                g_uint32 elementscount, void *elembuffer, g_uint16 BufID)
{
  if ((_device != NULL) && (elementscount != 0) && (elementsize != 0))
  {
	D3D10_BUFFER_DESC bd;
    bd.MiscFlags = 0;
    bd.ByteWidth = elementsize * elementscount;
    bd.BindFlags = bType;

	switch (bFlag)
	{
	  case G_BF_DEFAULT:
	           bd.Usage = D3D10_USAGE_DEFAULT;
               bd.CPUAccessFlags = 0;
			   break;
	  case G_BF_DYNAMIC:
	           bd.Usage = D3D10_USAGE_DYNAMIC;
               bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
			   break;   
      case G_BF_IMMUTABLE:
               bd.Usage = D3D10_USAGE_IMMUTABLE;
               bd.CPUAccessFlags = 0;
			   break;
	  default: return NULL;
	}	

    D3D10_SUBRESOURCE_DATA InitData;
	if (elembuffer != NULL)
      InitData.pSysMem = elembuffer;  
	else
      InitData.pSysMem = new g_uint8[bd.ByteWidth];

	G_BUFFER *NewBuffer = new G_BUFFER();
	NewBuffer->ID = BufID;
    if(FAILED(_device->CreateBuffer(&bd,&InitData,&NewBuffer->Buffer)))
	  FREENULL_VAR(NewBuffer);

	if (elembuffer == NULL)
      delete[]InitData.pSysMem;

    return NewBuffer;
  }
  return NULL;
}

//-----------------------------------------------

void GD3DBuffer::ReleaseAll()
{
  map<g_uint32, G_BUFFER*>::iterator it;

  for (it = Buffers.begin() ;it != Buffers.end(); it++ )
    if ((*it).second != NULL) delete (*it).second;

  Buffers.clear();
}

//-----------------------------------------------

void GD3DBuffer::DeleteBufferByID(g_uint16 BufID)
{
  G_BUFFER *Buf = Buffers[BufID];
  if (Buf != NULL)
	delete Buf;
  Buffers.erase(BufID);
}

//-----------------------------------------------

G_BUFFER* GD3DBuffer::GetBufferByID(g_uint16 BufID)
{
  return Buffers[BufID];
}

//-----------------------------------------------

void G_BUFFER::ReCreate(g_uint8 bType, g_uint8 bFlag, g_uint32 elementsize, 
	                    g_uint32 elementscount, void *elembuffer, ID3D10Device *Device)
{
  if ((Device != NULL) && (elementscount != 0) && (elementsize != 0))
  {
	D3D10_BUFFER_DESC bd;
    bd.MiscFlags = 0;
    bd.ByteWidth = elementsize * elementscount;
    bd.BindFlags = bType;

	switch (bFlag)
	{
	  case G_BF_DEFAULT:
	           bd.Usage = D3D10_USAGE_DEFAULT;
               bd.CPUAccessFlags = 0;
			   break;
      case G_BF_DYNAMIC:
	           bd.Usage = D3D10_USAGE_DYNAMIC;
               bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
			   break;
      case G_BF_IMMUTABLE:
               bd.Usage = D3D10_USAGE_IMMUTABLE;
               bd.CPUAccessFlags = 0;
			   break;
	  default: return;
	}	
  
    D3D10_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = elembuffer;

    ID3D10Buffer *NewBuffer = NULL, *ChgBuffer = NULL;
    if (FAILED(Device->CreateBuffer(&bd,&InitData,&NewBuffer)))
	  NewBuffer = NULL;

	ChgBuffer = Buffer;
	Buffer = NewBuffer;

	if (ChgBuffer != NULL) ChgBuffer->Release();    
  }
}

//-----------------------------------------------

ID3D10Buffer* G_BUFFER::GetBuffer()
{
  if (this == NULL) return NULL;
  return Buffer;
}

//-----------------------------------------------
