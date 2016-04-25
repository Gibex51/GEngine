#include "GCubeTerrain.h"

//-----------------------------------------------

g_uint32 ct_layout_elements = 3;

D3D10_INPUT_ELEMENT_DESC ct_vertex_layout[] =
{
  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,   D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "COLOR",    0, DXGI_FORMAT_R32_UINT,           0, 12,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
  { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 16,  D3D10_INPUT_PER_VERTEX_DATA, 0 }
};

//-----------------------------------------------

GCubeTerrain::GCubeTerrain(GD3DDevice *Device, GMaterial *UsedMaterial)
{
  pause_render = false;
  is_paused_render = false;
  _draw_dist = 9;
  //checkcollisions = false;

  ia_min_acpos = g_ivec3(0,0,0);
  ia_max_acpos = g_ivec3(0,0,0);

  SectorsMgr.Init(&Objects);
  SaveMgr.Init(&SectorsMgr);
  MapGenMgr.Init(&SectorsMgr);
  
  //Init Draw Variables
  _device = Device;
  _ct_material = UsedMaterial;

  _usedtech = UsedMaterial->GetTech("Render");
  _usedtech_sm = UsedMaterial->GetTech("SM_Render");
  
  _ct_il = _device->ILayoutList->AddInputLayout(UID_IL_CUBETERR, _usedtech->GetDXTech(), ct_vertex_layout, 
	                                            ct_layout_elements);

  //Load cube textures
  GC_SECTION *CTConfSect =  Device->ConfMgr->GetConfig(G_CUID_CUBE_TERRAIN)->GetSectionByName("TEXTURES"); 
  if (CTConfSect != NULL)
  {
	vector <GTexture*> CTTexArray;
    for (g_uint16 i = 0; i< CTConfSect->GetKeyByName("TexNumber")->AsInt(); i++)
    {
	  if ((UID_TEX_CT_FIRST + i) > UID_TEX_CT_LAST) break;
	  g_char TexName[256];
	  g_char TexFileName[256];
	  sprintf_s(TexName,256,"Tex%d",i);
	  sprintf_s(TexFileName,256,CTConfSect->GetKeyByName("TexPath")->Value,i);
	  CTTexArray.push_back(_device->ResMgr->AddTexture(UID_TEX_CT_FIRST + i, TexName, TexFileName, 1));
    }
    _ct_material->AddTextureArray(CTTexArray, 0);
  }

  //Default WorldName
  strcpy_s(WorldName,64,"New World\0");

  //Load Cube Objects and Models
  Objects.LoadCubeObjects("../Data/CubeObjects.gao");

  //Other inits
  s_vert = new G_CUBE_VERTEX[NCUBESXZ*NCUBESY*NCUBESXZ*4];
  s_ind  = new g_uint32[NCUBESXZ*NCUBESY*NCUBESXZ*18];
  s_face_inf = new G_OPTFACE_INFO[NCUBESXZ*NCUBESY*NCUBESXZ*3];

  //Init FaceArray Builder
  fa_builder = new GFaceArrayBuilder(MAXFACES,Device);
  SectorToOffset = new GOffset*[maxsectors];
  for (g_uint32 i = 0; i<maxsectors; i++)
    SectorToOffset[i] = NULL;

  //Physics
  //physics_plugin = NULL;
}

//-----------------------------------------------

GCubeTerrain::~GCubeTerrain(void)
{
  delete[]s_vert;
  delete[]s_ind;
  delete[]s_face_inf;
  delete[]SectorToOffset;
  delete fa_builder;
  Objects.Release();

  _device->ResMgr->RemoveResourcesByUIDRange(UID_TEX_CT_FIRST, UID_TEX_CT_LAST);
}

//-----------------------------------------------

void GCubeTerrain::Draw(g_mat4x4 &mWorld, GCamera *Camera, g_bool CheckDistance)
{ 
  _ct_material->Effect->GetVar(G_SVT_MWORLD, 0)->SetMatrix(mWorld);
  _ct_material->Effect->GetVar(G_SVT_MVP, 0)->SetMatrix(Camera->GetViewProjMatrix());
  _ct_material->Effect->GetVar(G_SVT_CUSTOM, 6)->SetRawData(Camera->GetPosition());
 
  _ct_il->Set();
  _device->SetPrimitiveTopology(G_PT_TRIANGLE_LIST);
   
  g_ivec3 cam_spos; 
  if (CheckDistance)
    GetSectorByVec3(Camera->GetPosition(), &cam_spos); 
  
  g_uint32 stride = sizeof(G_CUBE_VERTEX);
  g_uint32 offset = 0;

  for( g_uint32 p = 0; p < _usedtech->PassCount(); ++p )
  {     
    for (g_uint32 i = 0; i < SectorsMgr.Sectors.size(); i++)
    {      
      GCubeSector *Sector = SectorsMgr.Sectors[i];

	  if (!Sector->Empty)
	  {
        //Check Distance
	    if (CheckDistance)
          if ((abs(Sector->Position.x - cam_spos.x) > g_int32(_draw_dist)) || 
              (abs(Sector->Position.z - cam_spos.z) > g_int32(_draw_dist)))
            continue;

	    //Check In Frustum
	    if (!Camera->Frustum_CheckAABB(Sector->BoundBox)) continue;
  
	    ID3D10Buffer *_vb = _device->BufferList->GetBufferByID(i*2+1)->GetBuffer();
        ID3D10Buffer *_ib = _device->BufferList->GetBufferByID(i*2+2)->GetBuffer();
        
		//Render
		if ((_vb != NULL) && (_ib != NULL))
		{    
		  if (SectorToOffset[i] != NULL)
            fa_builder->SetOffsetToShader(SectorToOffset[i]->index);
       
		  _usedtech->ApplyPass(p);
          _device->D3DDevice->IASetVertexBuffers(0,1,&_vb,&stride,&offset);
          _device->D3DDevice->IASetIndexBuffer(_ib,DXGI_FORMAT_R32_UINT,0);
          _device->D3DDevice->DrawIndexed(Sector->icount, 0, 0 );
		}
	  }//if (!Sectors[i]->Empty)
    }//for (g_uint32 i = 0; i < Sectors.size(); i++)
  }//for( g_uint32 p = 0; p < techDesc.Passes; ++p )
}

