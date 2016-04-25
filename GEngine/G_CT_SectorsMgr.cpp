#include "G_CT_SectorsMgr.h"

//-----------------------------------------------

G_CT_SectorsMgr::G_CT_SectorsMgr(void)
{
  SectCoordHASHToIndex = NULL;
  sect_in_axis = g_ivec3(0,0,0);
  sect_coord_offset = sect_in_axis;
  smin = g_ivec3(0,0,0);
  smax = g_ivec3(0,0,0);
}

//-----------------------------------------------

G_CT_SectorsMgr::~G_CT_SectorsMgr(void)
{
  FREENULL_VAR(SectCoordHASHToIndex);
}

//-----------------------------------------------

GCubeSector* G_CT_SectorsMgr::GetSector(g_ivec3 spos)
{
  if ((spos.x<smin.x) || (spos.x>smax.x) ||
      (spos.y<smin.y) || (spos.y>smax.y) ||
      (spos.z<smin.z) || (spos.z>smax.z)) return NULL;

  if (SectCoordHASHToIndex == NULL) return NULL;

  g_int32 iHASH = g_uint32(((spos.x+sect_coord_offset.x)*sect_in_axis.y + 
                            (spos.y+sect_coord_offset.y))*sect_in_axis.z + 
                            (spos.z+sect_coord_offset.z));
  
  return Sectors[SectCoordHASHToIndex[iHASH]];
}

//-----------------------------------------------

g_int32 G_CT_SectorsMgr::GetSectorIndex(g_ivec3 spos)
{
  if ((spos.x<smin.x) || (spos.x>smax.x) ||
      (spos.y<smin.y) || (spos.y>smax.y) ||
      (spos.z<smin.z) || (spos.z>smax.z)) return -1;

  if (SectCoordHASHToIndex == NULL) return -1;

  g_uint32 iHASH = g_uint32(((spos.x+sect_coord_offset.x)*sect_in_axis.y + 
                             (spos.y+sect_coord_offset.y))*sect_in_axis.z + 
                            (spos.z+sect_coord_offset.z));
  
  return SectCoordHASHToIndex[iHASH];
}

//-----------------------------------------------

g_uint16& G_CT_SectorsMgr::GetAbsCube(const g_ivec3 &abs_cpos, g_bool real)
{
  g_ivec3 spos, cpos;
  GetSectorAndCubePosByAbsCubePos(abs_cpos, spos, cpos);
  return GetSector(spos)->Cube(cpos, real);
}

//-----------------------------------------------

void G_CT_SectorsMgr::GenerateSectorsHASHTable()
{
  g_int32 scount = Sectors.size();

  if (SectCoordHASHToIndex!=NULL) delete[]SectCoordHASHToIndex;
  SectCoordHASHToIndex = NULL;

  sect_in_axis = g_ivec3(0,0,0);
  sect_coord_offset = sect_in_axis;
  
  if (scount > 0)
  {
    SectCoordHASHToIndex = new g_uint32[scount];
    smin = g_ivec3(G_MAX_INT,G_MAX_INT,G_MAX_INT);
    smax = g_ivec3(G_MIN_INT,G_MIN_INT,G_MIN_INT);
    for (g_int32 i = 0; i<scount; i++)
    {
      if (Sectors[i]->Position.x > smax.x) smax.x = Sectors[i]->Position.x;
      if (Sectors[i]->Position.y > smax.y) smax.y = Sectors[i]->Position.y;
      if (Sectors[i]->Position.z > smax.z) smax.z = Sectors[i]->Position.z;

      if (Sectors[i]->Position.x < smin.x) smin.x = Sectors[i]->Position.x;
      if (Sectors[i]->Position.y < smin.y) smin.y = Sectors[i]->Position.y;
      if (Sectors[i]->Position.z < smin.z) smin.z = Sectors[i]->Position.z;
    }

    sect_in_axis = smax - smin + 1;
    sect_coord_offset = g_ivec3(0,0,0) - smin;

    for (g_int32 i = 0; i<scount; i++)
    {
      g_int32 HASHIndex = (Sectors[i]->Position.x+sect_coord_offset.x)*sect_in_axis.y*sect_in_axis.z + 
                          (Sectors[i]->Position.y+sect_coord_offset.y)*sect_in_axis.z + 
                          (Sectors[i]->Position.z+sect_coord_offset.z);
      SectCoordHASHToIndex[HASHIndex] = i;
    }
  }
}

//-----------------------------------------------

void G_CT_SectorsMgr::Init(G_CT_Objects *pObjects)
{
  _p_objects = pObjects;
}

//-----------------------------------------------

