#include "funcs.h"

#define base64ABC "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
#define hexletters "0123456789ABCDEF"

//-----------------------------------------------

void (*__logcallback__)(const std::string&) = NULL;

//-----------------------------------------------

void fill_dword(void *buffer, unsigned int bsize, unsigned int value)
{
  __asm
  {
    push eax
    push ecx
    push edi

    mov eax,value
    mov ecx,bsize
    mov edi,buffer
    rep stosd

    pop eax
    pop ecx
    pop edi
  }
}

//-----------------------------------------------

void fill_word(void *buffer, unsigned int bsize, unsigned short value)
{
  __asm
  {
    push eax
    push ecx
    push edi

    xor eax,eax
    mov ax,value
    mov ecx,bsize
    mov edi,buffer
    rep stosw

    pop eax
    pop ecx
    pop edi
  }
}

//-----------------------------------------------

g_int32 g_round(g_double fvar)
{
  if (fvar >= 0)
	return g_int32(fvar + 0.5f);
  return g_int32(fvar - 0.5f);
}

//-----------------------------------------------

void ResizeRect(g_rect &Rect, g_int32 width, g_int32 height)
{
  Rect.right = Rect.left + width;
  Rect.bottom = Rect.top + height;
}

//-----------------------------------------------

void MoveRect(g_rect &Rect, g_int32 x, g_int32 y)
{
  Rect.right = x + (Rect.right - Rect.left); Rect.left = x; 
  Rect.bottom = y + (Rect.bottom - Rect.top); Rect.top = y; 
}

//-----------------------------------------------

void ReduceRect(g_rect &DestRect, const g_rect &ModRect)
{
  DestRect.left += ModRect.left;
  DestRect.top += ModRect.top;

  DestRect.right -= ModRect.right;
  DestRect.bottom -= ModRect.bottom;
}

//-----------------------------------------------

double GetTicks()
{
  /*LARGE_INTEGER PC,PF;
  QueryPerformanceFrequency(&PF);
  QueryPerformanceCounter(&PC);

  return double(1000.0f * PC.QuadPart)/PF.QuadPart;*/ //Глючит
  return timeGetTime();
}

//-----------------------------------------------

void ToHexString(void *InBuffer, int BufferSize, char *OutString)
{
  byte *buf = (byte*)InBuffer;
  for (int i = 0; i<BufferSize; i++)
  {
	byte l2 = buf[i] % 16;
	byte l1 = (buf[i] - l2)/16;
	OutString[i*2]   = hexletters[l1];
	OutString[i*2+1] = hexletters[l2];
  }
  OutString[BufferSize*2] = 0;
}

//-----------------------------------------------

bool PointInRect(RECT Rect, POINT Point)
{
  return (((Point.x > Rect.left) && (Point.x < Rect.right)) &&
	      ((Point.y > Rect.top) && (Point.y < Rect.bottom)));
}

//-----------------------------------------------

g_vec3 CalcFaceNormal(g_vec3 p1, g_vec3 p2, g_vec3 p3)
{
  g_vec3 p1p2 = p1-p2;
  g_vec3 p1p3 = p1-p3;
  g_vec3 cross,result;
  D3DXVec3Cross(&cross,&p1p2,&p1p3);
  D3DXVec3Normalize(&result,&cross);
  return result;
}

//-----------------------------------------------

g_vec3 CalcFaceNormal(g_vec4 p1, g_vec4 p2, g_vec4 p3)
{
  g_vec3 p1p2 = p1-p2;
  g_vec3 p1p3 = p1-p3;
  g_vec3 cross,result;
  D3DXVec3Cross(&cross,&p1p2,&p1p3);
  D3DXVec3Normalize(&result,&cross);
  return result;
}

//-----------------------------------------------

