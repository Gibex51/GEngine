#include "G_PK_ZIP.h"

//-----------------------------------------------

G_PK_ZIP::G_PK_ZIP(void)
{
  zip_opened = false;
  unz_opened = false;
  zFile = NULL;
  uFile = NULL;
}

//-----------------------------------------------

G_PK_ZIP::~G_PK_ZIP(void)
{

}

//-----------------------------------------------
//ZIP
//-----------------------------------------------

g_bool G_PK_ZIP::ZIP_CreateArchive(g_string FileName)
{
  if (zip_opened)
    return false;

  //Create Dir
  g_char DirPath[300];
  memset(DirPath,0,300);
  memcpy(DirPath,FileName.c_str(),FileName.size());
  strrchr(DirPath, '\\')[1] = 0;
  RecursiveCreateDirectory(DirPath,NULL);

  //Open Zip
  zFile = zipOpen(FileName.c_str(),0);

  if (zFile == NULL)
    return false;
  
  zip_opened = true;
  return true;
}

//-----------------------------------------------

void G_PK_ZIP::ZIP_AddFileToPackList(g_string FileName)
{
  if (FileExists(FileName.c_str()))
  {
    G_PK_ITEM *NewItem = new G_PK_ITEM;
    NewItem->ItemType = G_PKIT_FILE;
    NewItem->FileName = FileName;
    NewItem->Buffer = NULL;
    NewItem->BufferSize = 0;
    PackList.push_back(NewItem);
  }
}

//-----------------------------------------------

void G_PK_ZIP::ZIP_AddBufferToPackList(g_string FileName, void *Buffer, g_int32 BufSize, g_bool AutoFreeBuffer)
{
  G_PK_ITEM *NewItem = new G_PK_ITEM;
  if (AutoFreeBuffer)
    NewItem->ItemType = G_PKIT_BUFFER_AUTOCLEAR;
  else
    NewItem->ItemType = G_PKIT_BUFFER;
  NewItem->FileName = FileName;
  NewItem->Buffer = Buffer;
  NewItem->BufferSize = BufSize;
  PackList.push_back(NewItem);
}

//-----------------------------------------------

g_bool G_PK_ZIP::ZIP_Pack()
{
  if (zip_opened)
  {
    g_int32 compress_lvl = Z_BEST_SPEED;//Z_BEST_COMPRESSION;

    for (g_uint32 i = 0; i<PackList.size(); i++)
    {
      const char* full_filename = PackList[i]->FileName.c_str();
      const char* p = strrchr(full_filename, '\\');
      const char* short_filename = (NULL == p) ? full_filename : p + 1;
      // Pack File
      if (PackList[i]->ItemType == G_PKIT_FILE)
      {   
        FILE *p_file = NULL;
        fopen_s(&p_file,full_filename,"rb");
        if (p_file != NULL)
        {
          zip_fileinfo zfinfo = {0};
 
          struct _stat file_stat = {0};
          _fstat(_fileno(p_file), &file_stat);
          struct tm file_time;
          localtime_s(&file_time, &file_stat.st_mtime);
          tm_zip* zip_time = &zfinfo.tmz_date;
          memcpy(zip_time, &file_time, sizeof(tm_zip));

          g_int32 zipres = zipOpenNewFileInZip(zFile, short_filename, &zfinfo, NULL, 0, 
                                               NULL, 0, NULL, Z_DEFLATED, compress_lvl);
          if (zipres == ZIP_OK)
          {
            fseek(p_file , 0 , SEEK_END);    
            g_int32 bsize = ftell(p_file); 
            rewind(p_file);
            g_char *buffer = new g_char[bsize];
            fread (buffer, 1, bsize, p_file);

            zipWriteInFileInZip(zFile, buffer, bsize);
            zipCloseFileInZip(zFile);

            delete[]buffer;
          }

          fclose(p_file);

          if (zipres != ZIP_OK)
            return false;
        }
        else
          return false;
      }
      // Pack Buffer
      if ((PackList[i]->ItemType == G_PKIT_BUFFER) || (PackList[i]->ItemType == G_PKIT_BUFFER_AUTOCLEAR))
      {
        zip_fileinfo zfinfo = {0};
 
        struct _stat file_stat = {0};
        //_fstat(_fileno(p_file), &file_stat);
        struct tm file_time;
        localtime_s(&file_time, &file_stat.st_mtime);
        tm_zip* zip_time = &zfinfo.tmz_date;
        memcpy(zip_time, &file_time, sizeof(tm_zip));

        if (zipOpenNewFileInZip(zFile, short_filename, &zfinfo, NULL, 0, 
                                NULL, 0, NULL, Z_DEFLATED, compress_lvl) == ZIP_OK)
        {
          zipWriteInFileInZip(zFile, PackList[i]->Buffer, PackList[i]->BufferSize);
          zipCloseFileInZip(zFile);
        }
        else
          return false;
      }
      //
    }
  }
  return true;
}