void G_CT_SectorsMgr::Release()
{
  for (g_uint32 i = 0; i<Sectors.size(); i++)
	delete Sectors[i];
  Sectors.clear();

  FREENULL_VAR(SectCoordHASHToIndex);
  sect_in_axis = g_ivec3(0,0,0);
  sect_coord_offset = sect_in_axis;
  smin = g_ivec3(0,0,0);
  smax = g_ivec3(0,0,0);
}

//-----------------------------------------------

void G_CT_SectorsMgr::CheckSectorsAdj()
{
  for (g_uint32 i = 0; i<Sectors.size(); i++)
  {
    GCubeSector *Sector = Sectors[i];
    Sector->AdjSect.nxs = GetSector(g_ivec3(Sector->Position.x-1,Sector->Position.y,Sector->Position.z));
    Sector->AdjSect.pxs = GetSector(g_ivec3(Sector->Position.x+1,Sector->Position.y,Sector->Position.z));
    Sector->AdjSect.nys = GetSector(g_ivec3(Sector->Position.x,Sector->Position.y-1,Sector->Position.z));
    Sector->AdjSect.pys = GetSector(g_ivec3(Sector->Position.x,Sector->Position.y+1,Sector->Position.z));
    Sector->AdjSect.nzs = GetSector(g_ivec3(Sector->Position.x,Sector->Position.y,Sector->Position.z-1));
    Sector->AdjSect.pzs = GetSector(g_ivec3(Sector->Position.x,Sector->Position.y,Sector->Position.z+1));
  }
}

//-----------------------------------------------

GCubeInfo& G_CT_SectorsMgr::GetAdjCube(GCubeSector *Sector, g_ivec3 curr_cpos, g_uint8 axis, 
				                       g_uint16 &adj_cube, g_bool SetUpdateFlag, g_bool Real)
{
  adj_cube = G_INVALID_CUBE;
  switch (axis)
  {
    case 0: // x+
	  if (curr_cpos.x == (cubes_in_sec.x-1))
	  {
		if (Sector->AdjSect.pxs != NULL)
		{
		  if (SetUpdateFlag) Sector->AdjSect.pxs->Changed = true;
		  adj_cube = Sector->AdjSect.pxs->Cube(0,curr_cpos.y,curr_cpos.z, Real);
		  return Sector->AdjSect.pxs->Info(0,curr_cpos.y,curr_cpos.z, Real);
		}
	  }
      else
	  {
		adj_cube = Sector->Cube(curr_cpos.x+1,curr_cpos.y,curr_cpos.z, Real);
		return Sector->Info(curr_cpos.x+1,curr_cpos.y,curr_cpos.z, Real);
	  }
	  break;

	case 1: // x-
	  if (curr_cpos.x == 0)
	  {
		if (Sector->AdjSect.nxs != NULL)
		{
		  if (SetUpdateFlag) Sector->AdjSect.nxs->Changed = true;
		  adj_cube = Sector->AdjSect.nxs->Cube(cubes_in_sec.x-1,curr_cpos.y,curr_cpos.z, Real);
		  return Sector->AdjSect.nxs->Info(cubes_in_sec.x-1,curr_cpos.y,curr_cpos.z, Real);
		}
	  }
      else
	  {
		adj_cube = Sector->Cube(curr_cpos.x-1,curr_cpos.y,curr_cpos.z, Real);
		return Sector->Info(curr_cpos.x-1,curr_cpos.y,curr_cpos.z, Real);
	  }
	  break;

	case 2: // y+
	  if (curr_cpos.y == (cubes_in_sec.y-1))
	  {
	    if (Sector->AdjSect.pys != NULL)
		{
		  if (SetUpdateFlag) Sector->AdjSect.pys->Changed = true;
		  adj_cube = Sector->AdjSect.pys->Cube(curr_cpos.x,0,curr_cpos.z, Real);
		  return Sector->AdjSect.pys->Info(curr_cpos.x,0,curr_cpos.z, Real);
		}
	  }
      else
	  {
		adj_cube = Sector->Cube(curr_cpos.x,curr_cpos.y+1,curr_cpos.z, Real);
		return Sector->Info(curr_cpos.x,curr_cpos.y+1,curr_cpos.z, Real);
	  }
	  break;

    case 3: //y-
	  if (curr_cpos.y == 0)
	  {
		if (Sector->AdjSect.nys != NULL)
		{
		  if (SetUpdateFlag) Sector->AdjSect.nys->Changed = true;
		  adj_cube = Sector->AdjSect.nys->Cube(curr_cpos.x,cubes_in_sec.y-1,curr_cpos.z, Real);
          return Sector->AdjSect.nys->Info(curr_cpos.x,cubes_in_sec.y-1,curr_cpos.z, Real);
		}
	  }
      else
	  {
		adj_cube = Sector->Cube(curr_cpos.x,curr_cpos.y-1,curr_cpos.z, Real);
		return Sector->Info(curr_cpos.x,curr_cpos.y-1,curr_cpos.z, Real);
	  }
	  break;

	case 4: // z+
	  if (curr_cpos.z == (cubes_in_sec.z-1))
	  {
		if (Sector->AdjSect.pzs != NULL)
		{
		  if (SetUpdateFlag) Sector->AdjSect.pzs->Changed = true;
		  adj_cube = Sector->AdjSect.pzs->Cube(curr_cpos.x,curr_cpos.y,0, Real);
          return Sector->AdjSect.pzs->Info(curr_cpos.x,curr_cpos.y,0, Real);
		}
	  }
      else
	  {
		adj_cube = Sector->Cube(curr_cpos.x,curr_cpos.y,curr_cpos.z+1, Real);
        return Sector->Info(curr_cpos.x,curr_cpos.y,curr_cpos.z+1, Real);
	  }
	  break;

	case 5: //z-
	  if (curr_cpos.z == 0)
	  {
		if (Sector->AdjSect.nzs != NULL)
		{
		  if (SetUpdateFlag) Sector->AdjSect.nzs->Changed = true;
		  adj_cube = Sector->AdjSect.nzs->Cube(curr_cpos.x,curr_cpos.y,cubes_in_sec.z-1, Real);
          return Sector->AdjSect.nzs->Info(curr_cpos.x,curr_cpos.y,cubes_in_sec.z-1, Real);
		}
	  }
      else
	  {
		adj_cube = Sector->Cube(curr_cpos.x,curr_cpos.y,curr_cpos.z-1, Real);
        return Sector->Info(curr_cpos.x,curr_cpos.y,curr_cpos.z-1, Real);
	  }
	  break;
  }

  _empty_info = 0;
  return *(GCubeInfo*)&_empty_info;
}

