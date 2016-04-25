#include "G_CT_MapGenMgr.h"

//-----------------------------------------------

G_CT_MapGenMgr::G_CT_MapGenMgr(void)
{
  _heightmap = NULL;
  _sect_heightmap = NULL;
  _gentype = 0;
  memset(_areas,0,sizeof(_areas));
}

//-----------------------------------------------

G_CT_MapGenMgr::~G_CT_MapGenMgr(void)
{

}

//-----------------------------------------------
// Общее
//-----------------------------------------------

void G_CT_MapGenMgr::Init(G_CT_SectorsMgr *pSectorsMgr)
{
  pSectMgr = pSectorsMgr;

  _areas[0].object_id = 57;
  _areas[0].deep_min = 1;
  _areas[0].deep_max = 1;

  _areas[1].object_id = 112;
  _areas[1].deep_min = 1;
  _areas[1].deep_max = 3;

  _areas[2].object_id = 118;
  _areas[2].deep_min = 1;
  _areas[2].deep_max = 3;

  _areas[3].object_id = 133;
  _areas[3].deep_min = 1;
  _areas[3].deep_max = 2;

  _areas[4].object_id = 51;
  _areas[4].deep_min = 1;
  _areas[4].deep_max = 1;

  _areas[5].object_id = 53;
  _areas[5].deep_min = 1;
  _areas[5].deep_max = 1;

  _areas[6].object_id = 54;
  _areas[6].deep_min = 1;
  _areas[6].deep_max = 1;
}

//-----------------------------------------------

void G_CT_MapGenMgr::Release()
{
  GT1_RemoveAllPGSectors();

  GT2_ReleaseHeightMap();
}

//-----------------------------------------------

void G_CT_MapGenMgr::SaveHeightMap(string filename)
{
  switch (_gentype)
  {
    case 1:break;
    case 2:GT2_SaveHeightMap(filename, true); break;
    default:break;
  }
}

//-----------------------------------------------

void G_CT_MapGenMgr::LoadHeightMap(string filename, g_ivec2 &mapsize)
{
  switch (_gentype)
  {
    case 1:{
             GT1_RemoveAllPGSectors();
             _gentype = 2;
             GT2_CreateHeightMap(mapsize);
             GT2_LoadHeightMap(filename); 
           }
           break;
    default:{
              _gentype = 2;
              GT2_ReleaseHeightMap();
              GT2_CreateHeightMap(mapsize);
              GT2_LoadHeightMap(filename); 
            }
            break;
  }
}

//-----------------------------------------------

g_float G_CT_MapGenMgr::GetMapHeightInPos(g_vec3 &pos)
{
  switch (_gentype)
  {
    case 1:return GT1_GetMapHeightInPos(pos);
    case 2:return GT2_GetMapHeightInPos(pos);
    default:break;
  }
  return default_height;
}

//-----------------------------------------------

void G_CT_MapGenMgr::GenTree(GCubeSector *Sector, g_ivec3 cpos, g_int8 height, g_uint8 type)
{
  if ((Sector != NULL) && (cpos.x >= 0) && (cpos.y >= 0) && (cpos.z >= 0)
      && (cpos.x < NCUBESXZ) && (cpos.y < NCUBESY) && (cpos.z < NCUBESXZ))   
    if (height > 3)
    {
      g_int32 leaves = rand()%3;
      for (g_int32 i = -2; i < 3; i++)
        for (g_int32 j = -2; j < 3; j++)
          for (g_int32 k = -2; k < 3; k++)
          {
            g_int32 cut = 0;
            if ((i == -2) || (i == 2)) cut ++;
            if ((j == -2) || (j == 2)) cut ++;
            if ((k == -2) || (k == 2)) cut ++;
            if (cut > 1) continue;

            g_ivec3 spos = Sector->Position;
            g_ivec3 ncpos = cpos + g_ivec3(i,j+height,k);

            if (ncpos.x < 0) {spos.x--;ncpos.x += NCUBESXZ;}
            if (ncpos.y < 0) {spos.y--;ncpos.y += NCUBESY;}
            if (ncpos.z < 0) {spos.z--;ncpos.z += NCUBESXZ;}
            
            if (ncpos.x >= NCUBESXZ) {spos.x++;ncpos.x -= NCUBESXZ;}
            if (ncpos.y >= NCUBESY)  {spos.y++;ncpos.y -= NCUBESY;}
            if (ncpos.z >= NCUBESXZ) {spos.z++;ncpos.z -= NCUBESXZ;}
            
            GCubeSector *nSect = pSectMgr->GetSector(spos);

            if (nSect != NULL)
            {
              nSect->Empty = false;
              nSect->Cube(ncpos) = 197+leaves;

              pSectMgr->CheckCubeAdj(nSect,ncpos,true);
            }
          }

      Sector->Empty = false;
      for (g_int32 i = 0; i< height; i++)
      {
        g_ivec3 ncpos = cpos + g_ivec3(0,i,0);
        if (ncpos.y < NCUBESY)
        {
          Sector->Cube(ncpos) = 45+leaves;

          pSectMgr->CheckCubeAdj(Sector,ncpos,true);
        }
        else
        {
          GCubeSector *nSect = Sector->AdjSect.pys;       
          if (nSect != NULL)
          {
            nSect->Empty = false;
            ncpos = ncpos - g_ivec3(0,NCUBESY,0);
            nSect->Cube(ncpos) = 45;

            pSectMgr->CheckCubeAdj(nSect,ncpos,true);
          }
        }
      }
    }
}

