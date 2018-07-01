/*
 * gfxfont.h
 *
 *  Created on: Mar 18, 2018
 *      Author: David
 */

#ifndef GFXFONT_H_
#define GFXFONT_H_

typedef struct _GFXglyph{ // Data stored PER GLYPH
  uint16_t bitmapOffset;     // Pointer into GFXfont->bitmap
  uint8_t  width, height;    // Bitmap dimensions in pixels
} GFXglyph;

typedef struct _GFXfont{ // Data stored for FONT AS A WHOLE:
  uint8_t  *bitmap;      // Glyph bitmaps, concatenated
  GFXglyph *glyph;       // Glyph array
  uint8_t   first, last; // ASCII extents
  uint8_t   spaceAdvance;    // How far to move x-position for space
} GFXfont;


#endif /* GFXFONT_H_ */
