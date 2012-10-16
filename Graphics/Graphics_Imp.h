#ifndef GRAPHICS_IMP_H
#define GRAPHICS_IMP_H

#include <SDL/SDL_error.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_types.h>
#include <SDL/SDL_video.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_SIZES_H
#include <list>
#include <map>
#include <string>

namespace Graphics
{
	/// @brief 26.6 fixed-point grid-fitting routines
	#define Round(x)((x) & -64)
	#define Floor(x) Round(x + 32)
	#define Ceiling(x) Round(x + 63)

	/// @brief Internal image representation
	struct Image {
	// Members
		GLuint mTexture;///< Texture used by image
		Uint32 mCount;	///< Reference count for image sprites
	// Methods
		Image (std::string const & name);
		~Image (void);
	};

	/// @brief Internal picture representation
	struct Picture {
	// Members
		Image * mImage;	///< Image used by picture
		GLfloat mS0;///< Initial s-coordinate
		GLfloat mS1;///< Terminal s-coordinate
		GLfloat mT0;///< Initial t-coordinate
		GLfloat mT1;///< Terminal t-coordinate
	// Methods
		Picture (Image * image);
		~Picture (void);
	};

	// @brief Internal face representation
	struct Face {
	// Members
		std::map<int, FT_Size> mSizes;	///< Sizes bound to face
		FT_Face mFace;	///< Face data used by FreeType
	// Methods
		Face (std::string const & name);
		~Face (void);

		FT_Size GetSize (std::string const & name, int size);
	};

	/// @brief Internal text image representation
	struct TextImage {
	// Members
		GLuint mTexture;///< Texture used by text image
		FT_Glyph mGlyph;///< Glyph information pertinent to character
		GLfloat mS;	///< Texture s-extent
		GLfloat mT;	///< Texture t-extent
	// Methods
		TextImage (FT_Size pSize, std::string const & text, SDL_Color color);
		~TextImage (void);
	};

	/// @brief Structure used to represent the graphics renderer
	struct Main {
	// Members
		std::map<std::string, Image*> mImages;	///< Images stored in the core
		std::map<std::string, Face*> mFaces;///< Faces stored in the core
		std::list<Picture*> mPictures;	///< Pictures stored in the core
		std::list<TextImage*> mTextImages;	///< Text images stored in the core
		FT_Library mFreeType;	///< Library used to maintain text
		GLsizei mResW;	///< Resolution width
		GLsizei mResH;	///< Resolution height
		bool mInit;	///< If true, the system is initialized
	// Methods
		Main (void);
		~Main (void);

		static Main & Get (void);
	};
};

#endif // GRAPHICS_IMP_H