//-----------------------------------------------
// Неограниченая генерация
//-----------------------------------------------

void G_CT_MapGenMgr::InitMap()
{
  _gentype = 1;
  //Генерируем центральный сектор по позиции игрока
  g_ivec2 pos = center_sector_xy; // center pos
  
  GT1_GenerateSectorPreview(pos,true);

  for (g_int32 l = 0; l<MAPDEEP; l++)
  {
	g_ivec3 spos = g_ivec3(pos.x,-l,pos.y);
    //if (!LoadMapSector(spos,WorldName))	  
	  GT1_GenerateSectorRelease(spos);
  }

  //Генерируем сектора вокруг
  g_ivec2 sectdir[4] = {g_ivec2(1,0),g_ivec2(0,-1),g_ivec2(-1,0),g_ivec2(0,1)};

  for (g_int32 i = 1; i<MAPRADIUS; i++)
  {
	pos.x -= 1;
	pos.y += 1;
    for (g_int32 j = 0; j<4; j++)
	  for (g_int32 k = 0; k<i*2; k++)
	  {
		pos.x += sectdir[j].x;
		pos.y += sectdir[j].y;

		GT1_GenerateSectorPreview(pos,true);

		for (g_int32 l = MAPDEEP - 1; l>=0; l--)
	    {
		  g_ivec3 spos = g_ivec3(pos.x,-l,pos.y);
          //if (!LoadMapSector(spos,WorldName))
		    GT1_GenerateSectorRelease(spos);
	    }
	  }
  }

  //Генерируем хэш-таблицу для поиска сектора
  pSectMgr->GenerateSectorsHASHTable();

  //Генерация деревьев
  for (g_uint32 ntrees = 0; ntrees<1500; ntrees++)
  {
    g_int32 rx = center_sector_xy.x-MAPRADIUS+rand()%(MAPRADIUS*2+1);
    g_int32 ry = center_sector_xy.y-MAPRADIUS+rand()%(MAPRADIUS*2+1);
    GCubeSector *Sector = pSectMgr->GetSector(g_ivec3(rx,-3,ry));
    GSectorPreGen *PGSector = GT1_GetPGSector(g_ivec2(rx,ry));
    rx = rand()%NCUBESXZ;
    ry = rand()%NCUBESXZ;    
    GenTree(Sector,g_ivec3(rx,PGSector->heightmap[rx][ry]-NCUBESXZ*2,ry),7,0);
  }
}

//-----------------------------------------------

void G_CT_MapGenMgr::InitMapClear()
{
  _gentype = 1;
  //Генерируем центральный сектор по позиции игрока
  g_ivec2 pos = center_sector_xy; // center pos
  
  //GenerateSectorPreview(pos,true);
  GT1_AddPGSector(pos,0);

  for (g_int32 l = 0; l<MAPDEEP; l++)
  {
	g_ivec3 spos = g_ivec3(pos.x,-l,pos.y);
    //if (!LoadMapSector(spos,WorldName))	  
	  GT1_GenerateSectorClear(spos);
  }

  //Генерируем сектора вокруг
  g_ivec2 sectdir[4] = {g_ivec2(1,0),g_ivec2(0,-1),g_ivec2(-1,0),g_ivec2(0,1)};

  for (g_int32 i = 1; i<MAPRADIUS; i++)
  {
	pos.x -= 1;
	pos.y += 1;
    for (g_int32 j = 0; j<4; j++)
	  for (g_int32 k = 0; k<i*2; k++)
	  {
		pos.x += sectdir[j].x;
		pos.y += sectdir[j].y;

		//GenerateSectorPreview(pos,true);
		GT1_AddPGSector(pos,0);

		for (g_int32 l = 0; l<MAPDEEP; l++)
	    {
		  g_ivec3 spos = g_ivec3(pos.x,-l,pos.y);
          //if (!LoadMapSector(spos,WorldName))
		    GT1_GenerateSectorClear(spos);
	    }
	  }
  }

  //Генерируем хэш-таблицу для поиска сектора
  pSectMgr->GenerateSectorsHASHTable();
}