//-----------------------------------------------

void G_PK_ZIP::ZIP_CloseArchive()
{
  if (zip_opened)
  {
    zipClose(zFile, NULL);
    zip_opened = false;

    for (g_uint32 i = 0; i<PackList.size(); i++)
    {
      if (PackList[i]->ItemType == G_PKIT_BUFFER_AUTOCLEAR)
        if (PackList[i]->Buffer != NULL)
          delete[]PackList[i]->Buffer;
      delete PackList[i];
    }

    PackList.clear();
  }
}

//-----------------------------------------------
//UNZIP
//-----------------------------------------------

g_bool G_PK_ZIP::UNZ_OpenArchive(g_string FileName)
{
  if (unz_opened)
    return false;

  uFile = unzOpen(FileName.c_str());

  if (uFile == NULL)
    return false;
  
  unz_opened = true;
  return true;
}

//-----------------------------------------------

g_bool G_PK_ZIP::UNZ_UnPack(g_string Dir)
{
  if (unz_opened)
  {
    unz_global_info glob_info;
    unzGetGlobalInfo(uFile,&glob_info);

    if (unzGoToFirstFile(uFile) == UNZ_OK)
      for (g_uint32 i = 0; i<glob_info.number_entry; i++)
      {
        unz_file_info file_info;
        g_char filename[300];
        memset(filename,0,300);
        unzGetCurrentFileInfo(uFile, &file_info, filename, 300, NULL, 0, NULL, 0);
        
        g_char *buffer = new g_char[file_info.uncompressed_size];

        if (unzOpenCurrentFile(uFile) == UNZ_OK)
        {
          if (unzReadCurrentFile(uFile, buffer, file_info.uncompressed_size) > 0)
          {
            if (!DirectoryExists(Dir.c_str()))
              RecursiveCreateDirectory(Dir.c_str(),NULL);

            g_char full_filename[300];
            sprintf_s(full_filename,300,"%s%s",Dir.c_str(),filename);

            FILE *p_file = NULL;
            fopen_s(&p_file,full_filename,"wb");
            if (p_file != NULL)
            {
              fwrite(buffer,1,file_info.uncompressed_size,p_file);
              fclose(p_file);
            }
          }
          unzCloseCurrentFile(uFile);
        }

        delete[]buffer;
        if (unzGoToNextFile(uFile) != UNZ_OK)
          break;
      }
  }
  return true;
}

//-----------------------------------------------

g_bool G_PK_ZIP::UNZ_UnPackAllToBuffer(vector <G_PK_ITEM*> &BufferList)
{
  if (unz_opened)
  {
    unz_global_info glob_info;
    unzGetGlobalInfo(uFile,&glob_info);

    if (unzGoToFirstFile(uFile) == UNZ_OK)
      for (g_uint32 i = 0; i<glob_info.number_entry; i++)
      {
        unz_file_info file_info;
        g_char filename[300];
        memset(filename,0,300);
        unzGetCurrentFileInfo(uFile, &file_info, filename, 300, NULL, 0, NULL, 0);
        
        g_char *buffer = new g_char[file_info.uncompressed_size];

        if (unzOpenCurrentFile(uFile) == UNZ_OK)
        {
          if (unzReadCurrentFile(uFile, buffer, file_info.uncompressed_size) > 0)
          {
            G_PK_ITEM *pkItem = new G_PK_ITEM;
            pkItem->FileName = filename;
            pkItem->Buffer = buffer;
            pkItem->BufferSize = file_info.uncompressed_size;
            pkItem->ItemType = G_PKIT_BUFFER;
            BufferList.push_back(pkItem);
            UnpackList.push_back(pkItem);
          }
          unzCloseCurrentFile(uFile);
        }

        if (unzGoToNextFile(uFile) != UNZ_OK)
          break;
      }
  }
  return true;
}

//-----------------------------------------------

void G_PK_ZIP::UNZ_CloseArchive()
{
  if (unz_opened)
  {
    unzClose(uFile);
    unz_opened = false;

    for (g_uint32 i = 0; i<UnpackList.size(); i++)
    {
      if (UnpackList[i]->Buffer != NULL)
        delete[]UnpackList[i]->Buffer;
      delete UnpackList[i];
    }

    UnpackList.clear();
  }
}

//-----------------------------------------------
