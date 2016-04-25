#include "FG_GM_Funcs.h"

//-----------------------------------------------

g_ivec2 GetRegionPosBySectorPos(g_ivec3 &spos)
{
  g_ivec2 rpos;
  rpos.x = g_int32(floor(g_float(spos.x)/SECTORSXZ_IN_REGION));
  rpos.y = g_int32(floor(g_float(spos.z)/SECTORSXZ_IN_REGION));
  return rpos;
}

//-----------------------------------------------

g_vec3 AbsCubePosToRealPos(const g_ivec3 &abs_cpos)
{
  g_vec3 real_pos;
  real_pos.x = g_float(abs_cpos.x)*csize.x + csize.x/2;
  real_pos.y = g_float(abs_cpos.y)*csize.y + csize.y/2;
  real_pos.z = g_float(abs_cpos.z)*csize.z + csize.z/2;
  return real_pos;
}

//-----------------------------------------------

g_vec3 CubePosToRealPos(g_ivec3 &cpos, g_ivec3 &spos)
{
  g_vec3 real_pos;

  real_pos.x = g_float(spos.x*NCUBESXZ + cpos.x)*csize.x + csize.x/2;
  real_pos.y = g_float(spos.y*NCUBESY + cpos.y)*csize.y + csize.y/2;
  real_pos.z = g_float(spos.z*NCUBESXZ + cpos.z)*csize.z + csize.z/2;
  return real_pos;
}

//-----------------------------------------------

g_ivec3 GetAbsCubePosByVec3(g_vec3 &inPos)
{
  g_ivec3 abs_cpos;
  abs_cpos.x = g_int32(floor(inPos.x / csize.x));
  abs_cpos.y = g_int32(floor(inPos.y / csize.x));  
  abs_cpos.z = g_int32(floor(inPos.z / csize.x));
  return abs_cpos;
}

//-----------------------------------------------

g_ivec3 GetAbsCubePosBySectorAndCubePos(g_ivec3 &cpos, g_ivec3 &spos)
{
  return (spos*cubes_in_sec + cpos);
}

//-----------------------------------------------

void GetSectorAndCubePosByVec3(const g_vec3 inPos, g_ivec3 &sPos, g_ivec3 &cPos)
{
  sPos.x = g_int32(floor(inPos.x / CUBE_SECTOR_SIZE));
  sPos.y = g_int32(floor(inPos.y / CUBE_SECTOR_SIZE));  
  sPos.z = g_int32(floor(inPos.z / CUBE_SECTOR_SIZE));

  cPos.x = g_int32(floor((inPos.x - g_float(sPos.x*CUBE_SECTOR_SIZE))/csize.x));
  cPos.y = g_int32(floor((inPos.y - g_float(sPos.y*CUBE_SECTOR_SIZE))/csize.y));
  cPos.z = g_int32(floor((inPos.z - g_float(sPos.z*CUBE_SECTOR_SIZE))/csize.z));
}

//-----------------------------------------------

void GetSectorByVec3(g_vec3 inPos, g_ivec3 *sPos)
{
  sPos->x = g_int32(floor(inPos.x / CUBE_SECTOR_SIZE));
  sPos->y = g_int32(floor(inPos.y / CUBE_SECTOR_SIZE));  
  sPos->z = g_int32(floor(inPos.z / CUBE_SECTOR_SIZE));
}

//-----------------------------------------------

void GetSectorAndCubePosByAbsCubePos(const g_ivec3 &abs_cPos, g_ivec3 &sPos, g_ivec3 &cPos)
{
  cPos = abs_cPos % cubes_in_sec;

  if (cPos.x < 0) cPos.x += cubes_in_sec.x;
  if (cPos.y < 0) cPos.y += cubes_in_sec.y;
  if (cPos.z < 0) cPos.z += cubes_in_sec.z;
  
  sPos = (abs_cPos-(cPos))/cubes_in_sec;
}

