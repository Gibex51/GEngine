#include "GCommandParser.h"

//-----------------------------------------------

GCommandParser::GCommandParser(void)
{
  
}

//-----------------------------------------------

GCommandParser::~GCommandParser(void)
{
  
}

//-----------------------------------------------

g_bool GCommandParser::ParseConsoleCmdLine(const g_string &CmdLine, G_PARSE_RESULT &ParseResult)
{
  std::string::size_type chrpos = CmdLine.find(" ");
  if (chrpos != std::string::npos)
  {
	ParseResult.Command.assign(CmdLine, 0, chrpos);
	
	g_bool QuotesOpened = false;
	std::string::size_type prev_chrpos = chrpos + 1;
	while (chrpos < CmdLine.size())
	{
	  chrpos++;
	  if (((CmdLine[chrpos] == ' ') && (!QuotesOpened)) || (chrpos == CmdLine.size()))
	  {	 
		if (chrpos - prev_chrpos > 0)
		{
		  ParseResult.ParamList.push_back(G_CMD_PARAM());
		  ParseResult.ParamList.back().PrmString.assign(CmdLine, prev_chrpos, chrpos - prev_chrpos);
		}
		prev_chrpos = chrpos + 1;
	  }
	  else if (CmdLine[chrpos] == '"')
		QuotesOpened = !QuotesOpened;
	}
  }
  else
    ParseResult.Command = CmdLine;

  return (ParseResult.Command.size() != 0);
}

//-----------------------------------------------
// G_CMD_PARAM
//-----------------------------------------------

const g_int32 G_CMD_PARAM::AsInt()
{  
  return stoi(PrmString);
}

//-----------------------------------------------

const g_float G_CMD_PARAM::AsFloat()
{
  return stof(PrmString);
}

//-----------------------------------------------

const g_bool G_CMD_PARAM::AsBool()
{
  std::transform(PrmString.begin(), PrmString.end(), PrmString.begin(), ::toupper);
  return ((PrmString == "TRUE") || (PrmString == "1") || (PrmString == "YES"));
}

//-----------------------------------------------