//-----------------------------------------------

void G_CT_MapGenMgr::GT1_SmoothHeightMap(g_uint16 **heightmap)
{
  g_int16 div;
  g_int16 tp;
  for (g_uint16 i = 0; i <NCUBESXZ; i++)
    for (g_uint16 j = 0; j <NCUBESXZ; j++)
	{
	  div = 1;
	  tp = heightmap[i][j];
	  if (i>0)
	    {tp += heightmap[i-1][j]; div++;}
	  if ((i>0) && (j>0))
		{tp += heightmap[i-1][j-1]; div++;}
	  if (j>0)
		{tp += heightmap[i][j-1]; div++;}
	  if (i<NCUBESXZ-1)
		{tp += heightmap[i+1][j]; div++;}
	  if ((i<NCUBESXZ-1) && (j<NCUBESXZ-1))
		{tp += heightmap[i+1][j+1]; div++;}
	  if (j<NCUBESXZ-1)
		{tp += heightmap[i][j+1]; div++;}
	  if ((i<NCUBESXZ-1) && (j>0))
		{tp += heightmap[i+1][j-1]; div++;}
	  if ((i>0) && (j<NCUBESXZ-1))
	  	{tp += heightmap[i-1][j+1]; div++;}
	  heightmap[i][j] = tp / div;
	}
}

//-----------------------------------------------

void G_CT_MapGenMgr::GT1_PrevCoordToSect(g_ivec2 incoord, g_int16 *secind, g_ivec2 *mappos)
{
  g_int16 conv[9] = {5,4,3,6,8,2,7,0,1};
  mappos->x = incoord.x%NCUBESXZ;
  mappos->y = incoord.y%NCUBESXZ;
  g_int16 x = (incoord.x - mappos->x)/NCUBESXZ;
  g_int16 y = (incoord.y - mappos->y)/NCUBESXZ;
  *secind = conv[y*3+x];
}

//-----------------------------------------------

void G_CT_MapGenMgr::GT1_GenerateSectorPreview(g_ivec2 pos, g_bool fullgen)
{
  srand(pos.x*1000+pos.y);
  GSectorPreGen* PGSectAround[9];
  PGSectAround[8] = GT1_GetPGSector(pos);
  if (PGSectAround[8] == NULL)
    if (!GT1_LoadPGSector(pos))
      PGSectAround[8] = GT1_AddPGSector(pos,SEALEVEL-1);
    else
	  PGSectAround[8] = GT1_GetPGSector(pos);
 
  if (PGSectAround[8]->status >= G_PGS_STATUS_FULLGEN)
    return;

  //Ajanced sectors
  g_ivec2 npos = g_ivec2(pos.x - 1,pos.y + 1);
  g_ivec2 sectdir[4] = {g_ivec2(1,0),g_ivec2(0,-1),g_ivec2(-1,0),g_ivec2(0,1)};
 
  for (g_int32 j = 0; j<4; j++)
	for (g_int32 k = 0; k<2; k++)
	{
	  npos.x += sectdir[j].x;
	  npos.y += sectdir[j].y;
	  g_int32 index = j*2+k;
	  PGSectAround[index] = GT1_GetPGSector(npos);
	  if (PGSectAround[index] == NULL)
        if (!GT1_LoadPGSector(npos))
          PGSectAround[index] = GT1_AddPGSector(npos,SEALEVEL-1);
        else
	      PGSectAround[index] = GT1_GetPGSector(npos);
	}
  //
  if (PGSectAround[8]->status == G_PGS_STATUS_NONGEN) 
  {
	// Generate
	for (g_uint16 i = 0; i <NCUBESXZ; i++)
      for (g_uint16 j = 0; j <NCUBESXZ; j++)
	  {
	    PGSectAround[8]->heightmap[i][j] += rand() % 5 - 3;
	  }

	g_uint16 nh = rand()%5 + 1;
	for (g_uint16 i = 0; i <nh; i++)
    {
	  g_ivec2 rp = g_ivec2((rand()%NCUBESXZ)+NCUBESXZ,(rand()%NCUBESXZ)+NCUBESXZ);
	  g_int32 h = rand() % 25 + 10;

	  for (g_int32 j = (rp.x - h); j <= (rp.x + h); j++)    
	    for (g_int32 k = (rp.y - h); k <= (rp.y + h); k++)
	    {
	  	  g_float dx = g_float(rp.x - j)*g_float(rp.x - j);
	 	  g_float dy = g_float(rp.y - k)*g_float(rp.y - k);
		  g_float dist = sqrt(dx+dy) + rand() % 10 - 5;
		  if (dist <= h)
		  {
		    g_int16 sind;
			g_ivec2 cpos;
			GT1_PrevCoordToSect(g_ivec2(j,k),&sind,&cpos);
			PGSectAround[sind]->heightmap[cpos.x][cpos.y] += g_int16(h/6 - dist/6 + 1);
		  }
	    }
    } 
	// End Generate
	PGSectAround[8]->status = G_PGS_STATUS_PARTGEN;
  }

  if (fullgen)
    if (PGSectAround[8]->status == G_PGS_STATUS_PARTGEN) 
	{	  
	  for (g_int32 i = 0; i<8; i++)
		GT1_GenerateSectorPreview(PGSectAround[i]->pos,false);

	  GT1_SmoothHeightMap(PGSectAround[8]->heightmap);
	  GT1_SmoothHeightMap(PGSectAround[8]->heightmap);

      PGSectAround[8]->status = G_PGS_STATUS_FULLGEN;
	}
}

