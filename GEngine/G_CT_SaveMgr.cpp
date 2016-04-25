#include "G_CT_SaveMgr.h"

//-----------------------------------------------

G_CT_SaveMgr::G_CT_SaveMgr(void)
{
  _psectmgr = NULL;
  _undo_enabled = true;
}

//-----------------------------------------------

G_CT_SaveMgr::~G_CT_SaveMgr(void)
{
  Release();
}

//-----------------------------------------------

void G_CT_SaveMgr::Init(G_CT_SectorsMgr *pSectMgr)
{
  _psectmgr = pSectMgr;
}

//-----------------------------------------------

void G_CT_SaveMgr::Release()
{
  _psectmgr = NULL;
}

//-----------------------------------------------

void G_CT_SaveMgr::SetObjectToMap(G_UNI_CUBE_OBJECT &uni_object, g_uint8 rot, g_bool replace_empty, g_bool centering, g_bool make_undo)
{
  if (uni_object._data != NULL)
  {
    g_ivec3 obj_size = uni_object._size;
    G_LOADED_CUBE *lc = uni_object._data;

    if (rot > 3) rot = 0;

    g_ivec3 offset = uni_object._pos;

	if (centering)
	{
      switch (rot)
      {
        case G_CUBE_ROT_0:offset.x -= obj_size.x/2;
                          offset.z -= obj_size.z/2;
                          break;
        case G_CUBE_ROT_90:offset.x -= obj_size.z/2;
                           offset.z -= obj_size.x/2 - 1;
                           break;
        case G_CUBE_ROT_180:offset.x -= obj_size.x/2 - 1;
                            offset.z -= obj_size.z/2 - 1;
                            break;
        case G_CUBE_ROT_270:offset.x -= obj_size.z/2 - 1;
                            offset.z -= obj_size.x/2;
                            break;
      }
	}

	if (make_undo)
	{
      g_ivec3 min_abs_cpos = offset;
      g_ivec3 max_abs_cpos = obj_size + offset;

	  switch (rot)
      {
        case G_CUBE_ROT_90:min_abs_cpos = g_ivec3(offset.x, offset.y, (obj_size.x - 1)+offset.z);
                           max_abs_cpos = g_ivec3(obj_size.z+offset.x, obj_size.y+offset.y, offset.z - 1);
                           break;
        case G_CUBE_ROT_180:min_abs_cpos = g_ivec3((obj_size.x - 1)+offset.x, offset.y, (obj_size.z - 1)+offset.z);;
                            max_abs_cpos = g_ivec3(offset.x - 1, obj_size.y+offset.y ,offset.z - 1);
                            break;
        case G_CUBE_ROT_270:min_abs_cpos = g_ivec3((obj_size.z - 1)+offset.x, offset.y, offset.z);
                            max_abs_cpos = g_ivec3(offset.x - 1, obj_size.y+offset.y, obj_size.x+offset.z);
                            break;
      }
	  
      SaveUndo("undo", min_abs_cpos, max_abs_cpos);
	}

    for (g_int32 x = 0; x <= obj_size.x; x++)
      for (g_int32 y = 0; y <= obj_size.y; y++)
        for (g_int32 z = 0; z <= obj_size.z; z++)
        {
          g_ivec3 abs_cpos = g_ivec3(x+offset.x,y+offset.y,z+offset.z);

          switch (rot)
          {
            case G_CUBE_ROT_90: abs_cpos = g_ivec3(z+offset.x,y+offset.y,(obj_size.x - x - 1)+offset.z);
                                break;
            case G_CUBE_ROT_180: abs_cpos = g_ivec3((obj_size.x - x - 1)+offset.x,y+offset.y,(obj_size.z - z - 1)+offset.z);
                                 break;
            case G_CUBE_ROT_270: abs_cpos = g_ivec3((obj_size.z - z - 1)+offset.x,y+offset.y,x+offset.z);
                                 break;
          }   

          g_ivec3 cpos,spos;
          g_int32 n = x*(obj_size.y+1)*(obj_size.z+1) + y*(obj_size.z+1) + z;

          GetSectorAndCubePosByAbsCubePos(abs_cpos,spos,cpos);

          GCubeSector *Sector = _psectmgr->GetSector(spos);

          if (Sector != NULL)
          {
            if ((lc[n].cube != 0) || (replace_empty))
            {
              Sector->InitCubes();
              Sector->Empty = false;
              Sector->Changed = true;
              Sector->Cube(cpos) = lc[n].cube;

              g_uint8 rot_info = (lc[n].info - lc[n].info%64)/64 + rot;
              if (rot_info > 3) rot_info -= 4;

              Sector->Info(cpos).info_rot = rot_info;

              _psectmgr->CheckCubeAdj(Sector,cpos,true);
            }
          }
        }
  }
}