//-----------------------------------------------

void GCubeTerrain::DrawSM(g_mat4x4 &mWorld, GCamera *Camera)
{ 
  _ct_material->Effect->GetVar(G_SVT_MWORLD, 0)->SetMatrix(mWorld);
  _ct_material->Effect->GetVar(G_SVT_MVP, 0)->SetMatrix(Camera->GetViewProjMatrix());

  _ct_il->Set();
  _device->SetPrimitiveTopology(G_PT_TRIANGLE_LIST);

  g_uint32 stride = sizeof(G_CUBE_VERTEX);
  g_uint32 offset = 0;

  for( g_uint32 p = 0; p < _usedtech_sm->PassCount(); ++p )
  {      	
    for (g_uint32 i = 0; i < SectorsMgr.Sectors.size(); i++)
    {      
      GCubeSector *Sector = SectorsMgr.Sectors[i];

	  if (!Sector->Empty)
	  {
	    if (!Camera->Frustum_CheckAABB(Sector->BoundBox)) continue;

	    ID3D10Buffer *_vb = _device->BufferList->GetBufferByID(i*2+1)->GetBuffer();
        ID3D10Buffer *_ib = _device->BufferList->GetBufferByID(i*2+2)->GetBuffer();
            
		if ((_vb != NULL) && (_ib != NULL))
		{        
		  if (SectorToOffset[i] != NULL)
            fa_builder->SetOffsetToShader(SectorToOffset[i]->index);

          _usedtech_sm->ApplyPass(p);
          _device->D3DDevice->IASetVertexBuffers(0,1,&_vb,&stride,&offset);
          _device->D3DDevice->IASetIndexBuffer(_ib,DXGI_FORMAT_R32_UINT,0);
          _device->D3DDevice->DrawIndexed(Sector->icount, 0, 0 );
		}
	  }//if (!Sectors[i]->Empty)
    }//for (g_uint32 i = 0; i < Sectors.size(); i++)
  }//for( g_uint32 p = 0; p < techDesc.Passes; ++p )
}

//-----------------------------------------------

void GCubeTerrain::UpdateSectorsMesh(g_uint16 index, g_uint16 num)
{
  g_double ticks = GetTicks();
  //Start Check Load Speed
  
  for (g_uint32 i = index; i<g_uint32(index+num); i++)
  {
    if (SectorsMgr.Sectors[i]->Changed)
	  UpdateSectorMesh(i);
  }

  /*if (physics_plugin != NULL)
  {
    G_IndexVertexArray *GIVA = physics_plugin->GetIndexVertexArray(PHYS_IVA_UID);
    if (GIVA != NULL)
    {
      //GIVA->AddMesh
    }
  }*/

  fa_builder->BuildChanges();

  _ct_material->ApplyTexture(fa_builder->fa_buffer,1);

  //End Check Load Speed
  CheckTimeAndWriteToFile(ticks, "update_mesh");
}

//-----------------------------------------------

void GCubeTerrain::UpdateSectorsMesh()
{
  UpdateSectorsMesh(0, SectorsMgr.Sectors.size());
}

//-----------------------------------------------

void GCubeTerrain::UpdateSectorMesh(g_uint16 index)
{
  GCubeSector *Sector = SectorsMgr.Sectors[index];

  g_uint32 v_count = 0;
  g_uint32 i_count = 0;

  g_ivec3 abs_spos = Sector->Position*CUBE_SECTOR_SIZE;

  fa_elemtype *facebuffer = new fa_elemtype[cubescount*3];
  g_uint32 fbsize = 0;

  if (!Sector->Empty)
  {
    Sector->Empty = true;
    for (g_int32 cind = 0; cind<cubescount; cind++)    
	{
	  if (Sector->Cube(cind, true) != 0)
		Sector->Empty = false;
	  if (Sector->Cube(cind) != 0)
      {  
		//Add Indices
		g_uint8 curr_ot = Objects.Objects[Sector->Cube(cind)-1].ObjectType;
        G_OBJECTTYPE *currobj = &Objects.ObjTypes[curr_ot];

        if (Sector->GetAdjInfo(cind) == 0) continue;

        g_uint8 rot_info = Sector->GetRotInfo(cind);
		
		if (curr_ot == 0) rot_info = 0;

		for (g_int32 m = 0; m<(currobj->nIndices/6); m++)
		{
		  const GFaceInfo &curr_info = currobj->_faceinfo[rot_info][m];
		  
		  if ((curr_info.faxis == 0) || ((Sector->Info(cind).info_adj & cAdjTypes[curr_info.faxis-1]) == cAdjTypes[curr_info.faxis-1]))
		  {
		    for (g_int32 l=0; l<6; l++)
	          s_ind[i_count + l] = curr_info.findices[l] + v_count;
            i_count += 6;

			s_face_inf[fbsize].f_type = Sector->Cube(cind);
		    s_face_inf[fbsize].f_dir = curr_info.faxis;
		    s_face_inf[fbsize].used = (curr_ot == 0);	    
            facebuffer[fbsize] = Objects.Objects[Sector->Cube(cind)-1].Properties.FaceMainTex[m] + 
				            (g_uint32(Objects.Objects[Sector->Cube(cind)-1].Properties.FaceModTex[m])<<9) + 
						    currobj->_packednormals[rot_info][m*2];
            fbsize++;
		  }
		}
            
        //Add Vertices	
        memcpy(&s_vert[v_count],currobj->_vertices[rot_info],sizeof(G_CUBE_VERTEX)*currobj->nVertices);
       
        g_ivec3 cpos = Sector->IndToPos(cind);
        for (g_int32 l=0; l<currobj->nVertices; l++)
		{               
		  
		  s_vert[v_count+l].Pos.x = (s_vert[v_count+l].Pos.x + cpos.x)*csize.x + abs_spos.x;
          s_vert[v_count+l].Pos.y = (s_vert[v_count+l].Pos.y + cpos.y)*csize.y + abs_spos.y;
          s_vert[v_count+l].Pos.z = (s_vert[v_count+l].Pos.z + cpos.z)*csize.z + abs_spos.z;
		  if (curr_ot == 0)
			s_vert[v_count+l].Tex = s_vert[v_count+l].Tex + cpos.AsVec3();
        }
        v_count += currobj->nVertices;
      }//if (Sector->Cube(cind) != 0)
	}//for (g_int32 cind = 0; cind<cubescount; cind++)
  }//if (!Sector->Empty)
  
  if (Sector->Empty) Sector->ReleaseCubes();

  Sector->icount = i_count;

  if (!Sector->Empty)
  {
	//Decrease VB size to ~40-50%
	Optimize_VI_Buffers(s_vert, v_count, s_ind, i_count, facebuffer, fbsize, s_face_inf);
	Sector->icount = i_count;

	G_BUFFER *v_buf = _device->BufferList->CreateBuffer(G_BT_VERTEX,G_BF_DEFAULT,sizeof(G_CUBE_VERTEX),v_count,s_vert,index*2+1);
    G_BUFFER *i_buf = _device->BufferList->CreateBuffer(G_BT_INDEX,G_BF_DEFAULT,sizeof(g_uint32),i_count,s_ind,index*2+2);

	_device->BufferList->SetBuffer(v_buf);
	_device->BufferList->SetBuffer(i_buf);
  }
   
  if (SectorToOffset[index] == NULL)
    SectorToOffset[index] = fa_builder->AddOffset(facebuffer,fbsize);
  else
    fa_builder->UpdateOffset(SectorToOffset[index]->index,facebuffer,fbsize);
  
  delete[]facebuffer;
  
  Sector->Changed = false;
}

