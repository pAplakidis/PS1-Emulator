#include "gpu.h"

// --------------------------------------------------------------

// Video output horizontal resolution methods
// Make hres from the 2 bit field hr1 and the 1 bit field hr2
uint8_t HorizontalRes::from_fields(uint8_t hr1, uint8_t hr2){
  hr = (hr2 & 1) | ((hr1 & 3) << 1);
  return hr;
}

// Retrieve value of bits [18:16] of the status register
uint32_t HorizontalRes::into_status(){
  return ((uint32_t)hr) << 16;
}

// --------------------------------------------------------------

Gpu::Gpu(){
  page_base_x = 0;
  page_base_y = 0;
  semi_transparency = 0;
  texture_depth = T4Bit;
  dithering = false;
  draw_to_display = false;
  force_set_mask_bit = false;
  preserve_masked_pixels = false;
  field = Top;
  texture_disable = false;
  horizontal_res = new HorizontalRes();
  hres = horizontal_res->from_fields(0, 0);
  vres = Y240Lines;
  vmode = Ntsc;
  display_depth = D15Bits;
  interlaced = false;
  display_disabled = true;
  interrupt = false;
  dma_direction = Off;
}

uint32_t Gpu::status(){
  uint32_t r = 0;

  r |= (uint32_t)page_base_x << 0;
  r |= (uint32_t)page_base_y << 4;
  r |= (uint32_t)semi_transparency << 5;
  r |= (uint32_t)texture_depth << 7;
  r |= (uint32_t)dithering << 9;
  r |= (uint32_t)draw_to_display << 10;
  r |= (uint32_t)force_set_mask_bit << 11;
  r |= (uint32_t)preserve_masked_pixels << 12;
  r |= (uint32_t)field << 13;
  // Bit 14: not supported
  r |= (uint32_t)texture_disable << 15;
  r |= horizontal_res->into_status();
  r |= (uint32_t)vres << 19;
  r |= (uint32_t)vmode << 20;
  r |= (uint32_t)display_depth << 21;
  r |= (uint32_t)interlaced << 22;
  r |= (uint32_t)display_disabled << 23;
  r |= (uint32_t)interrupt << 24;

  // For now we pretend that the GPU is always ready:
  // Ready to receive command
  r |= 1 << 26;
  // // Ready to send VRAM to CPU
  r |= 1 << 27;
  // Ready to receive DMA block
  r |= 1 << 28;

  // Probably the signal checked by the DMA in when sending data in Request synchronization mode
  uint32_t dma_request;
  switch(dma_direction){
    // Always 0
    case Off:
      dma_request = 0;
      break;
    // Should be 0 if FIFO is full, 1 otherwise
    case Fifo:
      dma_request = 1;
      break;
    // Shoud be the same as status bit 28
    case GpuToGp0:
      dma_request = (r >> 28) & 1;
      break;
    // Should be the same as status bit 27
    case VRamToCpu:
      dma_request = (r >> 27) & 1;
      break;
  }

  r |= dma_request << 25;
  return r;
}

// Handle writes to the GP0 command register
void Gpu::gp0(uint32_t val){
  uint32_t opcode = (val >> 24) & 0xff;

  switch(opcode){
    case 0x00:
      // NOP
      break;
    case 0xe1:
      gp0_draw_mode(val);
      break;
    default:
      printf("Unhandled GP0 command %08x\n", val);
      exit(1);
  }
}

// GP0(0xe1) command
void Gpu::gp0_draw_mode(uint32_t val){
  page_base_x = (uint8_t)(val & 0xf);
  page_base_y = (uint8_t)((val >> 4) & 1);
  semi_transparency = (uint8_t)((val >> 5) & 3);

  switch((val >> 7) & 3){
    case 0:
      texture_depth = T4Bit;
      break;
    case 1:
      texture_depth = T8Bit;
      break;
    case 2:
      texture_depth = T15Bit;
      break;
    default:
      printf("Unhandled texture depth %x\n", (val >> 7) & 3);
      exit(1);
  }

  dithering = ((val >> 9) & 1) != 0;
  draw_to_display = ((val >> 10) & 1) != 0;
  texture_disable = ((val >> 11) & 1) != 0;
  rectangle_texture_x_flip = ((val >> 12) & 1) != 0;
  rectangle_texture_y_flip = ((val >> 13) & 1) != 0;
}

// Handle writes to the GP1 command register
void Gpu::gp1(uint32_t val){
  uint32_t opcode = (val >> 24) & 0xff;

  switch(opcode){
    case 0x00:
      gp1_reset(val);
      break;
    default:
      printf("Unhandled GP1 command %08x\n", val);
      exit(1);
  }
}

// GP1(0x00): soft reset
void Gpu::gp1_reset(uint32_t _){
  interrupt = false;

  page_base_x = 0;
  page_base_y = 0;
  semi_transparency = 0;
  texture_depth = T4Bit;
  dithering = false;
  draw_to_display = false;
  texture_disable = false;
  force_set_mask_bit = false;
  preserve_masked_pixels = false;
  dma_direction = Off;
  display_disabled = true;
  hres = horizontal_res->from_fields(0, 0);
  vres = Y240Lines;
  vmode = Ntsc;
  interlaced = true;
  display_depth = D15Bits;

  rectangle_texture_x_flip = false;
  rectangle_texture_y_flip = false;

  texture_window_x_mask = 0;
  texture_window_y_mask = 0;
  texture_window_x_offset = 0;
  texture_window_y_offset = 0;
  drawing_area_left = 0;
  drawing_area_top = 0;
  drawing_area_right = 0;
  drawing_area_bottom = 0;
  drawing_x_offset = 0;
  drawing_y_offset = 0;
  display_vram_x_start = 0;
  display_vram_y_start = 0;
  display_horiz_start = 0x200;
  display_horiz_end = 0xc00;
  display_line_start = 0x10;
  display_line_end = 0x100;

  // TODO: clear the command FIFO when it is implemented
  // TODO: invalidate GPU cache when it is implemented
}

// Retrieve value of the "read" register
uint32_t Gpu::read(){
  // TODO: implement this
  return 0;
}

// GP1(0x80): Display Mode
void Gpu::gp1_display_mode(uint32_t val){
  uint8_t hr1 = (uint8_t)(val & 3);
  uint8_t hr2 = (uint8_t)((val >> 6) & 1);

  hres = horizontal_res->from_fields(hr1, hr2);

  switch(val & 0x4 != 0){
    case false:
      vres = Y240Lines;
      break;
    case true:
      vres = Y480Lines;
      break;
  }

  switch(val & 0x8 != 0){
    case false:
      vmode = Ntsc;
      break;
    case true:
      vmode = Pal;
      break;
  }

  switch(val & 0x10 != 0){
    case false:
      display_depth = D24Bits;
      break;
    case true:
      display_depth = D15Bits;
      break;
  }

  interlaced = val & 0x20 != 0;

  if(val & 0x80 != 0){
    printf("Unsupported display mode %08x\n", val);
    exit(1);
  }
}