//-----------------------------------------------

g_bool G_CT_SaveMgr::GetObjectFromMap(G_UNI_CUBE_OBJECT &uni_object, g_ivec3 min_abs_cpos, g_ivec3 max_abs_cpos)
{
  if (_psectmgr == NULL)
    return false;
  
  g_uint32 n_sec = _psectmgr->Sectors.size();

  if (n_sec == 0)
    return false;

  CheckMinMax(min_abs_cpos, max_abs_cpos);

  g_ivec3 spos, cpos, area_size = max_abs_cpos - min_abs_cpos;
  GetSectorAndCubePosByAbsCubePos(min_abs_cpos,spos,cpos);

  uni_object.Init(area_size, min_abs_cpos);

  GCubeSector *CurrSector = _psectmgr->GetSector(spos);

  if (CurrSector != NULL)
  {
    GCubeSector *st_sec_y, *st_sec_z;
    g_int32 st_cpos_y, st_cpos_z;
    for (g_int32 x = 0; x<=area_size.x; x++)
    { 
      if (cpos.x >= NCUBESXZ)
      {
        cpos.x = 0;
        CurrSector = CurrSector->AdjSect.pxs;
      }
      if (CurrSector == NULL) break;
      st_sec_z = CurrSector;
      st_cpos_z = cpos.z;
      for (g_int32 z = 0; z<=area_size.z; z++)
      {
        if (st_cpos_z >= NCUBESXZ)
        {
          st_cpos_z = 0;
          st_sec_z = st_sec_z->AdjSect.pzs;
        }
        if (st_sec_z == NULL) break;
        st_sec_y = st_sec_z;
        st_cpos_y = cpos.y;

        for (g_int32 y = 0; y<=area_size.y; y++)
        {
          if (st_cpos_y >= NCUBESY)
          {
            st_cpos_y = 0;
            st_sec_y = st_sec_y->AdjSect.pys;
          }
          if (st_sec_y == NULL) break;

          g_ivec3 curr_cube = g_ivec3(cpos.x,st_cpos_y,st_cpos_z);
		  g_uint32 data_pos = x*((area_size.y+1)*(area_size.z+1))+y*(area_size.z+1)+z;

		  uni_object._data[data_pos].cube = st_sec_y->Cube(curr_cube, true);
		  uni_object._data[data_pos].info = st_sec_y->Info(curr_cube, true).info_adj + st_sec_y->Info(curr_cube, true).info_rot*64;
          st_cpos_y += 1;
        } // for (g_int32 y = 0; ...
        st_cpos_z += 1;
      } // for (g_int32 z = 0; ...;
      cpos.x += 1;
    } // for (g_int32 x = 0; ... 
  } // if (CurrSector != NULL)
  else
	return false;

  return true;
}