//-----------------------------------------------

void GCubeTerrain::Release()
{
  //Очистка списка отмен
  SaveMgr.ClearUndo("undo");

  MapGenMgr.Release(); 
  SectorsMgr.Release();
   
  for (g_uint32 i = 0; i<maxsectors; i++)
    SectorToOffset[i] = NULL;

  fa_builder->ClearOffsets();
}

//-----------------------------------------------
/*
g_bool GCubeTerrain::CheckCollisions(G_AABB PlayerAABB, g_vec3 PrevPlayerPosition, g_vec3 *NewPosition)
{
  if (!checkcollisions)
    return false;

  g_ivec3 cpos,spos;
  GetSectorAndCubePosByVec3(PlayerAABB.center,&spos,&cpos);
  
  GCubeSector *ThisSector = GetSector(spos);

  g_bool ni,ci,pi,nj,cj,pj,nk,ck,pk;
  g_ivec3 cubepos,sectpos;
  g_bool result = false;

  if (ThisSector != NULL)
  {
    for (g_int32 pass = 0; pass<3; pass++)
    for (g_int32 i = cpos.x-collideradius.x; i<=cpos.x+collideradius.x; i++)
      for (g_int32 j = cpos.y-collideradius.y; j<=cpos.y+collideradius.y; j++)
        for (g_int32 k = cpos.z-collideradius.z; k<=cpos.z+collideradius.z; k++)
        {     
          ni = i<0; ci = (i>=0) && (i<NCUBESXZ); pi = i>=NCUBESXZ;
          nj = j<0; cj = (j>=0) && (j<NCUBESY);  pj = j>=NCUBESY;
          nk = k<0; ck = (k>=0) && (k<NCUBESXZ); pk = k>=NCUBESXZ;
          
          cubepos = g_ivec3(i,j,k);       
          if (ni) cubepos.x +=NCUBESXZ;
          if (pi) cubepos.x -=NCUBESXZ;
          if (nj) cubepos.y +=NCUBESY;
          if (pj) cubepos.y -=NCUBESY;
          if (nk) cubepos.z +=NCUBESXZ;
          if (pk) cubepos.z -=NCUBESXZ;

          sectpos = spos;
          if (ni) sectpos.x -=1;
          if (pi) sectpos.x +=1;
          if (nj) sectpos.y -=1;
          if (pj) sectpos.y +=1;
          if (nk) sectpos.z -=1;
          if (pk) sectpos.z +=1;

          ThisSector = GetSector(sectpos);
          
          if (ThisSector != NULL)
          {
            g_uint16 cval = ThisSector->CubesW[cubepos.x][cubepos.y][cubepos.z];
            if ((cval != 0) && (Objects->Objects[cval-1].Properties.Colliding))
            {             
              g_uint8 rot_info = ThisSector->CubesInfo[cubepos.x][cubepos.y][cubepos.z];
              rot_info = (rot_info - rot_info%64)/64;

              G_OBJECTTYPE *OM = &Objects->ObjTypes[Objects->Objects[cval-1].ObjectType];
              for (g_int32 m = 0; m<OM->nIndices/3; m++)
              {
                g_vec3 p[3];
                for (g_int32 n = 0; n<3; n++)
                {
                    p[n].x = (OM->_vertices[rot_info][OM->_indices[m*3+n]].Pos.x + cubepos.x)*csize.x + sectpos.x*CUBE_SECTOR_SIZE;
                    p[n].y = (OM->_vertices[rot_info][OM->_indices[m*3+n]].Pos.y + cubepos.y)*csize.y + sectpos.y*CUBE_SECTOR_SIZE;
                    p[n].z = (OM->_vertices[rot_info][OM->_indices[m*3+n]].Pos.z + cubepos.z)*csize.z + sectpos.z*CUBE_SECTOR_SIZE;
                }

                g_vec3 ipoint;
                if (GetIntersectionPoint(OM->_facenormals[rot_info][m],p[0],PrevPlayerPosition,
                                         PlayerAABB.center,&ipoint))
                {
                  if (PointInTriangle(ipoint,p[0],p[1],p[2]))
                  {
                    g_float dpap = OM->_facenormals[rot_info][m].x*PlayerAABB.center.x + 
                                   OM->_facenormals[rot_info][m].y*PlayerAABB.center.y +
                                   OM->_facenormals[rot_info][m].z*PlayerAABB.center.z - 
                                   D3DXVec3Dot(&OM->_facenormals[rot_info][m],&p[0]);
                    if (dpap < 0)
                    {
                      PlayerAABB.center -= OM->_facenormals[rot_info][m]*dpap*1.1f;
                      result = true;
                    }
                  }
                }
              }             
            }
          }
        }
  }
  *NewPosition = PlayerAABB.center;
  return result;
}
*/
//-----------------------------------------------

