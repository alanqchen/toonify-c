/*
 * median.h - Header File
 */

#include <stdlib.h>
#include <math.h>
#include "pixel.h"
#include "imageutil.h"

pixel_t neighborhood(pixel_t* pixels, int* grayPixels, int index, int width, 
                     int height, int maskWidth);

pixel_t* median(pixel_t* pixels, int* grayPixels, int width, int height, 
                int maskwidth);
