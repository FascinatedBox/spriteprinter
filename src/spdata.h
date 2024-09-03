#ifndef SPDATA_H
#define SPDATA_H

#include <map>
#include <png.h>
#include <vector>

struct SPData {
  std::vector<std::vector<int>> spot_for_pixel;
  std::map<int, int> spot_map;
  std::map<int, int> spot_counter_map;
  std::vector<int> height_table;
  int height;
  int width;
  int cube_x;
  int cube_y;
  int cube_z;
};

SPData *spdata_new_from_png_path(const char *path);
void spdata_set_cube_xyz(SPData *data, int x, int y, int z);
void spdata_write_os_to_path(SPData *data, const char *path);
void spdata_delete(SPData *data);

#endif
