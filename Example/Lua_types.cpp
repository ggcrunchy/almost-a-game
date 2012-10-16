#include "App.h"
#include <cstring>

#define N_(t, p) lua_pushnumber(L, *(t*)p)

/// @brief GetMetaFields
static Uint8 * GetMetaFields (lua_State * L)
{
	Uint8 * pData = static_cast<Uint8*>(UT(L, -2));	// ..., u, k

	lua_getmetatable(L, -2);// ..., u, k, mt
	lua_insert(L, -2);	// ..., u, mt, k
	lua_gettable(L, -2);// ..., u, mt, dt
	lua_pushliteral(L, "usertype");	// ..., u, mt, dt, "usertype"
	lua_gettable(L, -2);// ..., u, mt, dt, ut
	lua_pushliteral(L, "offset");	// ..., u, mt, dt, ut, "offset"
	lua_gettable(L, -3);// ..., u, mt, dt, ut, o
	lua_pushliteral(L, "type");	// ..., u, mt, dt, ut, o, "type"
	lua_gettable(L, -4);// ..., u, mt, dt, ut, o, t
	lua_insert(L, -6);	// ..., t, u, mt, dt, ut, o
	lua_insert(L, -6);	// ..., o, t, u, mt, dt, ut
	lua_insert(L, -6);	// ..., ut, o, t, u, mt, dt

	return pData;
}

/// @brief Index access method
static int Index (lua_State * L)
{
	// Point to the requested member.
	Uint8 * pData = GetMetaFields(L);	// ut, o, t

	pData += U(L, 2);

	// Return the appropriate type.
	switch (U(L, 3))
	{
	case UserType_Reg::eUserType:
		PushUserType(L, pData, S(L, 1));
		break;
	case UserType_Reg::ePointer:
		lua_pushlightuserdata(L, *(void**)pData);
		break;
	case UserType_Reg::eU8:
		N_(Uint8, pData);
		break;
	case UserType_Reg::eS8:
		N_(Sint8, pData);
		break;
	case UserType_Reg::eU16:
		N_(Uint16, pData);
		break;
	case UserType_Reg::eS16:
		N_(Sint16, pData);
		break;
	case UserType_Reg::eU32:
		N_(Uint32, pData);
		break;
	case UserType_Reg::eS32:
		N_(Sint32, pData);
		break;
	case UserType_Reg::eUChar:
		N_(unsigned char, pData);
		break;
	case UserType_Reg::eSChar:
		N_(signed char, pData);
		break;
	case UserType_Reg::eUShort:
		N_(unsigned short, pData);
		break;
	case UserType_Reg::eSShort:
		N_(signed short, pData);
		break;
	case UserType_Reg::eULong:
		N_(unsigned long, pData);
		break;
	case UserType_Reg::eSLong:
		N_(signed long, pData);
		break;
	case UserType_Reg::eUInt:
		N_(unsigned int, pData);
		break;
	case UserType_Reg::eSInt:
		N_(signed int, pData);
		break;
	case UserType_Reg::eFloat:
		N_(float, pData);
		break;
	case UserType_Reg::eString:
		lua_pushstring(L, *(char**)pData);
		break;
	case UserType_Reg::eBoolean:
		lua_pushboolean(L, *(SDL_bool*)pData);
		break;
	}

	return 1;
}

/// @brief Index assignment method
static int NewIndex (lua_State * L)
{
	// Point to the requested member.
	lua_insert(L, 1);	// v, u, k

	Uint8 * pData = GetMetaFields(L);	// v, ut, o, t

	pData += U(L, 3);

	// Assign the appropriate type.
	switch (U(L, 4))
	{
	case UserType_Reg::eUserType:
		break;
	case UserType_Reg::ePointer:
		*(void**)pData = UD(L, 1);
		break;
	case UserType_Reg::eU8:
		*(Uint8*)pData = U8(L, 1);
		break;
	case UserType_Reg::eS8:
		break;
	case UserType_Reg::eU16:
	case UserType_Reg::eUShort:
		*(Uint16*)pData = U16(L, 1);
		break;
	case UserType_Reg::eS16:
	case UserType_Reg::eSShort:
		*(Sint16*)pData = S16(L, 1);
		break;
	case UserType_Reg::eU32:
		*(Uint32*)pData = U(L, 1);
		break;
	case UserType_Reg::eS32:
		*(Sint32*)pData = S32(L, 1);
		break;
	case UserType_Reg::eUChar:
		break;
	case UserType_Reg::eSChar:
		break;
	case UserType_Reg::eULong:
		break;
	case UserType_Reg::eSLong:
		*(long*)pData = LI(L, 1);
		break;
	case UserType_Reg::eUInt:
		break;
	case UserType_Reg::eSInt:
		break;
	case UserType_Reg::eFloat:
		*(float*)pData = F(L, 1);
		break;
	case UserType_Reg::eString:
		break;
	case UserType_Reg::eBoolean:
		*(SDL_bool*)pData = B(L, 1);
		break;
	}

	return 0;
}

