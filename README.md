spriteprinter
=============

### Overview

Spriteprinter generates OpenSCAD files from (24-bit) png files. Spriteprinter
puts each color at a different height, so you don't need a 3d printer that
supports multiple colors at once. You can, instead, instruct your slicer to
place pauses for color changes as needed. Some slicers offer support for
automatically figuring out where to put in pauses for color changing, but I
recommend you check the output against the original image to avoid misprints.

### Caveats

* This program generates simplistic OpenSCAD files. They work, but they take a
  hot minute. I'm open to patches that make the output better.

* The result is always a single stl file. If anyone is interested in having this
  generate one stl file per layer, let me know.

### Questions

* Why not generate the stl directly? I tried that with an earlier version of
this tool. The resulting stl files were an enormous mess of triangles, and some
programs refused to import the files correctly. OpenSCAD generates optimized stl
files that programs are much happier to import.

* Why? I want to make keychains out of snes sprites.