//-----------------------------------------------

void G_CT_MapGenMgr::GT1_GenerateSectorRelease(g_ivec3 pos)
{
  GCubeSector *NewSector = new GCubeSector(pos);
  NewSector->BoundBox.SetMinMax((pos*CUBE_SECTOR_SIZE).AsVec3(),((pos+1)*CUBE_SECTOR_SIZE).AsVec3());
  NewSector->FillInfo(0);
  g_int16 rpy = pos.y+MAPDEEP-1;
  if (rpy > 1)
  {
    NewSector->FillCubes(0);
    for (g_int32 i = 0; i<NCUBESXZ; i++)
	  for (g_int32 k = 0; k<NCUBESXZ; k++)
	    for (g_int32 j = 0; j<NCUBESY; j++) 
	    {       
		  NewSector->Cube(i,j,k) = GT1_GetCubeFromPreview(pos,g_ivec3(i,j,k));
		  if (NewSector->Cube(i,j,k) == 0) 
		    break;
		  else
		    NewSector->Empty = false;
	    }
  }
  else
  {
    NewSector->FillCubes(50);
    NewSector->Empty = false;
    for (g_int32 i = 0; i<NCUBESXZ; i++)
	  for (g_int32 k = 0; k<NCUBESXZ; k++)
	    for (g_int32 j = NCUBESY-1; j>=0; j--) 
	    {       
		  NewSector->Cube(i,j,k) = GT1_GetCubeFromPreview(pos,g_ivec3(i,j,k));
		  if (NewSector->Cube(i,j,k) == 50) 
		    break;
	    }
  }
      
  pSectMgr->Sectors.push_back(NewSector);  
}

//-----------------------------------------------

void G_CT_MapGenMgr::GT1_GenerateSectorClear(g_ivec3 pos)
{
  GCubeSector *NewSector = new GCubeSector(pos);
  NewSector->BoundBox.SetMinMax((pos*CUBE_SECTOR_SIZE).AsVec3(),((pos+1)*CUBE_SECTOR_SIZE).AsVec3());

  NewSector->FillInfo(0);
  NewSector->FillCubes(0);

  pSectMgr->Sectors.push_back(NewSector);  
}

//-----------------------------------------------

g_uint16 G_CT_MapGenMgr::GT1_GetCubeFromPreview(g_ivec3 sectorpos, g_ivec3 cubepos)
{
  g_int16 rpx = cubepos.x;
  g_int16 rpz = cubepos.z;
  g_int16 rpy = (sectorpos.y+MAPDEEP-1)*NCUBESY + cubepos.y;

  GSectorPreGen* PGSect = GT1_GetPGSector(g_ivec2(sectorpos.x,sectorpos.z));

  if (PGSect->heightmap[rpx][rpz] < rpy)
  {
	if (rpy < SEALEVEL)
	  return 27;
	else
    {
      if ((PGSect->heightmap[rpx][rpz] > SEALEVEL) && ((rpy-PGSect->heightmap[rpx][rpz])==1))
        return (rand()%20 == 10 ? (139+rand()%5) : 0);
	  return 0;
    }
  }

  if (PGSect->heightmap[rpx][rpz] == rpy) 
  {
	if (rpy < (145 + rand()% 10))
	  return 51;

	if (rpy < (165 + rand()% 10))
	  return 50;

	return 133;
  }

  if (PGSect->heightmap[rpx][rpz] > rpy) return 50;

  return 0;
}