//-----------------------------------------------
/*
void CreateCubeForMesh(GMesh *Mesh, g_bool LineStripIndices, g_float Alpha)
{
  g_uint32 ts_ind[] =
  {
    2,1,0,   1,2,3,
    6,0,4,   0,6,2,
    3,5,1,   5,3,7,
    7,4,5,   4,7,6,
    10,4,0,  4,10,11,
    6,8,2,   8,6,9
  };

  g_uint32 ls_ind[] =
  {
    0,1, 1,3, 3,2, 2,0,
    4,5, 5,7, 7,6, 6,4,
    0,4, 1,5, 2,6, 3,7
  };

  G_VERTEX vert[] =
  {
    {g_vec3( -1.0f, 1.0f, -1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 0.0f, 0.0f ),g_vec3( -1.0f, 1.0f, -1.0f )},
    {g_vec3( 1.0f, 1.0f, -1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 1.0f, 0.0f ),g_vec3( 1.0f, 1.0f, -1.0f )},
    {g_vec3( -1.0f, 1.0f, 1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 0.0f, 1.0f ),g_vec3( -1.0f, 1.0f, 1.0f )},
    {g_vec3( 1.0f, 1.0f, 1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 1.0f, 1.0f ),g_vec3( 1.0f, 1.0f, 1.0f )},

    {g_vec3( -1.0f, -1.0f, -1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 1.0f, 0.0f ),g_vec3( -1.0f, -1.0f, -1.0f )},
    {g_vec3( 1.0f, -1.0f, -1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 0.0f, 0.0f ),g_vec3( 1.0f, -1.0f, -1.0f )},
    {g_vec3( -1.0f, -1.0f, 1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 1.0f, 1.0f ),g_vec3( -1.0f, -1.0f, 1.0f )},
    {g_vec3( 1.0f, -1.0f, 1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 0.0f, 1.0f ),g_vec3( 1.0f, -1.0f, 1.0f )},

    {g_vec3( 1.0f, 1.0f, 1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 0.0f, 0.0f ),g_vec3( 1.0f, 1.0f, 1.0f )},
    {g_vec3( 1.0f, -1.0f, 1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 1.0f, 0.0f ),g_vec3( 1.0f, -1.0f, 1.0f )},
    {g_vec3( 1.0f, 1.0f, -1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 0.0f, 1.0f ),g_vec3( 1.0f, 1.0f, -1.0f )},
    {g_vec3( 1.0f, -1.0f, -1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 1.0f, 1.0f ),g_vec3( 1.0f, -1.0f, -1.0f )},
  };

  Mesh->AddVertexArray(vert,12);
  if (LineStripIndices)
    Mesh->AddIndexArray(ls_ind,24);
  else
    Mesh->AddIndexArray(ts_ind,36);
}

//-----------------------------------------------

void CreateFrustumMesh(g_vec3 *fr_points, GMesh *OutMesh, g_bool LineStripIndices, g_float Alpha)
{
  g_uint32 ts_ind[] =
  {
    2,1,0,   1,2,3,
    6,0,4,   0,6,2,
    3,5,1,   5,3,7,
    7,4,5,   4,7,6,
    10,4,0,  4,10,11,
    6,8,2,   8,6,9
  };

  g_uint32 ls_ind[] =
  {
    0,1, 1,3, 3,2, 2,0,
    4,5, 5,7, 7,6, 6,4,
    0,4, 1,5, 2,6, 3,7
  };

  G_VERTEX vert[] =
  {
    {fr_points[2],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 0 1 0
    g_vec2( 0.0f, 0.0f ),g_vec3( -1.0f, 1.0f, -1.0f )},
    {fr_points[3],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 1 1 0
    g_vec2( 1.0f, 0.0f ),g_vec3( 1.0f, 1.0f, -1.0f )},
    {fr_points[6],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 0 1 1
    g_vec2( 0.0f, 1.0f ),g_vec3( -1.0f, 1.0f, 1.0f )},
    {fr_points[7],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 1 1 1
    g_vec2( 1.0f, 1.0f ),g_vec3( 1.0f, 1.0f, 1.0f )},

    {fr_points[0],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 0 0 0 
    g_vec2( 1.0f, 0.0f ),g_vec3( -1.0f, -1.0f, -1.0f )},
    {fr_points[1],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 1 0 0
    g_vec2( 0.0f, 0.0f ),g_vec3( 1.0f, -1.0f, -1.0f )},
    {fr_points[4],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 0 0 1
    g_vec2( 1.0f, 1.0f ),g_vec3( -1.0f, -1.0f, 1.0f )},
    {fr_points[5],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 1 0 1
    g_vec2( 0.0f, 1.0f ),g_vec3( 1.0f, -1.0f, 1.0f )},

    {fr_points[7],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 1 1 1
    g_vec2( 0.0f, 0.0f ),g_vec3( 1.0f, 1.0f, 1.0f )},
    {fr_points[5],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 1 0 1
    g_vec2( 1.0f, 0.0f ),g_vec3( 1.0f, -1.0f, 1.0f )},
    {fr_points[3],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 1 1 0
    g_vec2( 0.0f, 1.0f ),g_vec3( 1.0f, 1.0f, -1.0f )},
    {fr_points[1],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 1 0 0
    g_vec2( 1.0f, 1.0f ),g_vec3( 1.0f, -1.0f, -1.0f )},
  };

  OutMesh->AddVertexArray(vert,12);
  if (LineStripIndices)
    OutMesh->AddIndexArray(ls_ind,24);
  else
    OutMesh->AddIndexArray(ts_ind,36);
}
*/
//----------------------------------------------

