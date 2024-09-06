#include <algorithm>
#include <stdint.h>
#include <stdio.h>

#include "spdata.h"

static SPData *init_data(int height, int width) {
  SPData *d = new SPData();

  d->spot_map = std::map<int, int>();
  d->spot_counter_map = std::map<int, int>();
  d->height_table = std::vector<int>();
  d->height = height;
  d->width = width;
  d->spot_for_pixel.reserve(d->height);
  auto v = std::vector<std::vector<int>>(height, std::vector<int>());
  d->spot_for_pixel = v;
  d->cube_xy = 0;
  d->cube_z = 0;

  return d;
}

static int spot_for_color(SPData *d, unsigned int color) {
  auto search = d->spot_map.find(color);
  int result;

  if (search != d->spot_map.end()) {
    d->spot_counter_map[color]++;
    result = search->second;
  } else {
    result = d->spot_map.size();
    d->spot_map[color] = result;
    d->spot_counter_map[color] = 1;
  }

  return result;
}

static int get_first_color_spot(SPData *d)
{
  int color_key = d->spot_for_pixel[0][0];

  return d->spot_map[color_key];
}

static void init_height_table(SPData *d) {
  std::vector<std::pair<int, int>> table = std::vector<std::pair<int, int>>();
  int first_color_spot = get_first_color_spot(d);
  int total = 0;

  // Assume the top left pixel has the background color.
  d->spot_counter_map[first_color_spot] = 0;

  // Create a list of [spot id, frequency].
  for (auto it = d->spot_map.begin(); it != d->spot_map.end(); it++) {
    auto color_key = it->first;
    auto color_idx = it->second;
    auto count = d->spot_counter_map[color_key];

    if (count == 0)
      continue;

    table.push_back(std::make_pair(color_idx, count));
  }

  std::sort(table.begin(), table.end(),
            [](std::pair<int, int> a, std::pair<int, int> b) {
              return a.second > b.second;
            });

  for (int i = 0; i < d->spot_map.size(); i++) {
    d->height_table.push_back(0);
  }

  // Build translation from original spot -> corrected id.
  for (int i = 0; i < table.size(); i++) {
    int color_idx = table[i].first;

    d->height_table[color_idx] = i;
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

  init_height_table(d);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  fclose(fp);
  return d;
}

void spdata_set_cube_xyz(SPData *d, int xy, int z) {
  d->cube_xy = xy;
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
    int last_height = d->height_table[d->spot_for_pixel[y][0]];
    int x_start = 0;
    int x = 0;

    for (x = 1; x < d->width; x++) {
      int spot = d->height_table[d->spot_for_pixel[y][x]];

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
  fprintf(f, R"(cu_xy = %d;
cu_z = %d;

)",
          d->cube_xy, d->cube_z);

  // Both loops move in reverse to prevent image mirroring in X + Y.
  fputs(R"(for (y = [0:len(table)-1]) {
    render_y = len(table) - y;
    line = table[y];

    for (x = [len(line)-1:-3:0]) {
        x_start = line[x-2];
        z_height = line[x-1];
        x_wide = line[x];

        translate([x_start * cu_xy, render_y * cu_xy, 0])
            cube([x_wide, cu_xy, z_height * cu_z]);
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