//-----------------------------------------------

GSectorPreGen* G_CT_MapGenMgr::GT1_AddPGSector(g_ivec2 pos, g_uint16 initvalue)
{
  GSectorPreGen* NewPGS = new GSectorPreGen;
  NewPGS->pos = pos;
  NewPGS->status = G_PGS_STATUS_NONGEN;
  
  NewPGS->heightmap = new g_uint16* [NCUBESXZ];

  for (g_int16 i = 0; i<NCUBESXZ; i++)
	NewPGS->heightmap[i] = new g_uint16 [NCUBESXZ];
  
  for (g_int16 i = 0; i<NCUBESXZ; i++)
	for (g_int16 j = 0; j<NCUBESXZ; j++)
	{
	  NewPGS->heightmap[i][j] = initvalue;
	}
  PGSectors.push_back(NewPGS);
  return NewPGS;
}

//-----------------------------------------------

g_bool G_CT_MapGenMgr::GT1_LoadPGSector(g_ivec2 pos)
{
  return false; //Fix It
}

//-----------------------------------------------

void G_CT_MapGenMgr::GT1_SavePGSector(g_ivec2 pos)
{
  //Fix It
}

//-----------------------------------------------

void G_CT_MapGenMgr::GT1_RemovePGSector(g_ivec2 pos)
{
  g_int32 pgsind = GT1_GetPGSectorIndex(pos);
  if (pgsind > -1)
  {
	for (g_int16 i = 0; i<NCUBESXZ; i++)
	  delete[]PGSectors[pgsind]->heightmap[i];

	delete[]PGSectors[pgsind]->heightmap;

	delete PGSectors[pgsind];
	PGSectors.erase(PGSectors.begin()+pgsind);
  }
}

//-----------------------------------------------

void G_CT_MapGenMgr::GT1_RemoveAllPGSectors()
{
  for (g_uint32 j = 0; j<PGSectors.size(); j++)
  {
	for (g_int16 i = 0; i<NCUBESXZ; i++)
	  delete[]PGSectors[j]->heightmap[i];

	delete[]PGSectors[j]->heightmap;

	delete PGSectors[j];
  }
  PGSectors.clear();
}

//-----------------------------------------------

GSectorPreGen* G_CT_MapGenMgr::GT1_GetPGSector(g_ivec2 pos)
{
  for (g_uint32 i = 0; i<PGSectors.size(); i++)
  {
	if (PGSectors[i]->pos == pos)
	  return PGSectors[i];
  }
  return NULL;
}

//-----------------------------------------------

g_int32 G_CT_MapGenMgr::GT1_GetPGSectorIndex(g_ivec2 pos)
{
  for (g_uint32 i = 0; i<PGSectors.size(); i++)
  {
	if (PGSectors[i]->pos == pos)
	  return i;
  }
  return -1;
}

//-----------------------------------------------

g_float G_CT_MapGenMgr::GT1_GetMapHeightInPos(g_vec3 &pos)
{
  g_ivec3 cpos, spos;
  GetSectorAndCubePosByVec3(pos, spos, cpos);

  GSectorPreGen *pgs = GT1_GetPGSector(g_ivec2(spos.x,spos.z));
  
  if (pgs == NULL)
    return default_height;

  return g_float(-360 + pgs->heightmap[cpos.x][cpos.z]*csize.y);
}

//-----------------------------------------------
// Ограниченая генерация
//-----------------------------------------------