void CreateCubeForModel(GModel *Model, g_bool LineStripIndices, g_float Alpha)
{
  g_uint32 ts_ind[] =
  {
    2,1,0,   1,2,3,
    6,0,4,   0,6,2,
    3,5,1,   5,3,7,
    7,4,5,   4,7,6,
    10,4,0,  4,10,11,
    6,8,2,   8,6,9
  };

  g_uint32 ls_ind[] =
  {
    0,1, 1,3, 3,2, 2,0,
    4,5, 5,7, 7,6, 6,4,
    0,4, 1,5, 2,6, 3,7
  };

  G_VERTEX vert[] =
  {
    {g_vec3( -1.0f, 1.0f, -1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 0.0f, 0.0f ),g_vec3( -1.0f, 1.0f, -1.0f )},
    {g_vec3( 1.0f, 1.0f, -1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 1.0f, 0.0f ),g_vec3( 1.0f, 1.0f, -1.0f )},
    {g_vec3( -1.0f, 1.0f, 1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 0.0f, 1.0f ),g_vec3( -1.0f, 1.0f, 1.0f )},
    {g_vec3( 1.0f, 1.0f, 1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 1.0f, 1.0f ),g_vec3( 1.0f, 1.0f, 1.0f )},

    {g_vec3( -1.0f, -1.0f, -1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 1.0f, 0.0f ),g_vec3( -1.0f, -1.0f, -1.0f )},
    {g_vec3( 1.0f, -1.0f, -1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 0.0f, 0.0f ),g_vec3( 1.0f, -1.0f, -1.0f )},
    {g_vec3( -1.0f, -1.0f, 1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 1.0f, 1.0f ),g_vec3( -1.0f, -1.0f, 1.0f )},
    {g_vec3( 1.0f, -1.0f, 1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 0.0f, 1.0f ),g_vec3( 1.0f, -1.0f, 1.0f )},

    {g_vec3( 1.0f, 1.0f, 1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 0.0f, 0.0f ),g_vec3( 1.0f, 1.0f, 1.0f )},
    {g_vec3( 1.0f, -1.0f, 1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 1.0f, 0.0f ),g_vec3( 1.0f, -1.0f, 1.0f )},
    {g_vec3( 1.0f, 1.0f, -1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 0.0f, 1.0f ),g_vec3( 1.0f, 1.0f, -1.0f )},
    {g_vec3( 1.0f, -1.0f, -1.0f ),g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),
    g_vec2( 1.0f, 1.0f ),g_vec3( 1.0f, -1.0f, -1.0f )},
  };

  Model->AddVertexArray(vert,12);
  if (LineStripIndices)
    Model->AddIndexArray(ls_ind,24);
  else
    Model->AddIndexArray(ts_ind,36);
}

