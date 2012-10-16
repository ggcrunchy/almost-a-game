#include "Graphics_Imp.h"
#include "Graphics.h"
#include <iostream>

/// @brief Sets up the renderer used by the editor
/// @param width Screen width of mode
/// @param height Screen height of mode
/// @param bpp Bits per pixel of mode
/// @param bFullscreen If true, this is a full-screen video mode
/// @return 0 on failure, non-0 for success
/// @note Tested
int SetupGraphics (int width, int height, int bpp, SDL_bool bFullscreen)
{
	// Set up the video subsystem, enabling rendering.
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "Unable to initialize SDL video: " << SDL_GetError() << std::endl;

		return 0;
	}

	// Set up TrueType font support.
	if (FT_Init_FreeType(&Graphics::Main::Get().mFreeType) != 0)
	{
		std::cerr << "Unable to initialize FreeType2." << std::endl;

		return 0;
	}

	// Set some OpenGL configuration properties. Set the requested video mode.
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if (SetVideoMode(width, height, bpp, bFullscreen) == 0) return 0;

	// Set some "nice" initial graphical properties. Flag the initialization.
	glShadeModel(GL_SMOOTH);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Graphics::Main::Get().mInit = true;

	return 1;
}

/// @brief Closes the renderer used by the editor
/// @return 0 on failure, non-0 for success
/// @note Tested
int CloseGraphics (void)
{
	Graphics::Main & g = Graphics::Main::Get();

	// Unload all pictures; doing so unloads images, as well.
	while (!g.mPictures.empty())
	{
		UnloadPicture(*g.mPictures.begin());
	}

	// Unload all text images and fonts.
	while (!g.mTextImages.empty())
	{
		UnloadTextImage(*g.mTextImages.begin());
	}

	while (!g.mFaces.empty())
	{
		Graphics::Face * pFace = g.mFaces.begin()->second;

		while (!pFace->mSizes.empty())
		{
			UnloadFont(pFace->mSizes.begin()->second);
		}
	}

	// Close TrueType font support.
	FT_Done_FreeType(g.mFreeType);

	// Close the video subsystem.
	if (SDL_WasInit(SDL_INIT_VIDEO)) SDL_QuitSubSystem(SDL_INIT_VIDEO);

	// Flag the termination
	g.mInit = false;

	return 1;
}

/// @brief Sets the renderer's video mode
/// @param width Screen width of mode
/// @param height Screen height of mode
/// @param bpp Bits per pixel of mode
/// @param bFullscreen If true, this is a full-screen video mode
/// @return 0 on failure, non-0 for success
/// @note Tested
int SetVideoMode (int width, int height, int bpp, SDL_bool bFullscreen)
{
	Graphics::Main & g = Graphics::Main::Get();

	// Attempt to set an OpenGL video mode, either windowed or full-screen.
	Uint32 flags = SDL_OPENGL | (bFullscreen ? SDL_FULLSCREEN : 0);

	if (SDL_SetVideoMode(width, height, bpp, flags) == 0)
	{
		std::cerr << "Unable to set video mode: " << SDL_GetError() << std::endl;

		return 0;
	}
	// Record the resolution. Assign OpenGL's viewport, using the new dimensions to set an
	// orthographic projection.
	g.mResW = width;
	g.mResH = height;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0, double(width), 0.0, double(height), +1.0, -1.0);

	return 1;
}

/// @brief Gets the video dimensions
/// @param width [out] On success, the video width
/// @param height [out] On success, the video height
/// @return 0 on failure, non-0 for success
/// @note Tested
int GetVideoSize (Uint32 & width, Uint32 & height)
{
	Graphics::Main & g = Graphics::Main::Get();

	width = g.mResW;
	height = g.mResH;

	return 1;
}

/// @brief Prepares a render frame used by the editor
/// @return 0 on failure, non-0 for success
/// @note Tested
int PrepareFrame (void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	return 1;
}

/// @brief Performs the rendering for the editor
/// @return 0 on failure, non-0 for success
/// @note Tested
int DrawFrame (void)
{
	SDL_GL_SwapBuffers();

	return 1;
}

