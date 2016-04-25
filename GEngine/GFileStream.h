#pragma once

#include "GStructs.h"
#include <fstream>

using namespace std;

#define G_FSMODE_CREATE     0
#define G_FSMODE_READ       1
#define G_FSMODE_WRITE      2
#define G_FSMODE_READWRITE  3

class GFileStream
{
public:
  size_t  Position;

  g_bool  Create(const string &FileName, g_uint8 Mode);
  void    Release();

  size_t  GetSize();
  size_t  Read(void *Buffer, size_t Size);
  size_t  Write(void *Buffer, size_t Size);
  size_t  WriteString(const string &Str);
  
  size_t  ReadEntireFile(void **Buffer);
  
  GFileStream(void);
  ~GFileStream(void);
private:
  fstream  _fstream;
  g_uint8  _mode;
};

