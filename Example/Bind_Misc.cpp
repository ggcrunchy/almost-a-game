#include "App.h"

static int And (lua_State * L)
{
	lua_pushnumber(L, I(L, 1) & I(L, 2));

	return 1;
}

static int Or (lua_State * L)
{
	lua_pushnumber(L, I(L, 1) | I(L, 2));

	return 1;
}

static int Xor (lua_State * L)
{
	lua_pushnumber(L, I(L, 1) ^ I(L, 2));

	return 1;
}

static int Test (lua_State * L)
{
	int test = I(L, 2);

	lua_pushboolean(L, (I(L, 1) & test) == test);

	return 1;
}

static int Not (lua_State * L)
{
	lua_pushnumber(L, ~I(L, 1));

	return 1;
}

static int Lshift (lua_State * L)
{
	lua_pushnumber(L, I(L, 1) << U(L, 2));

	return 1;
}

static int Rshift (lua_State * L)
{
	lua_pushnumber(L, I(L, 1) >> U(L, 2));

	return 1;
}

static int Mod (lua_State * L)
{
	lua_pushnumber(L, I(L, 1) % U(L, 2));
	
	return 1;
}

static int BufferU8 (lua_State * L)
{								
	lua_getglobal(L, "table");	// d = { ..., d[x], ... }, "table" -> d, table
	lua_pushliteral(L, "getn");	// d, table, "getn"
	lua_gettable(L, -2);// d, table, table.getn
	lua_pushvalue(L, -3);	// d, table, table.getn, d
	lua_call(L, 1, 1);	// d, table, N

	int count = I(L, 3);

	lua_pop(L, 2);	// d

	Uint8 * pBuffer = static_cast<Uint8*>(lua_newuserdata(L, count));	// d, b = {}

	lua_insert(L, -2);	// b, d

	for (int index = 1; index <= count; ++index)
	{
		lua_pushnumber(L, index);	// b, d, i
		lua_gettable(L, -2);// b, d, d[i]

		pBuffer[index - 1] = U8(L, -1);

		lua_pop(L, 1);	// b, d
	}

	lua_pop(L, 1);	// b

	return 1;
}

static int Dump (lua_State * L)
{
	Post(L, "Dump");

	return 0;
}

/// @brief Binds miscellaneous utilities to the Lua scripting system
void luaopen_misc (lua_State * L)
{
	const luaL_reg MiscFuncs[] = {
		{ "And", And },
		{ "Or", Or },
		{ "Xor", Xor },
		{ "Test", Test },
		{ "Not", Not },
		{ "Lshift", Lshift },
		{ "Rshift", Rshift },
		{ "Mod", Mod },
		{ "BufferU8", BufferU8 },
		{ "Dump", Dump },
		{ 0, 0 }
	};

	luaL_openlib(L, "Misc", MiscFuncs, 0);
}