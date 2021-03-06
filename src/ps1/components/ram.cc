#include "ram.h"

// init main RAM with garbage values
Ram::Ram(){
  data.assign(2 * 1024 * 1024, 0xca);
}

// Fetch the 32bit little endian word at 'offset'
uint32_t Ram::load32(uint32_t offset){
  size_t off = (size_t)offset;

  // byte0, byte1, byte2, byte3 (32bit = 4bytes)
  uint32_t b0 = data[off + 0];
  uint32_t b1 = data[off + 1];
  uint32_t b2 = data[off + 2];
  uint32_t b3 = data[off + 3];

  uint32_t l_endian_byte = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
  return l_endian_byte;
}

// Fetch the 16bit little endian halfword at 'offset'
uint16_t Ram::load16(uint32_t offset){
  size_t off = (size_t)off;

  uint16_t b0 = (uint16_t)data[off + 0];
  uint16_t b1 = (uint16_t)data[off + 1];

  uint16_t l_endian_byte = b0 | (b1 << 8);
  return l_endian_byte;
}

uint8_t Ram::load8(uint32_t offset){
  return (uint8_t)data[(size_t)offset];
}

// Store the 32bit little endian word 'value' at 'offset'
void Ram::store32(uint32_t offset, uint32_t value){
  size_t off = (size_t)offset;

  uint8_t b0 = (uint8_t)value;
  uint8_t b1 = (uint8_t)(value >> 8);
  uint8_t b2 = (uint8_t)(value >> 16);
  uint8_t b3 = (uint8_t)(value >> 24);

  data[off + 0] = b0;
  data[off + 1] = b1;
  data[off + 2] = b2;
  data[off + 3] = b3;
}

// Store the 16bit little endian word 'value' into 'offset'
void Ram::store16(uint32_t offset, uint16_t value){
  size_t off = (size_t)offset;

  uint8_t b0 = (uint8_t)value;
  uint8_t b1 = (uint8_t)(value >> 8);

  data[off + 0] = b0;
  data[off + 1] = b1;
}

// Store the byte 'val' into 'offset'
void Ram::store8(uint32_t offset, uint8_t value){
  data[(size_t)offset] = value;
}

