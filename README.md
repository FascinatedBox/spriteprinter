spriteprinter
=============

### Overview

Spriteprinter reads a (24-bit) png and generates vrml. You can use the vrml with
a vrml to stl translator (https://github.com/djkonro/vrml-stl). The result is a
3d printable stl, wherein each color has a different height. You can then feed
the stl to a slicer and tell it that each layer has a different color.

Currently just a proof of concept.
