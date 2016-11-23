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
    import Image
except ImportError, e:
    print
    print "You need to install python-pil for this to work"
    print
    sys.exit(1)

#-------------------------------------------------------------------------------
# Get pixel data
#-------------------------------------------------------------------------------
def getPixelData(bmp):
  img = Image.open(bmp)
  px = img.load()
  width, height = img.size
  pixels = []
  for j in range(height):
    for i in range(width):
      if px[i, j] == (0, 0, 0):
        pixels.append(0)
      else:
        pixels.append(1)
  return (width, height, 1, pixels)

#-------------------------------------------------------------------------------
# Write bitmap
#-------------------------------------------------------------------------------
def writeBitmap(f, name, bitmap):
  f.write("// This file has been generated autmatically, do not edit!\n\n")
  f.write("#include <io/IO.h>\n\n")
  f.write("static const char " + name + "_data[] = ");
  f.write("{" + str(bitmap[3])[1:-1] + "};\n");
  f.write("const IO_bitmap " + name + " = {");
  f.write(str(bitmap[0]) + ", " + str(bitmap[1]) + ", " + str(bitmap[2]) + ", ")
  f.write("(void*)" + name + "_data };\n")

#-------------------------------------------------------------------------------
# Start the show
#-------------------------------------------------------------------------------
def main():
  #-----------------------------------------------------------------------------
  # Print usage
  #-----------------------------------------------------------------------------
  if len(sys.argv) != 4:
    print "Usage:"
    print "   ", sys.argv[0], "bitmap bmp_file output_file"
    return 1

  #-----------------------------------------------------------------------------
  # Check the input
  #-----------------------------------------------------------------------------
  name    = sys.argv[1]
  bmpfile = sys.argv[2]
  output  = sys.argv[3]

  if not name.isalnum():
    print "Bitmap name may only contain letters and numbers"
    return 1

  if name[0].isdigit():
    print "Bitmap name must start with a letter"
    return 1

  if not os.access(bmpfile, os.R_OK):
    print "Cannot open the BMP file for reading"
    return 1

  #-----------------------------------------------------------------------------
  # Open the font file and generate the glyph data
  #-----------------------------------------------------------------------------
  bmp = getPixelData(bmpfile)

  #-----------------------------------------------------------------------------
  # Open the result file and write the data
  #-----------------------------------------------------------------------------
  try:
    outdir='/'.join(sys.argv[3].split('/')[:-1])
    if not os.path.isdir(outdir):
      os.makedirs(outdir)
    fo = open(sys.argv[3], "w")
    writeBitmap(fo, name, bmp)
    fo.close()
  except IOError, e:
    print "Error writing to " + output + ":", str(e)
    return 1
  return 0

if __name__ == '__main__':
  sys.exit(main())