/// @brief Instantiates a picture object
/// @param name Name of file used to load picture's image data
/// @param fS0 Initial texture s-coordinate
/// @param fT0 Initial texture t-coordinate
/// @param fS1 Terminal texture s-coordinate
/// @param fT1 Terminal texture t-coordinate
/// @param picture [out] On success, handle to a picture object
/// @return 0 on failure, non-0 for success
/// @note Tested
int LoadPicture (char const * name, float fS0, float fT0, float fS1, float fT1, Picture_h & picture)
{
	if (0 == name) return 0;
	if (0 == picture) return 0;

	Graphics::Main & g = Graphics::Main::Get();

	// Look for the image associated with the file name. If it is not found, make a new
	// image and load it into the image map. Attempt to load data into it.
	if (g.mImages.find(name) == g.mImages.end())
	{
		try {
			g.mImages[name] = new Graphics::Image(name);
		} catch (std::bad_alloc &) {
			return 0;
		}
	}

	// Create a new picture bound to the image, assign its texture coordinates, and add it
	// to the core's list.
	Graphics::Picture * Pic = new Graphics::Picture(g.mImages[name]);

	Pic->mS0 = fS0;
	Pic->mT0 = fT0;
	Pic->mS1 = fS1;
	Pic->mT1 = fT1;

	g.mPictures.push_back(Pic);

	picture = Pic;

	return 1;
}

/// @brief Renders a picture
/// @param picture Handle to the picture object
/// @param fX Screen x coordinate, in [0, 1]
/// @param fY Screen y coordinate, in [0, 1]
/// @param fW Screen width, in [0, 1]
/// @param fH Screen height, in [0, 1]
/// @return 0 on failure, non-0 for success
/// @note Tested
int DrawPicture (Picture_h picture, float fX, float fY, float fW, float fH)
{
	if (0 == picture) return 0;

	Graphics::Main & g = Graphics::Main::Get();

	// Transform the provided coordinates and dimensions into a form OpenGL expects, and
	// scale them to the current resolution.
	float fSX = fX;
	float fEX = fX + fW;
	float fSY = 1.0f - fY - fH;
	float fEY = 1.0f - fY;

	fSX *= g.mResW;
	fEX *= g.mResW;
	fSY *= g.mResH;
	fEY *= g.mResH;

	// Convert the handle to a usable form. Set OpenGL up to render the picture's image
	// texture and draw a quad with the requested properties, using the texture data 
	// assigned to the picture.
	Graphics::Picture * Pic = static_cast<Graphics::Picture*>(picture);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Pic->mImage->mTexture);

	glBegin(GL_QUADS);
		glTexCoord2f(Pic->mS0, Pic->mT1);	glVertex2f(fSX, fSY);
		glTexCoord2f(Pic->mS1, Pic->mT1);	glVertex2f(fEX, fSY);
		glTexCoord2f(Pic->mS1, Pic->mT0);	glVertex2f(fEX, fEY);
		glTexCoord2f(Pic->mS0, Pic->mT0);	glVertex2f(fSX, fEY);
	glEnd();

	return 1;
}

/// @brief Assigns a picture's texels
/// @param picture Handle to the picture object
/// @param fS0 Initial texture s-coordinate
/// @param fT0 Initial texture t-coordinate
/// @param fS1 Terminal texture s-coordinate
/// @param fT1 Terminal texture t-coordinate
/// @return 0 on failure, non-0 for success
/// @note Tested
int SetPictureTexels (Picture_h picture, float fS0, float fT0, float fS1, float fT1)
{
	if (0 == picture) return 0;

	// Convert the handle to a usable form. Assign the texels.
	Graphics::Picture * Pic = static_cast<Graphics::Picture*>(picture);

	Pic->mS0 = fS0;
	Pic->mT0 = fT0;
	Pic->mS1 = fS1;
	Pic->mT1 = fT1;

	return 1;
}

/// @brief Acquires a picture's texels
/// @param picture Handle to the picture object
/// @param fS0 [out] On success, initial texture s-coordinate
/// @param fT0 [out] On success, initial texture t-coordinate
/// @param fS1 [out] On success, terminal texture s-coordinate
/// @param fT1 [out] On success, terminal texture t-coordinate
/// @return 0 on failure, non-0 for success
/// @note Tested
int GetPictureTexels (Picture_h picture, float & fS0, float & fT0, float & fS1, float & fT1)
{
	if (0 == picture) return 0;

	// Convert the handle to a usable form. Acquire the properties.
	Graphics::Picture * Pic = static_cast<Graphics::Picture*>(picture);

	fS0 = Pic->mS0;
	fT0 = Pic->mT0;
	fS1 = Pic->mS1;
	fT1 = Pic->mT1;

	return 1;
}