//-----------------------------------------------

void G_CT_SectorsMgr::CheckAllMapCubeAdj()
{
  for (g_uint32 i = 0; i<Sectors.size(); i++)
  {
    GCubeSector *Sector = Sectors[i];
    
    if (!Sector->Empty)
    {
      Sectors[i]->Changed = true;

      g_ivec3 cpos = g_ivec3(0,0,-1);

      for (g_int32 cind = 0; cind < cubescount; cind++)
      {
        cpos.z++;
        if (cpos.z >= NCUBESXZ) {cpos.y++; cpos.z -= NCUBESXZ;};
        if (cpos.y >= NCUBESY)  {cpos.x++; cpos.y -= NCUBESY;};

        g_uint16 curr_cube = Sector->Cube(cind, true);

		if (curr_cube == 0)
          continue;

		G_OBJECT &curr_obj = _p_objects->Objects[curr_cube-1];
		G_OBJECTTYPE &curr_ot = _p_objects->ObjTypes[curr_obj.ObjectType];

        Sector->SetAdjInfo(cind, 0x3F);

		if (curr_ot._fullvisible)
          continue;
            
		GCubeInfo &curr_info = Sector->Info(cind, true);
		g_uint8 curr_rot = curr_info.info_rot;

		for (g_uint32 axis = 0; axis < 6; axis++)
		  if (curr_ot._dockaxis[curr_rot][axis].ftype != 0)
		  {
			g_uint16 adj_cube;
			GCubeInfo &adj_info = GetAdjCube(Sector, cpos, axis, adj_cube, false, true);
			 
			if (adj_cube != 0)			
			  if (adj_cube != 65535)
              {
                G_OBJECT &adj_obj = _p_objects->Objects[adj_cube-1];
				G_OBJECTTYPE &adj_ot = _p_objects->ObjTypes[adj_obj.ObjectType]; 
				g_uint8 adj_axis = cAdjAxis[axis];
				g_uint8 adj_rot = adj_info.info_rot;
				g_uint8 conn_type = curr_ot._dockaxis[curr_rot][axis].ftype + adj_ot._dockaxis[adj_rot][adj_axis].ftype*4;
				g_bool invis = false;
				switch (conn_type)
				{
				  case 5: //1-1 
				  case 6: //2-1
				  case 7: //3-1
					  if (adj_obj.Properties.Transparency == false)
					    invis = true;
					  else if (curr_obj.Properties.Transparency == true)
						invis = (((axis % 2) == 0) || 
						         ((curr_obj.Properties.RemSameFaces) && (adj_cube == curr_cube)));
					  break;
				  case 10: //2-2
                  case 11: //3-2
					  if (curr_ot._dockaxis[curr_rot][axis].fsize <= adj_ot._dockaxis[adj_rot][adj_axis].fsize)
						invis = true;
					  break;
				  case 15: //3-3
					  if (curr_ot._dockaxis[curr_rot][axis].fhash64 == adj_ot._dockaxis[adj_rot][adj_axis].fhash64)
						invis = true;
					  break;
				}
				if (invis)
                  curr_info.info_adj -= cAdjTypes[axis];
			  }                
			  else
			    curr_info.info_adj -= cAdjTypes[axis];
		  } //if (curr_ot._dockaxis[curr_rot][axis].ftype != 0)
	     
		  if (curr_info.info_adj > 0)
			curr_info.info_adj = curr_info.info_adj;
      } //for (g_int32 cind = 0; ..
    } //if (!Sector->Empty)
  } //for (g_uint32 i = 0; ..
}