//-----------------------------------------------
/*
g_bool G_CT_SaveMgr::LoadOldObject(const g_string &filename, G_UNI_CUBE_OBJECT &uni_object)
{
  if (_psectmgr == NULL)
    return false;

  vector <G_PK_ITEM*> BufferList;

  if (!_pk_zip.UNZ_OpenArchive(filename))
    return false;

  _pk_zip.UNZ_UnPackAllToBuffer(BufferList);

  g_bool result = true;
  if (BufferList.size() > 0)
  {
    if (BufferList[0]->BufferSize >= sizeof(g_ivec3))
    {
      uni_object.Release();

	  g_int32 headersize = sizeof(g_ivec3);
      memcpy(&uni_object._size, BufferList[0]->Buffer, headersize);
      g_int32 arraysize = (uni_object._size.x+1)*(uni_object._size.y+1)*(uni_object._size.z+1);
      
      uni_object._data = new G_LOADED_CUBE[arraysize];
      memcpy(&uni_object._data[0], &((g_char*)BufferList[0]->Buffer)[headersize], 
             arraysize*sizeof(G_LOADED_CUBE));
    }
    else
      result = false;
  }
  else
    result = false;

  _pk_zip.UNZ_CloseArchive();

  return result;
}
*/
//-----------------------------------------------

g_bool G_CT_SaveMgr::LoadObject(const g_string &filename, G_UNI_CUBE_OBJECT &uni_object)
{
  if (_psectmgr == NULL)
    return false;

  vector <G_PK_ITEM*> BufferList;

  if (!_pk_zip.UNZ_OpenArchive(filename))
    return false;

  _pk_zip.UNZ_UnPackAllToBuffer(BufferList);

  g_bool result = true;
  if (BufferList.size() > 0)
  {
    g_int32 headersize = 2*sizeof(g_ivec3);
	if (BufferList[0]->BufferSize >= headersize)
    {    
	  uni_object.Release();
      
	  g_char *curr_buf = (g_char*)BufferList[0]->Buffer;
	  memcpy(&uni_object._pos, &curr_buf[0], sizeof(g_ivec3));
	  memcpy(&uni_object._size, &curr_buf[sizeof(g_ivec3)], sizeof(g_ivec3));
      g_int32 arraysize = (uni_object._size.x+1)*(uni_object._size.y+1)*(uni_object._size.z+1);
      
      uni_object._data = new G_LOADED_CUBE[arraysize];
      memcpy(&uni_object._data[0], &curr_buf[headersize], 
             arraysize*sizeof(G_LOADED_CUBE));
    }
    else
      result = false;
  }
  else
    result = false;

  _pk_zip.UNZ_CloseArchive();

  return result;
}

//-----------------------------------------------

g_bool G_CT_SaveMgr::SaveObject(const g_string &filename, const G_UNI_CUBE_OBJECT &uni_object)
{
  if (FileExists(filename.c_str()))
    if (DeleteFile(filename.c_str()) == FALSE)
      return false;
  
  g_int32 arraysize = (uni_object._size.x+1)*(uni_object._size.y+1)*(uni_object._size.z+1);
  
  g_uint32 buf_size = arraysize*sizeof(G_LOADED_CUBE) + 2*sizeof(g_ivec3);
  g_uint8 *buffer = new g_uint8[buf_size];
  memcpy(&buffer[0], &uni_object._pos, sizeof(g_ivec3));
  memcpy(&buffer[sizeof(g_ivec3)], &uni_object._size, sizeof(g_ivec3));
  memcpy(&buffer[2*sizeof(g_ivec3)], uni_object._data, arraysize*sizeof(G_LOADED_CUBE));
  
  if (_pk_zip.ZIP_CreateArchive(filename))
  {
    _pk_zip.ZIP_AddBufferToPackList(filename, buffer, buf_size, true);
    _pk_zip.ZIP_Pack();
    _pk_zip.ZIP_CloseArchive();

    if (!FileExists(filename.c_str()))
      return false;
  }
  else
    return false;

  return true; 
}

//-----------------------------------------------

