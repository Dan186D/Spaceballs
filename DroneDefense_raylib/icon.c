/* GIMP RGBA C-Source image dump (icon.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[32 * 32 * 4 + 1];
} sfml_icon = {
  32, 32, 4,
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000"
  "\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377"
  "\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000"
  "\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000"
  "\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\377"
  "\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000"
  "\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000"
  "\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000"
  "\377\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000"
  "\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000"
  "\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\000"
  "\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377"
  "\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000"
  "\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377"
  "\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000"
  "\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377"
  "\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\000\000"
  "\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000"
  "\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377"
  "\000\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377"
  "\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000"
  "\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000"
  "\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377"
  "\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377"
  "\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377"
  "\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000"
  "\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000"
  "\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000"
  "\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000"
  "\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000"
  "\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000"
  "\377\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377"
  "\000\000\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000"
  "\000\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000"
  "\377\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377"
  "\000\000\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000"
  "\000\377\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000"
  "\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377"
  "\377\377\000\377\377\377\000\377\377\377\000\000\000\000\377\000\000\000\377\000\000\000\377\377"
  "\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000",
};

