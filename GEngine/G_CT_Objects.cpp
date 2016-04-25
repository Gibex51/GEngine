#include "G_CT_Objects.h"


//-----------------------------------------------

G_CT_Objects::G_CT_Objects(void)
{

}

//-----------------------------------------------

G_CT_Objects::~G_CT_Objects(void)
{

}

//-----------------------------------------------

void G_CT_Objects::CalcPointHASH(const g_ivec3 &fvert, g_uint8 axis, g_uint16 &result_hash)
{
  result_hash = G_OT_INVALID_HASH;
  switch (axis)
  {
    case 1:
    case 2:result_hash = (fvert.z+100) + (fvert.y+100)*200;
		   break;
    case 3:
    case 4:result_hash = (fvert.z+100) + (fvert.x+100)*200;
		   break;
    case 5:
    case 6:result_hash = (fvert.y+100) + (fvert.x+100)*200;
		   break;
  }
}

//-----------------------------------------------

void G_CT_Objects::SortHASH(g_uint16 *fhash)
{
  for (g_uint32 i = 0; i < 4; i++)
    for (g_uint32 j = 0; j < 4; j++)
      if (i != j)
        if (fhash[i] < fhash[j])
		  swap_var(fhash[i],fhash[j]);
}

//-----------------------------------------------

void G_CT_Objects::AnalyseFace(G_CUBE_VERTEX *fvert, g_uint32 *find, g_vec3 *fnorm, GFaceInfo &finfo) //6x vertices
{
  memcpy(finfo.findices, find, sizeof(g_uint32)*6);

  g_ivec3 int_vert[4];

  for (g_uint32 i = 0; i<3; i++)
    int_vert[i] = g_ivec3(fvert[i].Pos * 100.0f + g_vec3(0.5f,0.5f,0.5f));

  g_ivec3 temp_vert[3];

  for (g_uint32 i = 0; i<3; i++)
    temp_vert[i] = g_ivec3(fvert[i+3].Pos * 100.0f + g_vec3(0.5f,0.5f,0.5f));
  
  for (g_uint32 i = 0; i<3; i++)  
    if ((temp_vert[i] != int_vert[0]) && (temp_vert[i] != int_vert[1]) && (temp_vert[i] != int_vert[2]))
	{
	  int_vert[3] = temp_vert[i];
	  break;
	}

  finfo.faxis = 0;
  finfo.fsize = 0;

  //calc axis
  g_ivec3 int_norm[2];
  for (g_uint32 i = 0; i<2; i++)
    int_norm[i].RoundVec3(fnorm[i]*100.0f);
  
  if (int_norm[0] == int_norm[1])
  {
	if ((int_norm[0].x == 100)  && (int_vert[0].x == 100)) finfo.faxis = 1;
	if ((int_norm[0].x == -100) && (int_vert[0].x == 0)) finfo.faxis = 2;
	if ((int_norm[0].y == 100)  && (int_vert[0].y == 100)) finfo.faxis = 3;
	if ((int_norm[0].y == -100) && (int_vert[0].y == 0)) finfo.faxis = 4;
	if ((int_norm[0].z == 100)  && (int_vert[0].z == 100)) finfo.faxis = 5;
	if ((int_norm[0].z == -100) && (int_vert[0].z == 0)) finfo.faxis = 6;
  }

  //calc hash
  for (g_uint32 i = 0; i<4; i++)
    CalcPointHASH(int_vert[i], finfo.faxis, finfo.fhash[i]);

  SortHASH(finfo.fhash);

  //calc type
  if (finfo.faxis != 0)
  {
	if (finfo._fhash != *(g_uint64*)hash_fulltype)
	{
	  g_ivec3 sym_vert[4];
	  for (g_uint32 i = 0; i<4; i++)
	  {
		if (int_vert[i].x > 50) sym_vert[i].x = int_vert[i].x - 50; else sym_vert[i].x = 50 - int_vert[i].x;
		if (int_vert[i].y > 50) sym_vert[i].y = int_vert[i].y - 50; else sym_vert[i].y = 50 - int_vert[i].y;
		if (int_vert[i].z > 50) sym_vert[i].z = int_vert[i].z - 50; else sym_vert[i].z = 50 - int_vert[i].z;
	  }

	  if ((sym_vert[0] == sym_vert[1]) && (sym_vert[0] == sym_vert[2]) && (sym_vert[0] == sym_vert[3]))
	  {
	    finfo.ftype = 2;
		finfo.fsize = g_uint8((int_vert[0] - 50).Length());
	  }
	  else
	  {
		finfo.ftype = 3;
		finfo.fsize = g_uint8((int_vert[0] - 50).Length());
        for (g_uint32 i = 0; i<3; i++)
		{
		  g_uint8 newsize = g_uint8((int_vert[i] - 50).Length());
          if (newsize > finfo.fsize) finfo.fsize = newsize;
		}
	  }
	}
	else
      finfo.ftype = 1;
  }
  else
    finfo.ftype = 0;
}