void G_CT_MapGenMgr::InitMap2(g_ivec2 &map_size, g_bool UseWater)
{
  // Генерируем карту высот

  // Генерируем сектора

  // Расставляем деревья, траву, камни

  // Выравниваем местность под здания и отмечаем куда ставить (?)

  // Генерируем сектора устанавливаем объекты и сохраняем в паки 
  // по SAVE_SECT_SIDE x SAVE_SECT_SIDE x MAPDEEP секторов
  // Сохраняем информацию о карте

  // Устанавливаем персонажей
  // Сохраняем информацию о персонажах

  // Загружаем сектора в области видимости

  // Инициализируем другие игровые переменные

  /*---------------------------------------------*/
  if ((map_size.x < 1) || (map_size.y < 1)) return;
  if ((map_size.x > MAX_MAP_SIZE) || (map_size.y > MAX_MAP_SIZE)) return;

  _gentype = 2;

  // Инициализируем карту высот
  GT2_CreateHeightMap(map_size);

  // Генерируем карту высот
  //GT2_GenerateHeightMap(G_HMF_GEN_CLEAR);
  // или загружаем
  GT2_LoadHeightMap("..\\Maps\\hm.ghm");
  
  // Применяем ее к кубикам
  GT2_SetHeightMapToSectors(UseWater);

  //Генерируем хэш-таблицу для поиска сектора
  pSectMgr->GenerateSectorsHASHTable();

  //Генерация деревьев // Переделать!!!
  /*
  for (g_uint32 ntrees = 0; ntrees<2000; ntrees++)
  {
    g_ivec2 sect_offset = g_ivec2(center_sector_xy.x - _map_size.x/2, 
                                  center_sector_xy.y - _map_size.y/2);
    g_int32 sx = rand()%_map_size.x;
    g_int32 sz = rand()%_map_size.y;
    GCubeSector *Sector = pSectMgr->GetSector(g_ivec3(sx + sect_offset.x, -3, sz + sect_offset.y));
    g_int32 rx = rand()%NCUBESXZ;
    g_int32 ry = rand()%NCUBESXZ;    
    GenTree(Sector,g_ivec3(rx,_sect_heightmap[sx][sz].heightmap[rx][ry].height-NCUBESXZ*2,ry),7,0);
  }
  */
}

//-----------------------------------------------

void G_CT_MapGenMgr::InitMap2Clear(g_ivec2 &map_size)
{
  if ((map_size.x < 1) || (map_size.y < 1)) return;
  if ((map_size.x > MAX_MAP_SIZE) || (map_size.y > MAX_MAP_SIZE)) return;

  _gentype = 2;

  // Инициализируем карту высот
  GT2_CreateHeightMap(map_size);

  // Применяем ее к кубикам
  GT2_CreateClearSectors();

  //Генерируем хэш-таблицу для поиска сектора
  pSectMgr->GenerateSectorsHASHTable();
}

//-----------------------------------------------

void G_CT_MapGenMgr::InitMapAutoSize(g_bool UseWater)
{
  _gentype = 2;
  // Загружаем карту высот
  GT2_LoadHeightMap("..\\Maps\\hm.ghm");
  
  // Применяем ее к кубикам
  GT2_SetHeightMapToSectors(UseWater);

  //Генерируем хэш-таблицу для поиска сектора
  pSectMgr->GenerateSectorsHASHTable();
}

//-----------------------------------------------

void G_CT_MapGenMgr::GT2_CreateHeightMap(g_ivec2 &size)
{
  _map_size = size;
  _full_map_size = g_ivec2(size.x*NCUBESXZ,size.y*NCUBESXZ);

  _heightmap = new GMapCell*[_full_map_size.x];
  for (g_int32 i = 0; i<_full_map_size.x; i++)
    _heightmap[i] = new GMapCell[_full_map_size.y];

  _sect_heightmap = new GMapSect*[_map_size.x];
  for (g_int32 i = 0; i<_map_size.x; i++)
    _sect_heightmap[i] = new GMapSect[_map_size.y];

  for (g_int32 x = 0; x<_map_size.x; x++)
    for (g_int32 y = 0; y<_map_size.y; y++)
      for (g_int32 hx = 0; hx<NCUBESXZ; hx++)
        _sect_heightmap[x][y].heightmap[hx] = &_heightmap[x*NCUBESXZ+hx][y*NCUBESXZ];
}

//-----------------------------------------------

void G_CT_MapGenMgr::GT2_GenerateHeightMap(g_uint8 flag)
{
  switch (flag)
  {
    case G_HMF_GEN_TYPE1:
        {

        }
        break;
    case G_HMF_GEN_TYPE2:
        {

        }
        break;
    default: break;
  }
}

//-----------------------------------------------

