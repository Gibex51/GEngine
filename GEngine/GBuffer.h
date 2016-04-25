#pragma once

#include "GFont.h"

#ifndef GBUFFER_H
#define GBUFFER_H

using namespace std;

class GBuffer
{
public:
  //SS Functions
  void    SS_Free();
  void    SS_DigitSubstitution(const SCRIPT_DIGITSUBSTITUTE *pSDS, SCRIPT_CONTROL *pSC, SCRIPT_STATE *pSS);
  g_bool  SS_Analyse(g_int32 start_index);
  g_bool  SS_CPtoX(g_int32 CharPos, g_bool Trail, g_int32* pX);
  g_bool  SS_XtoCP(g_int32 X, g_int32* pCharPos, g_int32* pTrail);
  g_int32 SS_pcOutChars();
  
  void SetFont(GFont *Font) {_font = Font;NeedAnalyse = true;};
  void SetTextBoxWidth(g_uint32 tbWidth) {_tb_width = tbWidth;};
  
  //Text Functions
  void SetText(string Text) {strBuffer = Text;NeedAnalyse = true;};
  void InsertText(g_int32 Pos, string Text); //0 based index
  void AddText(string Text) {strBuffer += Text;NeedAnalyse = true;};
  void AddText(char *Text) {strBuffer += Text;NeedAnalyse = true;};

  string GetText() {return strBuffer;};
  string GetText(g_int32 First_Char, g_int32 nChars); // 1 based index

  g_int32 size() {return strBuffer.size();};
  void erase(g_int32 index) {strBuffer.erase(index);};
  void erase(g_int32 index, g_int32 num) {strBuffer.erase(index, num);};

  string operator + (string str)  {AddText(str);return strBuffer;};
  string operator + (g_char *str) {AddText(str);return strBuffer;};
  string operator + (g_char str)  {AddText(&str);return strBuffer;};
  void   operator = (string str)  {SetText(str);};
  //Конструктор и деструктор
  GBuffer(void);
  ~GBuffer(void);
private:
  SCRIPT_STRING_ANALYSIS SSA;

  string strBuffer;
  g_uint32 _tb_width;

  GFont *_font;

  bool NeedAnalyse;
};

#endif //GBUFFER_H