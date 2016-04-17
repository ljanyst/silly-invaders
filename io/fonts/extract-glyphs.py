#!/usr/bin/env python
#-------------------------------------------------------------------------------
# Copyright (c) 2016 by Lukasz Janyst <lukasz@jany.st>
#-------------------------------------------------------------------------------
# This file is part of silly-invaders.
#
# silly-invaders is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# silly-invaders is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with silly-invaders.  If not, see <http://www.gnu.org/licenses/>.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------------
import sys, string, os
try:
    import Image, ImageFont, ImageDraw
except ImportError, e:
    print
    print "You need to install python-pil for this to work"
    print
    sys.exit(1)

#-------------------------------------------------------------------------------
# Convert a letter to pixel data
#-------------------------------------------------------------------------------
def getImage(font, letter, prefix):
  #-----------------------------------------------------------------------------
  # Draw the letter
  #-----------------------------------------------------------------------------
  img = Image.new("RGB", (10000, 300))
  draw = ImageDraw.Draw(img)
  draw.fontmode = "1" # no anti-aliasing
  draw.text((0, 0), prefix + letter, font = font)

  #-----------------------------------------------------------------------------
  # Crop the image
  #-----------------------------------------------------------------------------
  img = img.crop(img.getbbox())
  width, height = img.size
  px = img.load()

  #-----------------------------------------------------------------------------
  # Remove the letter from the left
  #-----------------------------------------------------------------------------
  newi = 0
  for i in reversed(range(width)):
    hasPixels = False
    for j in range(height):
      if px[i, j] != (0, 0, 0):
        hasPixels = True
    if not hasPixels:
      newi = i
      break

  img = img.crop((newi, 0, width, height))
  return img

#-------------------------------------------------------------------------------
# Get pixel data
#-------------------------------------------------------------------------------
def getPixelData(img):
  px = img.load()
  width, height = img.size
  pixels = []
  for j in range(height):
    for i in range(width):
      if px[i, j] == (0, 0, 0):
        pixels.append(1)
      else:
        pixels.append(0)
  return (width, height, 1, pixels)

#-------------------------------------------------------------------------------
# Generate all the glyphs
#-------------------------------------------------------------------------------
def generateGlyphs(font):
  #-----------------------------------------------------------------------------
  # List all glyphs for proper alignment
  #-----------------------------------------------------------------------------
  prefix = ''
  for i in range(127):
    ch = chr(i)
    if ch not in string.printable or ch in string.whitespace:
      continue
    prefix += chr(i)
  prefix += (' ' * 30);

  #-----------------------------------------------------------------------------
  # Iterate over all printable characters
  #-----------------------------------------------------------------------------
  glyphs = []
  for i in range(33, 95):
    image = getImage(font, chr(i), prefix)
    glyphs.append(getPixelData(image))

  for i in range(97, 127):
    image = getImage(font, chr(i), prefix)
    glyphs.append(getPixelData(image))

  #-----------------------------------------------------------------------------
  # Make space to be as wide as letter 'l'
  #-----------------------------------------------------------------------------
  l = glyphs[73]
  space = (l[0], l[1], l[2], [1] * (l[0]*l[1]))
  glyphs = [space] + glyphs
  return glyphs

#-------------------------------------------------------------------------------
# Write header
#-------------------------------------------------------------------------------
def writeHeader(f):
  f.write("// This file has been generated autmatically, do not edit!\n\n")
  f.write("#include <io/IO_font.h>\n")
  f.write("#include <io/IO_display.h>\n\n")

#-------------------------------------------------------------------------------
# Write glyphs
#-------------------------------------------------------------------------------
def writeGlyphs(f, glyphs):
  for i in range(len(glyphs)):
    glyph = glyphs[i]
    f.write("static const char glyph_" + str(i) + "_data[] = ");
    f.write("{" + str(glyph[3])[1:-1] + "};\n");
    f.write("static const IO_bitmap glyph_" + str(i) + " = {");
    f.write(str(glyph[0]) + ", " + str(glyph[1]) + ", " + str(glyph[2]) + ", ")
    f.write("(void*)glyph_" + str(i) + "_data};\n")

  f.write("\n")

#-------------------------------------------------------------------------------
# Write font struct
#-------------------------------------------------------------------------------
def writeFontStruct(f, name, glyphs):
  size = 0
  for g in glyphs:
    if g[1] > size:
      size = g[1]
  f.write("const IO_font " + name + " = { \"" + name + "\", " + str(size))
  f.write(", {\n")
  for i in range(len(glyphs)):
    f.write("  (IO_bitmap*)&glyph_" + str(i) + ",\n");
  f.write("}};\n")

#-------------------------------------------------------------------------------
# Start the show
#-------------------------------------------------------------------------------
def main():
  #-----------------------------------------------------------------------------
  # Print usage
  #-----------------------------------------------------------------------------
  if len(sys.argv) != 5:
    print "Usage:"
    print "   ", sys.argv[0], "font_name ttf_file size output_file"
    return 1

  #-----------------------------------------------------------------------------
  # Check the input
  #-----------------------------------------------------------------------------
  name     = sys.argv[1]
  fontfile = sys.argv[2]
  size     = sys.argv[3]
  output   = sys.argv[4]

  if not name.isalnum():
    print "Font name may only contain letters"
    return 1

  if name[0].isdigit():
    print "Font name must start with a letter"
    return 1

  if not size.isdigit():
    print "Size must be a number"
    return 1

  if not os.access(fontfile, os.R_OK):
    print "Cannot open the font file for reading"
    return 1

  #-----------------------------------------------------------------------------
  # Open the font file and generate the glyph data
  #-----------------------------------------------------------------------------
  font = ImageFont.truetype(fontfile, int(size))
  glyphs = generateGlyphs(font);

  #-----------------------------------------------------------------------------
  # Open the result file and write the data
  #-----------------------------------------------------------------------------
  try:
    fo = open(sys.argv[4], "w")
    writeHeader(fo)
    writeGlyphs(fo, glyphs)
    writeFontStruct(fo, name, glyphs)
    fo.close()
  except IOError, e:
    print "Error writing to " + output + ":", str(e)
    return 1
  return 0

if __name__ == '__main__':
  sys.exit(main())
