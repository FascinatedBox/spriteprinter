#ifndef SPDATA_H
#define SPDATA_H

#include <map>
#include <png.h>
#include <vector>

struct SPData {
  std::vector<std::vector<int>> spot_for_pixel;
  std::map<int, int> spot_map;
  std::vector<int> spot_height_list;
  int height;
  int width;
  int cube_x;
  int cube_y;
  int cube_z;
};

SPData *spdata_new_from_png_path(const char *path);
void spdata_set_cube_xyz(SPData *data, int x, int y, int z);
void spdata_write_spots_for_pixel(SPData *data);
void spdata_generate_to_path(SPData *data, const char *path);

#endif