g_bool G_CT_SaveMgr::SaveUndo(const g_string &objname, g_ivec3 min_abs_cpos, g_ivec3 max_abs_cpos)
{
  if (!_undo_enabled) return false;

  g_int8 new_undo_id = _undo_list.size();
  g_string filename;

  if (new_undo_id >= G_MAX_UNDO)
  {	
	new_undo_id--;

	g_string prevname = MakeUndoFileName(objname, 0);
	DeleteFile(prevname.c_str());	

	for (g_uint32 i = 1; i < G_MAX_UNDO; i++)
	{	  
	  filename = MakeUndoFileName(objname, i);
	  rename(filename.c_str(), prevname.c_str());
	  prevname = filename;
	}
  }
  else
    _undo_list.push_back(new_undo_id); 

  filename = MakeUndoFileName(objname, new_undo_id);

  G_UNI_CUBE_OBJECT uni_obj;

  if (!GetObjectFromMap(uni_obj, min_abs_cpos, max_abs_cpos))
	return false;

  if (!SaveObject(filename, uni_obj))
	return false;

  return true;
}

//-----------------------------------------------

g_bool G_CT_SaveMgr::LoadUndo(const g_string &objname, G_UNI_CUBE_OBJECT &loaded_undo)
{
  g_int32 undo_id = _undo_list.size() - 1;

  if (undo_id < 0)
    return false;

  _undo_list.pop_back();
    
  g_string filename = MakeUndoFileName(objname, undo_id);

  return LoadObject(filename, loaded_undo);
}

//-----------------------------------------------

void G_CT_SaveMgr::ClearUndo(const g_string &objname)
{
  g_string filename;
  for (g_uint32 i = 0; i < G_MAX_UNDO; i++)
  {    
	g_string filename = MakeUndoFileName(objname, i);

    if (FileExists(filename.c_str()))
      DeleteFile(filename.c_str());
  }
}

//-----------------------------------------------

g_bool G_CT_SaveMgr::SaveMapRegion(GMapRegion &MapRegion, const g_string &mapname)
{
  g_char filename[300];
  sprintf_s(filename, 300, SAVE_MAP_PATH_TEMPLATE, mapname.c_str(),
            MapRegion.RegionPos.x, MapRegion.RegionPos.y, "preg");

  if (FileExists(filename))
    if (DeleteFile(filename) == FALSE)
      return false;

  g_uint32 n_sec = MapRegion.SectorsList.size();

  if (n_sec == 0)
    return false;

  g_uint8 **MapBuff = new g_uint8* [n_sec/MAPDEEP];
  g_int32 szSec  = cubescount*sizeof(g_uint16);
  g_int32 szInfo  = cubescount*sizeof(g_uint16);
  g_int32 szHeader = sizeof(G_REG_HEADER);

  g_bool ArchCreated = _pk_zip.ZIP_CreateArchive(filename);

  if (ArchCreated)
  {
    for (g_uint32 i = 0; i < n_sec/MAPDEEP; i++)
    {
      MapBuff[i] = new g_uint8 [(szSec+szInfo)*6 + szHeader];
      g_char col_name[300];
      sprintf_s(col_name, "%d_%d.sec", MapRegion.SectorsList[i*6]->Position.x, 
                MapRegion.SectorsList[i*6]->Position.z);

      G_REG_HEADER rHeader;
      g_int32 not_empty_count = 0, not_empty_num = 0;
 
      for (g_uint32 j = 0; j < 6; j++)
        rHeader.IsEmpty[j] = MapRegion.SectorsList[i*6 + j]->Empty;

      // Доп информация
      memcpy(&MapBuff[i][0], &rHeader, szHeader);

      for (g_uint32 j = 0; j < 6; j++)
      {
        GCubeSector *Sector = MapRegion.SectorsList[i*6 + j];
        // Сохранение не пустых секторов
        if (!Sector->Empty)
        {
          memcpy(&MapBuff[i][szHeader + not_empty_count*szSec], &Sector->Cube(0), szSec);
          not_empty_count++;
        }
      }

      not_empty_num = not_empty_count;
      not_empty_count = 0;

      for (g_uint32 j = 0; j < 6; j++)
      {
        GCubeSector *Sector = MapRegion.SectorsList[i*6 + j];
        // Сохранение не пустых секторов
        if (!Sector->Empty)
        {
          memcpy(&MapBuff[i][szHeader + not_empty_num*szSec + not_empty_count*szInfo], &Sector->Info(0), szInfo);
          not_empty_count++;
        }
      }
        
      _pk_zip.ZIP_AddBufferToPackList(col_name, MapBuff[i], (szSec+szInfo)*not_empty_count + szHeader, true);
    }

    _pk_zip.ZIP_Pack();
    _pk_zip.ZIP_CloseArchive();
  }

  delete[]MapBuff;

  if (!FileExists(filename))
    return false;

  return ArchCreated;
}