void GCubeTerrain::RemoveCube(g_ivec3 &spos, g_ivec3 &cpos)
{
  g_int16 s_ind = SectorsMgr.GetSectorIndex(spos);
  if (s_ind >= 0)
  {
	g_ivec3 abs_cpos = GetAbsCubePosBySectorAndCubePos(cpos, spos);
	SaveMgr.SaveUndo("undo", abs_cpos, abs_cpos); 

    GCubeSector *Sector = SectorsMgr.Sectors[s_ind];
	Sector->Cube(cpos) = 0;
	Sector->Changed = true;
    SectorsMgr.CheckCubeAdj(Sector,cpos,true);
	UpdateSectorsMesh();
  }
}

//-----------------------------------------------

void GCubeTerrain::SetCube(g_ivec3 &spos, g_ivec3 &cpos, g_uint16 cType, g_uint8 cRot, g_bool UpdSector)
{
  if (cType == 0) return;
  g_int16 s_ind = SectorsMgr.GetSectorIndex(spos);
  if (s_ind >= 0)
  {
    if (cRot > 3) cRot = 0;
    GCubeSector *Sector = SectorsMgr.Sectors[s_ind];
    Sector->InitCubes();

	if (Sector->Cube(cpos) == cType)
	  return;

	g_ivec3 abs_cpos = GetAbsCubePosBySectorAndCubePos(cpos, spos);
	SaveMgr.SaveUndo("undo", abs_cpos, abs_cpos);

	Sector->Cube(cpos) = cType;
    Sector->SetRotInfo(cpos, cRot);
	Sector->Empty = false;
	Sector->Changed = true;
    SectorsMgr.CheckCubeAdj(Sector,cpos,true);
    if (UpdSector)
      UpdateSectorsMesh(s_ind,1);
  }
}

//-----------------------------------------------

void GCubeTerrain::FillAreaInvisible(g_ivec3 min_abs_cpos, g_ivec3 max_abs_cpos)
{
  CheckMinMax(min_abs_cpos,max_abs_cpos);

  if ((ia_min_acpos == min_abs_cpos) && (ia_max_acpos == max_abs_cpos))
	return;

  g_ivec3 spos, cpos;

  //Сделать видимой старую область

  GetSectorAndCubePosByAbsCubePos(ia_min_acpos,spos,cpos);
  GCubeSector *CurrSector = SectorsMgr.GetSector(spos);

  if (CurrSector != NULL)
  {
    GCubeSector *st_sec_y, *st_sec_z;
    g_int32 st_cpos_y, st_cpos_z;
    for (g_int32 x = ia_min_acpos.x; x<=ia_max_acpos.x; x++)
    { 
      if (cpos.x >= NCUBESXZ)
      {
        cpos.x = 0;
        CurrSector = CurrSector->AdjSect.pxs;
      }
      if (CurrSector == NULL) break;
      st_sec_z = CurrSector;
      st_cpos_z = cpos.z;
      for (g_int32 z = ia_min_acpos.z; z<=ia_max_acpos.z; z++)
      {
        if (st_cpos_z >= NCUBESXZ)
        {
          st_cpos_z = 0;
          st_sec_z = st_sec_z->AdjSect.pzs;
        }
        if (st_sec_z == NULL) break;
        st_sec_y = st_sec_z;
        st_cpos_y = cpos.y;

        for (g_int32 y = ia_min_acpos.y; y<=ia_max_acpos.y; y++)
        {
          if (st_cpos_y >= NCUBESY)
          {
            st_cpos_y = 0;
            st_sec_y = st_sec_y->AdjSect.pys;
          }
          if (st_sec_y == NULL) break;

		  if ((x == ia_min_acpos.x) || (x == ia_max_acpos.x) ||
			  (y == ia_min_acpos.y) || (y == ia_max_acpos.y) ||
			  (z == ia_min_acpos.z) || (z == ia_max_acpos.z))
			SectorsMgr.CheckCubeAdj(st_sec_y, g_ivec3(cpos.x,st_cpos_y,st_cpos_z),true); 

		  st_sec_y->SetVisInfo(cpos.x,st_cpos_y,st_cpos_z, false);
		  st_sec_y->Changed = true;

          st_cpos_y += 1;
        } // for (g_int32 y = ...
        st_cpos_z += 1;
      } // for (g_int32 z = ...
      cpos.x += 1;
    } // for (g_int32 x = ...
  } // if (CurrSector != NULL)

  //Сделать невидимой новую область
  ia_min_acpos = min_abs_cpos;
  ia_max_acpos = max_abs_cpos;

  GetSectorAndCubePosByAbsCubePos(min_abs_cpos,spos,cpos);
  CurrSector = SectorsMgr.GetSector(spos);

  if (CurrSector != NULL)
  {
    GCubeSector *st_sec_y, *st_sec_z;
    g_int32 st_cpos_y, st_cpos_z;
    for (g_int32 x = min_abs_cpos.x; x<=max_abs_cpos.x; x++)
    { 
      if (cpos.x >= NCUBESXZ)
      {
        cpos.x = 0;
        CurrSector = CurrSector->AdjSect.pxs;
      }
      if (CurrSector == NULL) break;
      st_sec_z = CurrSector;
      st_cpos_z = cpos.z;
      for (g_int32 z = min_abs_cpos.z; z<=max_abs_cpos.z; z++)
      {
        if (st_cpos_z >= NCUBESXZ)
        {
          st_cpos_z = 0;
          st_sec_z = st_sec_z->AdjSect.pzs;
        }
        if (st_sec_z == NULL) break;
        st_sec_y = st_sec_z;
        st_cpos_y = cpos.y;

        for (g_int32 y = min_abs_cpos.y; y<=max_abs_cpos.y; y++)
        {
          if (st_cpos_y >= NCUBESY)
          {
            st_cpos_y = 0;
            st_sec_y = st_sec_y->AdjSect.pys;
          }
          if (st_sec_y == NULL) break;

		  g_bool set_invis = true;

		  if (x == min_abs_cpos.x) 
		  {
			st_sec_y->Info(cpos.x,st_cpos_y,st_cpos_z).info_adj = st_sec_y->Info(cpos.x,st_cpos_y,st_cpos_z).info_adj | cAdjTypes[0];
		    set_invis = false;
		  }
		  if (x == max_abs_cpos.x)
		  {
			st_sec_y->Info(cpos.x,st_cpos_y,st_cpos_z).info_adj = st_sec_y->Info(cpos.x,st_cpos_y,st_cpos_z).info_adj | cAdjTypes[1];
		    set_invis = false;
		  }

		  if (y == min_abs_cpos.y)
		  {
			st_sec_y->Info(cpos.x,st_cpos_y,st_cpos_z).info_adj = st_sec_y->Info(cpos.x,st_cpos_y,st_cpos_z).info_adj | cAdjTypes[2];
            set_invis = false;
		  }
		  if (y == max_abs_cpos.y) 
		  {
			st_sec_y->Info(cpos.x,st_cpos_y,st_cpos_z).info_adj = st_sec_y->Info(cpos.x,st_cpos_y,st_cpos_z).info_adj | cAdjTypes[3];
            set_invis = false;
		  }

		  if (z == min_abs_cpos.z) 
		  {
			st_sec_y->Info(cpos.x,st_cpos_y,st_cpos_z).info_adj = st_sec_y->Info(cpos.x,st_cpos_y,st_cpos_z).info_adj | cAdjTypes[4];
            set_invis = false;
		  }
		  if (z == max_abs_cpos.z) 
		  {
			st_sec_y->Info(cpos.x,st_cpos_y,st_cpos_z).info_adj = st_sec_y->Info(cpos.x,st_cpos_y,st_cpos_z).info_adj | cAdjTypes[5];
		    set_invis = false;
		  }

		  if (set_invis)
		    st_sec_y->SetVisInfo(cpos.x,st_cpos_y,st_cpos_z, true);
		  st_sec_y->Changed = true;

          st_cpos_y += 1;
        } // for (g_int32 y = ...
        st_cpos_z += 1;
      } // for (g_int32 z = ...
      cpos.x += 1;
    } // for (g_int32 x = ...
  } // if (CurrSector != NULL)

  UpdateSectorsMesh();
}

