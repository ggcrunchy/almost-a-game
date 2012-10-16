#ifndef APP_H
#define APP_H

#include <SDL/SDL_types.h>
extern "C"
{
#include "../Lua/lua.h"
#include "../Lua/lualib.h"
#include "../Lua/lauxlib.h"
}

void luaopen_dirent (lua_State * L);
void luaopen_graphics (lua_State * L);
void luaopen_misc (lua_State * L);
void luaopen_sdl (lua_State * L);

void Post (lua_State * L, char const * message);

Uint32 U (lua_State * L, int index);
Uint8 U8 (lua_State * L, int index);
Uint16 U16 (lua_State * L, int index);
Sint32 S32 (lua_State * L, int index);
Sint16 S16 (lua_State * L, int index);
int I (lua_State * L, int index);
long LI (lua_State * L, int index);
float F (lua_State * L, int index);
SDL_bool B (lua_State * L, int index);
char const * S (lua_State * L, int index);
void * UD (lua_State * L, int index);
void * UT (lua_State * L, int index);
Uint8 * PU8 (lua_State * L, int index);

/// @brief Structure used to register a user type member
typedef struct {
	Uint32 mOffset;	///< Offset of member into structure
	char * mName;	///< Name used to reference member
	char * mUserType;	///< If mType is eUserType, the name of the user type
	enum {
		eUserType,
		ePointer,
		eU8, eU16, eU32,
		eS8, eS16, eS32,
		eUChar, eUShort, eULong, eUInt,
		eSChar, eSShort, eSLong, eSInt,
		eString,
		eBoolean,
		eFloat
	} mType;///< Type of member
} UserType_Reg;

void PushUserType (lua_State * L, void * data, char const * name);
void RegisterUserType (lua_State * L, char const * name, UserType_Reg * reg, Uint32 count, lua_CFunction GC);

#endif // APP_H