//-----------------------------------------------

g_bool G_CT_SaveMgr::LoadMapRegion(GMapRegion &MapRegion, const g_string &mapname)
{
  g_char filename[300];
  sprintf_s(filename, 300, SAVE_MAP_PATH_TEMPLATE, mapname.c_str(),
            MapRegion.RegionPos.x, MapRegion.RegionPos.y, "preg");

  if (!FileExists(filename))
    return false;

  g_int32 szSec  = cubescount*sizeof(g_uint16);
  g_int32 szInfo  = cubescount*sizeof(g_uint16);
  g_int32 szHeader = sizeof(G_REG_HEADER);
  vector <G_PK_ITEM*> BufferList;
  
  if (_pk_zip.UNZ_OpenArchive(filename))
  {
    _pk_zip.UNZ_UnPackAllToBuffer(BufferList);

    for (g_uint32 i = 0; i < BufferList.size(); i++)
    {
      g_ivec3 spos;
      G_REG_HEADER rHeader;
      g_int32 not_empty_count = 0, not_empty_num = 0;

      sscanf_s(BufferList[i]->FileName.c_str(),"%d_%d.sec",&spos.x,&spos.z);
    
      if (BufferList[i]->BufferSize < szHeader)
        break;

      memcpy(&rHeader, &((g_char*)BufferList[i]->Buffer)[0], szHeader);
    
      for (g_int32 j = 0; j < 6; j++)
        if (!rHeader.IsEmpty[j]) not_empty_num++;

      for (g_int32 j = MAPDEEP - 1; j>=0; j--)
      {
        //Загрузка секторов
        spos.y = -j;
        GCubeSector *NewSector = new GCubeSector(spos);
        NewSector->BoundBox.SetMinMax((spos*CUBE_SECTOR_SIZE).AsVec3(),((spos+1)*CUBE_SECTOR_SIZE).AsVec3());
        
        if (!rHeader.IsEmpty[MAPDEEP - j - 1])
        {
          NewSector->InitCubes();
          NewSector->Empty = false;

          memcpy(&NewSector->Cube(0), 
                 &((g_char*)BufferList[i]->Buffer)[szHeader + not_empty_count*szSec], szSec);

          if (BufferList[i]->BufferSize == (szHeader + not_empty_num*(szSec+szInfo)))
          {
            memcpy(&NewSector->Info(0), 
                   &((g_char*)BufferList[i]->Buffer)[szHeader + not_empty_num*szSec + not_empty_count*szInfo], 
                   szInfo);
          }

          not_empty_count++;
        }

        MapRegion.SectorsList.push_back(NewSector);
      }
    }

    _pk_zip.UNZ_CloseArchive();
  }
  else 
    return false;

  return true;
  
}

//-----------------------------------------------

g_bool G_CT_SaveMgr::SaveMap(const g_string &mapname)
{
  if (_psectmgr == NULL)
    return false;

  g_uint32 n_sec = _psectmgr->Sectors.size();

  if (n_sec == 0)
    return false;

  vector <GMapRegion> RegionsList;

  //Build Regions List
  for (g_uint32 i = 0; i < n_sec; i++)
  {
    g_ivec2 rpos = GetRegionPosBySectorPos(_psectmgr->Sectors[i]->Position);

    g_bool not_found = true;
    for (g_uint32 j = 0; j < RegionsList.size(); j++)
      if (RegionsList[j].RegionPos == rpos)
      {
        RegionsList[j].SectorsList.push_back(_psectmgr->Sectors[i]);
        not_found = false;
        break;
      }

    if (not_found)
    {
      GMapRegion NewReg;
      NewReg.RegionPos = rpos;
      NewReg.SectorsList.push_back(_psectmgr->Sectors[i]);
      RegionsList.push_back(NewReg);
    }
  }

  //Save Regions
  for (g_uint32 i = 0; i < RegionsList.size(); i++)
  {
    if (!SaveMapRegion(RegionsList[i], mapname))
      return false;
  }

  return true;
}