//-----------------------------------------------

void CreateFrustumModel(g_vec3 *fr_points, GModel *OutModel, g_bool LineStripIndices, g_float Alpha)
{
  g_uint32 ts_ind[] =
  {
    2,1,0,   1,2,3,
    6,0,4,   0,6,2,
    3,5,1,   5,3,7,
    7,4,5,   4,7,6,
    10,4,0,  4,10,11,
    6,8,2,   8,6,9
  };

  g_uint32 ls_ind[] =
  {
    0,1, 1,3, 3,2, 2,0,
    4,5, 5,7, 7,6, 6,4,
    0,4, 1,5, 2,6, 3,7
  };

  G_VERTEX vert[] =
  {
    {fr_points[2],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 0 1 0
    g_vec2( 0.0f, 0.0f ),g_vec3( -1.0f, 1.0f, -1.0f )},
    {fr_points[3],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 1 1 0
    g_vec2( 1.0f, 0.0f ),g_vec3( 1.0f, 1.0f, -1.0f )},
    {fr_points[6],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 0 1 1
    g_vec2( 0.0f, 1.0f ),g_vec3( -1.0f, 1.0f, 1.0f )},
    {fr_points[7],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 1 1 1
    g_vec2( 1.0f, 1.0f ),g_vec3( 1.0f, 1.0f, 1.0f )},

    {fr_points[0],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 0 0 0 
    g_vec2( 1.0f, 0.0f ),g_vec3( -1.0f, -1.0f, -1.0f )},
    {fr_points[1],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 1 0 0
    g_vec2( 0.0f, 0.0f ),g_vec3( 1.0f, -1.0f, -1.0f )},
    {fr_points[4],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 0 0 1
    g_vec2( 1.0f, 1.0f ),g_vec3( -1.0f, -1.0f, 1.0f )},
    {fr_points[5],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 1 0 1
    g_vec2( 0.0f, 1.0f ),g_vec3( 1.0f, -1.0f, 1.0f )},

    {fr_points[7],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 1 1 1
    g_vec2( 0.0f, 0.0f ),g_vec3( 1.0f, 1.0f, 1.0f )},
    {fr_points[5],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 1 0 1
    g_vec2( 1.0f, 0.0f ),g_vec3( 1.0f, -1.0f, 1.0f )},
    {fr_points[3],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 1 1 0
    g_vec2( 0.0f, 1.0f ),g_vec3( 1.0f, 1.0f, -1.0f )},
    {fr_points[1],g_vec4( 0.0f, 0.0f, 0.0f ,Alpha),    // 1 0 0
    g_vec2( 1.0f, 1.0f ),g_vec3( 1.0f, -1.0f, -1.0f )},
  };

  OutModel->AddVertexArray(vert,12);
  if (LineStripIndices)
    OutModel->AddIndexArray(ls_ind,24);
  else
    OutModel->AddIndexArray(ts_ind,36);
}

//----------------------------------------------

void CheckMinMax(g_ivec3 &min_pos, g_ivec3 &max_pos)
{
  if (max_pos.x < min_pos.x) swap_var(max_pos.x, min_pos.x);
  if (max_pos.y < min_pos.y) swap_var(max_pos.y, min_pos.y);
  if (max_pos.z < min_pos.z) swap_var(max_pos.z, min_pos.z);
}

//-----------------------------------------------

enum g_merge_dir {g_md_horiz, g_md_vert};

