#include "GFileStream.h"


GFileStream::GFileStream(void)
{
  Position = 0;
}

//-----------------------------------------------

GFileStream::~GFileStream(void)
{
  Release();
}

//-----------------------------------------------

g_bool GFileStream::Create(const string &FileName, g_uint8 Mode)
{
  _mode = Mode;

  switch (Mode)
  {
    case G_FSMODE_CREATE:    _fstream.open(FileName.c_str(), fstream::out | fstream::binary | fstream::trunc); break; 
    case G_FSMODE_READ:      _fstream.open(FileName.c_str(), fstream::in | fstream::binary); break;
    case G_FSMODE_WRITE:     _fstream.open(FileName.c_str(), fstream::out | fstream::binary); break;
	case G_FSMODE_READWRITE: _fstream.open(FileName.c_str(), fstream::in | fstream::out | fstream::binary); break; 
  }

  Position = 0;
  return true;
}

//-----------------------------------------------

void GFileStream::Release()
{
  _fstream.close();
  Position = 0;
}

//-----------------------------------------------

size_t GFileStream::GetSize()
{
  if (_fstream.is_open())
	_fstream.seekg(0, fstream::end);
  return size_t(_fstream.tellg());
}

//-----------------------------------------------

size_t GFileStream::Read(void *Buffer, size_t Size)
{
  if (Buffer == NULL) return 0;
  if (Size == 0) return 0;

  size_t fsize = GetSize();
  if (Position > fsize) Position = fsize - 1;

  _fstream.seekg(Position);

  _fstream.read((g_char*)Buffer, Size);
  size_t rbytes = size_t(_fstream.gcount());
  Position += rbytes;
  return rbytes;
}

//-----------------------------------------------

size_t GFileStream::Write(void *Buffer, size_t Size)
{
  if (Buffer == NULL) return 0;
  if (Size == 0) return 0;

  size_t fsize = GetSize();
  if (Position > fsize) Position = fsize - 1;

  _fstream.seekp(Position);

  _fstream.write((g_char*)Buffer, Size);
  Position += Size;
  return Size;
}

//-----------------------------------------------

size_t GFileStream::WriteString(const string &Str)
{
  if (Str.size() == 0) return 0;
  
  size_t fsize = GetSize();
  if (Position > fsize) Position = fsize - 1;

  _fstream.seekp(Position);

  size_t wbytes = Str.size();
  _fstream.write(Str.c_str(), wbytes);
  Position += wbytes;
  return wbytes;
}

//-----------------------------------------------

size_t GFileStream::ReadEntireFile(void **Buffer)
{
  if (Buffer == NULL) return 0;

  size_t fsize = GetSize();

  if (fsize <= 0) return 0;

  *Buffer = new g_char[fsize];
  memset(*Buffer, 0, fsize);

  _fstream.seekg(0, fstream::beg);
  _fstream.read((g_char*)*Buffer, fsize);
  fsize = size_t(_fstream.gcount());

  if (fsize <= 0) 
  {
	delete[]*Buffer;
	*Buffer = NULL;
	return 0;
  }

  return fsize;
}

//-----------------------------------------------
