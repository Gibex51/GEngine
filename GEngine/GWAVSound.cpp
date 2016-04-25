#include "GWAVSound.h"

//-----------------------------------------------

GWAVSound::GWAVSound(void)
{
  _type = G_ST_WAV;
  _buffer = NULL;
  _length = 0;
}

//-----------------------------------------------

GWAVSound::~GWAVSound(void)
{
  Release();
}

//-----------------------------------------------

g_bool GWAVSound::LoadFromFile(g_char *FileName)
{  
  memset(&_data_desc, 0, sizeof(WAVEFORMATEX));

  HANDLE hFile = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, NULL,
                            OPEN_EXISTING, 0, NULL);

  if (INVALID_HANDLE_VALUE == hFile)
    return HRESULT_FROM_WIN32(GetLastError()) == TRUE;

  if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
    return HRESULT_FROM_WIN32(GetLastError()) == TRUE;

  g_ulong dwChunkSize;
  g_ulong dwChunkPosition;
  
  //check the file type, should be fourccWAVE or 'XWMA'
  FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
  DWORD filetype;
  ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
  if (filetype != fourccWAVE)
    return false;
  
  //Locate the 'fmt ' chunk, and copy its contents into a WAVEFORMATEXTENSIBLE structure.
  FindChunk(hFile,fourccFMT, dwChunkSize, dwChunkPosition);
  ReadChunkData(hFile, &_data_desc, dwChunkSize, dwChunkPosition);

  //Locate the 'data' chunk, and read its contents into a buffer.
  //fill out the audio data buffer with the contents of the fourccDATA chunk
  FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
  if (_buffer != NULL) delete[]_buffer;
  _buffer = new g_uint8[dwChunkSize];
  _length = dwChunkSize;
  ReadChunkData(hFile, _buffer, dwChunkSize, dwChunkPosition);

  return true;
}

//-----------------------------------------------

void GWAVSound::Release()
{
  if (_buffer != NULL) delete[]_buffer;
  _buffer = NULL;
  _length = 0;
}

//-----------------------------------------------

void GWAVSound::GetBuffer(void **buffer, g_uint32 *length)
{
  if (buffer != NULL)
    *buffer = _buffer;
  if (length != NULL)
    *length = _length;
}

//-----------------------------------------------

void GWAVSound::GetDesc(void **desc)
{
  if (desc != NULL)
    *desc = &_data_desc;
}

//-----------------------------------------------

