#pragma once

#ifndef GCTBUFMGR_H
#define GCTBUFMGR_H

#include "GDevice.h"

class G_CT_BufferMgr
{
public:

	//Конструктор и деструктор
	G_CT_BufferMgr(GD3DDevice *Device);
	~G_CT_BufferMgr(void);
private:
	GD3DDevice  *_device;
};

#endif //GCTBUFMGR_H