/*
 * bilateral.cpp
 * 
 * Handles performing the bilateral filter.
 * This uses the neighborhood "naive" approach, which runs in O(n^2) time.
 * 
 * References: 
 *   https://en.wikipedia.org/wiki/Bilateral_filter
 *   https://stackoverflow.com/questions/5695865/bilateral-filter
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include "pixel.h"
#include "imageutil.h"
#include "bilateral.h"

using namespace std;

/* This function is called by the function below it. Implements the "neighborhood" methods to 
 * reduce run time of the bilateral filter by only looking at the pixels around a certain area,
 * determined by maskWidth 
 */
pixel_t neighborhood(pixel_t* pixels, int* grayPixels, int index, int width, 
                     int height, int maskWidth, int maskCenterX, int maskCenterY, 
                     float sigmaD, float sigmaR) 
{
  //Initialize and setup variable that will be used
  float totalWeight = 0;
  int halfMaskWidth = maskWidth/2;
  int validPixelIndex;
  int validPixelRed;
  int validPixelGreen;
  int validPixelBlue;
  float totalRed = 0;
  float totalGreen = 0;
  float totalBlue = 0;
  int indexRow;
  int indexCol;
  int testRow;
  int testCol;
  int centerRed = get_red(pixels[index]);
  int centerGreen = get_red(pixels[index]);
  int centerBlue = get_red(pixels[index]);
  //Loop through all pixels in the neighborhood/window
  for(int indexMask = 0; indexMask < maskWidth*maskWidth; indexMask++) {
    indexRow = get_row(index, width, height);
    indexCol = get_column(index, width, height);
    testRow = get_row(indexMask, maskWidth, maskWidth)-halfMaskWidth+indexRow;
    testCol = get_column(indexMask,maskWidth,maskWidth)-halfMaskWidth+indexCol;
    // Test if the row and column are valid positions. Useful for when the window 
    // extends over the edge of the image.
    if(is_valid_pixel(testRow,testCol,width,height)) {
      //Get the r,g,b values of the pixel in the neighborhood
      validPixelIndex = get_index(testRow,testCol,width,height);
      validPixelRed = get_red(pixels[validPixelIndex]);
      validPixelGreen = get_green(pixels[validPixelIndex]);
      validPixelBlue = get_blue(pixels[validPixelIndex]);
      //Compare distance and color between the center and window pixel.  
      //Calculate the pixel distance. Uses distance between two points formula.  
      float pixelDistance = (float)(sqrt((float)pow(maskCenterX-testCol,2) + 
                                         (float)pow(maskCenterY-testRow,2)));
      //Calculate the color difference. Uses the formula for r,g,b color difference.
      float colorDifference = (float)sqrt((float)(pow(centerRed-validPixelRed,2) + 
                                          (float)pow(centerGreen-validPixelGreen,2) + 
                                          (float)pow(centerBlue-validPixelBlue,2)));
      //Find ratios to be used in weight calculation - makes code neater
      float ratio1 = pixelDistance/sigmaD;
      float ratio2 = (grayPixels[index]-grayPixels[validPixelIndex])/sigmaR;
      //Calculate the pixelWeight, influenced by sigmaD and sigmaR
      float pixelWeight = 1.0f/(exp(ratio1*ratio1*0.5)*exp(ratio2*ratio2*0.5));
      //Add up the weights,r,g,b values of all the pixels in the window
      totalWeight += pixelWeight;
      totalRed += pixelWeight*validPixelRed;
      totalGreen += pixelWeight*validPixelGreen;
      totalBlue += pixelWeight*validPixelBlue;
    }
  }
  //Find the normalized(final) center pixel r,g,b values
  int normalizedRed = (int)(floor(totalRed/totalWeight));
  int normalizedGreen = (int)(floor(totalGreen/totalWeight));
  int normalizedBlue = (int)(floor(totalBlue/totalWeight));
  //Return the new pixel  
  return make_pixel(255, normalizedRed, normalizedGreen, normalizedBlue);
}

/* This is the bilateral function that is called by the main file */
pixel_t* bilateral(pixel_t* pixels, int* grayPixels, int width, int height, 
                   int maskwidth, float sigmaD, float sigmaR) 
{
  //Create the pixel_t array to store the result  
  pixel_t* result =  (pixel_t*)calloc(sizeof(pixel_t), width*height);
  //Loop through every pixel in the image
  for(int index = 0; index < width*height; index++) {
    //Store the result from neighborhood into the result image array
    result[index] = neighborhood(pixels, grayPixels, index, width, height, maskwidth, 
                                 get_column(index, width, height), 
                                 get_row(index, width, height), sigmaD, sigmaR);
  }
  return result;
}