/// @brief Unloads a picture object from the renderer
/// @param picture Handle to the picture object
/// @return 0 on failure, non-0 for success
/// @note Tested
int UnloadPicture (Picture_h picture)
{
	if (0 == picture) return 0;

	// Convert the handle to a usable form. Invoke the picture's destructor and remove it
	// from the picture list.
	Graphics::Picture * Pic = static_cast<Graphics::Picture*>(picture);

	delete Pic;

	Graphics::Main::Get().mPictures.remove(Pic);

	return 1;
}

/// @brief Instantiates a font object
/// @param name Name of file used to load font data
/// @param size Size in which to load font
/// @param font [out] On success, handle to a font object
/// @return 0 on failure, non-0 for success
/// @note Tested
int LoadFont (char const * name, int size, Font_h & font)
{
	if (0 == name) return 0;
	if (0 == font) return 0;
	if (size <= 0) return 0;

	Graphics::Main & g = Graphics::Main::Get();

	// Load the face if it is not loaded. Acquire the size.
	if (g.mFaces.find(name) == g.mFaces.end())
	{
		g.mFaces[name] = new Graphics::Face(name);
	}

	font = g.mFaces[name]->GetSize(name, size);

	return 1;
}

/// @brief Unloads a font object from the renderer
/// @param font Handle to a font object
/// @return 0 on failure, non-0 for success
/// @note Tested
int UnloadFont (Font_h font)
{
	if (0 == font) return 0;

	Graphics::Main & g = Graphics::Main::Get();

	// Remove the size from the graphics core.
	FT_Size pSize = static_cast<FT_Size>(font);

	std::map<std::string, Graphics::Face*>::iterator iter = g.mFaces.begin();

	while (iter != g.mFaces.end() && iter->second->mFace != pSize->face) ++iter;

	if (iter != g.mFaces.end())
	{
		for (std::map<int, FT_Size>::iterator sIter = iter->second->mSizes.begin(); sIter != iter->second->mSizes.end(); ++sIter)
		{
			if (sIter->second != pSize) continue;
			
            iter->second->mSizes.erase(sIter);

			// If the last size is removed, unload the font itself.
			if (iter->second->mSizes.empty()) g.mFaces.erase(iter);

			return 1;
		}
	}

	return 0;
}

/// @brief Gets the dimensions of a text string constructed from a given font object
/// @param font Handle to a font object
/// @param text Text string whose size is being determined
/// @param width [out] On success, the string's width
/// @param height [out] On success, the string's height
/// @return 0 on failure, non-0 for success
/// @note Tested
int GetTextSize (Font_h font, char const * text, int & width, int & height)
{
	if (0 == font) return 0;
	if (0 == text) return 0;

	FT_Size pSize = static_cast<FT_Size>(font);

	FT_Activate_Size(pSize);

	FT_GlyphSlot pSlot = pSize->face->glyph;

	width = 0;

	for (Uint32 index = 0; text[index] != '\0'; ++index)
	{
		FT_Load_Char(pSize->face, text[index], FT_LOAD_DEFAULT);

		width += pSlot->advance.x;
	}

	width /= 64;
	height = pSize->metrics.height / 64 + 1;

	return 1;
}

/// @brief Generates a renderable text image from a font
/// @param font Handle to a font object, used to generate the text image
/// @param text The text to render in the text image
/// @param color The color of the rendered text image
/// @param textImage [out] On success, handle to a text image object
/// @return 0 on failure, non-0 for success
/// @note Tested
int LoadTextImage (Font_h font, char const * text, SDL_Color color, TextImage_h & textImage)
{
	if (0 == font) return 0;
	if (0 == text) return 0;
	if (0 == textImage) return 0;

	Graphics::Main & g = Graphics::Main::Get();

	try {
		g.mTextImages.push_back(new Graphics::TextImage(static_cast<FT_Size>(font), text, color));
	} catch (std::bad_alloc &) {
		return 0;
	}

	textImage = g.mTextImages.back();

	return 1;
}

