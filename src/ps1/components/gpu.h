#pragma once

#include <stdint.h>

// Depth of the pixel values in a texture page
enum TextureDepth{
  // 4 bits per pixel
  T4Bit = 0,
  // 8 bits per pixel
  T8Bit = 1,
  // 15 bits per pixel
  T15Bit = 2
};

// Interlaced output splits each frame in two fields
enum Field{
  // Top field (odd lines)
  Top = 1,
  // Bottom field (even lines)
  Bottom = 0
};

// Video output horizontal resolution
class HorizontalRes{
private:
  uint8_t hr;
public:
  uint8_t from_fields(uint8_t hr1, uint8_t hr2);
  uint32_t into_status();
};

// Video output vertical resolution
enum VerticalRes{
  // 240 lines
  Y240Lines = 0,
  // 480 lines
  Y480Lines = 1
};

enum VMode{
  // NTSC: 480i60H
  Ntsc = 0,
  // PAL: 576i50Hz
  Pal = 1
};

// Display area color depth
enum DisplayDepth{
  // 15 bits per pixel
  D15Bits = 0,
  // 24 bits per pixel
  D24Bits = 1
};

// Requested DMA direction
enum DmaDirection{
  Off = 0,
  Fifo = 1,
  GpuToGp0 = 2,
  VRamToCpu = 3
};

class Gpu{
private:
  // Texture page base X coordinate (4 bits, 64 byte increment)
  uint8_t page_base_x;
  // Texture page base Y coordinate (1 bits, 256 line increment)
  uint8_t page_base_y;
  // Semi-transparency (describes how to blend the source and destination colors)
  uint8_t semi_transparency;
  // Texture page color depth
  enum TextureDepth texture_depth;
  // Enable dithering from 24 to 15 bits RGB
  bool dithering;
  // Allow drawing the the display area
  bool draw_to_display;
  // Force "mask" bit of the pixel to 1 when writing to VRAM (else don't modify it)
  bool force_set_mask_bit;
  // Do not draw to pixels which have the "mask" bit set
  bool preserve_masked_pixels;
  // Currently displayed field. FOr progressive output this is always Top
  enum Field field;
  // When true all textures are disabled
  bool texture_disable;
  // Video output horizontal resolution
  uint8_t hres;
  HorizontalRes *horizontal_res;
  // Video output vertical resolution
  enum VerticalRes vres;
  // Video mode
  enum VMode vmode;
  // Display depth. The GPU itself always draws 15bit RGB, 24bit output must be external assets (pre-rendered textures, MDEC, etc)
  enum DisplayDepth display_depth;
  // Output interlaced video signal instead of prograssive
  bool interlaced;
  // Disable the display
  bool display_disabled;
  // True when the interrupt is active
  bool interrupt;
  // DMA request direction
  enum DmaDirection dma_direction;

  Gpu();
  uint32_t status();
};