//-----------------------------------------------

g_bool G_CT_SaveMgr::LoadMap(const g_string & mapname)
{
  if (_psectmgr == NULL)
    return false;

  g_char filename[300];
  sprintf_s(filename, 300, SAVE_MAP_PATH"%s\\*.preg", mapname.c_str());

  if (!DirectoryExists(SAVE_MAP_PATH))
    return false;

  _WIN32_FIND_DATAA FindResult;
  vector <GMapRegion> RegionsList;

  HANDLE hFile = FindFirstFile(filename, &FindResult);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    GMapRegion NewReg;
    sscanf_s(FindResult.cFileName,"%d_%d",&NewReg.RegionPos.x,&NewReg.RegionPos.y);
    RegionsList.push_back(NewReg);

    while(FindNextFile(hFile,&FindResult))
    {
      sscanf_s(FindResult.cFileName,"%d_%d",&NewReg.RegionPos.x,&NewReg.RegionPos.y);
      RegionsList.push_back(NewReg);
    }
  }
  FindClose(hFile);

  if (RegionsList.size() == 0)
    return false;

  _psectmgr->Release();

  for (g_uint32 i = 0; i < RegionsList.size(); i++)
  {
    LoadMapRegion(RegionsList[i],mapname);

    for (g_uint32 j = 0; j < RegionsList[i].SectorsList.size(); j++)
      _psectmgr->Sectors.push_back(RegionsList[i].SectorsList[j]);

    RegionsList[i].SectorsList.clear();
  }

  RegionsList.clear();

  return true;
}

//-----------------------------------------------

g_bool G_CT_SaveMgr::LoadMapInfo(const g_string &mapname, G_MAP_INFO &mapinfo)
{
  g_char filename[300];
  sprintf_s(filename, 300, SAVE_MAP_PATH"%s\\map.info", mapname.c_str());

  GFileStream InfoFS;
  if (InfoFS.Create(filename,G_FSMODE_READ))
  {
    InfoFS.Read(&mapinfo, sizeof(mapinfo));    
    InfoFS.Release();
  }
  else
    return false;

  return true;
}

//-----------------------------------------------

g_bool G_CT_SaveMgr::SaveMapInfo(const g_string &mapname, G_MAP_INFO &mapinfo)
{
  g_char filename[300];
  sprintf_s(filename, 300, SAVE_MAP_PATH"%s\\map.info", mapname.c_str());

  GFileStream InfoFS;
  if (InfoFS.Create(filename,G_FSMODE_WRITE))
  {
    InfoFS.Write(&mapinfo, sizeof(mapinfo));    
    InfoFS.Release();
  }
  else
    return false;

  return true;
}

//-----------------------------------------------

g_string G_CT_SaveMgr::MakeObjectFileName(const g_string &objname)
{
  g_char filename[300];
  sprintf_s(filename, 300, SAVE_OBJ_PATH_TEMPLATE, objname.c_str(), "pobj");
  return filename;
}

//-----------------------------------------------

g_string G_CT_SaveMgr::MakeUndoFileName(const g_string &objname, g_int32 undo_ind)
{
  g_char filename[300];
  g_string save_path = GetFullTempPath() + SAVE_TMP_TEMPLATE;
  sprintf_s(filename, 300, save_path.c_str(), "undo", objname.c_str(), undo_ind, "pund");
  return filename;
}

//-----------------------------------------------
