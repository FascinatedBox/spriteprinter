#include <stdint.h>
#include <stdio.h>

#include "spdata.h"

static SPData *init_data(int height, int width) {
  SPData *d = new SPData();

  d->spot_map = std::map<int, int>();
  d->spot_height_list = std::vector<int>();
  d->height = height;
  d->width = width;
  d->spot_for_pixel.reserve(d->height);
  auto v = std::vector<std::vector<int>>(height, std::vector<int>());
  d->spot_for_pixel = v;
  d->cube_x = 0;
  d->cube_y = 0;
  d->cube_z = 0;

  return d;
}

static int spot_for_color(SPData *d, unsigned int color) {
  auto search = d->spot_map.find(color);
  int result;

  if (search != d->spot_map.end())
    result = search->second;
  else {
    result = d->spot_map.size();
    d->spot_map[color] = result;
  }

  return result;
}

static void init_spot_height_list(SPData *d) {
  for (int i = 0; i < d->spot_map.size(); i++) {
    d->spot_height_list.push_back(i);
  }
}

SPData *spdata_new_from_png_path(const char *path) {
  FILE *fp = fopen(path, "rb");
  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr = png_create_info_struct(png_ptr);
  png_init_io(png_ptr, fp);
  png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
  png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

  int width = png_get_image_width(png_ptr, info_ptr);
  int height = png_get_image_height(png_ptr, info_ptr);

  SPData *d = init_data(height, width);

  for (int y = 0; y < height; y++) {
    png_byte *p_byte = row_pointers[y];

    for (int x = 0; x < width; x++) {
      int red = *p_byte;
      *p_byte++;
      int green = *p_byte;
      *p_byte++;
      int blue = *p_byte;
      *p_byte++;
      int alpha = *p_byte;
      *p_byte++;

      (void)alpha;

      int pixel = red << 24 | green << 16 | blue << 8 | alpha;
      int spot = spot_for_color(d, pixel);

      d->spot_for_pixel[y].push_back(spot);
    }
  }

  init_spot_height_list(d);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  fclose(fp);
  return d;
}

void spdata_set_cube_xyz(SPData *d, int x, int y, int z) {
  d->cube_x = x;
  d->cube_y = y;
  d->cube_z = z;
}

void spdata_write_os_to_path(SPData *d, const char *path) {
  FILE *f = fopen(path, "w");
  fputs("table = [\n", f);

  // Simple algorithm for rebuilding the image line by line.
  // 1 image line = 1 table line, so encoding doesn't need y.
  // Don't dump cubes: OpenSCAD has to think more.
  // Instead, dump lines of equivalent Z depth.
  // Each entry is a triple: starting x, cube height, width.
  // OpenSCAD has referential transparency (no 'start_x += width').
  for (int y = 0; y < d->height; y++) {
    fputs("  [", f);
    int last_height = d->spot_for_pixel[y][0];
    int x_start = 0;
    int x = 0;

    for (x = 1; x < d->width; x++) {
      int spot = d->spot_for_pixel[y][x];

      if (last_height == -1)
        last_height = spot;
      else if (last_height != spot) {
        fprintf(f, "%d,%d,%d, ", x_start, last_height, x - x_start);

        last_height = spot;
        x_start = x;
      }
    }

    fprintf(f, "%d,%d,%d],\n", x_start, last_height, x - x_start);
  }

  fputs("];\n\n", f);
  fprintf(f, R"(cu_x = %d;
cu_y = %d;
cu_z = %d;

)",
          d->cube_x, d->cube_y, d->cube_z);

  fputs(R"(for (y = [0:len(table)-1]) {
    line = table[y];

    for (x = [0:3:len(line)-1]) {
        x_start = line[x];
        z_height = line[x+1];
        x_wide = line[x+2];

        translate([x_start * cu_x, y * cu_y, 0])
            cube([x_wide, cu_y, z_height * cu_z]);
    }
}
)",
        f);

  fclose(f);
}

void spdata_delete(SPData *d) {
  if (d == nullptr)
    return;

  delete d;
}