/// @brief Renders a text image
/// @param textImage Handle to a text image object
/// @param fX Screen x coordinate, in [0, 1]
/// @param fY Screen y coordinate, in [0, 1]
/// @param fW Screen width, in [0, 1]
/// @param fH Screen height, in [0, 1]
/// @return 0 on failure, non-0 for success
/// @note Tested
int DrawTextImage (TextImage_h textImage, float fX, float fY, float fW, float fH)
{
	if (0 == textImage) return 0;

	Graphics::Main & g = Graphics::Main::Get();

	// Transform the provided coordinates and dimensions into a form OpenGL expects, and
	// scale them to the current resolution.
	float fSX = fX;
	float fEX = fX + fW;
	float fSY = 1.0f - fY - fH;
	float fEY = 1.0f - fY;

	fSX *= g.mResW;
	fEX *= g.mResW;
	fSY *= g.mResH;
	fEY *= g.mResH;

	// Convert the handle to a usable form. Set OpenGL up to render the text image's
	// texture, and draw a quad with the requested properties.
	Graphics::TextImage * Text = static_cast<Graphics::TextImage*>(textImage);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Text->mTexture);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, Text->mT);		glVertex2f(fSX, fSY);
		glTexCoord2f(Text->mS, Text->mT);	glVertex2f(fEX, fSY);
		glTexCoord2f(Text->mS, 0.0f);		glVertex2f(fEX, fEY);
		glTexCoord2f(0.0f, 0.0f);			glVertex2f(fSX, fEY);
	glEnd();

	return 1;
}

/// @brief Renders a text image, clipped to maximal dimensions
/// @param textImage Handle to a text image object
/// @param fX Screen x coordinate, in [0, 1]
/// @param fY Screen y coordinate, in [0, 1]
/// @param fW Screen width, in [0, 1]
/// @param fH Screen height, in [0, 1]
/// @param fMW Maximal screen width, in [0, 1]
/// @param fMH Maximal screen height, in [0, 1]
/// @return 0 on failure, non-0 for success
/// @note Tested
int DrawClippedTextImage (TextImage_h textImage, float fX, float fY, float fW, float fH, float fMW, float fMH)
{
	if (0 == textImage) return 0;

	Graphics::Main & g = Graphics::Main::Get();

	// Find the minimum of the desired and maximum dimensions.
	float fMinW = fMW < fW ? fMW : fW;
	float fMinH = fMH < fH ? fMH : fH;

	// Transform the provided coordinates and dimensions into a form OpenGL expects, and
	// scale them to the current resolution.
	float fSX = fX;
	float fEX = fX + fMinW;
	float fSY = 1.0f - fY - fMinH;
	float fEY = 1.0f - fY;

	fSX *= g.mResW;
	fEX *= g.mResW;
	fSY *= g.mResH;
	fEY *= g.mResH;

	// Convert the handle to a usable form. Set OpenGL up to render the text image's
	// texture, and draw a quad with the requested properties.
	Graphics::TextImage * Text = static_cast<Graphics::TextImage*>(textImage);

	GLfloat fSC = fMinW * Text->mS / fW;
	GLfloat fTC = fMinH * Text->mT / fH;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Text->mTexture);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, fTC);	glVertex2f(fSX, fSY);
		glTexCoord2f(fSC, fTC);		glVertex2f(fEX, fSY);
		glTexCoord2f(fSC, 0.0f);	glVertex2f(fEX, fEY);
		glTexCoord2f(0.0f, 0.0f);	glVertex2f(fSX, fEY);
	glEnd();

	return 1;
}

/// @brief Unloads a text image object from the renderer
/// @param textImage Handle to a text image object
/// @return 0 on failure, non-0 for success
/// @note Tested
int UnloadTextImage (TextImage_h textImage)
{
	if (0 == textImage) return 0;

	// Convert the handle to a usable form. Invoke the text image's destructor and remove
	// it from the text image list.
	Graphics::TextImage * Text = static_cast<Graphics::TextImage*>(textImage);

	delete Text;

	Graphics::Main::Get().mTextImages.remove(Text);

	return 1;
}

/// @brief Renders a square, unfilled box in the given color
/// @param fX Screen x coordinate, in [0, 1]
/// @param fY Screen y coordinate, in [0, 1]
/// @param fW Screen width, in [0, 1]
/// @param fH Screen height, in [0, 1]
/// @param fR Red value, in [0, 1]
/// @param fG Green value, in [0, 1]
/// @param fB Blue value, in [0, 1]
/// @return 0 on failure, non-0 for success
/// @note Tested
int DrawBox (float fX, float fY, float fW, float fH, float fR, float fG, float fB)
{
	Graphics::Main & g = Graphics::Main::Get();

	// Transform the provided coordinates and dimensions into a form OpenGL expects, and
	// scale them to the current resolution.
	float fSX = fX;
	float fEX = fX + fW;
	float fSY = 1.0f - fY - fH;
	float fEY = 1.0f - fY;

	fSX *= g.mResW;
	fEX *= g.mResW;
	fSY *= g.mResH;
	fEY *= g.mResH;

	// Turn off texturing and switch to the desired color. Draw an unfilled box.
	glDisable(GL_TEXTURE_2D);
	
	GLfloat color[4];	glGetFloatv(GL_CURRENT_COLOR, color);
	glColor3f(fR, fG, fB);

	glBegin(GL_LINE_LOOP);
		glVertex2f(fSX, fSY);
		glVertex2f(fEX, fSY);
		glVertex2f(fEX, fEY);
		glVertex2f(fSX, fEY);
	glEnd();

	glColor4fv(color);

	return 1;
}

