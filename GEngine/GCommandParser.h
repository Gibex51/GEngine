#pragma once

#ifndef GCMDPARSER_H
#define GCMDPARSER_H

#include "funcs.h"
#include <algorithm>

using namespace std;

struct G_CMD_PARAM
{
  g_string PrmString;

  const g_int32 AsInt();
  const g_float AsFloat();
  const g_bool  AsBool();
};

struct G_PARSE_RESULT
{
  g_string Command;
  vector <G_CMD_PARAM> ParamList;
};

class GCommandParser
{
public:
	g_bool ParseConsoleCmdLine(const g_string &CmdLine, G_PARSE_RESULT &ParseResult);

	//Конструктор и деструктор
    GCommandParser(void);
	~GCommandParser(void);
private:

};

#endif //GCMDPARSER_H