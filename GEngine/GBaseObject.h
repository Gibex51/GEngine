#pragma once

#include "GDevice.h"
#include "FG_UIDCtrl.h"

#ifndef GBASEOBJ_H
#define GBASEOBJ_H

class GBaseObject
{
public:		
	void SetName(const g_string &Name) {_name = Name;};
	const g_string& GetName() {return _name;};

	void SetUID(const g_uint32 &UID) {_uid = UID;};
	const g_uint32& GetUID() {return _uid;};

	void SetType(const g_int32 &Type) {_type = Type;};
	const g_int32& GetType() {return _type;};

	//Конструктор и деструктор
	GBaseObject(const g_string &Name, GD3DDevice *Device);	
	virtual ~GBaseObject(void);
protected:
    GD3DDevice  *_device;
private:
	g_string    _name;
	g_int32     _type;
	g_uint32    _uid;
};

#endif /* GBASEOBJ_H */