/// @brief Pushes a user type onto the stack
void PushUserType (lua_State * L, void * data, char const * name)
{
	// Push the userdata on the stack and load its pointer.
	memcpy(lua_newuserdata(L, sizeof(void*)), &data, sizeof(void*));// u

	// Bind the userdata to its type's metatable.
	lua_getglobal(L, "Meta");	// u "Meta" -> u m
	lua_pushstring(L, name);// u m n
	lua_gettable(L, -2);// u m mt
	lua_setmetatable(L, -3);// u m
	lua_pop(L, 1);	// u
}

/// @brief Registers a user type with Lua
void RegisterUserType (lua_State * L, char const * name, UserType_Reg * reg, Uint32 count, lua_CFunction GC)
{
	lua_getglobal(L, "Meta");	// "Meta" -> M
	lua_pushstring(L, name);// M, N

	lua_newtable(L);// M, N, {}

	lua_pushliteral(L, "__index");	// M, N, {}, "__index"
	lua_pushcfunction(L, Index);// M, N, {}, "__index", Index
	lua_settable(L, -3);// M, N, { __index = Index }

	lua_pushliteral(L, "__newindex");	// M, N, { __index }, "__newindex"
	lua_pushcfunction(L, NewIndex);	// M, N, { __index }, "__newindex", NewIndex
	lua_settable(L, -3);// M, N, { __index, __newindex = NewIndex }

	if (GC != 0)
	{
		lua_pushliteral(L, "__gc");	// M, N, { __index, __newindex }, "__gc"
		lua_pushcfunction(L, GC);	// M, N, { __index, __newindex }, "__gc", GC
		lua_settable(L, -3);// M, N, { __index, __newindex, __gc = GC }
	}

	for (Uint32 index = 0; index < count; ++index)
	{
		lua_pushstring(L, reg[index].mName);// M, N, { __index, __newindex, [__gc], ... }, n
		lua_newtable(L);// M, N, { __index, __newindex, [__gc], ... }, n, {}

		lua_pushliteral(L, "usertype");	// M, N, { __index, __newindex, [__gc], ... }, n, {}, "usertype"
		lua_pushstring(L, reg[index].mUserType);// M, N, { __index, __newindex, [__gc], ... }, n, {}, "usertype", ut
		lua_settable(L, -3);// M, N, { __index, __newindex, [__gc], ... }, n, { usertype = ut }

		lua_pushliteral(L, "offset");	// M, N, { __index, __newindex, [__gc], ... }, n, { usertype }, "offset"
		lua_pushnumber(L, reg[index].mOffset);	// M, N, { __index, __newindex, [__gc], ... }, n, { usertype }, "offset", o
		lua_settable(L, -3);// M, N, { __index, __newindex, [__gc], ... }, n, { usertype, offset = o }

		lua_pushliteral(L, "type");	// M, N, { __index, __newindex, [__gc], ... }, n, { usertype, offset }, "type"
		lua_pushnumber(L, reg[index].mType);// M, N, { __index, __newindex, [__gc], ... }, n, { usertype, offset }, "type", t
		lua_settable(L, -3);// M, N, { __index, __newindex, [__gc], ... }, n, { usertype, offset, type = t }
		
		lua_settable(L, -3);// M, N, { __index, __newindex, [__gc], ..., n = { usertype, offset, type } }
	}

	lua_settable(L, -3);// M[N] = { ..., { __index, __newindex, [__gc], ..., n, ... }, ... }

	lua_pop(L, 1);
}

#undef N_