//-----------------------------------------------

void GCubeTerrain::FillArea(g_ivec3 min_abs_cpos, g_ivec3 max_abs_cpos, 
                            g_uint16 cType, g_uint8 cRot, g_bool overwrite, g_bool UpdSector, g_uint8 AreaType)
{
  CheckMinMax(min_abs_cpos,max_abs_cpos);

  SaveMgr.SaveUndo("undo", min_abs_cpos, max_abs_cpos);

  g_ivec3 spos, cpos, area_size = max_abs_cpos - min_abs_cpos;
  GetSectorAndCubePosByAbsCubePos(min_abs_cpos,spos,cpos);

  g_vec3 min_real_pos = AbsCubePosToRealPos(min_abs_cpos) - csize/2.0f;
  g_vec3 max_real_pos = AbsCubePosToRealPos(max_abs_cpos) + csize/2.0f;
  g_vec3 radius = (max_real_pos - min_real_pos)/2;
  g_vec3 center = min_real_pos + radius;
  
  GCubeSector *CurrSector = SectorsMgr.GetSector(spos);

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
          if ((!overwrite) && (st_sec_y->Cube(cpos.x,st_cpos_y,st_cpos_z) != 0)) 
          {
            st_cpos_y += 1;
            continue;
          }

          if (AreaType == G_CT_AREATYPE_SPHERE)
          {
            g_vec3 real_pos = CubePosToRealPos(g_ivec3(cpos.x,st_cpos_y,st_cpos_z),st_sec_y->Position);
            if ((real_pos.x - center.x)*(real_pos.x - center.x)/(radius.x*radius.x) +
                (real_pos.y - center.y)*(real_pos.y - center.y)/(radius.y*radius.y) +
                (real_pos.z - center.z)*(real_pos.z - center.z)/(radius.z*radius.z) > 1.0f)
            {
              st_cpos_y += 1;
              continue;
            }
          }

          if (AreaType == G_CT_AREATYPE_CYLINDER_Y)
          {
            g_vec3 real_pos = CubePosToRealPos(g_ivec3(cpos.x,st_cpos_y,st_cpos_z),st_sec_y->Position);
            if ((real_pos.x - center.x)*(real_pos.x - center.x)/(radius.x*radius.x) +
                (real_pos.z - center.z)*(real_pos.z - center.z)/(radius.z*radius.z) > 1.0f)
            {
              st_cpos_y += 1;
              continue;
            }
          }

          if (AreaType == G_CT_AREATYPE_CYLINDER_X)
          {
            g_vec3 real_pos = CubePosToRealPos(g_ivec3(cpos.x,st_cpos_y,st_cpos_z),st_sec_y->Position);
            if ((real_pos.y - center.y)*(real_pos.y - center.y)/(radius.y*radius.y) +
                (real_pos.z - center.z)*(real_pos.z - center.z)/(radius.z*radius.z) > 1.0f)
            {
              st_cpos_y += 1;
              continue;
            }
          }

          if (AreaType == G_CT_AREATYPE_CYLINDER_Z)
          {
            g_vec3 real_pos = CubePosToRealPos(g_ivec3(cpos.x,st_cpos_y,st_cpos_z),st_sec_y->Position);
            if ((real_pos.x - center.x)*(real_pos.x - center.x)/(radius.x*radius.x) +
                (real_pos.y - center.y)*(real_pos.y - center.y)/(radius.y*radius.y) > 1.0f)
            {
              st_cpos_y += 1;
              continue;
            }
          }

          g_ivec3 curr_cube = g_ivec3(cpos.x,st_cpos_y,st_cpos_z);
          st_sec_y->InitCubes();
          st_sec_y->Cube(curr_cube) = cType;
          st_sec_y->SetRotInfo(curr_cube, cRot);
          st_sec_y->Empty = false;
	      st_sec_y->Changed = true;
          SectorsMgr.CheckCubeAdj(st_sec_y,curr_cube,true);

          st_cpos_y += 1;
        } // for (g_int32 y = 0; ...
        st_cpos_z += 1;
      } // for (g_int32 z = 0; ...;
      cpos.x += 1;
    } // for (g_int32 x = 0; ... 
  } // if (CurrSector != NULL)

  if (UpdSector)
    UpdateSectorsMesh();
}

