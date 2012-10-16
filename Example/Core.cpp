#include <SDL/SDL_main.h>
#include "App.h"

int main (int argc, char * argv[])
{
	// Create a Lua state and load its standard libraries.
	lua_State * L = lua_open();

	luaopen_base(L);
	luaopen_debug(L);
	luaopen_io(L);
	luaopen_math(L);
	luaopen_string(L);
	luaopen_table(L);

	// Install some metatable support.
	lua_newtable(L);
	lua_setglobal(L, "Meta");

	// Give Lua some useful tools.
	luaopen_dirent(L);
	luaopen_graphics(L);
	luaopen_misc(L);
	luaopen_sdl(L);

	// Boot and run the application, and close Lua when done.
	lua_dofile(L, "Scripts/Boot.txt");
	lua_close(L);

	return 0;
}