//-----------------------------------------------

g_bool G_CT_Objects::LoadCubeObjects(g_char *FileName)
{
  GFileStream ObjFile;
  if (ObjFile.Create(FileName, G_FSMODE_READ))
  {
    // Очищаем предыдущий контент
	ClearCubeObjects();

	// Загружаем типы	
    g_uint8  nObjTypes = 0;    
	ObjFile.Read(&nObjTypes, 1);

	for (g_int32 i = 0; i < nObjTypes; i++)
	{
      ObjTypes.push_back(G_OBJECTTYPE());

	  ObjFile.Read(&ObjTypes[i].nIndices, 1);
	  ObjFile.Read(&ObjTypes[i].nVertices, 1);

	  if (ObjTypes[i].nIndices > 0)
	  {
	    ObjTypes[i]._indices = new g_uint32[ObjTypes[i].nIndices];
		for (g_int32 j = 0; j < ObjTypes[i].nIndices; j++)
		{
		  g_uint8 curr_ind = 0;
		  ObjFile.Read(&curr_ind, 1);
		  ObjTypes[i]._indices[j] = curr_ind;
		}
	  }
	  else
        ObjTypes[i]._indices = NULL;
	  
	  if (ObjTypes[i].nVertices > 0)
	  {
	    for (g_int32 j = 0; j < 4; j++)
		{
		  G_CUBE_VERTEX_PACK *pvert = new G_CUBE_VERTEX_PACK[ObjTypes[i].nVertices];
		  ObjFile.Read(pvert, ObjTypes[i].nVertices*sizeof(G_CUBE_VERTEX_PACK));
		  ObjTypes[i]._vertices[j] = new G_CUBE_VERTEX[ObjTypes[i].nVertices];

		  for (g_int32 k = 0; k < ObjTypes[i].nVertices; k++)
		  {
            ObjTypes[i]._vertices[j][k].Pos = pvert[k].Pos;
			ObjTypes[i]._vertices[j][k].Color = pvert[k].Color;
			ObjTypes[i]._vertices[j][k].Tex = UnpackTexCoord(pvert[k].Tex);
		  }
		  delete[]pvert;

		  ObjTypes[i]._facenormals[j] = new g_vec3[ObjTypes[i].nIndices/3];
		  ObjTypes[i]._packednormals[j] = new g_uint32[ObjTypes[i].nIndices/3];
          ObjTypes[i]._dirs[j] = new g_uint32[ObjTypes[i].nIndices/3];

          for (g_int32 k = 0; k<ObjTypes[i].nIndices/3; k++)
          {
            g_vec3 norm = CalcFaceNormal(ObjTypes[i]._vertices[j][ObjTypes[i]._indices[k*3]].Pos,
                                         ObjTypes[i]._vertices[j][ObjTypes[i]._indices[k*3+1]].Pos,
                                         ObjTypes[i]._vertices[j][ObjTypes[i]._indices[k*3+2]].Pos);
            ObjTypes[i]._facenormals[j][k] = norm;
			ObjTypes[i]._packednormals[j][k] = PackNormal(norm)*262144;
            ObjTypes[i]._dirs[j][k] = 0;

            if ((abs(norm.z) >= abs(norm.x)) && (abs(norm.z) >= abs(norm.y))) ObjTypes[i]._dirs[j][k] = 0;
            if ((abs(norm.x) >= abs(norm.z)) && (abs(norm.x) >= abs(norm.y))) ObjTypes[i]._dirs[j][k] = 262144;
            if ((abs(norm.y) >= abs(norm.x)) && (abs(norm.y) >= abs(norm.z))) ObjTypes[i]._dirs[j][k] = 524288;
          }
		}
	  }
	  else
        for (g_int32 j = 0; j < 4; j++)
		{
	      ObjTypes[i]._vertices[j] = NULL;
		  ObjTypes[i]._facenormals[j] = NULL;
          ObjTypes[i]._packednormals[j] = NULL;
          ObjTypes[i]._dirs[j] = NULL;
		}

	  //Fill Extra Info New
      ObjTypes[i]._fullvisible = true;
	  if (ObjTypes[i]._vertices != NULL)
	    for (g_uint32 rot = 0; rot < 4; rot++)
		{
          ObjTypes[i]._faceinfo[rot] = new GFaceInfo[ObjTypes[i].nIndices/6];
	      for (g_int32 ind = 0; ind < ObjTypes[i].nIndices/6; ind++)
	      {
		    G_CUBE_VERTEX fvert[6];
		    for (g_uint32 j = 0; j<6; j++)
		      fvert[j] = ObjTypes[i]._vertices[rot][ObjTypes[i]._indices[ind*6+j]];

			g_vec3 fnorm[2];
			for (g_uint32 j = 0; j<2; j++)
			  fnorm[j] = ObjTypes[i]._facenormals[rot][ind*2+j];
			
			AnalyseFace(fvert, &ObjTypes[i]._indices[ind*6], fnorm, ObjTypes[i]._faceinfo[rot][ind]);

			GFaceInfo &tfi = ObjTypes[i]._faceinfo[rot][ind];

			if (ObjTypes[i]._faceinfo[rot][ind].ftype > 0)
			  ObjTypes[i]._fullvisible = false;
	      }

		  for (g_uint32 ax = 0; ax < 6; ax++)
		  {
		    ObjTypes[i]._dockaxis[rot][ax].ftype = 0;
			ObjTypes[i]._dockaxis[rot][ax].fsize = 0;
			ObjTypes[i]._dockaxis[rot][ax].fhash64 = 0;
		  }

          for (g_int32 ind = 0; ind < ObjTypes[i].nIndices/6; ind++)
	      {
		    GFaceInfo &fi = ObjTypes[i]._faceinfo[rot][ind];
            if (fi.ftype > 0)
		    {
              GAxisInfo &ai = ObjTypes[i]._dockaxis[rot][fi.faxis-1];			  
			  switch (fi.ftype) 
			  {
			    case 1: ai.ftype = 1;
					    break;
				case 2: if ((ai.ftype == 0) || ((ai.ftype == 3) && (ai.fsize <= fi.fsize)))
                          ai.ftype = 2;
				        break;
				case 3: if ((ai.ftype == 0) || ((ai.ftype == 2) && (ai.fsize < fi.fsize)))
						{
                          ai.ftype = 3;
						  ai.fhash64 = fi._fhash;
						}
				        break;
			  }
			  if (ai.fsize < fi.fsize) ai.fsize = fi.fsize;
		    }
	      }
		} // for (g_uint32 rot = ...
	}

	
	
	// Загружаем объекты
    g_uint16 nObjects = 0;
	ObjFile.Read(&nObjects, 2);

	for (g_int32 i = 0; i < nObjects; i++)
	{
      Objects.push_back(G_OBJECT());
	  ObjFile.Read(&Objects[i], sizeof(G_OBJECT));
	}

	// Загружаем группы
    g_uint8  nGroups = 0;
    ObjFile.Read(&nGroups, 1);

	for (g_int32 i = 0; i < nGroups; i++)
	{    
	  ObjGroups.push_back(G_OBJECTGROUP());

	  ObjFile.Read(&ObjGroups[i].Name[0], 64);
	  ObjFile.Read(&ObjGroups[i].nObjects, 2);

	  ObjGroups[i].ObjIDList = new g_uint16[ObjGroups[i].nObjects];

	  ObjFile.Read(&ObjGroups[i].ObjIDList[0], ObjGroups[i].nObjects*sizeof(g_uint16));
	}

    ObjFile.Release();
  }
  else
	return false;

  return true;
}

