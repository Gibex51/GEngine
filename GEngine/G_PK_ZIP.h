#pragma once

#ifndef G_PKZIP
#define G_PKZIP

#include <sys/stat.h>
#include <time.h>
#include "zip\zip.h"
#include "zip\unzip.h"
#include "funcs.h"

using namespace std;

#pragma comment(lib, "zdll.lib")

#define G_PKIT_FILE              0
#define G_PKIT_BUFFER            1
#define G_PKIT_BUFFER_AUTOCLEAR  2

struct G_PK_ITEM
{
  void     *Buffer;
  g_int32  BufferSize;
  g_string FileName;
  g_uint8  ItemType;
};

class G_PK_ZIP
{
public:
  //ZIP
  g_bool ZIP_CreateArchive(g_string FileName);
  void   ZIP_AddFileToPackList(g_string FileName);
  void   ZIP_AddBufferToPackList(g_string FileName, void *Buffer, g_int32 BufSize, g_bool AutoFreeBuffer);
  g_bool ZIP_Pack();
  void   ZIP_CloseArchive();
  //UNZIP
  g_bool UNZ_OpenArchive(g_string FileName);
  //g_bool UNZ_GetFilesList();
  //g_bool UNZ_UnPackToBuffer(g_uint32 index, G_PK_ITEM *OutBuffer);
  //g_bool UNZ_UnPackToFile(g_uint32 index);
  g_bool UNZ_UnPack(g_string Dir);
  g_bool UNZ_UnPackAllToBuffer(vector <G_PK_ITEM*> &BufferList);
  void   UNZ_CloseArchive();
  //Конструктор и деструктор
  G_PK_ZIP(void);
  ~G_PK_ZIP(void);
private:
  vector <G_PK_ITEM*> PackList;
  vector <G_PK_ITEM*> UnpackList;

  g_bool zip_opened;
  g_bool unz_opened;

  zipFile zFile;
  unzFile uFile;
};

#endif //G_PKZIP