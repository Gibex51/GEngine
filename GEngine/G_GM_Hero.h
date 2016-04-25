#pragma once

#ifndef GGMHERO_H
#define GGMHERO_H

#include "G_GM_Unit.h"

using namespace std;

class G_GM_Hero :
  public G_GM_Unit
{
public:

  //Конструктор и деструктор
  G_GM_Hero(string Name, g_uint32 Health);
  ~G_GM_Hero(void);
private:
  
};

#endif //GGMHERO_H