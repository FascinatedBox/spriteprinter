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

void spdata_write_spots_for_pixel(SPData *d) {
  for (int y = 0; y < d->height; y++) {
    for (int x = 0; x < d->width - 1; x++) {
      printf("%d ", d->spot_for_pixel[y][x]);
    }

    printf("%d\n", d->spot_for_pixel[y][d->width - 1]);
  }

  putc('\n', stdout);
}

void spdata_generate_to_path(SPData *d, const char *path) {
  (void)path;
  int cu_x = d->cube_x;
  int cu_y = d->cube_y;
  int cu_z = d->cube_z;
  int height = d->height;
  int width = d->width;

  FILE *f = fopen(path, "w");

  fputs("#VRML V2.0 utf\n", f);
  fputs("Shape {\n", f);

  for (int y = 0; y < height; y++) {
    std::vector<int> row = d->spot_for_pixel[y];
    for (int x = 0; x < width; x++) {
      // Global position for the pixel.
      int global_y = y * cu_y;
      int global_x = x * cu_x;

      // What color is at this pixel?
      int spot = d->spot_for_pixel[y][x];

      // How high is that color?
      int z_repeat = d->spot_height_list[spot];

      // Todo: Ideally, write the stl out directly.
      // For the time being, this writes out vrml (I know), and another
      // tool converts vrml to stl.
      // The vrml converter appears to be confused by cubes of different
      // Z depth, so I am instead dumping Z count cubes to work around
      // that.

      for (int z = 0; z < z_repeat; z++) {
        int layer_z = cu_z * z;

        fputs("Transform {\n", f);
        fprintf(f, "  translation %d %d %d\n", global_y, global_x, layer_z);
        fputs("  children [\n", f);
        fputs("    Shape {\n", f);
        fputs("        geometry Box {\n", f);
        fprintf(f, "            size %d %d %d\n", cu_x, cu_y, cu_z);
        fputs("        }\n", f);
        fputs("    }\n", f);
        fputs("  ]\n", f);
        fputs("}\n", f);
      }
    }
  }

  fclose(f);
}
