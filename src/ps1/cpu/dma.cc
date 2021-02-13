#include "dma.h"

Dma::Dma(){
  // reset value taken from the Nocash PSX spec
  control = 0x07654321;
}

// Retrieve the value of the control register
uint32_t Dma::get_control(){
  return control;
}

