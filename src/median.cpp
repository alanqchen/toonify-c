/*
 * median.cpp
 * 
 * Handles performing the median blur filter.
 * Finds the median pixel by intensity and the uses the rgb
 * values of the pixel for the center pixel.
 *
 * Reference: 
 *   https://en.wikipedia.org/wiki/Median_filter
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include "pixel.h"
#include "imageutil.h"
#include "median.h"
#include "mergesort.h"

using namespace std;

pixel_t neighborhood(pixel_t* pixels, int* grayPixels, int index, int width, 
                     int height, int maskWidth) 
{
  int halfMaskWidth = maskWidth/2;
  int validPixelIndex;
  int indexRow;
  int indexCol;
  int testRow;
  int testCol;
  int validPixelsCount = 0;
  //Find the number of valid pixels in the window, to find the array size needed
  for(int indexMask = 0; indexMask < maskWidth*maskWidth; indexMask++) {
    indexRow = get_row(index, width, height);
    indexCol = get_column(index, width, height);
    testRow = get_row(indexMask, maskWidth, maskWidth)-halfMaskWidth+indexRow;
    testCol = get_column(indexMask,maskWidth,maskWidth)-halfMaskWidth+indexCol;
    if(is_valid_pixel(testRow,testCol,width,height)) {
      validPixelsCount++;
    }
  }
  //Create array and initialize values. grayPixel_t is declared in mergesort.h
  grayPixel_t data = (grayPixel_t)calloc(sizeof(grayPixelData), validPixelsCount);
  for(int i=0; i < validPixelsCount; i++) {
    data[i].intensity = 0;
    data[i].index = 0;
  }
  //loop through the window again, this time storing the intensity and index
  //values in the newly created array.
  int dataIndexCount = 0;
  for(int indexMask = 0; indexMask < maskWidth*maskWidth; indexMask++) {
    indexRow = get_row(index, width, height);
    indexCol = get_column(index, width, height);
    testRow = get_row(indexMask, maskWidth, maskWidth)-halfMaskWidth+indexRow;
    testCol = get_column(indexMask,maskWidth,maskWidth)-halfMaskWidth+indexCol;
    if(is_valid_pixel(testRow,testCol,width,height)) {
      validPixelIndex = get_index(testRow,testCol,width,height);
      data[dataIndexCount].index = validPixelIndex;
      data[dataIndexCount].intensity = grayPixels[validPixelIndex];
      dataIndexCount++;
    }
  }

  //Merge sort the pixels by intensity - see mergesort.cpp
  mergesort(data, 0, validPixelsCount);
  
  //Get median pixel index in window array
  int mid = validPixelsCount/2;
  //Get median pixel index in image array
  int medianPixelIndex = data[mid].index;

  //New pixel rgb values, copied from the median pixel
  int newRed = get_red(pixels[medianPixelIndex]);
  int newGreen = get_green(pixels[medianPixelIndex]);
  int newBlue = get_blue(pixels[medianPixelIndex]);
  free(data);
  return make_pixel(255, newRed, newGreen, newBlue);
}

/* This is the median function that is called by the main file */
pixel_t* median(pixel_t* pixels, int* grayPixels, int width, int height, 
                int maskwidth) 
{
  pixel_t* result =  (pixel_t*)calloc(sizeof(pixel_t), width*height);
  for(int index = 0; index < width*height; index++) {
    result[index] = neighborhood(pixels,grayPixels,index,width,height,maskwidth);
  }
  return result;
}
