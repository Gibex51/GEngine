#pragma once

#include "G_GM_Structs.h"
#include "GObjectLoader.h"
#include "GCamera.h"

//-----------------------------------------------

#ifndef GM_FUNCS_H
#define GM_FUNCS_H

g_ivec2 GetRegionPosBySectorPos(g_ivec3 &spos);

g_vec3 AbsCubePosToRealPos(const g_ivec3 &abs_cpos);
g_vec3 CubePosToRealPos(g_ivec3 &cpos, g_ivec3 &spos);

g_ivec3 GetAbsCubePosByVec3(g_vec3 &inPos);
g_ivec3 GetAbsCubePosBySectorAndCubePos(g_ivec3 &cpos, g_ivec3 &spos);
void GetSectorAndCubePosByVec3(const g_vec3 inPos, g_ivec3 &sPos, g_ivec3 &cPos);
void GetSectorByVec3(g_vec3 inPos, g_ivec3 *sPos);
void GetSectorAndCubePosByAbsCubePos(const g_ivec3 &abs_cPos, g_ivec3 &sPos, g_ivec3 &cPos);

//void CreateCubeForMesh(GMesh *Mesh, g_bool LineStripIndices = false, g_float Alpha = 1.0f);
//void CreateFrustumMesh(g_vec3 *fr_points, GMesh *OutMesh, g_bool LineStripIndices = false, g_float Alpha = 1.0f);
void CreateCubeForModel(GModel *Model, g_bool LineStripIndices = false, g_float Alpha = 1.0f);
void CreateFrustumModel(g_vec3 *fr_points, GModel *OutModel, g_bool LineStripIndices = false, g_float Alpha = 1.0f);


void CheckMinMax(g_ivec3 &min_pos, g_ivec3 &max_pos);

void GetAABBFromPoints(G_AABB &AABB, g_vec3 **Points, g_uint32 nPoints);

void Optimize_VI_Buffers(G_CUBE_VERTEX *vb, g_uint32 &vb_sz, g_uint32 *ib, g_uint32 &ib_sz, 
						 g_uint32 *fb, g_uint32 &fb_sz, G_OPTFACE_INFO *face_extrainfo = NULL);

g_uint32 PackTexCoord(g_vec3 TexCoord);
g_vec3 UnpackTexCoord(g_uint32 PackedTexCoord);

g_uint32 PackNormal(g_vec3 Normal);
g_vec3 UnpackNormal(g_uint32 PackedNormal);

#endif //GM_FUNCS_H