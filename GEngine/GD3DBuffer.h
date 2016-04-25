#pragma once

#ifndef GD3DBUFFER_H
#define GD3DBUFFER_H

#include "GStructs.h"

using namespace std;

struct G_BUFFER
{
  g_uint16 ID;
  ID3D10Buffer* Buffer;
  ID3D10Buffer* GetBuffer();

  void ReCreate(g_uint8 bType, g_uint8 bFlag, g_uint32 elementsize, 
	            g_uint32 elementscount, void *elembuffer, ID3D10Device *Device);

  G_BUFFER() {Buffer = NULL; ID = 0;};
  ~G_BUFFER() {NULLRELEASE(Buffer);};
};

class GD3DBuffer 
{
  friend class GD3DDevice;
public:
  map <g_uint32, G_BUFFER*> Buffers;

  G_BUFFER* AddBuffer(g_uint8 bType, g_uint8 bFlag, g_uint32 elementsize, 
	                  g_uint32 elementscount, void *elembuffer, g_uint16 BufID);
  
  G_BUFFER* CreateBuffer(g_uint8 bType, g_uint8 bFlag, g_uint32 elementsize, 
	                     g_uint32 elementscount, void *elembuffer, g_uint16 BufID);
  
  void SetBuffer(G_BUFFER* Buffer);

  void DeleteBufferByID(g_uint16 BufID);
  G_BUFFER* GetBufferByID(g_uint16 BufID);
  void ReleaseAll();
  
  //Конструктор и деструктор
  GD3DBuffer(void);
  ~GD3DBuffer(void);
private:
  ID3D10Device *_device;
};

#endif //GD3DBUFFER_H