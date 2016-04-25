#pragma once

#include "GStructs.h"

#ifndef FG_UIDCTRL_H
#define FG_UIDCTRL_H

void FG_AddUID(g_uint32 uid);
void FG_CreateUID(void *obj);

G_UID* FG_GetUID(g_uint32 uid);

#endif // FG_UIDCTRL_H

