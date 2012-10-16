#include <dirent.h>
#include "App.h"
#include <cassert>

static DIR * D (lua_State * L)
{
	return static_cast<DIR*>(UT(L, 1));
}

static struct dirent * E (lua_State * L)
{
	return static_cast<struct dirent*>(UT(L, 1));
}

static int OpenDir (lua_State * L)
{
	DIR * dir = opendir(S(L, 1));

	if (dir != 0)
	{
		PushUserType(L, dir, "DIR");

		return 1;
	}

	return 0;
}

static int ReadDir (lua_State * L)
{
	struct dirent * entry = readdir(D(L));

	if (entry != 0)
	{
		PushUserType(L, entry, "struct dirent");

		return 1;
	}

	return 0;
}

static int CloseDir (lua_State * L)
{
	DIR * dir = D(L);

	if (dir != 0) closedir(dir);

	return 0;
}

static int RewindDir (lua_State * L)
{
	DIR * dir = D(L);

	if (dir != 0) rewinddir(dir);

	return 0;
}

static int TellDir (lua_State * L)
{
	DIR * dir = D(L);

	if (dir != 0)
	{
		long index = telldir(dir);

		lua_pushnumber(L, index);

		return 1;
	}

	return 0;
}

static int SeekDir (lua_State * L)
{
	DIR * dir = D(L);

	if (dir != 0 && lua_isnumber(L, 2)) seekdir(dir, LI(L, 2));

	return 0;
}

static int DirentGet_d_namlen (lua_State * L)
{
	struct dirent * entry = E(L);

	if (entry != 0)
	{
		lua_pushnumber(L, entry->d_namlen);

		return 1;
	}

	return 0;
}

static int DirentGet_d_name (lua_State * L)
{
	struct dirent * entry = E(L);

	if (entry != 0)
	{
		lua_pushstring(L, entry->d_name);

		return 1;
	}

	return 0;
}

/// @brief Binds dirent to the Lua scripting system
void luaopen_dirent (lua_State * L)
{
	assert(L != 0);

	UserType_Reg UT[2] = { 0 };

	UT[0].mName = "d_namlen";
	UT[0].mOffset = offsetof(struct dirent, d_namlen);
	UT[0].mType = UserType_Reg::eUShort;

	UT[1].mName = "d_name";
	UT[1].mOffset = offsetof(struct dirent, d_name);
	UT[1].mType = UserType_Reg::eString;

	RegisterUserType(L, "DIR", 0, 0, 0);
	RegisterUserType(L, "struct dirent", UT, sizeof(UT) / sizeof(UT[0]), 0);

	const luaL_reg DirentFuncs[] = {
		{ "open", OpenDir },
		{ "read", ReadDir },
		{ "close", CloseDir },
		{ "rewind", RewindDir },
		{ "tell", TellDir },
		{ "seek", SeekDir },
		{ 0, 0 }
	};

	luaL_openlib(L, "dirent", DirentFuncs, 0);
}