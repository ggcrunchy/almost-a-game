#include "App.h"
#include "../Graphics/Graphics.h"
#include <iostream>
#include <cassert>

#define M_(w) { #w, w }

/// @brief Int return; void argument
static int I_V (lua_State * L, int (*func)(void))
{
	func();

	return 0;
}

/// @brief Int return; usertype argument
int I_Ut (lua_State * L, int (*func)(void *))
{
	func(UT(L, 1));

	return 0;
}

static int SetupGraphics (lua_State * L)
{
	SetupGraphics(I(L, 1), I(L, 2), I(L, 3), B(L, 4));

	return 0;
}

static int CloseGraphics (lua_State * L)
{
	return I_V(L, CloseGraphics);
}

static int SetVideoMode (lua_State * L)
{
	SetVideoMode(I(L, 1), I(L, 2), I(L, 3), B(L, 4));

	return 0;
}

static int GetVideoSize (lua_State * L)
{
	Uint32 width, height;	GetVideoSize(width, height);

	lua_pushnumber(L, width);
	lua_pushnumber(L, height);

	return 2;
}

static int PrepareFrame (lua_State * L)
{
	return I_V(L, PrepareFrame);
}

static int DrawFrame (lua_State * L)
{
	return I_V(L, DrawFrame);
}

static int LoadPicture (lua_State * L)
{
	Picture_h picture;

	if (LoadPicture(S(L, 1), F(L, 2), F(L, 3), F(L, 4), F(L, 5), picture) != 0)
	{
		PushUserType(L, picture, "Picture");

		return 1;
	}

	return 0;
}

static int DrawPicture (lua_State * L)
{
	DrawPicture(UT(L, 1), F(L, 2), F(L, 3), F(L, 4), F(L, 5));

	return 0;
}

static int SetPictureTexels (lua_State * L)
{
	SetPictureTexels(UT(L, 1), F(L, 2), F(L, 3), F(L, 4), F(L, 5));

	return 0;
}

static int GetPictureTexels (lua_State * L)
{
	float fS0, fT0, fS1, fT1;	GetPictureTexels(UT(L, 1), fS0, fT0, fS1, fT1);

	lua_pushnumber(L, fS0);
	lua_pushnumber(L, fT0);
	lua_pushnumber(L, fS1);
	lua_pushnumber(L, fT1);

	return 4;
}

static int UnloadPicture (lua_State * L)
{
	return I_Ut(L, UnloadPicture);
}

static int LoadFont (lua_State * L)
{
	Font_h font;

	if (LoadFont(S(L, 1), I(L, 2), font) != 0)
	{
		PushUserType(L, font, "Font");

		return 1;
	}

	return 0;
}

static int UnloadFont (lua_State * L)
{
	return I_Ut(L, UnloadFont);
}

static int GetTextSize (lua_State * L)
{
	int width, height;	GetTextSize(UT(L, 1), S(L, 2), width, height);

	lua_pushnumber(L, width);
	lua_pushnumber(L, height);

	return 2;
}

static int LoadTextImage (lua_State * L)
{
	SDL_Color color;

	color.r = U8(L, 3);
	color.g = U8(L, 4);
	color.b = U8(L, 5);

	TextImage_h textImage;

	if (LoadTextImage(UT(L, 1), S(L, 2), color, textImage) != 0)
	{
		PushUserType(L, textImage, "TextImage");

		return 1;
	}

	return 0;
}

static int DrawTextImage (lua_State * L)
{
	DrawTextImage(UT(L, 1), F(L, 2), F(L, 3), F(L, 4), F(L, 5));

	return 0;
}

static int DrawClippedTextImage (lua_State * L)
{
	DrawClippedTextImage(UT(L, 1), F(L, 2), F(L, 3), F(L, 4), F(L, 5), F(L, 6), F(L, 7));

	return 0;
}

static int UnloadTextImage (lua_State * L)
{
	return I_Ut(L, UnloadTextImage);
}

static int DrawBox (lua_State * L)
{
	DrawBox(F(L, 1), F(L, 2), F(L, 3), F(L, 4), F(L, 5), F(L, 6), F(L, 7));

	return 0;
}

static int DrawLine (lua_State * L)
{
	DrawLine(F(L, 1), F(L, 2), F(L, 3), F(L, 4), F(L, 5), F(L, 6), F(L, 7));

	return 0;
}

static int DrawGrid (lua_State * L)
{
	DrawGrid(F(L, 1), F(L, 2), F(L, 3), F(L, 4), F(L, 5), F(L, 6), F(L, 7), U(L, 8), U(L, 9));

	return 0;
}

static int GraphicsWasInit (lua_State * L)
{
	lua_pushboolean(L, GraphicsWasInit());

	return 1;
}

///
/// Garbage collectors
///
static int Unload (lua_State * L, int (*func)(void *))
{
	if (GraphicsWasInit()) I_Ut(L, func);

	return 0;
}

static int GC_Font (lua_State * L)
{
	return Unload(L, UnloadFont);
}

static int GC_Picture (lua_State * L)
{
	return Unload(L, UnloadPicture);
}

static int GC_TextImage (lua_State * L)
{
	return Unload(L, UnloadTextImage);
}

/// @brief Binds the graphics system to the Lua scripting system
void luaopen_graphics (lua_State * L)
{
	assert(L != 0);

	RegisterUserType(L, "Font", 0, 0, GC_Font);
	RegisterUserType(L, "Picture", 0, 0, GC_Picture);
	RegisterUserType(L, "TextImage", 0, 0, GC_TextImage);

	const luaL_reg RenderFuncs[] = {
		M_(SetupGraphics),
		M_(CloseGraphics),
		M_(SetVideoMode),
		M_(GetVideoSize),
		M_(PrepareFrame),
		M_(DrawFrame),
		M_(LoadPicture),
		M_(DrawPicture),
		M_(SetPictureTexels),
		M_(GetPictureTexels),
		M_(UnloadPicture),
		M_(LoadFont),
		M_(UnloadFont),
		M_(GetTextSize),
		M_(LoadTextImage),
		M_(DrawTextImage),
		M_(DrawClippedTextImage),
		M_(UnloadTextImage),
		M_(DrawBox),
		M_(DrawLine),
		M_(DrawGrid),
		{ 0, 0 }
	};

	luaL_openlib(L, "Render", RenderFuncs, 0);
}

#undef M_