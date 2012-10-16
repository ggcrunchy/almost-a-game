#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL/SDL_types.h>
#include <SDL/SDL_video.h>

/* Graphics */
typedef void * Picture_h;
typedef void * Font_h;
typedef void * TextImage_h;

int SetupGraphics (int width, int height, int bpp, SDL_bool bFullscreen);
int CloseGraphics (void);
int SetVideoMode (int width, int height, int bpp, SDL_bool bFullscreen);
int GetVideoSize (Uint32 & width, Uint32 & height);
int PrepareFrame (void);
int DrawFrame (void);
int LoadPicture (char const * name, float fS0, float fT0, float fS1, float fT1, Picture_h & picture);
int DrawPicture (Picture_h picture, float fX, float fY, float fW, float fH);
int SetPictureTexels (Picture_h picture, float fS0, float fT0, float fS1, float fT1);
int GetPictureTexels (Picture_h picture, float & fS0, float & fT0, float & fS1, float & fT1);
int UnloadPicture (Picture_h picture);
int LoadFont (char const * name, int size, Font_h & font);
int UnloadFont (Font_h font);
int GetTextSize (Font_h font, char const * text, int & width, int & height);
int LoadTextImage (Font_h font, char const * text, SDL_Color color, TextImage_h & textImage);
int DrawTextImage (TextImage_h textImage, float fX, float fY, float fW, float fH);
int DrawClippedTextImage (TextImage_h textImage, float fX, float fY, float fW, float fH, float fMW, float fMH);
int UnloadTextImage (TextImage_h textImage);
int DrawBox (float fX, float fY, float fW, float fH, float fR, float fG, float fB);
int DrawLine (float fSX, float fSY, float fEX, float fEY, float fR, float fG, float fB);
int DrawGrid (float fX, float fY, float fW, float fH, float fR, float fG, float fB, Uint32 xCuts, Uint32 yCuts);

SDL_bool GraphicsWasInit (void);

#endif // GRAPHICS_H