g_bool MergeEqualVertices(G_CUBE_VERTEX *vb, g_uint32 *first_ib, g_uint32 *second_ib, g_merge_dir mdir)
{
  switch (mdir)
  {
    case g_md_horiz:
		 if ((vb[first_ib[3]].Pos == vb[second_ib[0]].Pos) &&
			 (vb[first_ib[5]].Pos == vb[second_ib[1]].Pos))	 
         {          
		   second_ib[0] = first_ib[0];
		   second_ib[1] = first_ib[1];
		   second_ib[4] = first_ib[4];
		   return true;
         }
		 else if ((vb[first_ib[0]].Pos == vb[second_ib[3]].Pos) &&
			      (vb[first_ib[1]].Pos == vb[second_ib[5]].Pos))
         {
           second_ib[2] = first_ib[2];
           second_ib[3] = first_ib[3];	   
		   second_ib[5] = first_ib[5];
		   return true;
         }
	     break;
    case g_md_vert:
		 if ((vb[first_ib[0]].Pos == vb[second_ib[1]].Pos) &&
			 (vb[first_ib[3]].Pos == vb[second_ib[5]].Pos))	 
         {
   	       second_ib[1] = first_ib[1];
		   second_ib[4] = first_ib[4];
		   second_ib[5] = first_ib[5];
		   return true;
         }
		 else if ((vb[first_ib[1]].Pos == vb[second_ib[0]].Pos) &&
			      (vb[first_ib[5]].Pos == vb[second_ib[3]].Pos))
         {        
		   second_ib[2] = first_ib[2];
           second_ib[3] = first_ib[3];	   
		   second_ib[0] = first_ib[0];
		   return true;
         }
		 break;
  }
  return false;
}

//-----------------------------------------------

void Optimize_VI_Buffers(G_CUBE_VERTEX *vb, g_uint32 &vb_sz, g_uint32 *ib, g_uint32 &ib_sz, 
						 g_uint32 *fb, g_uint32 &fb_sz, G_OPTFACE_INFO *face_extrainfo)
{
  if ((vb_sz == 0) || (ib_sz == 0))
	return;
  // Объединение одинаковых соседних полигонов в один
  if (face_extrainfo != NULL)
  {
    // Инициализация
	g_bool *remove_ind = new g_bool[fb_sz];
	memset(remove_ind, 0, fb_sz*sizeof(g_bool));
	// Шаг 1 (Горизонтальное объединение)
	// Alg V2 (Slow)
	g_uint32 *axis_sorted_ind[6];
	g_uint32 axis_arr_sz[6];

	for (g_uint32 i = 0; i < 6; i++)
	{
	  axis_sorted_ind[i] = new g_uint32[fb_sz];
	  axis_arr_sz[i] = 0;
	}

	for (g_uint32 i = 0; i < fb_sz; i++)
	{
	  g_int32 axis = g_int32(face_extrainfo[i].f_dir) - 1;
	  if ((face_extrainfo[i].used) && (axis > -1) && (axis < 6))
	  {		
		axis_sorted_ind[axis][axis_arr_sz[axis]] = i;
		axis_arr_sz[axis]++;
	  }
	}
	  
	for (g_uint32 i = 0; i < 6; i++)
	  for (g_uint32 j = 0; j < axis_arr_sz[i]; j++)
	    for (g_uint32 k = j+1; k < axis_arr_sz[i]; k++)
		  if (face_extrainfo[axis_sorted_ind[i][j]].f_type == face_extrainfo[axis_sorted_ind[i][k]].f_type)
            if (MergeEqualVertices(vb, &ib[axis_sorted_ind[i][j]*6], &ib[axis_sorted_ind[i][k]*6], g_md_horiz))
	        {
              remove_ind[axis_sorted_ind[i][j]] = true;
			  break;
	        }	

	// Удаление ненужных индексов
    g_int32 offs = 0;
    for (g_uint32 i = 0; i < fb_sz; i++)
	{
      if (remove_ind[i] == true)
        offs++;
      else
	  {
		memcpy(&ib[(i-offs)*6], &ib[i*6], 6*sizeof(g_uint32));
		fb[i-offs] = fb[i];
		face_extrainfo[i-offs] = face_extrainfo[i];
	  }
	}
    ib_sz -= offs*6;
	fb_sz -= offs;
    // Шаг 2 (Вертикальное объединение) 
	memset(remove_ind, 0, fb_sz*sizeof(g_bool));
	// Alg V2 (Slow)
	for (g_uint32 i = 0; i < 6; i++)
	  axis_arr_sz[i] = 0;

	for (g_uint32 i = 0; i < fb_sz; i++)
	{
	  g_int32 axis = g_int32(face_extrainfo[i].f_dir) - 1;
	  if ((face_extrainfo[i].used) && (axis > -1) && (axis < 6))
	  {		
		axis_sorted_ind[axis][axis_arr_sz[axis]] = i;
		axis_arr_sz[axis]++;
	  }
	}
	  
	for (g_uint32 i = 0; i < 6; i++)
	  for (g_uint32 j = 0; j < axis_arr_sz[i]; j++)
	    for (g_uint32 k = j+1; k < axis_arr_sz[i]; k++)
		  if (face_extrainfo[axis_sorted_ind[i][j]].f_type == face_extrainfo[axis_sorted_ind[i][k]].f_type)
            if (MergeEqualVertices(vb, &ib[axis_sorted_ind[i][j]*6], &ib[axis_sorted_ind[i][k]*6], g_md_vert))
	        {
              remove_ind[axis_sorted_ind[i][j]] = true;
			  break;
	        }	

    for (g_uint32 i = 0; i < 6; i++)
	  delete[]axis_sorted_ind[i];

	// Удаление ненужных индексов
    offs = 0;
    for (g_uint32 i = 0; i < fb_sz; i++)
	{
      if (remove_ind[i] == true)
        offs++;
      else
	  {
		memcpy(&ib[(i-offs)*6], &ib[i*6], 6*sizeof(g_uint32));
		fb[i-offs] = fb[i];
	  }
	}
    ib_sz -= offs*6;
	fb_sz -= offs;

	delete[]remove_ind;
  }

  // Найти и удалить неиспользованые вертексы
  S_USED *used = new S_USED[vb_sz];
  memset(used, 0, vb_sz*sizeof(S_USED));

  for (g_uint32 ind_n = 0; ind_n < ib_sz; ind_n++)
	used[ib[ind_n]].used = true;

  g_int32 offset = 0;
  for (g_uint32 vert_n = 0; vert_n < vb_sz; vert_n++)
  {
	if (used[vert_n].used == false)
	  offset++;
	else
	{
	  used[vert_n].offset = offset;	  
	  vb[vert_n - offset] = vb[vert_n];
	}
  }
  vb_sz -= offset;

  // Обновить индексы
  for (g_uint32 ind_n = 0; ind_n < ib_sz; ind_n++)
	ib[ind_n] -= used[ib[ind_n]].offset;

  delete[]used;
}

