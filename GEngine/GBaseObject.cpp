#include "GBaseObject.h"

//-----------------------------------------------

GBaseObject::GBaseObject(const g_string &Name, GD3DDevice *Device)
{
  _name = Name;
  _type = 0;
  _uid = 0;
  _device = Device;
}

//-----------------------------------------------

GBaseObject::~GBaseObject(void)
{

}


//-----------------------------------------------
