/*
 * This is Toonify, a cartoon filter using Magick++ and a custom pixel
 * implementation. Written in C++.
 *
 * main.cpp
 * 
 * main file that handles user inputs, and calls the functions that make
 * the final cartoonified image.
 * 
 * Requires Magick++ 32 Bit
 * 
 * To complile, copy-paste the following:
 * g++ `Magick++-config --cxxflags --cppflags` -o toonify pixel.h 
 *     pixel.cpp imageutil.h imageutil.cpp bilateral.h bilateral.cpp 
 *     mergesort.h mergesort.cpp median.h median.cpp adaptivethreshold.h 
 *     adaptivethreshold.cpp bitwiseAnd.h bitwiseAnd.cpp main.cpp 
 *     `Magick++-config --ldflags --libs`
 *
 * Reference: 
 *   https://stackoverflow.com/questions/1357403/how-to-cartoon-ify-an-image-programmatically
 */

#include <Magick++.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include <string>
#include <string.h>
#include <ctime>
#include "pixel.h"
#include "imageutil.h"
#include "bilateral.h"
#include "median.h"
#include "adaptivethreshold.h"
#include "bitwiseAnd.h"

using namespace Magick;

using namespace std;

int truncate(int value)
{
    if(value < 0) return 0;
    if(value > 255) return 255;

    return value;
}


