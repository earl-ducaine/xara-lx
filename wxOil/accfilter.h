



#ifndef ACCFILTER_H
#define ACCFILTER_H

class AccusoftFilters
{

 public:
  static INT32 pfnIMG_bayer_color(INT32 handle);
  static INT32 pfnIMG_bayer_mono(INT32 handle);
  static INT32 pfnIMG_bitmap_info(INT32 handle, INT32* width_ptr, INT32* height_ptr, INT32* bp_ptr);
  static INT32 pfnIMG_color_gray(INT32 handle);
  static INT32 pfnIMG_delete_bitmap(INT32 handle);
  static INT32 pfnIMG_diffusion_color(INT32 handle);
  static INT32 pfnIMG_diffusion_mono(INT32 handle);
  static INT32 pfnIMG_flip_bitmapx(INT32 handle);
  static INT32 pfnIMG_flip_bitmapy(INT32 handle);
  static INT32 pfnIMG_halftone_mono(INT32 handle);
  static INT32 pfnIMG_octree_color(INT32 handle, INT32 number_transitions, void* unused, INT32 number_colors);
  static INT32 pfnIMG_popularity_color(INT32 handle);
  static INT32 pfnIMG_process_bitmap(INT32 handle, INT32 type, char* matrix_pointer);
  static INT32 pfnIMG_promote_24(INT32 handle);
  static INT32 pfnIMG_promote_8(INT32 handle);
  static INT32 pfnIMG_resize_bitmap(INT32 handle, INT32 new_width, INT32 new_height);
  static INT32 pfnIMG_resize_bitmap_interp(INT32 handle, INT32 new_width, INT32 height);
  static INT32 pfnIMG_rotate_bitmap(INT32 handle, INT32 degree_hundreth);
  static INT32 pfnIMG_sharpen_bitmap(INT32 handle, INT32 sharpen_degree);

};

#endif
