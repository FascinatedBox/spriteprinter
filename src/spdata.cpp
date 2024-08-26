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

#define TRIANGLE(a, b, c, d, e, f, g, h, i)                                    \
  *co = a;                                                                     \
  co++;                                                                        \
  *co = b;                                                                     \
  co++;                                                                        \
  *co = c;                                                                     \
  co++;                                                                        \
  *co = d;                                                                     \
  co++;                                                                        \
  *co = e;                                                                     \
  co++;                                                                        \
  *co = f;                                                                     \
  co++;                                                                        \
  *co = g;                                                                     \
  co++;                                                                        \
  *co = h;                                                                     \
  co++;                                                                        \
  *co = i;                                                                     \
  co++;                                                                        \
  co = co_start;                                                               \
  write_coords(out_f, co)

static void write_coords(FILE *f, float *coords) {
  float *co = coords;
  fprintf(f,
          R"( facet normal 0 0 0
  outer loop
   vertex %g %g %g
   vertex %g %g %g
   vertex %g %g %g
  endloop
 endfacet
)",
          co[0], co[1], co[2], co[3], co[4], co[5], co[6], co[7], co[8]);
}

#define F_TOP   0x01
#define F_NORTH 0x02
#define F_EAST  0x04
#define F_SOUTH 0x08
#define F_WEST  0x10

static void write_cube(SPData *d, FILE *out_f, int glo_x, int glo_y,
                       int glo_z, int flags) {
  float co_start[9] = {};
  float *co = co_start;
  float tr_x = d->cube_x + glo_x;
  float tr_y = d->cube_y + glo_y;
  float tr_z = d->cube_z + glo_z;
  float neg_x = (-d->cube_x) + glo_x;
  float neg_y = (-d->cube_y) + glo_y;
  float neg_z = (-d->cube_z) + glo_z;

  // A cube requires a total of 108 points:
  // 1 cube =
  //  6 sides
  //   12 triangles
  //   * 3 points
  //   * 3 (each point has x, y, z)
  // = 108
  // Each pair of triangles corresponds to a different side of the cube.

  // Top
  TRIANGLE(tr_x, tr_y, tr_z, neg_x, tr_y, tr_z, neg_x, neg_y, tr_z);
  TRIANGLE(tr_x, tr_y, tr_z, neg_x, neg_y, tr_z, tr_x, neg_y, tr_z);

  // Bottom
  TRIANGLE(neg_x, neg_y, neg_z, neg_x, tr_y, neg_z, tr_x, tr_y, neg_z);
  TRIANGLE(tr_x, neg_y, neg_z, neg_x, neg_y, neg_z, tr_x, tr_y, neg_z);

  // North
  TRIANGLE(tr_x, tr_y, tr_z, tr_x, tr_y, neg_z, neg_x, tr_y, neg_z);
  TRIANGLE(tr_x, tr_y, tr_z, neg_x, tr_y, neg_z, neg_x, tr_y, tr_z);

  // South
  TRIANGLE(tr_x, neg_y, tr_z, neg_x, neg_y, tr_z, neg_x, neg_y, neg_z);
  TRIANGLE(tr_x, neg_y, tr_z, neg_x, neg_y, neg_z, tr_x, neg_y, neg_z);

  // East
  TRIANGLE(tr_x, tr_y, tr_z, tr_x, neg_y, tr_z, tr_x, neg_y, neg_z);
  TRIANGLE(tr_x, tr_y, tr_z, tr_x, neg_y, neg_z, tr_x, tr_y, neg_z);

  // West
  TRIANGLE(neg_x, tr_y, tr_z, neg_x, tr_y, neg_z, neg_x, neg_y, neg_z);
  TRIANGLE(neg_x, tr_y, tr_z, neg_x, neg_y, neg_z, neg_x, neg_y, tr_z);
}

void spdata_generate_to_path(SPData *d, const char *path) {
  float cu_x = d->cube_x;
  float cu_y = d->cube_y;
  float cu_z = d->cube_z;
  int height = d->height;
  int width = d->width;
  int layer_count = d->spot_height_list.size();

  FILE *f = fopen(path, "w");
  fputs("solid SpritePrinterImage\n", f);

  for (int layer = 0; layer < layer_count; layer++) {
    float glo_z = layer * cu_z * 2;

    for (int y = 0; y < height; y++) {
      float glo_y = y * cu_y * 2;
      std::vector<int> row = d->spot_for_pixel[y];

      for (int x = 0; x < width; x++) {
        int pixel_z = row[x];
        int flags = F_NORTH | F_EAST | F_SOUTH | F_WEST | F_TOP;

        if (pixel_z < layer)
          continue;

        float glo_x = x * cu_x * 2;
        write_cube(d, f, glo_x, glo_y, glo_z, flags);
      }
    }
  }

  fputs("endsolid\n", f);
  fclose(f);
}