g_bool G_CT_MapGenMgr::GT2_LoadHeightMap(string filename)
{
  GFileStream hmFile;

  if (hmFile.Create(filename,G_FSMODE_READ))
  {
    G_HM_Header hmHeader;
    hmFile.Read(&hmHeader,sizeof(hmHeader));

	if (_heightmap == NULL)
	  GT2_CreateHeightMap(g_ivec2(hmHeader.xsize/cubes_in_sec.x, hmHeader.ysize/cubes_in_sec.x));

    g_uint8 *Buffer = new g_uint8[hmHeader.ysize];

	memset(Buffer,0,hmHeader.ysize);

    for (g_int32 i = 0; i<g_int32(hmHeader.xsize); i++)
    {
      hmFile.Read(Buffer, hmHeader.ysize);
      if (i >= _full_map_size.x) continue;
      for (g_int32 j = 0; j<g_int32(hmHeader.ysize); j++)
      {
        if (j >= _full_map_size.y) break;
        _heightmap[i][j].height = Buffer[j];
      }
    }

	memset(Buffer,0,hmHeader.ysize);

    if (hmHeader.areamap)
    {
      for (g_int32 i = 0; i<g_int32(hmHeader.xsize); i++)
      {
        hmFile.Read(Buffer, hmHeader.ysize);
        if (i >= _full_map_size.x) continue;
        for (g_int32 j = 0; j<g_int32(hmHeader.ysize); j++)
        {
          if (j >= _full_map_size.y) break;
          _heightmap[i][j].areatype = Buffer[j];
        }
      }
    }
    else
    {
      for (g_int32 i = 0; i<g_int32(hmHeader.xsize); i++)
      {
        if (i >= _full_map_size.x) break;
        for (g_int32 j = 0; j<g_int32(hmHeader.ysize); j++)
        {
          if (j >= _full_map_size.y) break;
          _heightmap[i][j].areatype = 0;
        }
      }
    }
    
	delete[]Buffer;
    hmFile.Release();
  }
  else
    return false;
  
  return true;
}

//-----------------------------------------------

g_bool G_CT_MapGenMgr::GT2_SaveHeightMap(string filename, g_bool save_areamap)
{
  GFileStream hmFile;

  if (hmFile.Create(filename,G_FSMODE_WRITE))
  {
    G_HM_Header hmHeader;
    memset(&hmHeader,0,sizeof(hmHeader));
    hmHeader.xsize = _full_map_size.x;
    hmHeader.ysize = _full_map_size.y;
    hmHeader.areamap = save_areamap;
    hmFile.Write(&hmHeader,sizeof(hmHeader));
    
    g_uint8 *Buffer = new g_uint8[hmHeader.ysize];

    for (g_int32 i = 0; i<g_int32(hmHeader.xsize); i++)
    {
      for (g_int32 j = 0; j<g_int32(hmHeader.ysize); j++)
        Buffer[j] = _heightmap[i][j].height;

      hmFile.Write(Buffer, hmHeader.ysize);
    }

    if (hmHeader.areamap)
      for (g_int32 i = 0; i<g_int32(hmHeader.xsize); i++)
      {      
        for (g_int32 j = 0; j<g_int32(hmHeader.ysize); j++)
          Buffer[j] = _heightmap[i][j].areatype;

        hmFile.Write(Buffer, hmHeader.ysize);
      }
 
    hmFile.Release();
  }
  else
    return false;
  
  return true;
}

//-----------------------------------------------

void G_CT_MapGenMgr::GT2_ReleaseHeightMap()
{
  if (_heightmap != NULL)
  {
    for (g_int32 i = 0; i<_full_map_size.x; i++)
      delete[]_heightmap[i];

    delete[]_heightmap;
    _heightmap = NULL;
  }

  if (_sect_heightmap != NULL)
  {
    for (g_int32 i = 0; i<_map_size.x; i++)
      delete[]_sect_heightmap[i];

    delete[]_sect_heightmap;
    _sect_heightmap = NULL;
  }
}

//-----------------------------------------------

void G_CT_MapGenMgr::GT2_SetHeightMapToSectors(g_bool UseWater)
{
  if ((_heightmap == NULL) || (_sect_heightmap == NULL)) return;

  g_ivec2 sect_offset = g_ivec2(center_sector_xy.x - _map_size.x/2, 
                                center_sector_xy.y - _map_size.y/2);

  for (g_int32 sx = 0; sx<_map_size.x; sx++)
  {
    for (g_int32 sz = 0; sz<_map_size.y; sz++)
    {
      GMapSect *current_sect = &_sect_heightmap[sx][sz];
      for (g_int32 sy = MAPDEEP - 1; sy>=0; sy--)
      {
        g_ivec3 pos = g_ivec3(sx + sect_offset.x, -sy, sz + sect_offset.y);
        GCubeSector *NewSector = new GCubeSector(pos);
        NewSector->BoundBox.SetMinMax((pos*CUBE_SECTOR_SIZE).AsVec3(),((pos+1)*CUBE_SECTOR_SIZE).AsVec3());
        NewSector->InitCubes();

        g_int32 offs_y = (MAPDEEP-sy-1)*NCUBESY;
        
        if (sy < 4)
        {
          for (g_int32 cx = 0; cx<NCUBESXZ; cx++)      
            for (g_int32 cz = 0; cz<NCUBESXZ; cz++)
              for (g_int32 cy = 0; cy<NCUBESY; cy++)
              {
                g_uint16 _cube = GT2_GetCubeFromMapCell(&current_sect->heightmap[cx][cz],offs_y + cy,UseWater);
                NewSector->Cube(cx,cy,cz) = _cube;
                if (_cube == 0) 
                  break;
                else
		          NewSector->Empty = false;
              }
        }
        else
        {
          NewSector->FillCubes(50);
          NewSector->Empty = false;
          for (g_int32 cx = 0; cx<NCUBESXZ; cx++)      
            for (g_int32 cz = 0; cz<NCUBESXZ; cz++)
              for (g_int32 cy = NCUBESY-1; cy>=0; cy--)
              {
                g_uint16 _cube = GT2_GetCubeFromMapCell(&current_sect->heightmap[cx][cz],offs_y + cy,UseWater);
                NewSector->Cube(cx,cy,cz) = _cube;
                if (_cube == 50) 
                  break;        		                 
              }
        }
        
        if (NewSector->Empty) NewSector->ReleaseCubes();

        pSectMgr->Sectors.push_back(NewSector);  
      }
    }
  }
}

