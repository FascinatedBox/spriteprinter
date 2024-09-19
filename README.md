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

* The color of the pixel at the very top left of the screen is treated as the
  background color, and not included in the output.

* Layers are organized by frequency: The most frequent color is at the bottom
  layer, and the least frequent is at the top.

### Usage

Use the file menu to select a png to load. Once it's loaded, save the resulting
scad. You will need to load the result in OpenSCAD.

### Questions

* Why not generate the stl directly? I tried that with an earlier version of
this tool. The resulting stl files were an enormous mess of triangles, and some
programs refused to import the files correctly. OpenSCAD generates optimized stl
files that programs are much happier to import.

* Why? I want to make keychains out of snes sprites.