g_float CalcVec3Angle(g_vec3 *vec1, g_vec3 *vec2, g_vec3 *axis)
{
  D3DXVec3Normalize(vec1,vec1);
  D3DXVec3Normalize(vec2,vec2);

  if ((D3DXVec3Length(vec1) == 0) || (D3DXVec3Length(vec2) == 0))
    return 0;

  g_mat4x4 _mat = D3DXMATRIX(axis->x, axis->y, axis->z, 0,
                             vec1->x, vec1->y, vec1->z, 0,
                             vec2->x, vec2->y, vec2->z, 0,
                             0,0,0,1);

  g_float det = D3DXMatrixDeterminant(&_mat);  
  g_float sign = 1; 
  if (det != 0)
    sign = det/abs(det);

  return acos(vec1->x*vec2->x + vec1->y*vec2->y +vec1->z*vec2->z)/G_PI_180*sign; 
}

//-----------------------------------------------

void AbsVec(g_vec2 *Vec)
{
  Vec->x = abs(Vec->x);
  Vec->y = abs(Vec->y);
}

//-----------------------------------------------

void AbsVec(g_vec3 *Vec)
{
  Vec->x = abs(Vec->x);
  Vec->y = abs(Vec->y);
  Vec->z = abs(Vec->z);
}

//-----------------------------------------------

void AbsVec(g_vec4 *Vec)
{
  Vec->x = abs(Vec->x);
  Vec->y = abs(Vec->y);
  Vec->z = abs(Vec->z);
  Vec->w = abs(Vec->w);
}

//-----------------------------------------------

g_bool GetIntersectionPoint(g_vec3 fnorm, g_vec3 fp1, g_vec3 pos_start, g_vec3 pos_end, g_vec3 *ipoint)
{
  g_vec3 dir = pos_end - pos_start;

  g_float pdist = sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);

  if (pdist == 0)
    return false;

  dir /= pdist;

  g_float dp = D3DXVec3Dot(&fnorm,&dir);

  if (dp >= 0)
    return false;

  g_vec3 p1pos1 = fp1 - pos_start;

  g_float dist = D3DXVec3Dot(&fnorm,&p1pos1)/dp;

  if (dist < 0 || dist > pdist)
    return false;

  *ipoint = pos_start + dir*dist;

  return true;
}

//-----------------------------------------------

g_bool GetIntersectionPoint2(g_vec3 fnorm, g_vec3 fp1, g_vec3 pos_start, g_vec3 norm_dir, g_vec3 *ipoint)
{
  g_float dp = D3DXVec3Dot(&fnorm,&norm_dir);

  if (dp >= 0)
    return false;

  g_vec3 p1pos1 = fp1 - pos_start;

  g_float dist = D3DXVec3Dot(&fnorm,&p1pos1)/dp;

  if (dist < 0)
    return false;

  *ipoint = pos_start + norm_dir*dist;

  return true;
}

//-----------------------------------------------

g_bool CheckCubeIntersect(g_vec3 &pos, g_vec3 &dir, g_vec3 &min, g_vec3 &max)
{
  g_vec3 ipoint;
  if (GetIntersectionPoint2(g_vec3(0,1,0),g_vec3(min.x,max.y,min.z),pos,dir,&ipoint))
    if ((ipoint.x >= min.x) && (ipoint.x <= max.x) && (ipoint.z >= min.z) && (ipoint.z <= max.z))
      return true;
  if (GetIntersectionPoint2(g_vec3(0,-1,0),g_vec3(min.x,min.y,min.z),pos,dir,&ipoint))
    if ((ipoint.x >= min.x) && (ipoint.x <= max.x) && (ipoint.z >= min.z) && (ipoint.z <= max.z))
      return true;
  if (GetIntersectionPoint2(g_vec3(1,0,0),g_vec3(max.x,min.y,min.z),pos,dir,&ipoint))
    if ((ipoint.y >= min.y) && (ipoint.y <= max.y) && (ipoint.z >= min.z) && (ipoint.z <= max.z))
      return true;
  if (GetIntersectionPoint2(g_vec3(-1,0,0),g_vec3(min.x,min.y,min.z),pos,dir,&ipoint))
    if ((ipoint.y >= min.y) && (ipoint.y <= max.y) && (ipoint.z >= min.z) && (ipoint.z <= max.z))
      return true;
  if (GetIntersectionPoint2(g_vec3(0,0,1),g_vec3(min.x,min.y,max.z),pos,dir,&ipoint))
    if ((ipoint.x >= min.x) && (ipoint.x <= max.x) && (ipoint.y >= min.y) && (ipoint.y <= max.y))
      return true;
  if (GetIntersectionPoint2(g_vec3(0,0,-1),g_vec3(min.x,min.y,min.z),pos,dir,&ipoint))
    if ((ipoint.x >= min.x) && (ipoint.x <= max.x) && (ipoint.y >= min.y) && (ipoint.y <= max.y))
      return true;
  return false;
}

