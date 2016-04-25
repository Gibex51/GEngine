#include "GLight.h"

//-----------------------------------------------

GLight::GLight(g_char *Name, GD3DDevice *Device):GBaseObject(Name,Device)
{
  ZeroMemory(&LightSet,sizeof(G_LIGHT_SET));
}

//-----------------------------------------------

GLight::~GLight(void)
{

}

//-----------------------------------------------
