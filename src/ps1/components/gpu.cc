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

// --------------------------------------------------------------

// Command Buffer methods
CommandBuffer::CommandBuffer(){
  for(int i=0;i < 12;i++)
    buffer[i] = 0;
  len = 0;
}

// Clear the command buffer
void CommandBuffer::clear(){
  len = 0;
}

void CommandBuffer::push_word(uint32_t word){
  buffer[(size_t)len] = word;
  len += 1;
}

uint32_t* CommandBuffer::index(size_t index){
  if(index >= (size_t)len){
    printf("Command buffer index out of range: %x (%x)\n", (uint32_t)index, len);
  }

  return &buffer[index];
}

// --------------------------------------------------------------

Gpu::Gpu(Renderer *renderer){
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
  gp0_mode = Command;

  this->renderer = renderer;
}

// Retrieve value of the status register
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
  // TODO: this a temporary workaround, if bit 31 is not emulated correctly, setting 'vres' to 1 locks the BIOS in an infinite loop
  //r |= (uint32_t)vres << 19;
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
    case CpuToGp0:
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

// Retrieve value of the "read" register
uint32_t Gpu::read(){
  // TODO: implement this
  return 0;
}

// Handle writes to the GP0 command register
void Gpu::gp0(uint32_t val){
  if(gp0_words_remaining == 0){
    uint32_t opcode = (val >> 24) & 0xff;
    uint32_t len;
    void (Gpu::*method)(uint32_t);

    switch(opcode){
      case 0x00:
        len = 1;
        method = &Gpu::gp0_nop;
        break;
      case 0x28:
        len = 1;
        method = &Gpu::gp0_quad_mono_opaque;
        break;
      case 0x2c:
        len = 9;
        method = &Gpu::gp0_quad_texture_blend_opaque;
        break;
      case 0x30:
        len = 6;
        method = &Gpu::gp0_triangle_shaded_opaque;
        break;
      case 0x38:
        len = 8;
        method = &Gpu::gp0_quad_shaded_opaque;
        break;
      case 0xa0:
        len = 3;
        method = &Gpu::gp0_image_load;
        break;
      case 0xe1:
        len = 1;
        method = &Gpu::gp0_draw_mode;
        break;
      case 0xe2:
        len = 1;
        method = &Gpu::gp0_texture_window;
        break;
      case 0xe3:
        len = 1;
        method = &Gpu::gp0_drawing_area_top_left;
        break;
      case 0xe4:
        len = 1;
        method = &Gpu::gp0_drawing_area_bottom_right;
        break;
      case 0xe5:
        len = 1;
        method = &Gpu::gp0_drawing_offset;
        break;
      case 0xe6:
        len = 1;
        method = &Gpu::gp0_mask_bit_setting;
        break;
      default:
        printf("Unhandled GP0 command %08x\n", val);
        exit(1);
    }

    gp0_words_remaining = len;
    gp0_command_method = method;
    gp0_command->clear();
  }

  gp0_words_remaining--;

  switch(gp0_mode){
    case Command:
    {
      gp0_command->push_word(val);

      if(gp0_words_remaining == 0){
        // We have all the parameters, we can run the command
        (this->*gp0_command_method)(val); // TODO: this may not be right
      }
    }
      break;
    case ImageLoad:
    {
      // TODO: copy pixel data to VRAM

      if(gp0_words_remaining == 0){
        // Load done, switch back to command mode
        gp0_mode = Command;
      }
    }
      break;
  }
}

// GP0(0x00): No Operation
void Gpu::gp0_nop(uint32_t val){
  // NOP
}

// GP0(0x01)
void Gpu::gp0_clear_cache(){
  // Not implemented yet since we don't have a texture cache yet
}

// GP0(0x28): Monochrome Opaque Quadrilateral
void Gpu::gp0_quad_mono_opaque(uint32_t val){
  Position positions[4];
  positions[0].from_gp0(gp0_command->buffer[1]);
  positions[1].from_gp0(gp0_command->buffer[2]);
  positions[2].from_gp0(gp0_command->buffer[3]);
  positions[3].from_gp0(gp0_command->buffer[4]);

  Color colors[4];
  colors[0].from_gp0(gp0_command->buffer[0]);
  colors[1].from_gp0(gp0_command->buffer[0]);
  colors[2].from_gp0(gp0_command->buffer[0]);
  colors[3].from_gp0(gp0_command->buffer[0]);

  renderer->push_quad(positions, colors);
}

// GP0(0x2c): Textured Opaque Quadrilateral
void Gpu::gp0_quad_texture_blend_opaque(uint32_t val){
  Position positions[4];
  positions[0].from_gp0(gp0_command->buffer[1]);
  positions[1].from_gp0(gp0_command->buffer[3]);
  positions[2].from_gp0(gp0_command->buffer[5]);
  positions[3].from_gp0(gp0_command->buffer[7]);

  // TODO: we don't support textures for now, use a solid red color instead
  Color colors[4];
  colors[0].rgb[0] = 0x80;
  colors[0].rgb[1] = 0x00;
  colors[0].rgb[2] = 0x00;
  colors[1].rgb[0] = 0x80;
  colors[1].rgb[1] = 0x00;
  colors[1].rgb[2] = 0x00;
  colors[2].rgb[0] = 0x80;
  colors[2].rgb[1] = 0x00;
  colors[2].rgb[2] = 0x00;
  colors[3].rgb[0] = 0x80;
  colors[3].rgb[1] = 0x00;
  colors[3].rgb[2] = 0x00;

  renderer->push_quad(positions, colors);
}

