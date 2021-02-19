#include "texture.hpp"

const uint8_t test_texture_data[] =
  {
/*  BMP header
 *  0x50, 0x36, 0x0A, 0x32, 0x34, 0x20, 0x31, 0x36, 0x0A, 0x32, 0x35, 0x35, 
    0x0A,*/ 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 
    0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 
    0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 
    0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 
    0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 
    0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 
    0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 
    0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xD8, 
    0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 
    0x00, 0xB6, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB6, 0xFF, 
    0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 
    0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 
    0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xD8, 
    0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 
    0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 
    0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 
    0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 
    0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xD8, 
    0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 
    0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 
    0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 
    0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xD8, 
    0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 
    0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 
    0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 
    0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 
    0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xD8, 
    0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 
    0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 
    0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 
    0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 
    0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 
    0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 
    0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 
    0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 
    0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 
    0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 0x00, 0xFF, 0xD8, 
    0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 
    0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 0x00, 0xB6, 0xFF, 
    0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 
    0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 
    0xFF, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 
    0xFF, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 
    0xFF, 0xFF, 0x00, 0xDC, 0xFF, 0x00, 0xDC, 0xFF, 0x00, 0xDC, 0xFF, 0x00, 
    0xDC, 0xFF, 0x00, 0xDC, 0xFF, 0x00, 0xDC, 0xFF, 0x00, 0xDC, 0xFF, 0x00, 
    0xDC, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 
    0xFF, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 
    0xFF, 0xFF, 0x00, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0xFF, 0x00, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 
    0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 
    0xFF, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xB2, 0x00, 
    0xFF, 0xB2, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 
    0xFF, 0xFF, 0x00, 0xDC, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xDC, 0x00, 0x00, 
    0x00, 0xFF, 0x00, 0xDC, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xDC, 0x00, 0x00, 
    0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 
    0xFF, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xB2, 0x00, 
    0xFF, 0xB2, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 
    0xFF, 0xFF, 0x00, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 
    0xDC, 0xFF, 0x00, 0xDC, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xDC, 0x00, 0x00, 
    0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 
    0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 
    0xFF, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 
    0xFF, 0xFF, 0x00, 0xDC, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xDC, 0x00, 0x00, 
    0x00, 0xFF, 0x00, 0xDC, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xDC, 0x00, 0x00, 
    0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 
    0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 
    0xFF, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xB2, 0x00, 
    0xFF, 0xB2, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 
    0xFF, 0xFF, 0x00, 0xDC, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xDC, 0x00, 0x00, 
    0x00, 0xFF, 0x00, 0xDC, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xDC, 0x00, 0x00, 
    0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 
    0xFF, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xB2, 0x00, 
    0xFF, 0xB2, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 
    0xFF, 0xFF, 0x00, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 
    0xDC, 0xFF, 0x00, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 
    0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 
    0xFF, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 
    0xFF, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 0xFF, 0xB2, 0x00, 
    0xFF, 0xFF, 0x00, 0xDC, 0xFF, 0x00, 0xDC, 0xFF, 0x00, 0xDC, 0xFF, 0x00, 
    0xDC, 0xFF, 0x00, 0xDC, 0xFF, 0x00, 0xDC, 0xFF, 0x00, 0xDC, 0xFF, 0x00, 
    0xDC, 
  };