//-----------------------------------------------

g_uint32 PackTexCoord(g_vec3 TexCoord)
{
  return ((g_int32(TexCoord.x*100))%128 + 127) + 
		 ((g_int32(TexCoord.y*100))%128 + 127) * 256 + 
		 ((g_int32(TexCoord.z*100))%128 + 127) * 65536; 
}

//-----------------------------------------------

g_vec3 UnpackTexCoord(g_uint32 PackedTexCoord)
{
  g_vec3 Result;
  Result.x = ((PackedTexCoord%256) - 127)/100.0f;
  Result.y = ((g_int32(PackedTexCoord/256)%256) - 127)/100.0f;
  Result.z = ((g_int32(PackedTexCoord/65536)%256) - 127)/100.0f;
  return Result;
}

//-----------------------------------------------

g_uint32 PackNormal(g_vec3 Normal)
{
  g_vec3 NormalizedNormal;
  D3DXVec3Normalize(&NormalizedNormal,&Normal);

  g_uint32 res = 0;
  return g_int32(floor((NormalizedNormal.x + 1.0f) * 12)) +
         g_int32(floor((NormalizedNormal.y + 1.0f) * 12)) * 25 +
         g_int32(floor((NormalizedNormal.z + 1.0f) * 12)) * 625;
}

//-----------------------------------------------

g_vec3 UnpackNormal(g_uint32 PackedNormal)
{
  g_vec3 Result;
  Result.x = (PackedNormal%25)/12.0f - 1.0f;
  Result.y = (g_int32(PackedNormal/25)%25)/12.0f - 1.0f;
  Result.z = (g_int32(PackedNormal/625)%25)/12.0f - 1.0f;
  return Result;
}

//-----------------------------------------------