// GP0(0x30): Shaded Opque Triangle
void Gpu::gp0_triangle_shaded_opaque(uint32_t val){
  Position positions[3];
  positions[0].from_gp0(gp0_command->buffer[1]);
  positions[1].from_gp0(gp0_command->buffer[3]);
  positions[2].from_gp0(gp0_command->buffer[5]);

  Color colors[3];
  colors[0].from_gp0(gp0_command->buffer[0]);
  colors[1].from_gp0(gp0_command->buffer[2]);
  colors[2].from_gp0(gp0_command->buffer[4]);

  renderer->push_triangle(positions, colors);
}

// GP0(0x38): Shaded Opaque Quadrilateral
void Gpu::gp0_quad_shaded_opaque(uint32_t val){
  Position positions[4];
  positions[0].from_gp0(gp0_command->buffer[1]);
  positions[1].from_gp0(gp0_command->buffer[3]);
  positions[2].from_gp0(gp0_command->buffer[5]);
  positions[3].from_gp0(gp0_command->buffer[7]);

  Color colors[4];
  colors[0].from_gp0(gp0_command->buffer[0]);
  colors[1].from_gp0(gp0_command->buffer[2]);
  colors[2].from_gp0(gp0_command->buffer[4]);
  colors[3].from_gp0(gp0_command->buffer[6]);

  renderer->push_quad(positions, colors);
}

// GP0(0xa0): Image Load
void Gpu::gp0_image_load(uint32_t val){
  // Parameter 2 contains the image resolution
  uint32_t res = gp0_command->buffer[2];  // TODO: this might not be right

  uint32_t width = res & 0xffff;
  uint32_t height = res >> 16;
  
  // Size of the image in 16bit pixels
  uint32_t imgsize = width * height;
  
  // If we have an odd number of pixels we must round up since we transfer 32 bits at a time
  // There will be 16 bits of padding in the last word;
  imgsize = (imgsize + 1) & !1;

  // Store number of words expected for this image
  gp0_words_remaining = imgsize / 2;

  // Put the GP0 state machine in ImageLoad mode
  gp0_mode = ImageLoad;
}

// GP0(0xc0): Image Store
void Gpu::cp0_image_store(uint32_t val){
  // Parameter 2 contains image resolution
  uint32_t res = gp0_command->buffer[2];  // TODO: this might not be right

  uint32_t width = res & 0xffff;
  uint32_t height = res >> 16;

  printf("Unhdandled image store: %x x %x\n", width, height);
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

// GP0(0xe2): set texture window
void Gpu::gp0_texture_window(uint32_t val){
  texture_window_x_mask = (uint8_t)(val & 0x1f);
  texture_window_y_mask = (uint8_t)((val >> 5) & 0x1f);
  texture_window_x_offset = (uint8_t)((val >> 10) & 0x1f);
  texture_window_y_offset = (uint8_t)((val >> 15) & 0x1f);
}

// GP0(0xe3): set drawing area top left
void Gpu::gp0_drawing_area_top_left(uint32_t val){
  drawing_area_top = (uint16_t)((val >> 10) & 0x3ff);
  drawing_area_left = (uint16_t)(val & 0x3ff);
}

// GP0(0xe4): set drawing area bottom right
void Gpu::gp0_drawing_area_bottom_right(uint32_t val){
  drawing_area_bottom = (uint16_t)((val >> 10) & 0x3ff);
  drawing_area_right = (uint16_t)(val & 0x3ff);
}

// GP0(0xe5): set drawing offset
void Gpu::gp0_drawing_offset(uint32_t val){
  uint16_t x = (uint16_t)(val & 0x7ff);
  uint16_t y = (uint16_t)((val >> 11) & 0x7ff);

  // Values are 11bit 2's complement signed values, we need to shift the value to 16bits to force sign extension
  drawing_x_offset = ((int16_t)(x << 5)) >> 5;
  drawing_y_offset = ((int16_t)(y << 5)) >> 5;

  // TODO: temporary hack, force display when changing offset since we don't have proper timings
  renderer->display();
}

// GP0(0xe6): set mask bit setting
void Gpu::gp0_mask_bit_setting(uint32_t val){
  force_set_mask_bit = (val & 1) != 0;
  preserve_masked_pixels = (val & 2) != 0;
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

// GP1(0x01): Reset Command Buffer
void Gpu::gp1_reset_command_buffer(){
  gp0_command->clear();
  gp0_words_remaining = 0;
  gp0_mode = Command;
  // TODO: clear the command FIFO when it is implemented
}

// GP1(0x02): Acknowledge Interrupt
void Gpu::gp1_acknowledge_irq(){
  interrupt = false;
}

// GP1(0x03): Display Enable
void Gpu::gp1_display_enable(uint32_t val){
  display_disabled = val & 1 != 0;
}

// GP1(0x04): DMA direction
void Gpu::gp1_dma_direction(uint32_t val){
  switch(val & 3){
    case 0:
      dma_direction = Off;
      break;
    case 1:
      dma_direction = Fifo;
      break;
    case 2:
      dma_direction = CpuToGp0;
      break;
    case 3:
      dma_direction = VRamToCpu;
      break;
  }
}

// GP1(0x05): display VRAM start
void Gpu::gp1_display_vram_start(uint32_t val){
  display_vram_x_start = (uint16_t)(val & 0x3fe);
  display_vram_y_start = (uint16_t)((val >> 10) & 0x1ff);
}

// GP1(0x06): display horizontal range
void Gpu::gp1_display_horizontal_range(uint32_t val){
  display_horiz_start = (uint16_t)(val & 0xfff);
  display_horiz_end = (uint16_t)((val >> 12) & 0xfff);
}

// GP1(0x07): display vertical range
void Gpu::gp1_display_vertical_range(uint32_t val){
  display_line_start = (uint16_t)(val & 0x3ff);
  display_line_end = (uint16_t)((val >> 10) & 0x3ff);
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

