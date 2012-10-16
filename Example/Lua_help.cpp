#include "App.h"
#include <cstring>
#include <iostream>

// @brief Posts an error and prints a string
void Post (lua_State * L, char const * message)
{
	for (int level = 0; ; ++level)
	{
		lua_Debug debug = { 0 };

		if (lua_getstack(L, level, &debug) == 0) break;

		lua_getinfo(L, "Sl", &debug);

		std::cerr << "Level " << level << std::endl;
		std::cerr << "\tsource: " << debug.source << std::endl;
		
		if (debug.currentline != -1) std::cerr << "\tcurrentline: " << debug.currentline << std::endl;
	}

	lua_pushstring(L, message);
	lua_error(L);
}

/// @brief Validates and returns a Uint32 argument
Uint32 U (lua_State * L, int index)
{
	return static_cast<Uint32>(luaL_checkint(L, index));
}

/// @brief Validates and returns a Uint8 argument
Uint8 U8 (lua_State * L, int index)
{
	return static_cast<Uint8>(luaL_checkint(L, index));
}

/// @brief Validates and returns a Uint16 argument
Uint16 U16 (lua_State * L, int index)
{
	return static_cast<Uint16>(luaL_checknumber(L, index));
}

/// @brief Validates and returns a Sint32 argument
Sint32 S32 (lua_State * L, int index)
{
	return static_cast<Sint32>(luaL_checknumber(L, index));
}

/// @brief Validates and returns a Sint16 argument
Sint16 S16 (lua_State * L, int index)
{
	return static_cast<Sint16>(luaL_checknumber(L, index));
}

/// @brief Validates and returns an int argument
int I (lua_State * L, int index)
{
	return luaL_checkint(L, index);
}

/// @brief Validates and returns a long int argument
long LI (lua_State * L, int index)
{
	return luaL_checklong(L, index);
}

/// @brief Validates and return a float argument
float F (lua_State * L, int index)
{
	return static_cast<float>(luaL_checknumber(L, index));
}

/// @brief Validates and returns an SDL_bool argument
SDL_bool B (lua_State * L, int index)
{
	luaL_checktype(L, index, LUA_TBOOLEAN);

	return static_cast<SDL_bool>(lua_toboolean(L, index));
}

/// @brief Validates and returns a char const * argument
char const * S (lua_State * L, int index)
{
	return luaL_checkstring(L, index);
}

/// @brief Validates and returns a void * argument
void * UD (lua_State * L, int index)
{
	int type = lua_type(L, index);

	if (type != LUA_TLIGHTUSERDATA && type != LUA_TUSERDATA) luaL_error(L, "Argument %i is not a userdata", index);

	return lua_touserdata(L, index);
}

/// @brief Validates and returns a void * argument
void * UT (lua_State * L, int index)
{
	luaL_checktype(L, index, LUA_TUSERDATA);

	return *(void**)UD(L, index);
}

/// @brief Validates and returns a Uint8 * argument
Uint8 * PU8 (lua_State * L, int index)
{
	return static_cast<Uint8*>(UD(L, index));
}