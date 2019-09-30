/*
 * adaptivethreshold.cpp
 * 
 * Handles performing the adaptive threshold edge detection.
 * Finds the mean intensity of the pixels in the neightborhood
 * then compares the mean to the center pixel's intensity and
 * makes the new pixel white or black accordingly
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include "pixel.h"
#include "imageutil.h"
#include "adaptivethreshold.h"

using namespace std;

pixel_t neighborhood(int* grayPixels, int index, int width, int height, 
                     int maskWidth) 
{
  //initialize variables
  int halfMaskWidth = maskWidth/2;
  int validPixelIndex;
  int indexRow;
  int indexCol;
  int testRow;
  int testCol;
  int validPixelsCount = 0;
  int totalIntensity = 0;
  //loop through window/neighborhood
  for(int indexMask = 0; indexMask < maskWidth*maskWidth; indexMask++) {
    indexRow = get_row(index, width, height);
    indexCol = get_column(index, width, height);
    testRow = get_row(indexMask, maskWidth, maskWidth)-halfMaskWidth+indexRow;
    testCol = get_column(indexMask,maskWidth,maskWidth)-halfMaskWidth+indexCol;
    //test if pixel is a valid position(for edges)
    if(is_valid_pixel(testRow,testCol,width,height)) {
        validPixelIndex = get_index(testRow,testCol,width,height);
        totalIntensity += grayPixels[validPixelIndex];
        //add pixel's intensity to the total to be averaged
        validPixelsCount++;
    }
  }
  //Find average intensity in window
  int mean = totalIntensity/validPixelsCount - 2;
  //Make new pixel black if > mean or white if <= mean
  if(grayPixels[index] > mean) {
    return make_pixel(255, 255, 255, 255);
  } else {
    return make_pixel(255, 0, 0, 0);
  }
}

/* This is the adaptiveThreshold function that is called by the main file */
pixel_t* adaptiveThreshold(int* grayPixels, int width, int height, int maskwidth) {
  //Create pixel_t array to store result
  pixel_t* result =  (pixel_t*)calloc(sizeof(pixel_t), width*height);
  //Loop through whole image
  for(int index = 0; index < width*height; index++) {
    result[index] = neighborhood(grayPixels, index, width, height, maskwidth);
  }
  return result;
}
