#pragma once

#include <vector>
#include <windows.h>
#include <stdio.h>
#include "GConstants.h"

#ifndef FUNCS_H
#define FUNCS_H

template<typename TempType> void swap_var( TempType &var1, TempType &var2 )
{
  TempType tmp(var1);
  var1 = var2;
  var2 = tmp;
}

template<typename TempType> void clear_del_vector( std::vector <TempType> &vec )
{
  for (g_uint32 i = 0; i < vec.size(); i++)
    delete vec[i];
  vec.clear();
}

g_int32 g_round(g_double dvar);

void ResizeRect(g_rect &Rect, g_int32 width, g_int32 height);
void MoveRect(g_rect &Rect, g_int32 x, g_int32 y);
void ReduceRect(g_rect &DestRect, const g_rect &ModRect);

void CheckMinMax(g_vec3 &min_pos, g_vec3 &max_pos);

void trim(char *pinstr, char *poutstr, g_int32 instrsize);

void fill_word(void *buffer, unsigned int bsize, unsigned short value);
void fill_dword(void *buffer, unsigned int bsize, unsigned int value);

g_bool ReadEntireFile(const g_char *FileName, g_char **outbuffer, g_uint32 *size); 

void ToHexString(void *InBuffer, int BufferSize, char *OutString);

void CheckRange(int* Value, int Min, int Max);
void CheckRange(short* Value, short Min, short Max);
void CheckRange(float* Value, float Min, float Max);
void CheckRangeVec(g_vec3* Value, g_vec3 Min, g_vec3 Max);

double GetTicks();

bool PointInRect(RECT Rect, POINT Point);

g_bool PointInTriangle(g_vec3 p, g_vec3 tri0, g_vec3 tri1, g_vec3 tri2);

g_vec3 CalcFaceNormal(g_vec3 p1, g_vec3 p2, g_vec3 p3);
g_vec3 CalcFaceNormal(g_vec4 p1, g_vec4 p2, g_vec4 p3);

g_float CalcVec3Angle(g_vec3 *vec1, g_vec3 *vec2, g_vec3 *axis);

void AbsVec(g_vec2 *Vec);
void AbsVec(g_vec3 *Vec);
void AbsVec(g_vec4 *Vec);

g_bool GetIntersectionPoint(g_vec3 fnorm, g_vec3 fp1, g_vec3 pos_start, g_vec3 pos_end, g_vec3 *ipoint);
g_bool GetIntersectionPoint2(g_vec3 fnorm, g_vec3 fp1, g_vec3 pos_start, g_vec3 norm_dir, g_vec3 *ipoint);
g_bool CheckCubeIntersect(g_vec3 &pos, g_vec3 &dir, g_vec3 &min, g_vec3 &max);

unsigned int CheckSum32(void *buffer, int size);

bool FileExists(const char* FileName);
bool DirectoryExists(const char* DirName);
BOOL RecursiveCreateDirectory(const char *szDirPath, LPSECURITY_ATTRIBUTES lpSA);
bool IsDir(LPWIN32_FIND_DATA pFindData);
const g_string GetFullTempPath();

void WriteToLogFile(const char *mesg);

void Crypt_v1(const char *SrcBuffer,int BufSize,const char *Key,int KeySize,char *OutBuffer,int *OutSize);

HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition);
HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset);

void CheckTimeAndWriteToFile(g_double start_time, g_string comment);

void SetLogFunction(void (*LogCallback)(const std::string&));
void WriteToLogProc(const std::string &Message);

void AlignSizeToMin(g_int32 st1, g_int32 st2, g_int32 &end1, g_int32 &end2);

g_bool IsVectorEqual(const g_vec3 &v1, const g_vec3 &v2);

#endif // FUNCS_H
