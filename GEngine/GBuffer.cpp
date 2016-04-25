#include "GBuffer.h"

//-----------------------------------------------

GBuffer::GBuffer(void)
{
  SSA = NULL;
  NeedAnalyse = true;
  _font = NULL;
  _tb_width = 0;
  strBuffer = "";
}

//-----------------------------------------------

GBuffer::~GBuffer(void)
{
  SS_Free();
}

//-----------------------------------------------

void GBuffer::SS_Free()
{
  if (SSA != NULL)
  {
    ScriptStringFree(&SSA);
    SSA = NULL;
  }
}

//-----------------------------------------------

void GBuffer::SS_DigitSubstitution(const SCRIPT_DIGITSUBSTITUTE *pSDS, SCRIPT_CONTROL *pSC, SCRIPT_STATE *pSS)
{
  ScriptApplyDigitSubstitution(pSDS,pSC,pSS);
}

//-----------------------------------------------

g_bool GBuffer::SS_Analyse(g_int32 start_index)
{
  SS_Free();

  SCRIPT_CONTROL ScriptControl; // For uniscribe
  SCRIPT_STATE ScriptState;   // For uniscribe
  ZeroMemory(&ScriptControl, sizeof(ScriptControl));
  ZeroMemory(&ScriptState, sizeof(ScriptState));
  SS_DigitSubstitution( NULL, &ScriptControl, &ScriptState );

  if (_font == NULL)
	return false;

  HDC nfDC = CreateCompatibleDC(_font->GetFontDC());
  
  HRESULT hr = ScriptStringAnalyse( nfDC,
                                    &strBuffer.c_str()[start_index],
                                    strBuffer.length() + 1 - start_index,
                                    (strBuffer.length() - start_index) * 3 / 2 + 16,
                                    RUSSIAN_CHARSET,
                                    SSA_BREAK | SSA_GLYPHS | SSA_FALLBACK | SSA_CLIP,
                                    _tb_width,
                                    &ScriptControl,
                                    &ScriptState,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &SSA);

  DeleteDC(nfDC);

  if (FAILED(hr))
    return false;

  NeedAnalyse = false;

  return true;
}

//-----------------------------------------------

g_bool GBuffer::SS_CPtoX(g_int32 CharPos, g_bool Trail, g_int32* pX)
{
  assert(pX!=NULL);
  *pX = 0;

  if (NeedAnalyse)
    if (!SS_Analyse(0)) 
      return false;

  if FAILED(ScriptStringCPtoX(SSA, CharPos, Trail, pX))
	return false;

  return true;
}

//-----------------------------------------------

g_bool GBuffer::SS_XtoCP(g_int32 X, g_int32* pCharPos, g_int32* pTrail)
{
  assert(pCharPos && pTrail);
  *pCharPos = 0; 
  *pTrail   = FALSE;

  if (NeedAnalyse)
    if (!SS_Analyse(0)) 
      return false;

  if FAILED(ScriptStringXtoCP(SSA, X, pCharPos, pTrail))
    return false;

  if(*pCharPos == -1 && *pTrail == TRUE)
  {
    *pCharPos = 0; 
	*pTrail = FALSE;
  }
  else 
    if((g_uint32(*pCharPos) > strBuffer.length()) && (*pTrail == FALSE) )
    {
      *pCharPos = strBuffer.length(); 
      *pTrail = TRUE;
    }

  return true;
}

//-----------------------------------------------

string GBuffer::GetText(g_int32 First_Char, g_int32 nChars)
{
  return strBuffer.substr(First_Char-1, nChars);
}

//-----------------------------------------------

void GBuffer::InsertText(g_int32 Pos, string Text)
{
  strBuffer.insert(Pos, Text);
  NeedAnalyse = true;
}

//-----------------------------------------------

g_int32 GBuffer::SS_pcOutChars()
{
  const g_int32 *res = ScriptString_pcOutChars(SSA);
  if (!res) return 0;
  return *res;
}