//-----------------------------------------------

void GCubeTerrain::FillLine(g_ivec3 min_abs_cpos, g_ivec3 max_abs_cpos, g_uint16 cType, g_uint8 cRot,
                            g_bool overwrite, g_bool UpdSector)
{
  SaveMgr.SaveUndo("undo", min_abs_cpos, max_abs_cpos);

  g_ivec3 area_size = max_abs_cpos - min_abs_cpos;

  if (area_size.x == 0)
  {
    g_ivec3 acpos = min_abs_cpos;
    for (g_int32 i = 0; i <= abs(area_size.y); i++)
    {            
      g_ivec3 curr_spos, curr_cpos;
      GetSectorAndCubePosByAbsCubePos(acpos,curr_spos,curr_cpos);
      SetCube(curr_spos, curr_cpos, cType, cRot, false);
      if (area_size.y > 0) acpos.y ++;
      if (area_size.y < 0) acpos.y --;
      if (area_size.z > 0) acpos.z ++;
      if (area_size.z < 0) acpos.z --;
    }
  }

  if (area_size.y == 0)
  {
    g_ivec3 acpos = min_abs_cpos;
    for (g_int32 i = 0; i <= abs(area_size.x); i++)
    {           
      g_ivec3 curr_spos, curr_cpos;
      GetSectorAndCubePosByAbsCubePos(acpos,curr_spos,curr_cpos);
      SetCube(curr_spos, curr_cpos, cType, cRot, false);
      if (area_size.x > 0) acpos.x ++;
      if (area_size.x < 0) acpos.x --;
      if (area_size.z > 0) acpos.z ++;
      if (area_size.z < 0) acpos.z --;
    }
  }

  if (area_size.z == 0)
  {
    g_ivec3 acpos = min_abs_cpos;
    for (g_int32 i = 0; i <= abs(area_size.y); i++)
    {        
      g_ivec3 curr_spos, curr_cpos;
      GetSectorAndCubePosByAbsCubePos(acpos,curr_spos,curr_cpos);
      SetCube(curr_spos, curr_cpos, cType, cRot, false);
      if (area_size.y > 0) acpos.y ++;
      if (area_size.y < 0) acpos.y --;
      if (area_size.x > 0) acpos.x ++;
      if (area_size.x < 0) acpos.x --;
    }
  }

  if (UpdSector)
    UpdateSectorsMesh();
}

//-----------------------------------------------

void GCubeTerrain::MakeRoadV1(g_ivec3 st_abs_cpos, g_ivec3 end_abs_cpos, g_uint16 main_ctype, g_uint16 bord_ctype, g_bool UpdSector)
{
  end_abs_cpos.y = st_abs_cpos.y;

  g_ivec3 wide_vector = g_ivec3(0,0,0);
  g_ivec3 len_vector = g_ivec3(0,0,0);
  g_ivec3 y_offset = g_ivec3(0,6,0);

  if (abs(end_abs_cpos.x - st_abs_cpos.x) > abs(end_abs_cpos.z - st_abs_cpos.z))
  {
    end_abs_cpos.z = st_abs_cpos.z;
	wide_vector.z = 1;
	len_vector.x = 1;
  }
  else
  {
    end_abs_cpos.x = st_abs_cpos.x;
    wide_vector.x = 1;
	len_vector.z = 1;
  }

  g_ivec3 xz_offset = wide_vector*3;

  CheckMinMax(st_abs_cpos , end_abs_cpos);

  SaveMgr.SaveUndo("undo", st_abs_cpos + xz_offset, end_abs_cpos - xz_offset + y_offset);

  g_bool backup_undo_state = SaveMgr.GetUndoEnabled();
  SaveMgr.SetUndoEnabled(false);

  //Erase 
  FillArea(st_abs_cpos + xz_offset, end_abs_cpos - xz_offset + y_offset, 0, 0, true, false, 0);

  //Draw Base Road
  FillArea(st_abs_cpos + xz_offset, end_abs_cpos - xz_offset, main_ctype, 0, true, false, 0);

  //Draw Borders
  FillArea(st_abs_cpos + xz_offset + g_ivec3(0,1,0), end_abs_cpos + xz_offset + g_ivec3(0,1,0), main_ctype, 0, true, false, 0);
  FillArea(st_abs_cpos - xz_offset + g_ivec3(0,1,0), end_abs_cpos - xz_offset + g_ivec3(0,1,0), main_ctype, 0, true, false, 0);

  //Analyze 
  g_uint32 length_size = 0;
  g_uint32 width_size = 7;
  g_uint32 *road_deep = NULL;
  g_bool len_is_x = true;
  
  if (xz_offset.x == 0)
    length_size = end_abs_cpos.x - st_abs_cpos.x + 1;   
  else
  {
    length_size = end_abs_cpos.z - st_abs_cpos.z + 1;
	len_is_x = false;
  }

  road_deep = new g_uint32[length_size];
  memset(road_deep, 0, sizeof(g_uint32)*length_size);

  g_ivec3 begin_pos = st_abs_cpos - xz_offset - g_ivec3(0,1,0);

  for (g_uint32 l = 0; l < length_size; l++)
    for (g_uint32 w = 0; w < width_size; w++)
	{	  
	  g_ivec3 curr_pos;
	  if (len_is_x) 
		curr_pos = begin_pos + g_ivec3(l, 0, w); 
	  else
		curr_pos = begin_pos + g_ivec3(w, 0, l);
	  
	  g_uint32 depth = 0;
	  while (depth < 50)
	  {
		if (SectorsMgr.GetAbsCube(curr_pos, true) != 0)
		  break;

		curr_pos.y--;
        depth++;	
	  }
	  
	  if (road_deep[l] < depth)
        road_deep[l] = depth;	   
	}

  // Build
  for (g_uint32 l = 0; l < length_size; l++)
  {
    if (road_deep[l] > 3)
	{
      g_ivec3 curr_pos;
	  if (len_is_x) 
	  {
		curr_pos = st_abs_cpos + g_ivec3(l, 0, 0);
		if ((curr_pos.x % 9) != 0) continue;
	  }
	  else
	  {
		curr_pos = st_abs_cpos + g_ivec3(0, 0, l);
		if ((curr_pos.z % 9) != 0) continue;
	  }

	  FillArea(curr_pos + xz_offset - g_ivec3(0,1,0), curr_pos + xz_offset + wide_vector + len_vector - g_ivec3(0,road_deep[l],0), main_ctype, 0, true, false, 0);
	  FillArea(curr_pos - xz_offset - g_ivec3(0,1,0), curr_pos - xz_offset - wide_vector + len_vector - g_ivec3(0,road_deep[l],0), main_ctype, 0, true, false, 0);
	}
  }

  delete[]road_deep;
  //End

  SaveMgr.SetUndoEnabled(backup_undo_state);

  if (UpdSector)
    UpdateSectorsMesh();
}

