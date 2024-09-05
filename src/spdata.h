#ifndef SPDATA_H
#define SPDATA_H

#include <map>
#include <png.h>
#include <vector>

struct SPData {
  // [y][x] -> spot
  std::vector<std::vector<int>> spot_for_pixel;

  // rgba color -> spot
  std::map<int, int> spot_map;

  // spot -> frequency
  std::map<int, int> spot_counter_map;

  // spot -> Z height
  std::vector<int> height_table;

  // image dimensions
  int height;
  int width;

  // base cube dimensions
  int cube_xy;
  int cube_z;
};

SPData *spdata_new_from_png_path(const char *path);
void spdata_set_cube_xyz(SPData *data, int xy, int z);
void spdata_write_os_to_path(SPData *data, const char *path);
void spdata_delete(SPData *data);

#endif