//-----------------------------------------------

void CheckRange(int* Value, int Min, int Max)
{
  if (Value != NULL)
  {
    if (*Value < Min) *Value = Min;
    if (*Value > Max) *Value = Max;
  }
}

//-----------------------------------------------

void CheckRange(short* Value, short Min, short Max)
{
  if (Value != NULL)
  {
    if (*Value < Min) *Value = Min;
    if (*Value > Max) *Value = Max;
  }
}

//-----------------------------------------------

void CheckRange(float* Value, float Min, float Max)
{
  if (Value != NULL)
  {
    if (*Value < Min) *Value = Min;
    if (*Value > Max) *Value = Max;
  }
}

//-----------------------------------------------

void CheckRangeVec(g_vec3* Value, g_vec3 Min, g_vec3 Max)
{
  if (Value != NULL)
  {
    if (Value->x < Min.x) Value->x = Min.x;
    if (Value->y < Min.y) Value->y = Min.y;
    if (Value->z < Min.z) Value->z = Min.z;
  
    if (Value->x > Max.x) Value->x = Max.x;
    if (Value->y > Max.y) Value->y = Max.y;
    if (Value->z > Max.z) Value->z = Max.z;
  }
}

//------------------------------------------------------------------------------

unsigned int CheckSum32(void *buffer, int size)
{
  unsigned int result = 0x01100110;
  unsigned int nBlocks = size/4;
  for (unsigned int i=0;i<nBlocks;i++)
    result += ((unsigned int*)buffer)[i];
  return result;       
}

//------------------------------------------------------------------------------

bool FileExists(const char* FileName)
{
  return (GetFileAttributes(FileName) != -1);
}

//------------------------------------------------------------------------------

bool DirectoryExists(const char* DirName)
{
  int Code = GetFileAttributes(DirName);
  return (Code != -1) && (FILE_ATTRIBUTE_DIRECTORY & Code);
}

//------------------------------------------------------------------------------

BOOL RecursiveCreateDirectory(const char *szDirPath, LPSECURITY_ATTRIBUTES lpSA)
{
  const char szSlash[] = "\\";
  BOOL bRet = TRUE;
 
  if(!strstr(szDirPath, szSlash))
    return CreateDirectoryA(szDirPath, lpSA);

  int size = strlen(szDirPath)+1;
  char* szDblDirPath  = new char[size];
  strcpy_s(szDblDirPath,size,szDirPath);
  char* pszDblDirPath = szDblDirPath;
 
  while((pszDblDirPath != NULL) && (bRet == TRUE))
  {
    pszDblDirPath = strstr(pszDblDirPath, szSlash);
    if(pszDblDirPath!=NULL)
    {
      if(*(pszDblDirPath - 1) == ':' && strstr(pszDblDirPath + 1, szSlash ) == 0)
      {
        bRet = CreateDirectoryA(pszDblDirPath, lpSA);
        break;
      }
                        
      if(*(pszDblDirPath - 1) == ':')
        pszDblDirPath = strstr(pszDblDirPath + 1, szSlash);
      
      *pszDblDirPath = 0;
      if(!::CreateDirectoryA(szDblDirPath, lpSA))
        if(::GetLastError() != ERROR_ALREADY_EXISTS)
        {
          bRet = FALSE;
          break;
        }
      
      if(pszDblDirPath)
      {
        *pszDblDirPath = szSlash[0];
        ++pszDblDirPath;
      }
    }
  }

  delete szDblDirPath;
  return bRet;
}