//-----------------------------------------------

void G_CT_Objects::ClearCubeObjects()
{
  for (g_uint32 i = 0; i < ObjTypes.size(); i++)
  {
    FREENULL_ARR(ObjTypes[i]._indices);
    for (g_int32 k = 0; k<4; k++)
    {
      FREENULL_ARR(ObjTypes[i]._vertices[k]);
	  FREENULL_ARR(ObjTypes[i]._facenormals[k]);
	  FREENULL_ARR(ObjTypes[i]._packednormals[k]);
	  FREENULL_ARR(ObjTypes[i]._dirs[k]);
	  FREENULL_ARR(ObjTypes[i]._faceinfo[k]);
    }
  }

  for (g_uint32 i = 0; i < ObjGroups.size(); i++)
    FREENULL_ARR(ObjGroups[i].ObjIDList);

  ObjGroups.clear();
  Objects.clear();
  ObjTypes.clear();
}

//-----------------------------------------------
/*
g_bool G_CT_Objects::LoadObjects(g_char *FileName)
{
  if (FileExists(FileName))
  { 
    //Reading File
    GFileStream ObjFile;
    g_char *buffer;
    g_uint32 bsize;

    ObjFile.Create(FileName,G_FSMODE_READ);
    bsize = ObjFile.ReadEntireFile((void**)&buffer);
    ObjFile.Release();
    if (bsize == 0) return false;

    //Clear Previous Content
    ClearObjects();

    //Parsing Data
    memcpy(&nObjects,&buffer[0],2);
    Objects = new G_OBJECT[nObjects];

    memcpy(&Objects[0],&buffer[2],sizeof(G_OBJECT)*nObjects);

    //Clear
    delete[]buffer;
  }
  else
    return false;

  return true;
}
*/
//-----------------------------------------------
/*
g_bool G_CT_Objects::LoadObjTypes(g_char *FileName)
{
  if (FileExists(FileName))
  { 
    //Reading File
    GFileStream ObjFile;
    g_char *buffer;
    g_uint32 bsize;

    ObjFile.Create(FileName,G_FSMODE_READ);
    bsize = ObjFile.ReadEntireFile((void**)&buffer);
    ObjFile.Release();
    if (bsize == 0) return false;

    //Clear Previous Content
    ClearObjTypes();

    //Parsing Data
    nObjTypes = buffer[0];
    ObjTypes = new G_OBJECTTYPE[nObjTypes];

    g_uint32 pos = 1;

    for (g_uint32 i = 0; i<nObjTypes; i++)
    {
      //Fill Extra Info
      ObjTypes[i].CutType = en_ct_Misc;
      ObjTypes[i].CubeFlatValue = 0;

      if (i == 0)  { ObjTypes[i].CutType = en_ct_Cube; ObjTypes[i].CubeFlatValue = 100;}
      if (i == 5)  { ObjTypes[i].CutType = en_ct_Cube; ObjTypes[i].CubeFlatValue = 25;}
      if (i == 6)  { ObjTypes[i].CutType = en_ct_Cube; ObjTypes[i].CubeFlatValue = 50;}
      if (i == 9)  { ObjTypes[i].CutType = en_ct_Cube; ObjTypes[i].CubeFlatValue = 75;}
      if (i == 14) { ObjTypes[i].CutType = en_ct_Cube; ObjTypes[i].CubeFlatValue = 10;}
      //---------------
      ObjTypes[i].nIndices = buffer[pos];
      ObjTypes[i].nVertices = buffer[pos + 1];
      pos += 2;

      if (ObjTypes[i].nIndices > 0)
      {
        ObjTypes[i]._indices = new g_uint32[ObjTypes[i].nIndices];
        for (g_uint32 j = 0; j<ObjTypes[i].nIndices; j++)
          ObjTypes[i]._indices[j] = buffer[pos+j];
        pos += ObjTypes[i].nIndices;
      }
      else
        ObjTypes[i]._indices = NULL;
      
      if (ObjTypes[i].nVertices > 0)
      {
        ObjTypes[i]._vertices[0] = new G_CUBE_VERTEX[ObjTypes[i].nVertices];
        memcpy(&ObjTypes[i]._vertices[0][0],&buffer[pos],sizeof(G_CUBE_VERTEX)*ObjTypes[i].nVertices);
        pos += sizeof(G_CUBE_VERTEX)*ObjTypes[i].nVertices;
		
		for (g_int32 k = 0; k<ObjTypes[i].nVertices; k++)
        {
		  g_vec3 tx = ObjTypes[i]._vertices[0][k].Tex;

		  g_uint32 PackedTex = ((g_int32(tx.x*100))%128 + 127) + 
				                ((g_int32(tx.y*100))%128 + 127) * 256 + 
								((g_int32(tx.z*100))%128 + 127) * 65536; 

		  ObjTypes[i]._vertices[0][k].Color = PackedTex;
		}

        for (g_int32 j = 1; j<4; j++)
        {
          ObjTypes[i]._vertices[j] = new G_CUBE_VERTEX[ObjTypes[i].nVertices];
          memcpy(&ObjTypes[i]._vertices[j][0],&ObjTypes[i]._vertices[0][0],sizeof(G_CUBE_VERTEX)*ObjTypes[i].nVertices);
          for (g_int32 k = 0; k<ObjTypes[i].nVertices; k++)
          {
            g_vec3 pos = ObjTypes[i]._vertices[j][k].Pos;
            g_vec3 tex = ObjTypes[i]._vertices[j][k].Tex;
            D3DXVec3TransformCoord(&ObjTypes[i]._vertices[j][k].Pos,&pos,&r_mat[j]); 
            D3DXVec3TransformCoord(&ObjTypes[i]._vertices[j][k].Tex,&tex,&r_mat[j]);

			g_vec3 tx = ObjTypes[i]._vertices[j][k].Tex;

			g_uint32 PackedTex = ((g_int32(tx.x*100))%128 + 127) + 
				                 ((g_int32(tx.y*100))%128 + 127) * 256 + 
								 ((g_int32(tx.z*100))%128 + 127) * 65536; 

			ObjTypes[i]._vertices[j][k].Color = PackedTex;
          }
        }
      }
      else
        for (g_int32 j = 0; j<4; j++)
          ObjTypes[i]._vertices[j] = NULL;

      if (ObjTypes[i].nIndices > 0)
      {
        for (g_int32 k = 0; k<4; k++)
        {
          ObjTypes[i]._facenormals[k] = new g_vec3[ObjTypes[i].nIndices/3];
          ObjTypes[i]._dirs[k] = new g_uint32[ObjTypes[i].nIndices/3];
          for (g_int32 j = 0; j<ObjTypes[i].nIndices/3; j++)
          {
            g_vec3 norm = CalcFaceNormal(ObjTypes[i]._vertices[k][ObjTypes[i]._indices[j*3]].Pos,
                                         ObjTypes[i]._vertices[k][ObjTypes[i]._indices[j*3+1]].Pos,
                                         ObjTypes[i]._vertices[k][ObjTypes[i]._indices[j*3+2]].Pos);
            ObjTypes[i]._facenormals[k][j] = norm;

            ObjTypes[i]._dirs[k][j] = 0;

            if ((abs(norm.z) >= abs(norm.x)) && (abs(norm.z) >= abs(norm.y))) ObjTypes[i]._dirs[k][j] = 0;
            if ((abs(norm.x) >= abs(norm.z)) && (abs(norm.x) >= abs(norm.y))) ObjTypes[i]._dirs[k][j] = 262144;
            if ((abs(norm.y) >= abs(norm.x)) && (abs(norm.y) >= abs(norm.z))) ObjTypes[i]._dirs[k][j] = 524288;
          }
        }
      }
      else
        for (g_int32 j = 0; j<4; j++)
        {
          ObjTypes[i]._facenormals[j] = NULL;
          ObjTypes[i]._dirs[j] = NULL;
        }
    }
    //Clear
    delete[]buffer;
  }
  else
    return false;

  return true;
}
*/
//-----------------------------------------------

void G_CT_Objects::Release()
{
  ClearCubeObjects();
}

//-----------------------------------------------