//-----------------------------------------------

void G_CT_MapGenMgr::GT2_CreateClearSectors()
{
  g_ivec2 sect_offset = g_ivec2(center_sector_xy.x - _map_size.x/2, 
                                center_sector_xy.y - _map_size.y/2);

  for (g_int32 sx = 0; sx<_map_size.x; sx++)
    for (g_int32 sz = 0; sz<_map_size.y; sz++)
      for (g_int32 sy = MAPDEEP - 1; sy>=0; sy--)
      {
        g_ivec3 pos = g_ivec3(sx + sect_offset.x, -sy, sz + sect_offset.y);
        GCubeSector *NewSector = new GCubeSector(pos);
        NewSector->BoundBox.SetMinMax((pos*CUBE_SECTOR_SIZE).AsVec3(),((pos+1)*CUBE_SECTOR_SIZE).AsVec3());
        pSectMgr->Sectors.push_back(NewSector);  
      }
}

//-----------------------------------------------

g_uint16 G_CT_MapGenMgr::GT2_GetCubeFromMapCell(GMapCell *mapcell, g_int32 abs_y_cpos, g_bool UseWater)
{
  //mapcell->height = 0;
  if (mapcell->height < abs_y_cpos)
  {
    if (UseWater)
    {
	  if (abs_y_cpos < SEALEVEL)
	    return 27;

      if ((mapcell->height > SEALEVEL) && ((abs_y_cpos-mapcell->height)==1))
        return (rand()%20 == 10 ? (139+rand()%5) : 0);
    }
    else
    {
      if ((abs_y_cpos-mapcell->height)==1)
        return (rand()%20 == 10 ? (139+rand()%5) : 0);
    }
	return 0;
  }

  if (mapcell->height == abs_y_cpos) 
  {
	if (abs_y_cpos < (145 + rand()% 10))
	  return _areas[mapcell->areatype].object_id;

	if (abs_y_cpos < (165 + rand()% 10))
    {
      if (mapcell->areatype == 0)
	    return 50;
      else
        return _areas[mapcell->areatype].object_id;
    }

	return 133;
  }

  if (mapcell->height > abs_y_cpos)
  { 
    g_int32 deep = _areas[mapcell->areatype].deep_max - _areas[mapcell->areatype].deep_min + 1;
    if (deep < 1) deep = 1;

    if ((mapcell->areatype > 0) && (mapcell->height-abs_y_cpos <= rand()%deep))
	  return _areas[mapcell->areatype].object_id;
    else
      return 50;
  }

  return 0;
}

//-----------------------------------------------

g_float G_CT_MapGenMgr::GT2_GetMapHeightInPos(g_vec3 &pos)
{
  if (_heightmap == NULL) return rand()%20 + default_height;

  g_ivec2 cPos;

  g_ivec2 sect_offset = g_ivec2(center_sector_xy.x - _map_size.x/2, 
                                center_sector_xy.y - _map_size.y/2);

  cPos.x = g_int32(floor(pos.x/csize.x)) - sect_offset.x*NCUBESXZ;
  cPos.y = g_int32(floor(pos.z/csize.z)) - sect_offset.y*NCUBESXZ;

  if ((cPos.x < 0) || (cPos.x >= _full_map_size.x) ||
      (cPos.y < 0) || (cPos.y >= _full_map_size.y))
    return default_height;

  return g_float(-720 + _heightmap[cPos.x][cPos.y].height*csize.y); // <-?
}

//-----------------------------------------------