int main(int argc, char **argv) {
  try {

		/* ============================ */	
    /* INITIALIZATION AND GET IMAGE */
		/* ============================ */
    InitializeMagick(*argv);
    string path;
    cout << "Enter image name/path: ";
    cin >> path;
    cout << "Parsing image..." << flush;
    Image sourceImage(path); //create Magick image

    int width = sourceImage.columns(); //get width + height
    int height = sourceImage.rows();
    
    //get pixel data
    Quantum *sourcePixels = sourceImage.getPixels(0, 0, width, height);
    //create our pixel type
    pixel_t* pixels = (pixel_t*)calloc(sizeof(pixel_t), width*height);
    //convert from image magick data to imageutil data(easier to work with)
    for(int i = 0; i < width*height; i++) {
      int row = get_row(i, width, height);
      int col = get_column(i, width, height);
      size_t offset = (width * row + col);
      size_t mainOffset = sourceImage.channels() * offset;
      int alpha;
      if(sourceImage.channels() != 4) { //check if there is an alpha channel
        alpha = 255; //if not, set to 255
      } else {
        alpha = sourcePixels[mainOffset + 3]; //if there is, set actual value
      }
      int red = sourcePixels[mainOffset + 0]; //get red channel
      int green = sourcePixels[mainOffset + 1]; //get green channel
      int blue = sourcePixels[mainOffset + 2]; //get blue channel
      pixels[i] = make_pixel(alpha, red, green, blue); //create our pixel and store
    }

    cout << "Done.\n" << flush;
		
		/* ======= */
		/* PROMPTS */
		/* ======= */
		/* Setup Prompts */
    float sigmaR;
    float sigmaD;
    int bilateralWidth;
    int timeBilateral;
    int medianWidth;
    int edgeWidth;
    string input;
    bool advancedMode = false;
    string destination; //store destination
    int brightness;
		/* The Prompts Themselves */
    cout << "Enter Advanced Mode(y/n)?";
    cin >> input;
    if(input == "y" || input == "Y" || input == "yes" || input == "Yes") 
      advancedMode = true;
    
    cout << "Enter range parameter(sigma r): ";
    cin >> sigmaR;
    cout << "Enter spatial parameter(sigma d): ";
    cin >> sigmaD;

    if(advancedMode) {
      cout << "Enter bilateral window width: ";
      cin >> bilateralWidth;
      cout << "Enter times to loop bilateral filter: ";
      cin >> timeBilateral;
      cout << "Enter median window width: ";
      cin >> medianWidth;
      cout << "Enter edge window width: ";
      cin >> edgeWidth;
      cout << "Enter brightness adjustment[-30,30]: ";
      cin >> brightness;
    } else {
      cout << "Enter window width: ";
      cin >> bilateralWidth;
      medianWidth = bilateralWidth;
      edgeWidth = bilateralWidth;
      timeBilateral = 1;
      brightness = 0;
    }
    cout << "Enter destination file/path, nothing for default(imgs/cartoon.png): ";
    cin.ignore();
    getline(cin, destination);
    if(destination == "") {
      destination = "imgs/cartoon.png"; //default path
    }

		/* Start Execution Timer */
    clock_t start;
    double time;
    cout << "Creating bilateral..." << flush; //display progress
    start = clock();
	
		/* ================ */
		/* BILATERAL FILTER */
		/* ================ */
		/* Bilateral Filter Setup */
    
		//Create new image copy for grayscale image
    Image grayScaleIMG(sourceImage);
    //Convert image from rgb to grayscale(1 channel)
    grayScaleIMG.type(GrayscaleType);
    //get pixel data
    Quantum *graySourcePixels = grayScaleIMG.getPixels(0, 0, width, height);
    //create grayscale pixel type
    int* grayPixels = (int*)calloc(sizeof(int), width*height);
    //convert from image magick data to imageutil data(easier to work with)
    for(int i = 0; i < width*height; i++) {
      int row = get_row(i, width, height);
      int col = get_column(i, width, height);
      size_t offset = (width * row + col);
      size_t mainOffset = grayScaleIMG.channels() * offset;
      //create our pixel and store it(only 1 channel)
      grayPixels[i] = graySourcePixels[mainOffset + 0]; 
    }
		
		/* Calling the bilateral filter */
    pixel_t* bilateralPixels = bilateral(pixels, grayPixels, width, height, 
                                         bilateralWidth, sigmaD, sigmaR);
    timeBilateral--;
		/* Loop to call the bilateral filter additonal times if > 1 */
    for(int i=0; i < timeBilateral; i++) {
      bilateralPixels = bilateral(bilateralPixels, grayPixels, width, height, 
                                  bilateralWidth, sigmaD, sigmaR);
    }

    /* Convert and Write Bilateral Filter Pixels into Image */
    //create dimension string
    string dimensions = to_string(width) + "x" + to_string(height);
    Image newImage((Geometry)dimensions, "transparent");
    Quantum *newImagePixels = newImage.getPixels(0, 0, width, height);
    for(int i = 0; i < width*height; i++) { //Copy to new image BLOB
      int row = get_row(i, width, height);
      int col = get_column(i, width, height);
      size_t offset = (width * row + col);
      size_t mainOffset = newImage.channels() * offset;
      int newRed = truncate(get_red(bilateralPixels[i]) + brightness);
      int newGreen = truncate(get_green(bilateralPixels[i]) + brightness);
      int newBlue = truncate(get_blue(bilateralPixels[i]) + brightness);
      bilateralPixels[i] = make_pixel(get_alpha(bilateralPixels[i]), 
                                      newRed, newGreen, newBlue); 
      newImagePixels[mainOffset + 0] = get_red(bilateralPixels[i]);
      newImagePixels[mainOffset + 1] = get_green(bilateralPixels[i]);
      newImagePixels[mainOffset + 2] = get_blue(bilateralPixels[i]);
      newImagePixels[mainOffset + 3] = get_alpha(bilateralPixels[i]);
    }
    newImage.write("imgs/bilateral.png"); //write to output
    cout << "Done.\n" << flush; //signal finish

		/* ================== */
		/* MEDIAN FILTER/BLUR */
		/* ================== */
    cout << "Creating median..." << flush;
    //Create deep copy of gray pixels, it will be sorted during median
    int* grayPixelsCopy = (int*)calloc(sizeof(int), width*height);
    for(int i = 0; i < width*height; i++) {
      grayPixelsCopy[i] = grayPixels[i];
    }
		
		/* Calling the Median Blur */
    pixel_t* medianPixels = median(pixels, grayPixelsCopy, width, height, 
                                   medianWidth);

    /* Convert and Write Median Filter/Blur into Image */
    newImagePixels = newImage.getPixels(0, 0, width, height);
    for(int i = 0; i < width*height; i++) { //Copy to new image BLOB
      int row = get_row(i, width, height);
      int col = get_column(i, width, height);
      size_t offset = (width * row + col);
      size_t mainOffset = newImage.channels() * offset;
      newImagePixels[mainOffset + 0] = get_red(medianPixels[i]);
      newImagePixels[mainOffset + 1] = get_green(medianPixels[i]);
      newImagePixels[mainOffset + 2] = get_blue(medianPixels[i]);
      newImagePixels[mainOffset + 3] = get_alpha(medianPixels[i]);
    }
    newImage.write("imgs/median.png");
		//Get the median pixels so that it can be converted to grayscale
    Image medianImage("imgs/median.png");
    cout << "Done.\n" << flush; //signal finish

    /* ================================= */
		/* ADAPTIVE THRESHOLD EDGE DETECTION */
		/* ================================= */
    cout << "Applying adaptive threshold..." << flush;
    /* Grayscale conversion of median blur pixels */
		Image grayScaleMedianIMG(medianImage);
    grayScaleMedianIMG.type(GrayscaleType);
    Quantum *grayMedianPixels = grayScaleMedianIMG.getPixels(0, 0, width, height);
    int* grayMedPixels = (int*)calloc(sizeof(int), width*height);
    for(int i = 0; i < width*height; i++) {
      int row = get_row(i, width, height);
      int col = get_column(i, width, height);
      size_t offset = (width * row + col);
      size_t mainOffset = grayScaleMedianIMG.channels() * offset;
      //create our pixel and store it(only 1 channel)
      grayMedPixels[i] = grayMedianPixels[mainOffset + 0];
    }

    /* Calling the Adaptive Threshold Edge Detection */
    pixel_t* edgePixels = adaptiveThreshold(grayMedPixels,width,height,edgeWidth);
    
    newImagePixels = newImage.getPixels(0, 0, width, height);
    for(int i = 0; i < width*height; i++) { //Copy to new image BLOB
      int row = get_row(i, width, height);
      int col = get_column(i, width, height);
      size_t offset = (width * row + col);
      size_t mainOffset = newImage.channels() * offset;
      newImagePixels[mainOffset + 0] = get_red(edgePixels[i]);
      newImagePixels[mainOffset + 1] = get_green(edgePixels[i]);
      newImagePixels[mainOffset + 2] = get_blue(edgePixels[i]);
      newImagePixels[mainOffset + 3] = get_alpha(edgePixels[i]);
    }
    newImage.write("imgs/edge.png");
    cout << "Done.\n" <<flush;

    /* ================================================ */
		/* Bitwise AND(to combine bilateral+edge detection) */
		/* ================================================ */
    cout << "Combining with bitwise AND..." << flush;

    /* Calling the Bitwise AND */
    pixel_t* newPixels =  bitwiseAnd(bilateralPixels, edgePixels, width, height);
    cout << "Done.\n" << flush;
    cout << "Writing image to "<< destination << "..." << flush;
    newImagePixels = newImage.getPixels(0, 0, width, height);
    //copy custom pixels to final Magick++ BLOB
    for(int i = 0; i < width*height; i++) {
      int row = get_row(i, width, height);
      int col = get_column(i, width, height);
      size_t offset = (width * row + col);
      size_t mainOffset = newImage.channels() * offset;
      newImagePixels[mainOffset + 0] = get_red(newPixels[i]);
      newImagePixels[mainOffset + 1] = get_green(newPixels[i]);
      newImagePixels[mainOffset + 2] = get_blue(newPixels[i]);
      newImagePixels[mainOffset + 3] = get_alpha(newPixels[i]);
    }
    newImage.write(destination);

    /* Free allocated memory */
    free(pixels);
    free(grayPixels);
    free(grayPixelsCopy);
    free(grayMedPixels);
    free(bilateralPixels);
    free(medianPixels);
    free(edgePixels);
    free(newPixels);

    /* Stop Timer */
    time = (clock() - start)/(double) CLOCKS_PER_SEC;

    cout << "Done.\n";
    cout << "Execution time: " << time << "s\n";
  } 
  catch ( Magick::Exception & error) { 
    //Catch statement for any errors from Magick++
    cerr << "\nCaught Magick++ exception: " << error.what() << endl;
  }
  return 0;
}