//------------------------------------------------------------------------------

bool IsDir(LPWIN32_FIND_DATA pFindData)
{
  return (((pFindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) && 
          (strncmp(pFindData->cFileName,".",1) != 0));
}

//------------------------------------------------------------------------------

void Crypt_v1(const char *SrcBuffer,int BufSize,const char *Key,int KeySize,char *OutBuffer,int *OutSize)
{
  //For Test
  memcpy(OutBuffer,SrcBuffer,BufSize);
  *OutSize = BufSize;
}

//------------------------------------------------------------------------------

g_bool ReadEntireFile(const g_char *FileName, g_char **outbuffer, g_uint32 *size)
{
  if (FileExists(FileName))
  {
    FILE *r_file;
    if (fopen_s(&r_file,FileName,"a+b") != 0)
      return false;

    fseek(r_file , 0 , SEEK_END);
    *size = ftell(r_file);
  
    rewind(r_file);

    *outbuffer = new g_char[*size];
    memset(*outbuffer, 0, *size);
    fread (*outbuffer, 1, *size, r_file); 

    fclose(r_file);
  }
  else
    return false;

  return true;
}

//------------------------------------------------------------------------------

void WriteToLogFile(const char *mesg)
{
  FILE *logfile;
  if (fopen_s(&logfile,"..\\Logs\\log1.txt","a+") == 0)
  {
    fwrite(mesg,1,strlen(mesg),logfile);
    fclose(logfile);
  }
}

//------------------------------------------------------------------------------

g_bool SameSide(g_vec3 p1, g_vec3 p2, g_vec3 a, g_vec3 b)
{
  g_vec3 ba = b - a;
  g_vec3 p1a = p1 - a;
  g_vec3 p2a = p2 - a;
  g_vec3 cp1,cp2;

  D3DXVec3Cross(&cp1,&ba,&p1a);
  D3DXVec3Cross(&cp2,&ba,&p2a);
  return (D3DXVec3Dot(&cp1,&cp2) >= 0);
}

//------------------------------------------------------------------------------

bool PointInTriangle(g_vec3 p, g_vec3 tri0, g_vec3 tri1, g_vec3 tri2)
{
  return (SameSide(p,tri0,tri1,tri2) &&
          SameSide(p,tri1,tri0,tri2) &&
          SameSide(p,tri2,tri1,tri0));
}

//------------------------------------------------------------------------------

void trim(char *pinstr, char *poutstr, g_int32 instrsize)
{
  g_int32 str_beg = 0, str_end = instrsize - 2;
  
  while((pinstr[str_beg] == ' ') || (pinstr[str_beg] == '\t')) 
  {
    if (str_beg == str_end) break;
    str_beg++;    
  }

  while((pinstr[str_end] == ' ') || (pinstr[str_end] == '\t')) 
  {
    if (str_end == 0) break;
    str_end--;
  }

  memset(poutstr,0,instrsize);
  strncpy_s(poutstr, instrsize, &pinstr[str_beg], (str_end - str_beg) + 1);
} 

//------------------------------------------------------------------------------

HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
{
  HRESULT hr = S_OK;
  if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
    return HRESULT_FROM_WIN32( GetLastError() );

  DWORD dwChunkType;
  DWORD dwChunkDataSize;
  DWORD dwRIFFDataSize = 0;
  DWORD dwFileType;
  DWORD bytesRead = 0;
  DWORD dwOffset = 0;

  while (hr == S_OK)
  {
    DWORD dwRead;
    if(0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
      hr = HRESULT_FROM_WIN32( GetLastError() );

    if(0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
      hr = HRESULT_FROM_WIN32( GetLastError() );

    switch (dwChunkType)
    {
      case fourccRIFF:
             dwRIFFDataSize = dwChunkDataSize;
             dwChunkDataSize = 4;
             if(0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
               hr = HRESULT_FROM_WIN32(GetLastError());
             break;

      default:
             if(INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
             return HRESULT_FROM_WIN32(GetLastError());            
    }

    dwOffset += sizeof(DWORD) * 2;
        
    if (dwChunkType == fourcc)
    {
      dwChunkSize = dwChunkDataSize;
      dwChunkDataPosition = dwOffset;
      return S_OK;
    }

    dwOffset += dwChunkDataSize;
        
    if (bytesRead >= dwRIFFDataSize) return S_FALSE;
  }
    
  return S_OK;  
}

//------------------------------------------------------------------------------

HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset)
{
  HRESULT hr = S_OK;
  if(INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
    return HRESULT_FROM_WIN32(GetLastError());
  DWORD dwRead;
  if(0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
    hr = HRESULT_FROM_WIN32(GetLastError());
  return hr;
}

//------------------------------------------------------------------------------

void CheckTimeAndWriteToFile(g_double start_time, g_string comment)
{
  start_time = GetTicks() - start_time;
  g_char outstr[50];
  sprintf_s(outstr, 50, "%s:%.6g ms\n", comment.c_str(), start_time);

  WriteToLogProc(outstr);
  /*FILE *f;
  if (fopen_s(&f, "..\\Logs\\sp.txt", "a") == 0)
  {
    fwrite(outstr, 1, strlen(outstr), f); 
    fclose(f);
  }*/
}

//------------------------------------------------------------------------------

void WriteToLogProc(const std::string &Message)
{
  if (__logcallback__ != NULL)
	__logcallback__(Message);
}

//------------------------------------------------------------------------------

void SetLogFunction(void (*LogCallback)(const std::string&)) 
{
  __logcallback__ = LogCallback;
}

//------------------------------------------------------------------------------

void AlignSizeToMin(g_int32 st1, g_int32 st2, g_int32 &end1, g_int32 &end2)
{
  g_int32 sz1 = end1 - st1;
  g_int32 sz2 = end2 - st2;
  if (abs(sz1) > abs(sz2)) 
  {
    if (sz1 > 0) end1 -= (abs(sz1) - abs(sz2));
    if (sz1 < 0) end1 += (abs(sz1) - abs(sz2));
  }
  if (abs(sz2) > abs(sz1))  
  {
    if (sz2 > 0) end2 -= (abs(sz2) - abs(sz1));
    if (sz2 < 0) end2 += (abs(sz2) - abs(sz1));
  }
}

//------------------------------------------------------------------------------

void CheckMinMax(g_vec3 &min_pos, g_vec3 &max_pos)
{
  if (max_pos.x < min_pos.x) swap_var(max_pos.x, min_pos.x);
  if (max_pos.y < min_pos.y) swap_var(max_pos.y, min_pos.y);
  if (max_pos.z < min_pos.z) swap_var(max_pos.z, min_pos.z);
}

//------------------------------------------------------------------------------

g_bool IsVectorEqual(const g_vec3 &v1, const g_vec3 &v2)
{
  return ((abs(v1.x - v2.x) < G_HI_EPS) && 
	      (abs(v1.y - v2.y) < G_HI_EPS) &&
		  (abs(v1.z - v2.z) < G_HI_EPS));
}

//------------------------------------------------------------------------------

const g_string GetFullTempPath() 
{
  g_char buffer[MAX_PATH];
  GetTempPathA(MAX_PATH, buffer);
  g_string temp_path = buffer;
  GetLongPathNameA(temp_path.c_str(), buffer, MAX_PATH);
  temp_path = buffer;
  return temp_path;
}

//------------------------------------------------------------------------------
