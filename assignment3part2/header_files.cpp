// preamble for header functions
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define VALS_PER_VERT 3
#define VALS_PER_ELEMENT 3
#define VALS_PER_NORM 3
#define VALS_PER_TEX 2

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define NUM_TEXTURES 3
#define TEX_SIZE 64