/// @brief Renders a line in the given color
/// @param fSX Screen start x coordinate, in [0, 1]
/// @param fSY Screen start y coordinate, in [0, 1]
/// @param fEX Screen end x coordinate, in [0, 1]
/// @param fEY Screen end y coordinate, in [0, 1]
/// @param fR Red value, in [0, 1]
/// @param fG Green value, in [0, 1]
/// @param fB Blue value, in [0, 1]
/// @return 0 on failure, non-0 for success
/// @note Tested
int DrawLine (float fSX, float fSY, float fEX, float fEY, float fR, float fG, float fB)
{
	Graphics::Main & g = Graphics::Main::Get();

	// Transform the provided coordinates and dimensions into a form OpenGL expects, and
	// scale them to the current resolution.
	fSX *= g.mResW;
	fEX *= g.mResW;
	fSY = g.mResH * (1.0f - fSY);
	fEY = g.mResH * (1.0f - fEY);

	// Turn off texturing and switch to the desired color. Draw a line.
	glDisable(GL_TEXTURE_2D);
	
	GLfloat color[4];	glGetFloatv(GL_CURRENT_COLOR, color);
	glColor3f(fR, fG, fB);

	glBegin(GL_LINES);
		glVertex2f(fSX, fSY);
		glVertex2f(fEX, fEY);
	glEnd();

	glColor4fv(color);

	return 1;
}

/// @brief Renders a square, unfilled grid in the current color
/// @param fX Screen x coordinate, in [0, 1]
/// @param fY Screen y coordinate, in [0, 1]
/// @param fW Screen width, in [0, 1]
/// @param fH Screen height, in [0, 1]
/// @param fR Red value, in [0, 1]
/// @param fG Green value, in [0, 1]
/// @param fB Blue value, in [0, 1]
/// @param xCuts Number of horizontal partitions
/// @param yCuts Number of vertical partitions
/// @return 0 on failure, non-0 for success
/// @note Tested
int DrawGrid (float fX, float fY, float fW, float fH, float fR, float fG, float fB, Uint32 xCuts, Uint32 yCuts)
{
	Graphics::Main & g = Graphics::Main::Get();

	// Transform the provided coordinates and dimensions into a form OpenGL expects, and
	// scale them to the current resolution.
	float fSX = fX;
	float fEX = fX + fW;
	float fSY = 1.0f - fY - fH;
	float fEY = 1.0f - fY;

	fSX *= g.mResW;
	fEX *= g.mResW;
	fSY *= g.mResH;
	fEY *= g.mResH;

	// Turn off texturing and switch to the desired color. Draw an unfilled box.
	glDisable(GL_TEXTURE_2D);

	GLfloat color[4];	glGetFloatv(GL_CURRENT_COLOR, color);
	glColor3f(fR, fG, fB);

	glBegin(GL_LINE_LOOP);
		glVertex2f(fSX, fSY);
		glVertex2f(fEX, fSY);
		glVertex2f(fEX, fEY);
		glVertex2f(fSX, fEY);
	glEnd();

	// Draw the horizontal and vertical lines.
	glBegin(GL_LINES);
		float fDW = (fEX - fSX) / (xCuts + 1);

		for (fX = fSX + fDW; xCuts-- != 0; fX += fDW)
		{
			glVertex2f(fX, fSY);
			glVertex2f(fX, fEY);
		}

		float fDH = (fEY - fSY) / (yCuts + 1);

		for (fY = fSY + fDH; yCuts-- != 0; fY += fDH)
		{
			glVertex2f(fSX, fY);
			glVertex2f(fEX, fY);
		}
	glEnd();

	glColor4fv(color);

	return 1;
}

/// @brief Indicates whether the graphics system is initialized
/// @return If true, the system is intialized
/// @note Tested
SDL_bool GraphicsWasInit (void)
{
	return Graphics::Main::Get().mInit ? SDL_TRUE : SDL_FALSE;
}