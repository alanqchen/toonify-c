/*
 * adaptivethreshold.h - Header File
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include "pixel.h"
#include "imageutil.h"

pixel_t neighborhood(int* grayPixels, int index, int width, int height, 
                     int maskWidth);
                     
pixel_t* adaptiveThreshold(int* grayPixels, int width, int height, int maskwidth);