//-----------------------------------------------

g_uint8 GCubeTerrain::GetCubeRotFromDir(g_vec3 &dir_vec)
{
  if (abs(dir_vec.x) > abs(dir_vec.z))
  {
    if (dir_vec.x >= 0)
      return G_CUBE_ROT_270;
    return G_CUBE_ROT_90;
  }

  if (dir_vec.z >= 0)
    return G_CUBE_ROT_180;

  return G_CUBE_ROT_0;
}

//-----------------------------------------------

g_bool GCubeTerrain::SaveCroppedObject(g_string objname)
{
  g_uint32 n_sec = SectorsMgr.Sectors.size();

  if (n_sec == 0)
    return false;

  g_ivec3 minc = g_ivec3(G_MAX_INT,G_MAX_INT,G_MAX_INT);
  g_ivec3 maxc = g_ivec3(G_MIN_INT,G_MIN_INT,G_MIN_INT);
  
  for (g_uint32 i = 0; i < n_sec; i++)
  {
    GCubeSector *Sector = SectorsMgr.Sectors[i];
    if (!Sector->Empty)  
	{
	  g_ivec3 base_abs_cpos = Sector->Position*cubes_in_sec; 
	  for (g_int32 x = 0; x < NCUBESXZ; x++)
        for (g_int32 y = 0; y < NCUBESY; y++)
          for (g_int32 z = 0; z < NCUBESXZ; z++)
            if (Sector->Cube(x,y,z, true) != 0)
            {
              g_ivec3 abs_cpos = base_abs_cpos + g_ivec3(x,y,z);
			 
			  minc.SetIfMin(abs_cpos);
		      maxc.SetIfMax(abs_cpos);
            }
	}
  }

  G_UNI_CUBE_OBJECT uni_obj;

  if (!SaveMgr.GetObjectFromMap(uni_obj, minc, maxc))
	return false;

  if (!SaveMgr.SaveObject(SaveMgr.MakeObjectFileName(objname), uni_obj))
	return false;

  return true;
}

//-----------------------------------------------

g_bool GCubeTerrain::LoadCubeObject(g_string objname, g_ivec3 position, g_uint8 rot)
{
  G_UNI_CUBE_OBJECT CubeObject;
  if (!SaveMgr.LoadObject(SaveMgr.MakeObjectFileName(objname),CubeObject))
    return false;

  CubeObject._pos = position;
  SaveMgr.SetObjectToMap(CubeObject, rot, false, true, true);

  UpdateSectorsMesh();
  return true;
}

//-----------------------------------------------

g_bool GCubeTerrain::SaveMap(g_string mapname)
{
  //Сохраняем карту
  if (!SaveMgr.SaveMap(mapname))
    return false;

  //Сохраняем инфу карты
  G_MAP_INFO mapinfo;
  mapinfo.map_size = MapGenMgr.GetMapSize();
  
  if (!SaveMgr.SaveMapInfo(mapname,mapinfo))
    return false;

  //Сохраняем карту высот
  g_char filename[300];
  sprintf_s(filename, 300, SAVE_MAP_PATH"%s\\hm.ghm", mapname.c_str());
  MapGenMgr.SaveHeightMap(filename);

  return true;
}

g_bool GCubeTerrain::LoadMap(g_string mapname, G_MAP_INFO &mapinfo) 
{
  //Получаем переменную оффсета
  fa_builder->SetEffectVariable(_ct_material->Effect->GetVar(G_SVT_CUSTOM, 1)->GetDXVar());
  
  // Загружаем карту
  if (!SaveMgr.LoadMap(mapname))
    return false;

  //Загружаем инфу карты
  if (!SaveMgr.LoadMapInfo(mapname,mapinfo))
    return false;

  //Загружаем карту высот
  g_char filename[300];
  sprintf_s(filename, 300, SAVE_MAP_PATH"%s\\hm.ghm", mapname.c_str());
  MapGenMgr.LoadHeightMap(filename, mapinfo.map_size);

  SectorsMgr.GenerateSectorsHASHTable();
  
  //Определяем соседей
  SectorsMgr.CheckSectorsAdj();

  //Устанавливаем текстуры
  _ct_material->ApplyAllTextures();

  return true;
}