//-----------------------------------------------

void G_CT_SectorsMgr::CheckCubeAdj(GCubeSector *Sector, g_ivec3 cInd, g_bool Real)
{
  if (!Sector->Empty)
  {
	g_uint16 curr_cube = Sector->Cube(cInd, Real);

	GCubeInfo &curr_info = Sector->Info(cInd, Real);

	g_uint8 curr_rot = curr_info.info_rot;
	
	G_OBJECT *curr_obj = NULL;
	G_OBJECTTYPE *curr_ot = NULL;
	
	if (curr_cube != 0)
	{
	  curr_obj = &_p_objects->Objects[curr_cube-1];
	  curr_ot = &_p_objects->ObjTypes[curr_obj->ObjectType];
	}
    Sector->Changed = true;
    
    curr_info.info_adj = 0x3F;

	for (g_uint32 axis = 0; axis < 6; axis++)
	{
	  g_uint16 adj_cube;
	  GCubeInfo &adj_info = GetAdjCube(Sector, cInd, axis, adj_cube, true, Real);

	  if (adj_cube != 0)		
		if (adj_cube != 65535)
        {
		  G_OBJECT &adj_obj = _p_objects->Objects[adj_cube-1];
		  G_OBJECTTYPE &adj_ot = _p_objects->ObjTypes[adj_obj.ObjectType]; 
		  g_uint8 adj_axis = cAdjAxis[axis];
		  g_uint8 adj_rot = adj_info.info_rot;
		  adj_info.info_adj |= cAdjTypes[adj_axis];

		  if (curr_cube == 0) continue;

		  g_uint8 conn_type = curr_ot->_dockaxis[curr_rot][axis].ftype + adj_ot._dockaxis[adj_rot][adj_axis].ftype*4;
	      g_bool invis = false;
		  g_bool adj_invis = false;
		  
		  switch (conn_type)
		  {
			case 5: //1-1 
              if (curr_obj->Properties.Transparency == false)
			    adj_invis = true;
			  else if (adj_obj.Properties.Transparency == true)
				adj_invis = ((curr_obj->Properties.RemSameFaces) && (adj_cube == curr_cube));
			case 6: //2-1
			case 7: //3-1
			  if ((curr_obj->Properties.Transparency == true) || (adj_obj.Properties.Transparency == false))
			    invis = true;
			  break;
			case 9:  //1-2
			case 13: //1-3
              adj_invis = true;
			  break;
			case 10: //2-2
			  if (curr_ot->_dockaxis[curr_rot][axis].fsize >= adj_ot._dockaxis[adj_rot][adj_axis].fsize)
				adj_invis = true;
            case 11: //3-2
			  if (curr_ot->_dockaxis[curr_rot][axis].fsize <= adj_ot._dockaxis[adj_rot][adj_axis].fsize)
				invis = true;
			  break;
            case 14: //2-3
              if (adj_ot._dockaxis[adj_rot][adj_axis].fsize <= curr_ot->_dockaxis[curr_rot][axis].fsize)
				adj_invis = true;
			  break;
			case 15: //3-3
			  if (curr_ot->_dockaxis[curr_rot][axis].fhash64 == adj_ot._dockaxis[adj_rot][adj_axis].fhash64)
				invis = adj_invis = true;
			  break;
		  }
		  if (invis)
            curr_info.info_adj -= cAdjTypes[axis];
		  if (adj_invis)
            adj_info.info_adj -= cAdjTypes[adj_axis];
		}                
		else
		  curr_info.info_adj -= cAdjTypes[axis];
	} //for (g_uint32 axis = 0; ...
  } //if (!Sector->Empty)
}

//-----------------------------------------------