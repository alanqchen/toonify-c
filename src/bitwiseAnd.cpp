/*
 * bitwiseAnd.cpp
 * 
 * Performs bitwise AND on the pixels to combine the edge pixel mask and
 * the bilateral filter image to create the final cartoon image.
 */

#include <stdlib.h>
#include <math.h>
#include "pixel.h"
#include "imageutil.h"
#include "bitwiseAnd.h"

pixel_t* bitwiseAnd(pixel_t* pixels, pixel_t* mask, int width, int height) {
  pixel_t* result =  (pixel_t*)calloc(sizeof(pixel_t), width*height);
  for(int index = 0; index < width*height; index++) {
    int newAlpha = get_alpha(pixels[index]) & get_alpha(mask[index]);
    int newRed = get_red(pixels[index]) & get_red(mask[index]);
    int newGreen = get_green(pixels[index]) & get_green(mask[index]);
    int newBlue = get_blue(pixels[index]) & get_blue(mask[index]);
    result[index] = make_pixel(newAlpha, newRed, newGreen, newBlue);
  } 
  return result;
}