//-----------------------------------------------

g_bool GCubeTerrain::GetTargetCube(g_vec3 &pos, g_vec3 &norm_dir, g_ivec3 &out_abs_cpos, 
								   g_ivec3 &prev_abs_cpos, g_bool Real, g_uint8 PlaneBound, 
								   g_int32 PlaneValue)
{
  //Init variables
  g_double dirxy, diryx, dirzx; 
  if (norm_dir.y == 0) dirxy = NAN; else dirxy = norm_dir.x/norm_dir.y;
  if (norm_dir.x == 0) diryx = NAN; else diryx = norm_dir.y/norm_dir.x;
  if (norm_dir.x == 0) dirzx = NAN; else dirzx = norm_dir.z/norm_dir.x;
  //Get first point
  g_ivec3 prevpoint,thispoint,lastvalidcube = max_ivec3;
  g_vec3 pptemp = g_vec3(pos.x/csize.x, pos.y/csize.y, pos.z/csize.z);
  prevpoint.FloorVec3(pptemp);
  prev_abs_cpos = g_vec3(0,0,0);
  for (g_int32 i = 1; i<=tcSegmMaxNum; i++)
  {
    //Get next point
    pptemp = g_vec3((pos.x+norm_dir.x*tcSegmSize.x*i)/csize.x, 
                    (pos.y+norm_dir.y*tcSegmSize.y*i)/csize.y, 
                    (pos.z+norm_dir.z*tcSegmSize.z*i)/csize.z);
    thispoint.FloorVec3(pptemp); 
    
    g_ivec3 sign;
    if (prevpoint.x < thispoint.x) sign.x = 1; else sign.x = -1;
    if (prevpoint.y < thispoint.y) sign.y = 1; else sign.y = -1;
    if (prevpoint.z < thispoint.z) sign.z = 1; else sign.z = -1;
    //Check all points between previous point and this
    g_vec3 min,max;
    for (g_int32 px = prevpoint.x; px != (thispoint.x + sign.x); px += sign.x)
    {
      min.x = px*csize.x;
      max.x = (px+1)*csize.x;
      for (g_int32 py = prevpoint.y; py != (thispoint.y + sign.y); py += sign.y)
      {
        min.y = py*csize.y;
        max.y = (py+1)*csize.y;
        for (g_int32 pz = prevpoint.z; pz != (thispoint.z + sign.z); pz += sign.z)
        {
          if ((px == prevpoint.x) && (py == prevpoint.y) && (pz == prevpoint.z)) continue; 
          min.z = pz*csize.z;
          max.z = (pz+1)*csize.z;
          if (CheckCubeIntersect(pos,norm_dir,min,max))
            if (PlaneBound == 0)
            {
              g_int32 cube_res = IsValidCube(g_ivec3(px,py,pz), Real);
              
              if (cube_res >= 0)
                lastvalidcube = g_ivec3(px,py,pz);

              if (cube_res > 0)
              {
                out_abs_cpos = lastvalidcube;
                return true;
              }

              if (cube_res <= 0)
                prev_abs_cpos = g_ivec3(px,py,pz);
            }
            else
            {
              g_int32 chkval;
              switch (PlaneBound)
              {
                case 1:chkval = px;
                       break;
                case 2:chkval = py;
                       break;
                case 3:chkval = pz;
                       break;
                default: return false;
              }
              if (chkval == PlaneValue)
              {
                out_abs_cpos = g_ivec3(px,py,pz);
                return true;
              }
              else
                prev_abs_cpos = g_ivec3(px,py,pz);
            }
        } //pz
      } //py
    } //px
    prevpoint = thispoint;
  } //i

  g_int32 cube_res = IsValidCube(prev_abs_cpos, Real);
              
  if ((cube_res < 0) && (lastvalidcube != max_ivec3))
  {
    out_abs_cpos = lastvalidcube;
    prev_abs_cpos = lastvalidcube;
    return true;
  }

  return false;
}

//-----------------------------------------------

g_int32 GCubeTerrain::IsValidCube(g_ivec3 &abs_cpos, g_bool Real)
{
  g_ivec3 cpos,spos;
  GetSectorAndCubePosByAbsCubePos(abs_cpos,spos,cpos);

  GCubeSector *sect = SectorsMgr.GetSector(spos);
  if (sect == NULL) return -1;
  return g_int32(sect->Cube(cpos, Real));
}

//-----------------------------------------------
// Генерация общая
//-----------------------------------------------

void GCubeTerrain::InitMap_Test(g_uint8 type, g_ivec2 size)
{
  g_double ticks = GetTicks();
  //Start Check Init Speed

  //Получаем переменную оффсета
  fa_builder->SetEffectVariable(_ct_material->Effect->GetVar(G_SVT_CUSTOM, 1)->GetDXVar());

  switch (type)
  {
    /*case 0:{
             MapGenMgr.InitMap();
           }
           break;
    case 1:{
             MapGenMgr.InitMapClear();
           }
           break;*/
    case 2:{
             MapGenMgr.InitMap2(size,true);
           }
           break;
    case 3:{
             MapGenMgr.InitMap2Clear(size);
           }
           break;
    case 4:{
             MapGenMgr.InitMap2(size,false);
           }
           break;
    case 5:{
             MapGenMgr.InitMapAutoSize(false);
           }
           break;
    case 6:{
             MapGenMgr.InitMapAutoSize(true);
           }
           break;
    default:break;
  }

  //Определяем соседние сектора
  SectorsMgr.CheckSectorsAdj();

  //Определяем видимость граней соседних кубов на всей карте
  SectorsMgr.CheckAllMapCubeAdj();

  //Устанавливаем текстуры
  _ct_material->ApplyAllTextures();

  //End Check Init Speed
  CheckTimeAndWriteToFile(ticks, "gen_map");
}

//-----------------------------------------------

g_float GCubeTerrain::GetMapHeightInPos(g_vec3 &pos)
{
  return MapGenMgr.GetMapHeightInPos(pos);
}

//-----------------------------------------------
