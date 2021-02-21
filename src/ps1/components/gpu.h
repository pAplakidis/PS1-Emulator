#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "renderer.h"
#include "buffer.h"

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
  CpuToGp0 = 2,
  VRamToCpu = 3
};

// BUffer holding multi-word fixed-length GP0 command parameters
class CommandBuffer{
public:
  // Command buffer: the longuest possible command is GP0(0x3e) which takes 12 parameters
  uint32_t buffer[12];
  // Number of words queued in bufffer
  uint8_t len;

  CommandBuffer();
  void clear();
  void push_word(uint32_t word);
  uint32_t* index(size_t index);
};

enum Gp0Mode{
  // Default mode: handling commands
  Command,
  // Loading an image into VRAM
  ImageLoad
};

// Position in VRAM
class Position{
public:
  GLshort pos[2];
  void from_gp0(uint32_t val);
};

// RGB color
class Color{
public:
  GLubyte rgb[3];
  void from_gp0(uint32_t val);
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
  // Current mode of the GP0 register
  enum Gp0Mode gp0_mode;

  // GP0 stuff
  // Mirror textured rectangles alogn the x axis
  bool rectangle_texture_x_flip;
  // Mirror textured rectangles alogn the y axis
  bool rectangle_texture_y_flip;
  
  // Buffer containing the current GP0 command
  CommandBuffer *gp0_command;
  // Remaining words for the current GP0 command
  uint32_t gp0_words_remaining;
  // Pointer to the method implementing the current GP0 command
  void (Gpu::*gp0_command_method)(uint32_t); // TODO: this may not be right (check what functions are called later on)

  // GP1 stuff
  // Texture window x mask (8 pixel steps)
  uint8_t texture_window_x_mask;
  // Texture window y mask (8 pixel steps)
  uint8_t texture_window_y_mask;
  // Texture window x offset (8 pixel steps)
  uint8_t texture_window_x_offset;
  // Texture window y offset (8 pixel steps)
  uint8_t texture_window_y_offset;
  // Left-most column of drawing area
  uint16_t drawing_area_left;
  // Top-most column of drawing area
  uint16_t drawing_area_top;
  // Right-most column of drawing area
  uint16_t drawing_area_right;
  // Bottom-most column of drawing area
  uint16_t drawing_area_bottom;
  // Horizontal drawing offset applied to all vertices
  int16_t drawing_x_offset;
  // Vertical drawing offset applied to all vertices
  int16_t drawing_y_offset;
  // First column of the display area in VRAM
  uint16_t display_vram_x_start;
  // First line of the display area in VRAM
  uint16_t display_vram_y_start;
  // Display output horizontal start relative to HSYNC
  uint16_t display_horiz_start;
  // Display output horizontal end relative to HSYNC
  uint16_t display_horiz_end;
  // Display output first line relative to VSYNC
  uint16_t display_line_start;
  // Display output last line relative to VSYNC
  uint16_t display_line_end;

  // OpenGL renderer
  Renderer *renderer;

public:
  Gpu();
  uint32_t status();
  uint32_t read();
  void gp0(uint32_t val);
  void gp0_nop(uint32_t val);
  void gp0_clear_cache();
  void gp0_quad_mono_opaque(uint32_t val);
  void gp0_quad_texture_blend_opaque(uint32_t val);
  void gp0_triangle_shaded_opaque(uint32_t val);
  void gp0_quad_shaded_opaque(uint32_t val);
  void gp0_image_load(uint32_t val);
  void cp0_image_store(uint32_t val);
  void gp0_draw_mode(uint32_t val);
  void gp0_drawing_area_top_left(uint32_t val);
  void gp0_drawing_area_bottom_right(uint32_t val);
  void gp0_drawing_offset(uint32_t val);
  void gp0_texture_window(uint32_t val);
  void gp0_mask_bit_setting(uint32_t val);
  void gp1(uint32_t val);
  void gp1_reset(uint32_t _);
  void gp1_reset_command_buffer();
  void gp1_acknowledge_irq();
  void gp1_display_enable(uint32_t val);
  void gp1_dma_direction(uint32_t val);
  void gp1_display_vram_start(uint32_t val);
  void gp1_display_horizontal_range(uint32_t val);
  void gp1_display_vertical_range(uint32_t val);
  void gp1_display_mode(uint32_t val);
};

