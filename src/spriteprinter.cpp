#include <png.h>
#include <stdlib.h>

#include "spdata.h"

int main(int argc, char **argv) {
  SPData *data = spdata_new_from_png_path(argv[1]);

  spdata_set_cube_xyz(data, 1, 1, 1);
  spdata_generate_to_path(data, argv[2]);
  return 0;
}
