/*
** $Id: lapi.h,v 1.1 2009-10-17 00:11:40 sbirmanns Exp $
** Auxiliary functions from Lua API
** See Copyright Notice in lua.h
*/

#ifndef lapi_h
#define lapi_h


#include "lobject.h"


LUAI_FUNC void luaA_pushobject (lua_State *L, const TValue *o);

#endif
