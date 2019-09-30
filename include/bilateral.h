/*
 * bilateral.h - Header File
 */

#include <stdlib.h>
#include <math.h>
#include "pixel.h"
#include "imageutil.h"

pixel_t neighborhood(pixel_t* pixels, int* grayPixels, int index, int width, 
                     int height, int maskWidth, int maskCenterX, int maskCenterY, 
                     float sigmaD, float sigmaR);

pixel_t* bilateral(pixel_t* pixels, int* grayPixels, int width, int height, 
                   int maskwidth, float sigmaD, float sigmaR);
